# Script to build a rpm package for diamond
#
set -e

if [ ! -d "/etc/rpmdevtools" ]; then
    if [ ! "$(grep -i 'REDHAT' /etc/*release)" ]; then
        echo "This script can only be run on REDHAT based distribution with rpmdevtools installed"
        exit 1
    fi
fi

echo "MUST BE RUN FROM ROOT OF PROJECT DIRECTORY TREE"
echo ""
echo "YOU MUST HAVE RUN rpmdev-setuptree one time from the command line prior to running this."
echo "that creates a permanent $HOME/rpmbuild directory tree. You only ever need to run"
echo "that command once."
echo ""
echo "This script ASSUMES it can create and use diamond_rpm_build and diamond_rpm_release"
echo "directories one level up from where this script is being run. If directories"
echo "exist they will be deleted and recreated. The diamond_rpm_release directory exists"
echo "only for a build target in case someone executes a 'ninja install' command from the "
echo "build directory. The only reason the build directory gets created is so cmake can "
echo "populate variable values in the diamond.spec file."
echo ""
echo "Script also ASSUMES you are running from the root of the Git project directory"
echo "where all source is in a directory named src at the same level as this file."
echo ""
echo "You must have ninja, astyle, and a valid build environment. "
echo ""
echo "After creating fresh directory and styling the source this script will run the cmake"
echo "command to populate the diamond_rpm_build directory. After that it will use the "
echo "rpmbuild command to actually create the RPM package."
echo ""
echo "NOTE: this prefix path expected to be valid: /usr/lib/cs_lib/lib64/cmake/CopperSpice"
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
BUILD_DIR="$SCRIPT_DIR/../diamond_rpm_build"
RELEASE_DIR="$SCRIPT_DIR/../diamond_rpm_release"

echo "SCRIPT_DIR  $SCRIPT_DIR"
echo "BUILD_DIR   $BUILD_DIR"
echo "RELEASE_DIR $RELEASE_DIR"

#  nuke the directories we will use if they already exist
#
if [ -d "$BUILD_DIR" ]; then
  rm -rf "$BUILD_DIR"
fi

if [ -d "$RELEASE_DIR" ]; then
  rm -rf "$RELEASE_DIR"
fi

#  Because of the way diamond currently builds and is designed
#  I'm going to violate RPM convention and thump this into a single directory
#  under opt.
#
#  Properly packaged the binary would be in /usr/local/bin
#  the CopperSpice libraries would be under /usr/local/lib64
#  the support sub-directories of dictionary, platforms, printerdrivers, and syntax
#  would all be under /usr/local/share/diamond
#
#  If one was properly packaging as a maintainer for a distro the directories would be
#  /usr/bin
#  /usr/lib64
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

#  save working directory just in case
#
pushd `pwd`

#  Step 3 : Prepare build directory
#           We do this just to get diamond.spec with all of the CMake variables filled in.
#
echo "*** Prepping build directory"
cd "$BUILD_DIR"
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$RELEASE_DIR" -DCMAKE_PREFIX_PATH=/usr/lib/cs_lib/lib64/cmake/CopperSpice "$SCRIPT_DIR"

#  Step 4 : build diamond and generate the RPM
#
rpmbuild -ba diamond.spec

echo ""
echo "If this completed without errors you will find the RPM in {$HOME}/rpmbuild/RPMS"
echo ""

popd

set -e

exit 0

