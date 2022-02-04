find_package(Doxygen QUIET COMPONENTS dot)

if (DOXYGEN_FOUND)
    set(DOXYFILE_IN Doxyfile.in)
    set(DOXYFILE_OUT Doxyfile)
    set(DOXYGEN_PROJECT_NUMBER ${PALISADE_VERSION})

    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
#    find_package_handle_standard_args(Sphinx
#            "Failed to find sphinx-build executable"
#            SPHINX_EXECUTABLE)

    ###############################################
    # Doxygen + Sphinx + Breathe
    ###############################################
    if (Sphinx_FOUND)
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
        set(SPHINX_BUILD ${CMAKE_CURRENT_SOURCE_DIR}/sphinx)

        message(STATUS "Sphinx documentation will be built")
        add_custom_target(apidocs
                COMMAND ${SPHINX_EXECUTABLE} -b html
                ${SPHINX_SOURCE} ${SPHINX_BUILD}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Sphinx"
                )


    ###############################################
    # Doxygen only
    ###############################################
    else(Sphinx_FOUND)
        message(STATUS "Sphinx was not found. Standard Doxygen documentation will be built instead")
        add_custom_target(apidocs
#                COMMAND sh -c "( cat ${CMAKE_CURRENT_SOURCE_DIR}/lbcrypto-doxy-config && echo PROJECT_NUMBER=${PALISADE_VERSION} ) | ${DOXYGEN_EXECUTABLE} -"
                COMMAND ${DOXYGEN_EXECUTABLE} -g ${CMAKE_CURRENT_SOURCE_DIR}/${DOXYFILE_IN}
                COMMAND sh -c "( cat ${DOXYFILE_IN} ) | ${DOXYGEN_EXECUTABLE} -"
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)
        message(STATUS "Doxygen and dot are found")


    endif(Sphinx_FOUND)

    # TODO: replace configure_file with doxygen_add_docs which is the best practice
    # I think this uses positional arg replacement
else (DOXYGEN_FOUND)
    message(STATUS "Doxygen and dot (from graphviz) need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
