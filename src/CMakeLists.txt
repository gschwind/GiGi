cmake_minimum_required(VERSION 2.6)

message("-- Configuring GiGi")

set(THIS_LIB_SOURCES

    AlignmentFlags.cpp
    Base.cpp
    BrowseInfoWnd.cpp
    Button.cpp
    ClrConstants.cpp
    Control.cpp
    Cursor.cpp
    DrawUtil.cpp
    DropDownList.cpp
    DynamicGraphic.cpp
    Edit.cpp
    EventPump.cpp
    Font.cpp
    GroupBox.cpp
    GUI.cpp
    Layout.cpp
    ListBox.cpp
    Menu.cpp
    MultiEdit.cpp
    PluginInterface.cpp
    PtRect.cpp
    Scroll.cpp
    StaticGraphic.cpp
    StyleFactory.cpp
    TabWnd.cpp
    TextControl.cpp
    Texture.cpp
    Timer.cpp
    UnicodeCharsets.cpp
    Wnd.cpp
    WndEvent.cpp
    ZList.cpp

    dialogs/ColorDlg.cpp
    dialogs/FileDlg.cpp
    dialogs/ThreeButtonDlg.cpp

    ../libltdl/ltdl.c
)

set_source_files_properties(${CMAKE_HOME_DIRECTORY}/libltdl/ltdl.c PROPERTIES COMPILE_DEFINITIONS HAVE_CONFIG_H=1)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_HOME_DIRECTORY}/libltdl/ltdl.h ${CMAKE_HOME_DIRECTORY}/GG/ltdl.h
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_HOME_DIRECTORY}/libltdl/config.h ${CMAKE_HOME_DIRECTORY}/GG/ltdl_config.h
)
set_source_files_properties(${CMAKE_HOME_DIRECTORY}/GG/ltdl.h ${CMAKE_HOME_DIRECTORY}/GG/ltdl_config.h PROPERTIES GENERATED true)

set(THIS_LIB_LINK_LIBS ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES})
if (USE_DEVIL)
    list(APPEND THIS_LIB_LINK_LIBS ${IL_LIBRARY} ${ILUT_LIBRARY})
else ()
    if (JPEG_FOUND)
        list(APPEND THIS_LIB_LINK_LIBS ${JPEG_LIBRARIES})
    endif ()
    if (PNG_FOUND)
        list(APPEND THIS_LIB_LINK_LIBS ${PNG_LIBRARIES})
    endif ()
    if (TIFF_FOUND)
        list(APPEND THIS_LIB_LINK_LIBS ${TIFF_LIBRARIES})
    endif ()
endif ()

if (WIN32)
    add_definitions(-DADOBE_TEST_MICROSOFT_NO_DEPRECATE=0)
    if (MSVC)
        add_definitions(/wd4146 /wd4099 /wd4251 /wd4800 /wd4267 /wd4275
                        /wd4244 /wd4101 /wd4258 /wd4351 /wd4996)
    endif ()
endif ()

library_all_variants(GiGi)

if (WIN32)
    if (TARGET GiGi)
        set_target_properties(GiGi
            PROPERTIES
            COMPILE_DEFINITIONS BOOST_ALL_DYN_LINK
            LINK_FLAGS /NODEFAULTLIB:LIBCMT
        )
    elseif (TARGET GiGi_static)
        set_target_properties(GiGi_static
            PROPERTIES
            LINK_FLAGS /NODEFAULTLIB:LIBCMT
        )
    endif ()
endif ()

if (UNIX)
    get_pkg_config_libs(pkg_config_libs ${THIS_LIB_LINK_LIBS})
    configure_file(
        ${CMAKE_HOME_DIRECTORY}/cmake/GiGi.pc.in
        ${CMAKE_BINARY_DIR}/GiGi.pc
        @ONLY
    )
endif ()

add_subdirectory(SDL)
add_subdirectory(Ogre)
