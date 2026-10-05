#include "data.hpp"

#include "dusk/app_info.hpp"

#include <borealis/io.hpp>
#include <borealis/log.hpp>

#include <string>

#if defined(__ANDROID__)
#include <SDL3/SDL_system.h>
#include <jni.h>
#endif

namespace dusk::data {
namespace {

constexpr borealis::Log Log{"dusk::data"};

std::string status_message(const borealis::data::Status& status) {
    using enum borealis::data::ErrorCode;

    std::string message;
    switch (status.code) {
    case None:
        return {};
    case NotInitialized:
        message = "Data folders have not been initialized.";
        break;
    case PreferencePathUnavailable:
        message = "The system data folder is unavailable.";
        break;
    case EmptyPath:
        message = "Choose a folder.";
        break;
    case CreateDirectoryFailed:
        message = "The selected folder could not be created.";
        break;
    case NotDirectory:
        message = "The selected path is not a folder.";
        break;
    case WriteProbeFailed:
        message = "The selected folder is not writable.";
        break;
    case WriteProbeCleanupFailed:
        message = "The selected folder could not be validated.";
        break;
    case DescriptorWriteFailed:
        message = "Dusklight could not save the data folder setting.";
        break;
    case MigrationIncomplete:
        message = "Data migration could not be completed.";
        break;
    case OverrideActive:
        message = "The data folder is fixed by --user-dir for this session.";
        break;
    case Unsupported:
        message = "Changing the data folder is not supported on this platform.";
        break;
    case OpenFolderFailed:
        message = "The data folder could not be opened.";
        break;
    }

    if (!status.path.empty()) {
        message += " (" + borealis::io::fs_path_to_string(status.path) + ")";
    }
    if (status.systemError) {
        message += ": " + status.systemError.message();
    }
    return message;
}

bool operation_succeeded(const borealis::data::Status& status, std::string* errorOut = nullptr) {
    if (status) {
        return true;
    }
    const auto message = status_message(status);
    if (errorOut != nullptr) {
        *errorOut = message;
    }
    Log.warn("{}", message);
    return false;
}

}  // namespace

borealis::data::Manager& manager() {
    static borealis::data::Manager instance{
        AppInfo,
        {
            .defaultPath = {.useDocumentsOnIOS = true},
            .portableRelativePath = "data",
            .legacyApps =
                {
                    {.orgName = "TwilitRealm", .appName = "Dusk"},
                },
            .migration =
                {
                    .directories =
                        {
                            "texture_replacements",
                            "USA",
                            "EUR",
                            "JAP",
                            "MemoryCardA.USA.raw.mods",
                            "MemoryCardA.EUR.raw.mods",
                            "MemoryCardA.JAP.raw.mods",
                            "MemoryCardB.USA.raw.mods",
                            "MemoryCardB.EUR.raw.mods",
                            "MemoryCardB.JAP.raw.mods",
                        },
                    .files =
                        {
                            "achievements.json",
                            "config.json",
                            "controller_ports.dat",
                            "gamecontrollerdb.txt",
                            "imgui.ini",
                            "keyboard_bindings.dat",
                            "states.json",
                        },
                    .extensions = {".controller", ".gci"},
                    .filenamePatterns =
                        {
                            {.prefix = "MemoryCard", .suffix = ".raw"},
                        },
                },
        },
    };
    return instance;
}

#if defined(__ANDROID__)
// The default data path on Android is app-internal storage, which (along with borealis's location
// descriptor) is wiped whenever the app is uninstalled, taking the user's saves with it. When
// DuskActivity has all-files access it returns a shared-storage folder, and a default-mode install
// is switched onto it so saves outlive the app. Migration never overwrites existing files, so after
// a reinstall the saves already in the shared folder win over the fresh internal ones.
namespace {

std::filesystem::path await_shared_data_dir() {
    auto* env = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
    auto activity = static_cast<jobject>(SDL_GetAndroidActivity());
    if (env == nullptr || activity == nullptr) {
        return {};
    }
    std::filesystem::path result;
    jclass activityClass = env->GetObjectClass(activity);
    jmethodID method =
        env->GetMethodID(activityClass, "awaitSharedDataDir", "()Ljava/lang/String;");
    if (method == nullptr) {
        env->ExceptionClear();
    } else if (auto path = static_cast<jstring>(env->CallObjectMethod(activity, method))) {
        if (const char* chars = env->GetStringUTFChars(path, nullptr)) {
            result = borealis::io::fs_path_from_utf8(chars);
            env->ReleaseStringUTFChars(path, chars);
        }
        env->DeleteLocalRef(path);
    } else if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
    return result;
}

bool switch_to_shared_storage(const std::filesystem::path& userDirectoryOverride) {
    if (!userDirectoryOverride.empty() ||
        manager().configured_mode() != borealis::data::LocationMode::Default)
    {
        return false;
    }
    const auto sharedPath = await_shared_data_dir();
    if (sharedPath.empty() || !operation_succeeded(manager().set_custom_data_path(sharedPath))) {
        return false;
    }
    Log.info("Moving data to shared storage: {}", borealis::io::fs_path_to_string(sharedPath));
    return true;
}

}  // namespace
#endif

Paths initialize_data(const std::filesystem::path& userDirectoryOverride) {
    auto status = manager().initialize(userDirectoryOverride);
#if defined(__ANDROID__)
    if ((status || status.code == borealis::data::ErrorCode::MigrationIncomplete) &&
        switch_to_shared_storage(userDirectoryOverride))
    {
        // Re-initialize to pick up the new descriptor and migrate now rather than next launch.
        status = manager().initialize(userDirectoryOverride);
    }
#endif
    if (!status && status.code != borealis::data::ErrorCode::MigrationIncomplete) {
        Log.fatal("Failed to initialize data folders: {}", status_message(status));
    }
    if (!status) {
        Log.warn("{} Migration will be retried on the next launch.", status_message(status));
    }
    return manager().paths();
}

std::filesystem::path base_path_relative(const std::filesystem::path& path) {
    return manager().base_path_relative(path);
}

std::filesystem::path configured_data_path() {
    return manager().configured_data_path();
}

std::filesystem::path cache_path() {
    return manager().paths().cachePath;
}

bool open_data_path() {
    return operation_succeeded(manager().open_active_data_path());
}

bool set_custom_data_path(const std::filesystem::path& path, std::string* errorOut) {
    return operation_succeeded(manager().set_custom_data_path(path), errorOut);
}

bool set_custom_data_path(const char* path, std::string* errorOut) {
    if (path == nullptr) {
        if (errorOut != nullptr) {
            *errorOut = "Choose a folder.";
        }
        return false;
    }
    return set_custom_data_path(borealis::io::fs_path_from_utf8(path), errorOut);
}

bool set_portable_data_path() {
    return operation_succeeded(manager().set_portable_data_path());
}

bool reset_data_path() {
    return operation_succeeded(manager().reset_data_path());
}

bool is_default_data_path() {
    return manager().is_default_data_path();
}

bool is_data_path_restart_pending() {
    return manager().is_data_path_restart_pending();
}

std::filesystem::path user_home_path() {
    return manager().user_home_path();
}

std::filesystem::path normalized_display_path(const std::filesystem::path& path) {
    return manager().normalized_display_path(path);
}

std::string abbreviated_path_string(const std::filesystem::path& path) {
    return manager().abbreviated_path_string(path);
}

}  // namespace dusk::data
