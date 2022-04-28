message("Configure documentation of ${FULL_PROJECT_NAME}")

add_custom_target(hash_tables_doc)

set(HASH_TABLES_HTML_ROOT ${CMAKE_CURRENT_BINARY_DIR}/html)

add_subdirectory(doxygen)

option(HASH_TABLES_BUILD_DOC_SPHINX
       "build documentation of ${FULL_PROJECT_NAME} with Sphinx" ON)
if(HASH_TABLES_BUILD_DOC_SPHINX)
    add_subdirectory(sphinx)
endif()
