# Install script for directory: /home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotool" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotool")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotool"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/picotool" TYPE EXECUTABLE MESSAGE_NEVER FILES "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/picotool")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotool" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotool")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotool")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotoolTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotoolTargets.cmake"
         "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/CMakeFiles/Export/4428c4be664630400018e610c73b3270/picotoolTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotoolTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/picotool/picotoolTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/picotool" TYPE FILE MESSAGE_NEVER FILES "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/CMakeFiles/Export/4428c4be664630400018e610c73b3270/picotoolTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/picotool" TYPE FILE MESSAGE_NEVER FILES "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/CMakeFiles/Export/4428c4be664630400018e610c73b3270/picotoolTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/picotool" TYPE FILE MESSAGE_NEVER FILES
    "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/picotoolConfig.cmake"
    "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/picotoolConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/errors/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/picoboot_connection/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/elf/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/elf2uf2/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/lib/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/bintool/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/pico_binary_info/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/boot_uf2_headers/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/boot_picoboot_headers/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/boot_picobin_headers/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/pico_usb_reset_interface_headers/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/boot_bootrom_headers/cmake_install.cmake")
  include("/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/pico_platform/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/david/Documentos/Disciplinas/RTS/Pratica_04/build/_deps/picotool-build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
