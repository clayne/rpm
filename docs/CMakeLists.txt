add_subdirectory(man)

if (WITH_DOXYGEN)
    find_package(Doxygen REQUIRED)
endif()
if (DOXYGEN_FOUND)
	# XXX API docs should be pre-built in tarballs
	file(GLOB headers ${CMAKE_SOURCE_DIR}/include/rpm/*.h)
	set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
	set(DOXYGEN_OPTIMIZE_OUTPUT_FOR_C YES)
	if (ENABLE_WERROR)
		set(DOXYGEN_WARN_AS_ERROR YES)
	endif()
	doxygen_add_docs(apidoc librpm/Doxyheader.h ${headers}
			ALL USE_STAMP_FILE)
	set(doxsrc ${CMAKE_CURRENT_BINARY_DIR})
elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/html/index.html)
	set(doxsrc ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (doxsrc)
	install(DIRECTORY ${doxsrc}/html/ DESTINATION ${CMAKE_INSTALL_DOCDIR}/API)
endif()

set(manuals
  	manual/about.md
	manual/arch_dependencies.md
	manual/autosetup.md
	manual/boolean_dependencies.md
	manual/buildprocess.md
	manual/conditionalbuilds.md
	manual/dependencies.md
	manual/dependency_generators.md
	manual/devel_documentation.md
	manual/dynamic_specs.md
	manual/file_triggers.md
	manual/format_v3.md
	manual/format_v4.md
	manual/index.md
	manual/large_files.md
	manual/lua.md
	manual/macros.md
	manual/more_dependencies.md
	manual/philosophy.md
	manual/plugins.md
	manual/queryformat.md
	manual/relocatable.md
	manual/scriptlet_expansion.md
	manual/signatures_digests.md
	manual/spec.md
	manual/tags.md
	manual/triggers.md
	manual/tsort.md
)
install(FILES ${manuals} TYPE DOC)

set(site_files
	_config.yml
	_layouts/default.html
	_layouts/favicon.ico
	Gemfile
	index.md
	${manuals}
)

foreach(file ${site_files})
	configure_file(${file} ${file} COPYONLY)
endforeach()

add_custom_target(pages)

if (ENABLE_JEKYLL)
	option(JEKYLL_SERVE "Serve site locally when built" ON)
	set(JEKYLL_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH
	    "Directory to build from")

	find_program(PODMAN podman REQUIRED)
	mark_as_advanced(PODMAN)
	configure_file(Containerfile Containerfile COPYONLY)

	set(image rpm/jekyll)
	set(JEKYLL ${PODMAN} run -v ${JEKYLL_SOURCE_DIR}:/srv:z
				 -it --rm -p 4000:4000 ${image})
	if (JEKYLL_SERVE)
		set(JEKYLL ${JEKYLL} serve --host 0.0.0.0)
	else()
		set(JEKYLL ${JEKYLL} build)
	endif()

	add_custom_command(OUTPUT image
		COMMAND ${PODMAN} build -t ${image} .
		COMMAND touch image
		DEPENDS Containerfile
	)
	add_custom_target(site COMMAND ${JEKYLL} DEPENDS pages image)
endif()
