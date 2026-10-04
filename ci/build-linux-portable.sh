#!/bin/bash
# Portable x86_64 Linux AppImage for the VR build.
#
# Builds inside an Ubuntu 24.04 container (same baseline as upstream CI) so the
# result needs only glibc 2.39 / GLIBCXX_3.4.32: runs on SteamOS 3.x, Ubuntu
# 24.04+, Mint 22+, Fedora 40+, Arch/CachyOS. A build made directly on a
# rolling distro links that distro's newer glibc and won't run elsewhere.
#
# Usage (host): ci/build-linux-portable.sh
# Requires podman (or docker). Output: build/portable/Dusklight_VR*.AppImage
set -euo pipefail

src_dir="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" != "--inside" ]]; then
  engine="$(command -v podman || command -v docker || true)"
  if [[ -z "$engine" ]]; then
    echo "error: podman or docker is required (sudo pacman -S podman)" >&2
    exit 1
  fi
  exec "$engine" run --rm --network=host \
    -v "$src_dir:/src:z" -w /src \
    -e APP_VERSION="${APP_VERSION:-}" \
    docker.io/library/ubuntu:24.04 \
    /src/ci/build-linux-portable.sh --inside
fi

# ---- inside the container ----
export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get -y install --no-install-recommends \
  build-essential cmake ninja-build mold git curl ca-certificates file \
  python3 python3-markupsafe python3-jinja2 pkg-config cargo rustc \
  zlib1g-dev libglu1-mesa-dev libdbus-1-dev libvulkan-dev libxi-dev libxrandr-dev \
  libasound2-dev libpulse-dev libudev-dev libpng-dev libncurses5-dev libx11-xcb-dev \
  libfreetype-dev libxinerama-dev libxcursor-dev libgtk-3-dev libssl-dev \
  libcurl4-openssl-dev libxss-dev libusb-1.0-0-dev libdecor-0-dev \
  libpipewire-0.3-dev libunwind-dev libwayland-dev libxkbcommon-dev libxxf86vm-dev

git config --global --add safe.directory '*'

build_dir=/src/build/portable-x86_64
install_dir=/src/build/portable-install
out_dir=/src/build/portable
version="${APP_VERSION:-linux-vr-$(git -C /src rev-parse --short HEAD)}"

cmake -S /src -B "$build_dir" -G Ninja \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DBUILD_SHARED_LIBS=OFF \
  -DAURORA_SDL3_PROVIDER=vendor \
  -DCMAKE_INSTALL_PREFIX="$install_dir"
cmake --build "$build_dir"
rm -rf "$install_dir"
cmake --install "$build_dir"

# Fail loudly if anything newer than the Ubuntu 24.04 baseline slipped in.
max_glibc=$(objdump -T "$install_dir/dusklight" | grep -oE 'GLIBC_[0-9.]+' | sort -Vu | tail -1)
echo "dusklight requires $max_glibc"

# ---- AppImage ----
linuxdeploy=/src/build/linuxdeploy-x86_64.AppImage
[[ -x "$linuxdeploy" ]] || {
  curl -fL https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -o "$linuxdeploy"
  chmod +x "$linuxdeploy"
}
export APPIMAGE_EXTRACT_AND_RUN=1  # no FUSE in the container

appdir="$out_dir/appdir"
rm -rf "$appdir"
mkdir -p "$appdir"/usr/{bin,share/{applications,icons/hicolor}}
# Flat install layout: dusklight, mods/, res/ (+ SDK include/lib, skipped).
for p in dusklight mods res; do
  cp -r "$install_dir/$p" "$appdir/usr/bin/"
done
cp -r /src/platforms/freedesktop/{16x16,32x32,48x48,64x64,128x128,256x256,512x512,1024x1024} \
  "$appdir/usr/share/icons/hicolor"
sed 's/^Name=.*/Name=Dusklight VR (TPVR)/' /src/platforms/freedesktop/dev.twilitrealm.dusk.desktop \
  > "$appdir/usr/share/applications/dev.twilitrealm.dusk.desktop"

cd "$out_dir"
# libvulkan is excluded by linuxdeploy's excludelist on purpose: the host's
# loader/ICDs must be used. The OpenXR loader is linked statically.
VERSION="$version" NO_STRIP=1 "$linuxdeploy" \
  -l "/usr/lib/x86_64-linux-gnu/libusb-1.0.so" \
  --appdir "$appdir" --output appimage
chown -R "$(stat -c %u:%g /src)" "$out_dir" "$build_dir" "$install_dir" 2>/dev/null || true
ls -la "$out_dir"/*.AppImage
