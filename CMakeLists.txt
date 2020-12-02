cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)

cmake_policy(VERSION 3.8..3.13)

# support using rpath on Mac OS X
cmake_policy(SET CMP0042 NEW)

project(diamond)

set(BUILD_MAJOR "2")
set(BUILD_MINOR "0")
# To more cleanly support RPM packaging BUILD_MICRO needs to be non-zero
set(BUILD_MICRO "1")

file (READ "description.txt" DIAMOND_DESCRIPTION)

set(BUILD_COMPONENTS "diamond")

include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckTypeSize)

find_package(CopperSpice REQUIRED)

# file locations for installing
if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   include(GNUInstallDirs)

   # indicates where libraries are located relative to the executable
   set(CMAKE_INSTALL_RPATH "@executable_path/../Resources")

elseif(CMAKE_SYSTEM_NAME MATCHES "(Linux|OpenBSD|FreeBSD|NetBSD|DragonFly)")
   include(GNUInstallDirs)

   set(CMAKE_INSTALL_RPATH "\$ORIGIN")

   set(DIAMOND_INSTALLED_SIZE "150000")

   # this architecture value is only used on the control file for a debian package.
   # if the value is bogus or empty on other platforms it simply doesn't matter.
   # 
   EXECUTE_PROCESS( COMMAND dpkg --print-architecture  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE DIAMOND_ARCHITECTURE)
   

   message( STATUS "Architecture: ${DIAMOND_ARCHITECTURE}")

   # lots of warnings and all warnings as errors
   add_compile_options(-Wall -Wextra -pedantic -Werror)

elseif(MSVC)
   # use defaults

elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
   set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
   include(${CopperSpice_DIR}/InstallMinGW.cmake)

endif()

set(PACKAGE           "diamond")
set(PACKAGE_NAME      "Diamond")
set(PACKAGE_VERSION   "${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_MICRO}")
set(PACKAGE_STRING    "diamond ${BUILD_MAJOR}.${BUILD_MINOR}.${BUILD_MICRO}")
set(PACKAGE_TARNAME   "diamond")
set(PACKAGE_BUGREPORT "info@copperspice.com")
set(PACKAGE_URL       "https://www.copperspice.com/")

set(RPM_VERSION       "${BUILD_MAJOR}.${BUILD_MINOR}")
set(RPM_RELEASE       "${BUILD_MICRO}")

set(CPACK_PACKAGE_NAME    ${PROJECT_NAME} )
set(CPACK_PACKAGE_VENDOR  "CopperSpice")

set(CPACK_PACKAGE_VERSION_MAJOR ${BUILD_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${BUILD_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${BUILD_MICRO})

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross platform programmers editor")
set(CPACK_PACKAGE_DESCRIPTION_FILE    "${CMAKE_SOURCE_DIR}/README.md")

set(CPACK_PACKAGE_INSTALL_DIRECTORY   ${CMAKE_INSTALL_PREFIX})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY  OFF)

# mac osx only
set(CPACK_BUNDLE_NAME    Diamond)
set(CPACK_BUNDLE_ICON    "${CMAKE_SOURCE_DIR}/resources/diamond.icns")
set(CPACK_BUNDLE_PLIST   "${CMAKE_SOURCE_DIR}/resources/Info.plist")

include(CPack)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DQT_NO_MULTIMEDIA)

if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,-undefined,error")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined,error")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-undefined,error")
else()
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--no-undefined")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
endif()

string(TIMESTAMP BUILD_DATE "%m/%d/%Y")
add_definitions(-DBUILD_DATE="${BUILD_DATE}")

configure_file(
   ${CMAKE_SOURCE_DIR}/src/diamond_build_info.h.in
   src/diamond_build_info.h
)

if(CMAKE_SYSTEM_NAME MATCHES "(Linux|OpenBSD|FreeBSD|NetBSD|DragonFly)")
    #  Update files needed for Debian
    #  Note: You need to update DIAMOND_INSTALLED_SIZE to reflect current installed size
    #
    configure_file(
       ${CMAKE_SOURCE_DIR}/deb_build.etc/control.in
       deb_build.etc/control
    )
    
    configure_file(
       ${CMAKE_SOURCE_DIR}/diamond.spec.in
       diamond.spec
    )
    
    configure_file(
       ${CMAKE_SOURCE_DIR}/diamond.desktop.in
       diamond.desktop
    )

    #  postrm has some heavy syntax - just copy
    configure_file(
       ${CMAKE_SOURCE_DIR}/deb_build.etc/postrm.in
       deb_build.etc/postrm
       COPYONLY
    )

    #  postinst has some heavy syntax - just copy
    configure_file(
       ${CMAKE_SOURCE_DIR}/deb_build.etc/postinst.in
       deb_build.etc/postinst
       COPYONLY
    )
    
    #  hopefully we never need to replace values in rules
    configure_file(
       ${CMAKE_SOURCE_DIR}/deb_build.etc/rules.in
       deb_build.etc/rules
       COPYONLY
    )

endif()


# location for building binary files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)


if(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
   set(TARGETBITS 32)
else()
   set(TARGETBITS 64)
endif()

message("")
message("Diamond configured to run on:  ${CMAKE_SYSTEM_NAME} ${TARGETBITS} bit, ${CMAKE_BUILD_TYPE} Mode")
message("Diamond will be built in:      ${CMAKE_BINARY_DIR}")
message("Diamond will be installed in:  ${CMAKE_INSTALL_PREFIX}")
message("\n")
