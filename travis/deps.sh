#!/bin/bash -eux

case "$TRAVIS_OS_NAME" in
  linux)
    sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 1397BC53640DB551
    sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
    sudo add-apt-repository --yes ppa:beineri/opt-qt57-trusty
    sudo apt-get update -qq
    sudo apt-get install -qq g++-6 ninja-build gcovr lcov qt57-meta-minimal libasound2-dev

    sudo wget https://sourceforge.net/projects/boost/files/boost/1.62.0/boost_1_62_0.tar.bz2 -O /opt/boost.tar.bz2
    (cd /opt; sudo tar xaf boost.tar.bz2; sudo mv boost_* boost ; sudo chmod -R a+rwx boost)

    wget https://cmake.org/files/v3.7/cmake-3.7.0-Linux-x86_64.tar.gz -O cmake-linux.tgz
    tar xaf cmake-linux.tgz
    mv cmake-*-x86_64
    if [[ "x$TARGET" = "xmingw-w64" ]]; then

        wget https://dl.dropboxusercontent.com/u/3680458/mingw-w64-install.tar.gz
        wget https://dl.dropboxusercontent.com/u/3680458/mingw-w64-install.sha
        shasum -c mingw-w64-install.sha
        tar -xf mingw-w64-install.tar.gz

        # Download PureData for Windows
        wget http://msp.ucsd.edu/Software/pd-0.47-1.msw.zip -0 pd-msw.zip
        unzip -q pd-msw.zip
    fi
  ;;
  osx)
    # work around a homebrew bug
    set +e
    brew install gnu-tar xz
    ARCHIVE=homebrew-cache.tar.xz
    wget "https://github.com/OSSIA/iscore-sdk/releases/download/6.0-osx/$ARCHIVE" -O "$ARCHIVE"
    gtar xhzf "$ARCHIVE" --directory /usr/local/Cellar
    brew link --force boost cmake ninja qt5

    set -e
  ;;
esac
