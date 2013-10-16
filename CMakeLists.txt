cmake_minimum_required(VERSION 2.6)

# additional cmake files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

OPTION(ENABLE_POLICYKIT "Enable policykit integration" ON)

if (ENABLE_POLICYKIT)

find_package(PolkitQt-1)

if (NOT POLKITQT-1_LIB_DIR AND NOT POLKITQT-1_INCLUDE_DIR)
    message(WARNING "Qt bindings for policykit are not found. Razor policykit integration won't be built")
    message(STATUS "Hint: package libpolkit-qt-1-devel on openSUSE")
else ()
    project(lxqt-policykit-agent)

    message(STATUS "PolkitQt-1: ${POLKITQT-1_VERSION_STRING}")
    message(STATUS "      libs: ${POLKITQT-1_CORE_LIBRARY} ${POLKITQT-1_AGENT_LIBRARY}")
    message(STATUS "   include: ${POLKITQT-1_INCLUDE_DIR}")
    
    find_package(PkgConfig)
    pkg_check_modules(POLKIT_AGENT REQUIRED polkit-agent-1)
    message(STATUS "polkit agent: ${POLKIT_AGENT_INCLUDE_DIRS} ${POLKIT_AGENT_LIBRARIES}")

    # lxde-qt tools for cmake
    find_package(LXQT REQUIRED )
    include(${LXQT_USE_FILE})

    find_package(QTXDG REQUIRED QUIET)
    include(${QTXDG_USE_FILE})

    # TODO/FIXME: pkg-config for hardcoded paths
    # pkg-config --cflags glib-2.0
    # pkg-config --cflags polkit-agent-1
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/libraries
        ${POLKIT_AGENT_INCLUDE_DIRS}
        ${POLKITQT-1_INCLUDE_DIR}
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
    include(RazorTranslate)
    razor_translate_ts(lxqt-policykit-agent_QM_FILES
        SOURCES
            ${lxqt-policykit-agent_H}
            ${lxqt-policykit-agent_SRCS}
            ${lxqt-policykit-agent_UI}
    )
    #************************************************
    
    QT4_WRAP_CPP(lxqt-policykit-agent_MOCS ${lxqt-policykit-agent_H})
    QT4_WRAP_UI(lxqt-policykit-agent_UI_CPP ${lxqt-policykit-agent_UI})

    add_executable(lxqt-policykit-agent
        ${lxqt-policykit-agent_SRCS}
        ${lxqt-policykit-agent_MOCS}
        ${lxqt-policykit-agent_UI_CPP}
        ${lxqt-policykit-agent_QM_FILES}
    )

    target_link_libraries(lxqt-policykit-agent
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QTXDG_LIBRARY}
        ${LXQT_LIBRARIES}
        ${POLKITQT-1_CORE_LIBRARY}
        ${POLKITQT-1_AGENT_LIBRARY}
        ${POLKIT_AGENT_LIBRARIES}
    )
    
    # install
    install(TARGETS lxqt-policykit-agent DESTINATION bin)

endif()

endif (ENABLE_POLICYKIT)
