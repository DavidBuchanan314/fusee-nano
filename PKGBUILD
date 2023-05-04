# Maintainer: David Buchanan <d@vidbuchanan.co.uk>
pkgname=fusee-nano
pkgver=0.5.2
pkgrel=1
pkgdesc="A minimalist re-implementation of the Fusée Gelée exploit"
arch=('any')
url="https://github.com/DavidBuchanan314/fusee-nano"
license=('MIT')
groups=()
depends=()
makedepends=("vim") # used for xxd
checkdepends=()
optdepends=()
provides=("fusee-nano")
conflicts=("fusee-nano-git")
replaces=()
backup=()
options=()
install=
changelog=
source=("$pkgname-$pkgver.tar.gz::https://github.com/DavidBuchanan314/fusee-nano/archive/refs/tags/$pkgver.tar.gz")
noextract=()
sha256sums=("a58539e5f8073a36ac1bec799abdca6ac40d13ec26604c7aca0db7c8308e8194")

build() {
	cd "$pkgname-$pkgver"
	make
}

package() {
	cd "$pkgname-$pkgver"
	make install "PREFIX=${pkgdir}/usr/bin"
}
