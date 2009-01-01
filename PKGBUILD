pkgname=qtosd
pkgver=1.0
pkgrel=1
pkgdesc="Display various information in an OSD"
url="http://senfdax.de/~christof/programs/"
license="GPL-3"
depends=(qt alsa-lib)
makedepends=()
conflicts=()
replaces=()
backup=()
arch=('i686' 'x86_64')
install=
source=(http://senfdax.de/~christof/programs/$pkgname-$pkgver.tar.gz)
md5sums=(19a418752389660b73d70a39accb23c7)

build() {
  cd $startdir/src/$pkgname-$pkgver
  make || return 1
  make DESTDIR=$startdir/pkg install
}
