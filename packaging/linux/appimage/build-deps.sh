#!/bin/bash
#
# Build all KMyMoney's dependencies on Ubuntu 14.04.
#
# Prerequisites: cmake git build-essential libxcb-keysyms1-dev plus all deps for Qt5
#

# Halt on errors and be verbose about what we are doing
set -e
set -x

# Read in our parameters
export BUILD_PREFIX=$1
export KMYMONEY_SOURCES=$2

# qjsonparser, used to add metadata to the plugins needs to work in a en_US.UTF-8 environment.
# That's not always the case, so make sure it is
export LC_ALL=en_US.UTF-8
export LANG=en_us.UTF-8

# We want to use $prefix/deps/usr/ for all our dependencies
export DEPS_INSTALL_PREFIX=$BUILD_PREFIX/deps/usr/
export DOWNLOADS_DIR=$BUILD_PREFIX/downloads/

# Setup variables needed to help everything find what we build
export LD_LIBRARY_PATH=$DEPS_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DEPS_INSTALL_PREFIX/bin:$PATH
export PKG_CONFIG_PATH=$DEPS_INSTALL_PREFIX/share/pkgconfig:$DEPS_INSTALL_PREFIX/lib/pkgconfig:/usr/lib/pkgconfig:$PKG_CONFIG_PATH
export CMAKE_PREFIX_PATH=$DEPS_INSTALL_PREFIX:$CMAKE_PREFIX_PATH

# A kmymoney build layout looks like this:
# kmymoney/ -- the source directory
# downloads/ -- downloads of the dependencies from files.kde.org
# deps-build/ -- build directory for the dependencies
# deps/ -- the location for the built dependencies
# build/ -- build directory for kmymoney itself
# kmymoney.appdir/ -- install directory for kmymoney and the dependencies

# Make sure our downloads directory exists
if [ ! -d $DOWNLOADS_DIR ] ; then
    mkdir -p $DOWNLOADS_DIR
fi

# Make sure our build directory exists
if [ ! -d $BUILD_PREFIX/deps-build/ ] ; then
    mkdir -p $BUILD_PREFIX/deps-build/
fi

# The 3rdparty dependency handling in KMyMoney also requires the install directory to be pre-created
if [ ! -d $DEPS_INSTALL_PREFIX ] ; then
    mkdir -p $DEPS_INSTALL_PREFIX
fi

# Switch to our build directory as we're basically ready to start building...
cd $BUILD_PREFIX/deps-build/

# Configure the dependencies for building
cmake $KMYMONEY_SOURCES/3rdparty -DCMAKE_INSTALL_PREFIX=$DEPS_INSTALL_PREFIX -DINSTALL_ROOT=$DEPS_INSTALL_PREFIX -DEXTERNALS_DOWNLOAD_DIR=$DOWNLOADS_DIR

# Now start building everything we need, in the appropriate order
cmake --build . --config RelWithDebInfo --target ext_iconv
cmake --build . --config RelWithDebInfo --target ext_gettext
cmake --build . --config RelWithDebInfo --target ext_zlib
cmake --build . --config RelWithDebInfo --target ext_boost
cmake --build . --config RelWithDebInfo --target ext_png
cmake --build . --config RelWithDebInfo --target ext_icu
cmake --build . --config RelWithDebInfo --target ext_xml
cmake --build . --config RelWithDebInfo --target ext_xslt
cmake --build . --config RelWithDebInfo --target ext_lzma
# cmake --build . --config RelWithDebInfo --target ext_fontconfig
# cmake --build . --config RelWithDebInfo --target ext_freetype
cmake --build . --config RelWithDebInfo --target ext_qt
cmake --build . --config RelWithDebInfo --target ext_gmp
cmake --build . --config RelWithDebInfo --target ext_alkimia
cmake --build . --config RelWithDebInfo --target ext_kdiagram