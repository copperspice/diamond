# Script to build a debian package for diamond
#
set -e

if [ ! -f "/etc/debian_version" ]; then
   if [ ! "$(grep -i 'ID_LIKE=debian' /etc/*release)" ]; then
      echo "This script can only be run on a Debian based distribution"
      exit 1
   fi
fi

echo "MUST BE RUN FROM ROOT OF PROJECT DIRECTORY TREE"
echo ""
echo "This script ASSUMES it can create or use diamond_debian_build and diamond_debian_release"
echo "directories one level up from where this script is being run. If they"
echo "exist they will be deleted and recreated."
echo ""
echo "Script also ASSUMES you are running from the root of the Git directory"
echo "where all source is in a directory named src at the same level as this file."
echo ""
echo "You must have ninja, astyle, and a valid build environment. Diamond will be built"
echo "from source and installed in diamond_debian_release. We will then create a"
echo "DEBIAN directory to assemble all files needed for creation of a .deb."
echo ""
echo ""
echo "NOTE: this prefix path expected to be valid: /usr/lib/cs_lib/lib/cmake/CopperSpice"
echo "      CopperSpice library is expected to be in /usr/lib/cs_lib"
echo ""

#  Step 1 : pretty up code
#
echo "*** Styling source"
astyle --quiet -n src/*.cpp src/*.h

# Cleanup backup files
#
echo "Cleanup backup files"
find . -type f -iname '*~' -exec rm -f {} \;

#  Step 2 : Establish fresh clean directories
#
echo "*** Establishing fresh directories"
SCRIPT_DIR="$PWD"
BUILD_DIR="$SCRIPT_DIR/../diamond_debian_build"
RELEASE_DIR="$SCRIPT_DIR/../diamond_debian_release"
DEBIAN_DIR="$SCRIPT_DIR/../diamond_debian"
DEBIAN_WORK_DIR="$SCRIPT_DIR/../diamond_debian_work"

echo "SCRIPT_DIR  $SCRIPT_DIR"
echo "BUILD_DIR   $BUILD_DIR"
echo "RELEASE_DIR $RELEASE_DIR"
echo "DEBIAN_DIR  $DEBIAN_DIR"


#  nuke the directories we will use if they already exist
#
if [ -d "$BUILD_DIR" ]; then
  rm -rf "$BUILD_DIR"
fi

if [ -d "$RELEASE_DIR" ]; then
  rm -rf "$RELEASE_DIR"
fi

if [ -d "$DEBIAN_DIR" ]; then
  rm -rf "$DEBIAN_DIR"
fi

#  Because of the way diamond currently builds and is designed
#  I'm going to violate Debian convention and thump this into a single directory
#  under opt.
#
#  Properly packaged the binary would be in /usr/local/bin
#  the CopperSpice libraries would be under /usr/lib (provided by OS)
#  the support sub-directories of dictionary, platforms, printerdrivers, and syntax
#  would all be under /usr/local/share/diamond
#
#  If one was properly packaging as a maintainer for a distro the directories would be
#  /usr/bin
#  /usr/lib
#  /usr/share/diamond
#
#  I have to change too much to fix that right now.
#
#  NOTE: This means that Diamond will always use the CopperSpice libraries in
#        its directory even if newer, better ones are installed for systemwide use.
#
#  create the directories we will use so they are fresh and clean
#
mkdir -p "$BUILD_DIR"
mkdir -p "$RELEASE_DIR"
mkdir -p "$DEBIAN_DIR"/DEBIAN
mkdir -p "$DEBIAN_DIR"/opt/diamond/dictionary
mkdir -p "$DEBIAN_DIR"/opt/diamond/platforms
mkdir -p "$DEBIAN_DIR"/opt/diamond/printerdrivers
mkdir -p "$DEBIAN_DIR"/opt/diamond/syntax
mkdir -p "$DEBIAN_DIR"/usr/share/doc/diamond
mkdir -p "$DEBIAN_DIR"/usr/share/applications
mkdir -p "$DEBIAN_DIR"/usr/share/pixmaps
mkdir -p "$DEBIAN_DIR"/usr/share/menu

#  save working directory just in case
#
pushd `pwd`

#  Step 3 : Prepare build directory
#
echo "*** Prepping build directory"
cd "$BUILD_DIR"
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$RELEASE_DIR" -DCMAKE_PREFIX_PATH=/usr/lib/cs_lib/lib/cmake/CopperSpice "$SCRIPT_DIR"

#  Step 4: Actually build Diamond
#
echo "*** Building Diamond"
ninja install

#  Step 5 : Move files to the debian tree
#
echo "*** Copying files to Debian tree"
# deb_build.etc/preinst deb_build.etc/postinst deb_build.etc/prerm
cp -f "$BUILD_DIR"/deb_build.etc/control "$BUILD_DIR"/deb_build.etc/postrm "$BUILD_DIR"/deb_build.etc/postinst "$DEBIAN_DIR"/DEBIAN
#
# Files in this directory need to be marked executable.
#
chmod 0664 "$DEBIAN_DIR"/DEBIAN/*
chmod +x "$DEBIAN_DIR"/DEBIAN/postrm
chmod +x "$DEBIAN_DIR"/DEBIAN/postinst

#  Note: If you want changelog to actually have anything in it, you need to create one
#        I put a placeholder in the project for now because I didn't want to
#        saddle this build script with git-buildpackage dependencies
#
cp "$SCRIPT_DIR"/LICENSE "$DEBIAN_DIR"/usr/share/doc/diamond
cp "$SCRIPT_DIR"/changelog "$DEBIAN_DIR"/usr/share/doc/diamond/changelog.Debian
cp "$SCRIPT_DIR"/menu/diamond "$DEBIAN_DIR"/usr/share/menu

cp "$RELEASE_DIR"/diamond.desktop "$DEBIAN_DIR"/usr/share/applications
cp "$RELEASE_DIR"/dictionary/* "$DEBIAN_DIR"/opt/diamond/dictionary
cp "$RELEASE_DIR"/platforms/* "$DEBIAN_DIR"/opt/diamond/platforms
cp "$RELEASE_DIR"/printerdrivers/* "$DEBIAN_DIR"/opt/diamond/printerdrivers
cp "$RELEASE_DIR"/syntax/* "$DEBIAN_DIR"/opt/diamond/syntax
cp "$RELEASE_DIR"/diamond "$DEBIAN_DIR"/opt/diamond
cp "$RELEASE_DIR"/lib*.so "$DEBIAN_DIR"/opt/diamond
cp "$RELEASE_DIR"/diamond.png "$DEBIAN_DIR"/usr/share/pixmaps

chmod 0664 "$DEBIAN_DIR"/usr/share/doc/diamond/*

gzip --best "$DEBIAN_DIR"/usr/share/doc/diamond/changelog*

#  Step 6 : generate md5sum
#
echo "Generating md5sums"
cd "$DEBIAN_DIR"
md5sum usr/share/doc/diamond/* 2>/dev/null >DEBIAN/md5sums
# don't currently have any pre files
# DEBIAN/pre*
#
chmod go-w DEBIAN/md5sums DEBIAN/post* usr usr/share usr/share/doc usr/share/doc/diamond

# Step 7 : build .deb
#
cd ..
D_DIR="$PWD"
echo "Building .deb IN $D_DIR"
fakeroot dpkg-deb -Zgzip --build "$DEBIAN_DIR"

# Step 8 : rename .deb
#          file will be named diamond_debian.deb and should just be diamond-version-architecture.deb
#
D_VERSION=$(grep -i "Version:" "$DEBIAN_DIR"/DEBIAN/control | cut -d' ' -f2)
D_ARCH=$(grep -i "Architecture:" "$DEBIAN_DIR"/DEBIAN/control | cut -d' ' -f2)
DEB_NAME="diamond-$D_VERSION-$D_ARCH.deb"
echo "look for:  $DEB_NAME"

mv diamond_debian.deb "$DEB_NAME"

#  restore working directory
#
popd

set -e

exit 0
