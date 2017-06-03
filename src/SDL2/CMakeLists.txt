cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6.4)

project(GiGiSDL2)

if (BUILD_SDL2_DRIVER)
    message("-- Configuring GiGiSDL2")
    set(SDL2_BUILDING_LIBRARY true)
    find_package(SDL2)
    if (SDL2_FOUND)
        include_directories(${SDL2_INCLUDE_DIR})
    else ()
        set(BUILD_SDL2_DRIVER OFF)
        message("     Warning: SDL2 could not be found.  Disabling the SDL2 build.")
    endif ()
endif ()

if (BUILD_SDL2_DRIVER)
    set(THIS_LIB_SOURCES SDL2GUI.cpp)
    set(THIS_LIB_LINK_LIBS GiGi ${SDL2_LIBRARY})
    library_all_variants(GiGiSDL2)

    if (WIN32)
        if (TARGET GiGiSDL2)
            set_target_properties(GiGiSDL2
                PROPERTIES
                COMPILE_DEFINITIONS BOOST_ALL_DYN_LINK
                LINK_FLAGS /NODEFAULTLIB:LIBCMT
            )
        elseif (TARGET GiGiSDL2_static)
            set_target_properties(GiGiSDL2_static
                PROPERTIES
                LINK_FLAGS /NODEFAULTLIB:LIBCMT
            )
        endif ()
    endif ()

    if (UNIX)
        get_pkg_config_libs(pkg_config_libs ${THIS_LIB_LINK_LIBS})
        configure_file(
            ${CMAKE_HOME_DIRECTORY}/cmake/GiGiSDL2.pc.in
            ${CMAKE_BINARY_DIR}/GiGiSDL2.pc
            @ONLY
        )
    endif ()
endif ()
