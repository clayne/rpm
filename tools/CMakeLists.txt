add_library(cliutils OBJECT cliutils.c cliutils.h)

add_executable(rpm rpm.c cliutils)
add_executable(rpmdb rpmdb.c cliutils)
add_executable(rpmkeys rpmkeys.c cliutils)
add_executable(rpm2cpio rpm2cpio.c cliutils)
add_executable(rpmsign rpmsign.c cliutils)
add_executable(rpmbuild rpmbuild.c cliutils)
add_executable(rpmspec rpmspec.c cliutils)

add_executable(rpmdeps rpmdeps.c)
add_executable(rpmgraph rpmgraph.c)
add_executable(rpmlua rpmlua.c)
add_executable(rpmsort rpmsort.c)
add_executable(rpmuncompress rpmuncompress.c)
add_executable(elfdeps elfdeps.c)

target_link_libraries(rpmsign PRIVATE librpmsign)
target_link_libraries(rpmlua PRIVATE LUA::LUA)
target_link_libraries(elfdeps PRIVATE PkgConfig::LIBELF)
target_link_libraries(rpmbuild PRIVATE librpmbuild)
target_link_libraries(rpmspec PRIVATE librpmbuild)
target_link_libraries(rpmdeps PRIVATE librpmbuild)

target_include_directories(rpmlua PRIVATE ${CMAKE_SOURCE_DIR}/rpmio)
target_include_directories(rpmgraph PRIVATE ${CMAKE_SOURCE_DIR}/lib)

if (READLINE_FOUND)
	target_link_libraries(rpmspec PRIVATE PkgConfig::READLINE)
	target_link_libraries(rpmlua PRIVATE PkgConfig::READLINE)
endif()

if (WITH_ARCHIVE)
	add_executable(rpm2archive rpm2archive.c)
	target_link_libraries(rpm2archive PRIVATE PkgConfig::LIBARCHIVE)
	install(TARGETS rpm2archive)
endif()

# Everything links to these
get_property(executables DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(exe ${executables})
	target_link_libraries(${exe} PRIVATE librpmio librpm PkgConfig::POPT)
endforeach()

foreach(cmd rpmverify rpmquery)
	add_custom_target(${cmd} ALL COMMAND
			${CMAKE_COMMAND} -E create_symlink rpm ${cmd}
			BYPRODUCTS ${cmd})
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${cmd} TYPE BIN)
endforeach()
install(TARGETS
	rpm rpmdb rpmkeys rpm2cpio rpmsign rpmbuild rpmspec
	rpmlua rpmgraph rpmsort
)
install(TARGETS elfdeps rpmdeps rpmuncompress DESTINATION ${RPM_CONFIGDIR})
