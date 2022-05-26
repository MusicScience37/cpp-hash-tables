if(DEFINED ENV{PLANTUML_JAR_PATH})
    set(DEFAULT_PLANTUML_JAR_PATH $ENV{PLANTUML_JAR_PATH})
else()
    set(DEFAULT_PLANTUML_JAR_PATH "")
endif()
set(PLANTUML_JAR_PATH
    "${DEFAULT_PLANTUML_JAR_PATH}"
    CACHE FILEPATH "path of plantuml.jar")

set(HASH_TABLES_SPHINX_OPTIONS
    -D "plantuml=java -jar ${PLANTUML_JAR_PATH}" -D
    "release=${PROJECT_VERSION}" -D
    breathe_projects.hash_tables="${HASH_TABLES_DOXYGEN_XML_PATH}")

add_custom_target(
    hash_tables_doc_sphinx_html ALL
    COMMAND
        ${PIPENV_EXECUTABLE} run sphinx-build -b html -W
        ${CMAKE_CURRENT_SOURCE_DIR}/src ${HASH_TABLES_HTML_ROOT}
        ${HASH_TABLES_SPHINX_OPTIONS} -q
    COMMENT "Generate Sphinx HTML API documentation for ${FULL_PROJECT_NAME}"
    WORKING_DIRECTORY ${HASH_TABLES_SOURCE_DIR})

add_dependencies(hash_tables_doc_sphinx_html hash_tables_doc_doxygen)
add_dependencies(hash_tables_doc hash_tables_doc_sphinx_html)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/start_auto_build.sh.in
               ${CMAKE_CURRENT_BINARY_DIR}/start_auto_build.sh)
