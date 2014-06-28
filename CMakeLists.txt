cmake_minimum_required(VERSION 2.8.9)
project(lxqt-policykit-agent)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Support different versions of Qt
option(USE_QT5 "Build with Qt5." $ENV{USE_QT5})

# find Qt binding of polkit
if(USE_QT5)
    find_package(PolkitQt5-1) # Qt5 version
else()
    find_package(PolkitQt-1) # Qt4 version
endif()

if (NOT POLKITQT-1_LIB_DIR AND NOT POLKITQT-1_INCLUDE_DIR)
    message(WARNING "Qt bindings for policykit are not found. LxQt policykit integration won't be built")
    message(STATUS "Hint: package libpolkit-qt-1-devel on openSUSE")
else ()
    message(STATUS "PolkitQt-1: ${POLKITQT-1_VERSION_STRING}")
    message(STATUS "      libs: ${POLKITQT-1_CORE_LIBRARY} ${POLKITQT-1_AGENT_LIBRARY}")
    message(STATUS "   include: ${POLKITQT-1_INCLUDE_DIR}")

    find_package(PkgConfig)
    pkg_check_modules(POLKIT_AGENT REQUIRED polkit-agent-1)
    message(STATUS "polkit agent: ${POLKIT_AGENT_INCLUDE_DIRS} ${POLKIT_AGENT_LIBRARIES}")

    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_AUTOMOC ON)
    if(USE_QT5)
        find_package(Qt5Widgets REQUIRED QUIET)
        find_package(Qt5LinguistTools REQUIRED QUIET)
        find_package(lxqt-qt5 REQUIRED QUIET)
        find_package(qt5xdg REQUIRED QUIET)
        set(QTX_INCLUDE_DIRS ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR})
        set(QTX_LIBRARIES ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
    else()
        find_package(Qt4 REQUIRED)
        include(${QT_USE_FILE})
        find_package(LXQT REQUIRED)
        find_package(QTXDG REQUIRED QUIET)
        set(QTX_INCLUDE_DIRS ${Qt5Widgets_INCLUDE_DIR})
        set(QTX_LIBRARIES ${Qt5Widgets_LIBRARY})
    endif()
    include(${LXQT_USE_FILE})
    include(${QTXDG_USE_FILE})

    # TODO/FIXME: pkg-config for hardcoded paths
    # pkg-config --cflags glib-2.0
    # pkg-config --cflags polkit-agent-1
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${POLKIT_AGENT_INCLUDE_DIRS}
        ${POLKITQT-1_INCLUDE_DIR}
        ${QTX_INCLUDE_DIR}
    )

    set(lxqt-policykit-agent_SRCS
        src/main.cpp
        src/policykitagent.cpp
        src/policykitagentgui.cpp
    )

    set(lxqt-policykit-agent_H
        src/policykitagent.h
        src/policykitagentgui.h
    )

    set(lxqt-policykit-agent_UI
        src/policykitagentgui.ui
    )

    # Translations **********************************
    include(LxQtTranslate)
    lxqt_translate_ts(lxqt-policykit-agent_QM_FILES
        SOURCES
            ${lxqt-policykit-agent_H}
            ${lxqt-policykit-agent_SRCS}
            ${lxqt-policykit-agent_UI}
    )
    #************************************************

    if(USE_QT5)
        qt5_wrap_ui(lxqt-policykit-agent_UI_CPP ${lxqt-policykit-agent_UI})
    else()
        qt4_wrap_ui(lxqt-policykit-agent_UI_CPP ${lxqt-policykit-agent_UI})
    endif()

    add_executable(lxqt-policykit-agent
        ${lxqt-policykit-agent_SRCS}
        ${lxqt-policykit-agent_MOCS}
        ${lxqt-policykit-agent_UI_CPP}
        ${lxqt-policykit-agent_QM_FILES}
    )

    target_link_libraries(lxqt-policykit-agent
        ${QTX_LIBRARY}
        ${QTXDG_LIBRARY}
        ${LXQT_LIBRARIES}
        ${POLKITQT-1_CORE_LIBRARY}
        ${POLKITQT-1_AGENT_LIBRARY}
        ${POLKIT_AGENT_LIBRARIES}
    )

    # install
    install(TARGETS lxqt-policykit-agent DESTINATION bin)

endif()

# building tarball with CPack -------------------------------------------------
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_VERSION_MAJOR ${LXQT_MAJOR_VERSION})
set (CPACK_PACKAGE_VERSION_MINOR ${LXQT_MINOR_VERSION})
set (CPACK_PACKAGE_VERSION_PATCH ${LXQT_PATCH_VERSION})
set (CPACK_GENERATOR TBZ2)
set (CPACK_SOURCE_GENERATOR TBZ2)
set (CPACK_SOURCE_IGNORE_FILES /build/;.gitignore;.*~;.git;.kdev4;temp)
include (CPack)
