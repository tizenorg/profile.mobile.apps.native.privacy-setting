INCLUDE(FindPkgConfig)
SET(pkg_name "org.tizen.ug-setting-privacy-efl")

pkg_check_modules(${pkg_name} REQUIRED glib-2.0 ui-gadget-1 evas elementary edje eina efl-extension security-privilege-manager icu-i18n dlog security-manager pkgmgr-info)

SET(SRCS
	${CMAKE_SOURCE_DIR}/common/src/common_utils.c
	src/main_privacy_setting_ug.c
	src/privacy_menu_view.c
	src/privacy_list_view.c
	src/privacy_package_list_view.c
)

FOREACH(flag ${${pkg_name}_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIE -Werror-implicit-function-declaration")
SET(INC_DIR include)
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/common/include
	${CMAKE_SOURCE_DIR}/ug/include
)

ADD_EXECUTABLE(${pkg_name} ${SRCS})

TARGET_LINK_LIBRARIES(${pkg_name}
	${${pkg_name}_LDFLAGS}
)

INSTALL(TARGETS ${pkg_name} DESTINATION ${BINDIR})
