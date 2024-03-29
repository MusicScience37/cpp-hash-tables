find_package(Doxygen REQUIRED dot)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

file(MAKE_DIRECTORY ${HASH_TABLES_HTML_ROOT}/api)
file(MAKE_DIRECTORY ${HASH_TABLES_DOXYGEN_XML_PATH})

add_custom_target(
    hash_tables_doc_doxygen ALL
    COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generate Doxygen HTML API documentation for ${FULL_PROJECT_NAME}"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${HASH_TABLES_SOURCE_DIR})

add_dependencies(hash_tables_doc hash_tables_doc_doxygen)
