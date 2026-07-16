Maintainer: Cristopher "borjascristopher88@gmail.com.com" (mailto: borjascristopher88@gmail.com)

pkgname=doom-demo
pkgver=1.0.0
pkgrel=1
pkgdesc="Doom Demo - Multi-Platform Edition (Linux build from source)"
arch=('x86_64')
url="https://github.com/CRISTOP-bot/doom-demo"
license=('MIT')
depends=(
'electron'
'nodejs'
'npm'
'pnpm'
'gcc'
'make'
'cmake'
'glib2'
'gtk3'
'libx11'
'libxrandr'
'libxinerama'
'libxcursor'
'libxi'
'nss'
'alsa-lib'
'libpulse'
'libglvnd'
'mesa'
)
makedepends=(
'git'
'pnpm'
'node-gyp'
)
optdepends=(
'xdg-utils: open links from the app'
)
provides=('doom-demo')
conflicts=('doom-demo-git')
source=(
"$pkgname::git+https://github.com/CRISTOP-bot/doom-demo.git#tag=v$pkgver"
)
sha256sums=('SKIP')

prepare() {
cd "$srcdir/$pkgname"

Limpia posibles builds previos

if [[ -d node_modules ]]; then
rm -rf node_modules
fi

Si tu repo usa lockfile, pnpm lo respetará.

Si necesitas versión exacta de pnpm, puedes fijarla aquí.

corepack enable >/dev/null 2>&1 || true
}

build() {
cd "$srcdir/$pkgname/ports/doom-linux"

Instalar dependencias JS

pnpm install --frozen-lockfile

Compilar frontend React

pnpm build:react

Compilar módulos nativos

pnpm build:native

Compilar Electron / empaquetado

pnpm build:electron
}

package() {
cd "$srcdir/$pkgname/ports/doom-linux"

install -Dm755 "$srcdir/$pkgname/ports/doom-linux/dist/doom-demo" 
"$pkgdir/usr/bin/doom-demo"

Si tu build genera un AppImage u otros binarios, instala aquí:

if [[ -f "$srcdir/$pkgname/releases/Doom-1.0.0.AppImage" ]]; then
install -Dm755 "$srcdir/$pkgname/releases/Doom-1.0.0.AppImage" 
"$pkgdir/usr/lib/doom-demo/Doom-1.0.0.AppImage"
ln -s /usr/lib/doom-demo/Doom-1.0.0.AppImage 
"$pkgdir/usr/bin/doom-demo-appimage"
fi

Desktop file

if [[ -f "$srcdir/$pkgname/ports/doom-linux/assets/doom-demo.desktop" ]]; then
install -Dm644 "$srcdir/$pkgname/ports/doom-linux/assets/doom-demo.desktop" 
"$pkgdir/usr/share/applications/doom-demo.desktop"
fi

Iconos

if [[ -d "$srcdir/$pkgname/ports/doom-linux/assets/icons" ]]; then
find "$srcdir/$pkgname/ports/doom-linux/assets/icons" -type f -name '.png' -o -name '.svg' -o -name '*.xpm' | while read -r icon; do
size_dir="$(basename "$(dirname "$icon")")"
icon_name="$(basename "$icon")"
install -Dm644 "$icon" "$pkgdir/usr/share/icons/hicolor/$size_dir/apps/$icon_name"
done
fi

Documentación

install -Dm644 "$srcdir/$pkgname/README.md" 
"$pkgdir/usr/share/doc/$pkgname/README.md"

for doc in COMPREHENSIVE_CODE_REVIEW.md ERROR_ANALYSIS.md BUG_FIXES.md IMPROVEMENTS_ROADMAP.md; do
[[ -f "$srcdir/$pkgname/$doc" ]] && install -Dm644 "$srcdir/$pkgname/$doc" 
"$pkgdir/usr/share/doc/$pkgname/$doc"
done

Licencia

if [[ -f "$srcdir/$pkgname/LICENSE" ]]; then
install -Dm644 "$srcdir/$pkgname/LICENSE" 
"$pkgdir/usr/share/licenses/$pkgname/LICENSE"
fi
}

Opcional, pero útil para paquetes Electron

options=('!strip')
