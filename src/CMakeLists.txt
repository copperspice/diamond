list(APPEND DIAMOND_INCLUDES
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_advfind.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_buffer.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_colors.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_config.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_find.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_fonts.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_getline.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_macro.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_open.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_options.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_preset.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_print_opt.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_replace.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_symbols.h
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_xp_getdir.h

   ${CMAKE_CURRENT_SOURCE_DIR}/diamond_edit.h
   ${CMAKE_CURRENT_SOURCE_DIR}/keylineedit.h
   ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
   ${CMAKE_CURRENT_SOURCE_DIR}/search.h
   ${CMAKE_CURRENT_SOURCE_DIR}/spellcheck.h
   ${CMAKE_CURRENT_SOURCE_DIR}/syntax.h
   ${CMAKE_CURRENT_SOURCE_DIR}/util.h

   ${CMAKE_CURRENT_SOURCE_DIR}/diamond_build_info.h
)

list(APPEND DIAMOND_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/about.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_advfind.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_buffer.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_colors.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_config.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_find.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_fonts.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_getline.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_macro.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_open.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_options.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_preset.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_print_opt.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_replace.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_symbols.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/dialog_xp_getdir.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/diamond_edit.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/json.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/keylineedit.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/menu_action.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/options.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/print.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/recent_files.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/recent_tabs.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/search.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/spell.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/spellcheck.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/split_window.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/syntax.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/support.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/util.cpp

   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_advfind.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_buffer.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_colors.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_config.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_find.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_fonts.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_getline.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_macro.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_open.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_options.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_preset.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_print_opt.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_replace.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_symbols.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_xp_getdir.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/mainwindow.ui

   qrc_diamond.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/../diamond.rc
)

# run rcc to generate qrc output
COPPERSPICE_RESOURCES(
   ${CMAKE_CURRENT_SOURCE_DIR}/../diamond.qrc
)

# run uic to generate source
COPPERSPICE_RESOURCES(
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_advfind.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_buffer.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_colors.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_config.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_find.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_fonts.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_getline.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_macro.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_open.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_options.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_preset.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_print_opt.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_replace.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_symbols.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/dialog_xp_getdir.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/../forms/mainwindow.ui
)

add_executable(Diamond ${DIAMOND_SOURCES})

target_include_directories(Diamond
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(Diamond
   CopperSpice::CsCore
   CopperSpice::CsGui
   CopperSpice::CsNetwork
)

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   set_target_properties(Diamond PROPERTIES OUTPUT_NAME diamond)

   find_package(PkgConfig)
   pkg_check_modules(Hunspell IMPORTED_TARGET hunspell)

   find_library(hunspell_lib_A
      NAMES ${Hunspell_LIBRARIES}
      HINTS ${Hunspell_LIBRARY_DIRS}
   )

   get_filename_component(hunspell_lib_B
      ${hunspell_lib_A} REALPATH
   )

   get_filename_component(hunspell_install_name
      ${hunspell_lib_A} NAME
   )

   target_link_libraries(Diamond
      PkgConfig::Hunspell
   )

   execute_process(COMMAND otool -X -D ${hunspell_lib_A} OUTPUT_VARIABLE hunspell_dylib_name)
   string(STRIP "${hunspell_dylib_name}" hunspell_dylib_name)

   install(FILES ${hunspell_lib_A}  DESTINATION .)
   install(FILES ${hunspell_lib_B}  DESTINATION .)

   install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/diamond.icns  DESTINATION .)

   # correct the hunspell path
   add_custom_command(TARGET Diamond
      POST_BUILD COMMAND
      ${CMAKE_INSTALL_NAME_TOOL} -change ${hunspell_dylib_name} "@rpath/${hunspell_install_name}"
      $<TARGET_FILE:Diamond>
   )

elseif(CMAKE_SYSTEM_NAME MATCHES "(Linux|OpenBSD|FreeBSD|NetBSD|DragonFly)")
   set_target_properties(Diamond PROPERTIES OUTPUT_NAME diamond)

   find_package(PkgConfig)
   pkg_check_modules(Hunspell IMPORTED_TARGET hunspell)

   target_link_libraries(Diamond
      PkgConfig::Hunspell
   )

   install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/diamond.png  DESTINATION .)

elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")

   # path to hunspell
   get_filename_component(hunspell_path ../hunspell_lib ABSOLUTE)

   set(hunspell_dll  ${hunspell_path}/bin/libhunspell-1.7-0.dll)
   set(hunspell_a    ${hunspell_path}/lib/libhunspell-1.7.dll.a)

   target_include_directories(Diamond
      PRIVATE
      ${hunspell_path}/include
   )

   target_link_libraries(Diamond
      netapi32
      mpr
      ${hunspell_a}
   )

   if (MSVC)
      # disables the console window
      target_link_options(Diamond
         PRIVATE
         /subsystem:windows
         /entry:mainCRTStartup
      )

   else()
      # disables the console window
      target_link_libraries(Diamond
         -mwindows
      )

   endif()

   install(FILES ${hunspell_dll}  DESTINATION .)
   install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/diamond.ico  DESTINATION .)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../source_extra/syntax/ DESTINATION syntax)

# installs English by default
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../source_extra/dictionary/en_US.aff DESTINATION dictionary)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../source_extra/dictionary/en_US.dic DESTINATION dictionary)

# will not overwrite dictionary/userDict.txt
install(CODE "
   if (NOT EXISTS  \"${CMAKE_INSTALL_PREFIX}/dictionary/userDict.txt    \")
      file(INSTALL \"${CMAKE_CURRENT_SOURCE_DIR}/../source_extra/dictionary/userDict.txt\" DESTINATION  \"${CMAKE_INSTALL_PREFIX}/dictionary\")
   endif()
")

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   install(TARGETS Diamond DESTINATION ../MacOS)
else()
   install(TARGETS Diamond DESTINATION .)
endif()

cs_copy_library(CsCore)
cs_copy_library(CsGui)
cs_copy_library(CsNetwork)

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
   # copy required plugins to the bundle
   cs_copy_plugins(CsGui ../plugins)
   cs_copy_plugins(CsPrinterDriver ../plugins)
else()
   # installs required plugins
   cs_copy_plugins(CsGui)
   cs_copy_plugins(CsPrinterDriver)
endif()
