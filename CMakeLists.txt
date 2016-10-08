cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
project(lxqt-policykit-agent)

option(UPDATE_TRANSLATIONS "Update source translation translations/*.ts files" OFF)

include(GNUInstallDirs)

set(LXQTBT_MINIMUM_VERSION "0.1.0")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(PolkitQt5-1 REQUIRED)
find_package(Qt5Widgets REQUIRED QUIET)
find_package(Qt5LinguistTools REQUIRED QUIET)
find_package(lxqt-build-tools ${LXQTBT_MINIMUM_VERSION} REQUIRED)
find_package(lxqt REQUIRED QUIET)
find_package(PkgConfig)

include(LXQtCompilerSettings NO_POLICY_SCOPE)
include(LXQtTranslate)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

pkg_check_modules(POLKIT_AGENT REQUIRED polkit-agent-1)
message(STATUS "polkit agent: ${POLKIT_AGENT_INCLUDE_DIRS} ${POLKIT_AGENT_LIBRARIES}")

set(POLKIT_AGENT_BINARY_DIR "${CMAKE_INSTALL_BINDIR}" CACHE FILEPATH "Directory for install polkit agent")


# Deal with a bug in polkit-qt5
if (NOT IS_ABSOLUTE "${POLKITQT-1_INCLUDE_DIR}")
    set(POLKITQT-1_INCLUDE_DIR "${POLKITQT-1_INSTALL_DIR}/${POLKITQT-1_INCLUDE_DIR}")
    set(POLKITQT-1_AGENT_LIBRARY "${POLKITQT-1_INSTALL_DIR}/${POLKITQT-1_AGENT_LIBRARY}")
    set(POLKITQT-1_CORE_LIBRARY "${POLKITQT-1_INSTALL_DIR}/${POLKITQT-1_CORE_LIBRARY}")
endif()
message(STATUS "PolkitQt-1: ${POLKITQT-1_VERSION_STRING}")
message(STATUS "   install: ${POLKITQT-1_INSTALL_DIR}")
message(STATUS "      libs: ${POLKITQT-1_CORE_LIBRARY} ${POLKITQT-1_AGENT_LIBRARY}")
message(STATUS "   include: ${POLKITQT-1_INCLUDE_DIR}")

# TODO/FIXME: pkg-config for hardcoded paths
# pkg-config --cflags glib-2.0
# pkg-config --cflags polkit-agent-1
include_directories(
    ${POLKIT_AGENT_INCLUDE_DIRS}
    "${POLKITQT-1_INCLUDE_DIR}"
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

# Translations
lxqt_translate_ts(lxqt-policykit-agent_QM_FILES
    UPDATE_TRANSLATIONS
        ${UPDATE_TRANSLATIONS}
    SOURCES
        ${lxqt-policykit-agent_H}
        ${lxqt-policykit-agent_SRCS}
        ${lxqt-policykit-agent_UI}
    INSTALL_DIR
        "${LXQT_TRANSLATIONS_DIR}/${PROJECT_NAME}"
    PULL_TRANSLATIONS
        ${PULL_TRANSLATIONS}
    CLEAN_TRANSLATIONS
        ${CLEAN_TRANSLATIONS}
    TRANSLATIONS_REPO
        ${TRANSLATIONS_REPO}
    TRANSLATIONS_REFSPEC
        ${TRANSLATIONS_REFSPEC}
    REPO_SUBDIR
        "lxqt-policykit/${PROJECT_NAME}"
)

lxqt_app_translation_loader(QM_LOADER ${PROJECT_NAME})

add_executable(lxqt-policykit-agent
    ${lxqt-policykit-agent_SRCS}
    ${lxqt-policykit-agent_MOCS}
    ${lxqt-policykit-agent_QM_FILES}
    ${QM_LOADER}
)

target_link_libraries(lxqt-policykit-agent
    Qt5::Widgets
    lxqt
    ${POLKITQT-1_CORE_LIBRARY}
    ${POLKITQT-1_AGENT_LIBRARY}
    ${POLKIT_AGENT_LIBRARIES}
)

# install
install(TARGETS
    lxqt-policykit-agent
    DESTINATION "${POLKIT_AGENT_BINARY_DIR}"
    COMPONENT Runtime
)
