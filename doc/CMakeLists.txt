find_package(Doxygen QUIET COMPONENTS dot)

if (DOXYGEN_FOUND)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(PROCESSED_DOXYFILE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYGEN_PROJECT_NUMBER ${PALISADE_VERSION})
    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/src)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apidocs)

    configure_file(${DOXYFILE_IN} ${PROCESSED_DOXYFILE} @ONLY)

    ###############################################
    # Doxygen + Sphinx + Breathe
    ###############################################
    if (Sphinx_FOUND)
        message(STATUS "Sphinx documentation will be built")
        add_custom_target(apidocs
                COMMAND ${SPHINX_EXECUTABLE} -b html
                ${DOXYGEN_INPUT_DIR} ${DOXYGEN_OUTPUT_DIRECTORY}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Sphinx"
                )

    ###############################################
    # Doxygen only
    ###############################################
    else (Sphinx_FOUND)
        message(STATUS "Sphinx was not found. Standard Doxygen documentation will be built instead")
        add_custom_target(apidocs
                COMMAND ${DOXYGEN_EXECUTABLE} ${PROCESSED_DOXYFILE}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)
        message(STATUS "Doxygen and dot are found")


    endif (Sphinx_FOUND)

    # TODO: replace configure_file with doxygen_add_docs which is the best practice
    # I think this uses positional arg replacement
else (DOXYGEN_FOUND)
    message(STATUS "Doxygen and dot (from graphviz) need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
