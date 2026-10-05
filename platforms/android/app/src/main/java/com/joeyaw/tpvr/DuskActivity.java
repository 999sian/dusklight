package com.joeyaw.tpvr;

import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.Log;

import dev.encounter.borealis.BorealisActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class DuskActivity extends BorealisActivity {
    private static final String TAG = "DuskActivity";
    private static final String PREFS_NAME = "dusklight";
    private static final String PREF_ASKED_STORAGE = "askedSharedStoragePermission";
    // Shared-storage data folder handed to dusk::data::initialize_data(). Unlike app-internal
    // storage it survives an uninstall, so saves aren't lost when an update can't be installed
    // in place.
    private static final String SHARED_DATA_DIR_NAME = "Dusklight";
    private static final long STORAGE_PROMPT_TIMEOUT_MS = 120000;
    private static final int STORAGE_PROMPT_REQUEST_CODE = 0x4455;

    private final Object storagePromptLock = new Object();
    private boolean storagePromptPending = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        extractBundledMods();
        super.onCreate(savedInstanceState);
        requestSharedStoragePermissionOnce();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == STORAGE_PROMPT_REQUEST_CODE) {
            // The settings screen closed (it reports no meaningful result code).
            Log.i(TAG, "All-files access prompt closed, granted=" + hasSharedStorageAccess());
            synchronized (storagePromptLock) {
                storagePromptPending = false;
                storagePromptLock.notifyAll();
            }
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    private static boolean hasSharedStorageAccess() {
        // Pre-R would need the runtime WRITE_EXTERNAL_STORAGE permission instead; Quest is R+.
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && Environment.isExternalStorageManager();
    }

    // Asks for all-files access once per install.
    private void requestSharedStoragePermissionOnce() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R || hasSharedStorageAccess()) {
            return;
        }
        SharedPreferences prefs = getSharedPreferences(PREFS_NAME, MODE_PRIVATE);
        if (prefs.getBoolean(PREF_ASKED_STORAGE, false)) {
            return;
        }
        prefs.edit().putBoolean(PREF_ASKED_STORAGE, true).apply();
        synchronized (storagePromptLock) {
            storagePromptPending = true;
        }
        try {
            Log.i(TAG, "Requesting all-files access");
            startActivityForResult(
                new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
                    .setData(Uri.parse("package:" + getPackageName())),
                STORAGE_PROMPT_REQUEST_CODE);
        } catch (Exception e) {
            Log.w(TAG, "Unable to request all-files access", e);
            synchronized (storagePromptLock) {
                storagePromptPending = false;
            }
        }
    }

    /**
     * Called from native code (dusk::data::initialize_data) on the SDL thread. On Quest the game
     * keeps running behind the settings panel, so this blocks until the permission prompt is
     * answered, then returns the shared data folder, or null without all-files access.
     */
    public String awaitSharedDataDir() {
        synchronized (storagePromptLock) {
            Log.i(TAG, "Data folder: waiting for prompt=" + storagePromptPending);
            long deadline = SystemClock.uptimeMillis() + STORAGE_PROMPT_TIMEOUT_MS;
            while (storagePromptPending && !hasSharedStorageAccess()) {
                long remaining = deadline - SystemClock.uptimeMillis();
                if (remaining <= 0) {
                    Log.w(TAG, "Timed out waiting for the all-files access prompt");
                    break;
                }
                try {
                    // Wake periodically to notice a grant while the settings panel is still open.
                    storagePromptLock.wait(Math.min(remaining, 250));
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
            storagePromptPending = false;
        }
        if (!hasSharedStorageAccess()) {
            Log.i(TAG, "No all-files access; keeping data in internal storage");
            return null;
        }
        return new File(Environment.getExternalStorageDirectory(), SHARED_DATA_DIR_NAME)
            .getAbsolutePath();
    }

    // Bundled mod packages ship as APK assets, which the native loader cannot read directly;
    // mirror them into internal storage (the loader's CachePath/bundled_mods search dir)
    // before SDL_main starts.
    private void extractBundledMods() {
        File outDir = new File(getFilesDir(), "bundled_mods");
        try {
            deleteRecursively(outDir); // drop packages removed by an app update
            String[] names = getAssets().list("mods");
            if (names == null || names.length == 0) {
                return;
            }
            if (!outDir.mkdirs()) {
                Log.w(TAG, "Unable to create " + outDir);
                return;
            }
            byte[] buffer = new byte[65536];
            for (String name : names) {
                if (!name.endsWith(".dusk")) {
                    continue;
                }
                try (InputStream in = getAssets().open("mods/" + name);
                     OutputStream out = new FileOutputStream(new File(outDir, name)))
                {
                    int count;
                    while ((count = in.read(buffer)) > 0) {
                        out.write(buffer, 0, count);
                    }
                }
            }
        } catch (IOException e) {
            Log.w(TAG, "Failed to extract bundled mods", e);
        }
    }

    private static void deleteRecursively(File file) {
        File[] children = file.listFiles();
        if (children != null) {
            for (File child : children) {
                deleteRecursively(child);
            }
        }
        file.delete();
    }

    @Override
    protected String[] getArguments() {
        String[] arguments = super.getArguments();
        if (arguments.length > 0) {
            return arguments;
        }

        Intent intent = getIntent();
        if (intent == null) {
            return arguments;
        }
        String[] argv = intent.getStringArrayExtra("dusk_argv");
        if (argv != null && argv.length > 0) {
            return argv;
        }
        String rawArgs = intent.getStringExtra("dusk_args");
        return rawArgs == null ? arguments : splitArguments(rawArgs.trim());
    }

}
