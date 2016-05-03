CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(ug-setting-privacy-efl C)

INCLUDE(FindPkgConfig)

STRING(REGEX MATCH "([^.]*)" SO_VERSION "${VERSION}")

# Set compiler flags
SET(CMAKE_C_FLAGS_RELEASE "-O2 -Wall")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
SET(CMAKE_C_FLAGS_CCOV "-O2 -Wall --coverage")

ADD_DEFINITIONS("-Werror")                      # Make all warnings into errors.
ADD_DEFINITIONS("-Wall")                        # Generate all warnings
ADD_DEFINITIONS("-Wextra")                      # Generate even more extra warnings
ADD_DEFINITIONS("-Wno-unused-parameter")        # Ignore unused parameter
ADD_DEFINITIONS("-Wno-unused-function")
ADD_DEFINITIONS("-Wno-unused-variable")

# Set linker flags
SET(CMAKE_SHARED_LINKER_FLAGS  "-Wl,--as-needed")
SET(CMAKE_EXE_LINKER_FLAGS     "-Wl,--as-needed -pie")

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(BINDIR "${PREFIX}/bin")
SET(LIBDIR "${PREFIX}/lib")
SET(RESDIR "${PREFIX}/res")
SET(LOCALEDIR "${RESDIR}/locale")

IF (CMAKE_BUILD_TYPE MATCHES "DEBUG")
ADD_DEFINITIONS("-DTIZEN_DEBUG_ENABLE")
ADD_DEFINITIONS("-DBUILD_TYPE_DEBUG")
ADD_DEFINITIONS("-DDPL_LOGS_ENABLED")
ENDIF (CMAKE_BUILD_TYPE MATCHES "DEBUG")

ADD_DEFINITIONS("-DPACKAGE=\"${PROJECT_NAME}\"")
ADD_DEFINITIONS("-DPREFIX=\"${PREFIX}\"")
ADD_DEFINITIONS("-DPREFIX=\"${PREFIX}\"")
ADD_DEFINITIONS("-DRESDIR=\"${RESDIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")

SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#ADD_DEFINITIONS("-DUG_WAYLAND")

ADD_SUBDIRECTORY(ug)
#ADD_SUBDIRECTORY(po)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/packaging/org.tizen.ug-setting-privacy-efl.xml DESTINATION ${TZ_SYS_RO_PACKAGES}/)

