#!/bin/bash
# ══════════════════════════════════════════════════════════════════
#  HMS Nexus Pro — Build & Package Script
#  Creates a distributable .deb installer
#  Usage: chmod +x build_package.sh && ./build_package.sh
# ══════════════════════════════════════════════════════════════════
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_DIR/build"
PKG_DIR="$PROJECT_DIR/deb-package"
OUTPUT_DIR="$PROJECT_DIR/dist"
BINARY_NAME="HMSNexusPro"
PKG_NAME="hmsnexuspro"
VERSION="2.0.0"

echo "╔══════════════════════════════════════════════════════╗"
echo "║       HMS Nexus Pro — Build & Package                ║"
echo "╚══════════════════════════════════════════════════════╝"
echo ""

# ── Step 1: Build ──────────────────────────────────────────────
echo "▶  Step 1/4: Building with CMake (Release mode)..."
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j$(nproc)
echo "   ✅ Build complete: $BUILD_DIR/$BINARY_NAME"
echo ""

# ── Step 2: Copy binary into package tree ─────────────────────
echo "▶  Step 2/4: Preparing package structure..."
cp "$BUILD_DIR/$BINARY_NAME" "$PKG_DIR/usr/bin/hmsnexuspro"
chmod 755 "$PKG_DIR/usr/bin/hmsnexuspro"
chmod 755 "$PKG_DIR/DEBIAN/postinst"
chmod 755 "$PKG_DIR/DEBIAN/postrm"

# Generate a simple SVG icon (hospital cross) if no PNG available
if [ ! -f "$PKG_DIR/usr/share/pixmaps/hmsnexuspro.png" ]; then
    echo "   ℹ️  No icon found — generating default SVG icon..."
    cat > "$PKG_DIR/usr/share/pixmaps/hmsnexuspro.svg" << 'SVGEOF'
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
  <rect width="64" height="64" rx="12" fill="#0f172a"/>
  <rect x="27" y="12" width="10" height="40" rx="3" fill="#06b6d4"/>
  <rect x="12" y="27" width="40" height="10" rx="3" fill="#06b6d4"/>
</svg>
SVGEOF
fi

echo "   ✅ Package tree ready"
echo ""

# ── Step 3: Build .deb ────────────────────────────────────────
echo "▶  Step 3/4: Building .deb package..."
mkdir -p "$OUTPUT_DIR"
DEB_FILE="$OUTPUT_DIR/${PKG_NAME}_${VERSION}_amd64.deb"
dpkg-deb --build --root-owner-group "$PKG_DIR" "$DEB_FILE"
echo "   ✅ Package built: $DEB_FILE"
echo ""

# ── Step 4: Verify ────────────────────────────────────────────
echo "▶  Step 4/4: Verifying package..."
dpkg-deb --info "$DEB_FILE"
echo ""
echo "════════════════════════════════════════════════════════"
echo "  ✅  SUCCESS!"
echo ""
echo "  Installer:  $DEB_FILE"
echo "  Size:       $(du -h "$DEB_FILE" | cut -f1)"
echo ""
echo "  ── To install on this machine ──"
echo "  sudo dpkg -i $DEB_FILE"
echo "  sudo apt-get install -f   # fix any missing deps"
echo ""
echo "  ── To share with others ──"
echo "  Copy  $(basename "$DEB_FILE")  to their Ubuntu/Debian machine"
echo "  They run: sudo dpkg -i $(basename "$DEB_FILE")"
echo ""
echo "  ── To uninstall ──"
echo "  sudo apt remove hmsnexuspro"
echo "════════════════════════════════════════════════════════"
