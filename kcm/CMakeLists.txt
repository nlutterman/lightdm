find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

# Makes it easy to reuse code from the qml lib. A bit ugly but I don't feel
# like setting up a static library for one class.
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# Add the compiler flags
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${KDE4_INCLUDES}
  ${QLIGHTDM_INCLUDE_DIR}
  ${LIB_DIR}
)

add_subdirectory(configwidgets)

# Insert your sources here
set(LIGHTDM_KCM_SRCS
    themeconfig.cpp
    themesmodel.cpp
    themesdelegate.cpp
    coreconfig.cpp
    lightdmkcm.cpp
    configoptions.cpp
    ${LIB_DIR}/extrarowproxymodel.cpp
    ${LIB_DIR}/usersmodel.cpp
    ${LIB_DIR}/lightdmpackagestructure.cpp
)

set(LIGHTDM_KCM_UI
    themeconfig.ui
    coreconfig.ui)

kde4_add_ui_files(LIGHTDM_KCM_SRCS ${LIGHTDM_KCM_UI})

kde4_add_plugin(kcm_lightdm
                ${LIGHTDM_KCM_SRCS})


# Link against the needed libraries
target_link_libraries(kcm_lightdm
  ${KDE4_KDEUI_LIBS}
  ${QT_QTUITOOLS_LIBRARY}
  ${QT_QTDECLARATIVE_LIBRARY}
  ${KDE4_PLASMA_LIBS}
  ${QLIGHTDM_LIBRARIES}
  ${KDE4_KIO_LIBS}
  lightdm_config_widgets
)

install(TARGETS kcm_lightdm
          DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kcm_lightdm.desktop
         DESTINATION ${SERVICES_INSTALL_DIR}
)

kde4_add_executable(kcmlightdmhelper helper.cpp)
target_link_libraries(kcmlightdmhelper ${KDE4_KDECORE_LIBS} ${KDE4_PLASMA_LIBS})

kde4_install_auth_helper_files(kcmlightdmhelper org.kde.kcontrol.kcmlightdm root)
install(TARGETS kcmlightdmhelper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_actions(org.kde.kcontrol.kcmlightdm kcm_lightdm.actions)

macro_display_feature_log()


