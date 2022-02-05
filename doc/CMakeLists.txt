find_package(Doxygen QUIET COMPONENTS dot)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
if (DOXYGEN_FOUND)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(PROCESSED_DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_PROJECT_NUMBER ${PALISADE_VERSION})
    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src)

    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apidocs)
    set(INDEX_FILE ${DOXYGEN_OUTPUT_DIR}index.html)
    configure_file(${DOXYFILE_IN} ${PROCESSED_DOXYFILE} @ONLY)

    add_custom_target(apidocs
            COMMAND ${DOXYGEN_EXECUTABLE} ${PROCESSED_DOXYFILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

    ###############################################
    # Doxygen + Sphinx + Breathe
    ###############################################
    find_package(Sphinx)
    if (Sphinx_FOUND)
        message(STATUS "Sphinx documentation will be built")
        add_custom_target(apidocs
                COMMAND ${SPHINX_EXECUTABLE} -b html -c ${CMAKE_CURRENT_SOURCE_DIR}
                ${DOXYGEN_INPUT_DIR} ${DOXYGEN_OUTPUT_DIR}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Sphinx"
                )

        ###############################################
        # Doxygen only
        ###############################################
    else (Sphinx_FOUND)
        message(STATUS "Sphinx was not found. Standard Doxygen documentation will be built instead")

    endif (Sphinx_FOUND)

    # TODO: Move the index.<FILE_EXTENSION> one dir up to make it easier to access

else (DOXYGEN_FOUND)
    message(STATUS "Doxygen and dot (from graphviz) need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
