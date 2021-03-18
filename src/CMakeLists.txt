find_package (KDecoration2 REQUIRED)

find_package (Qt5 REQUIRED COMPONENTS
    Core
    Gui
)

find_package (KF5 REQUIRED COMPONENTS
    Config
    CoreAddons
    GuiAddons
    WindowSystem
)

set (decoration_SRCS
    BoxShadowHelper.cc
    CloseButton.cc
    Decoration.cc
    DecorationMagicButtonGroup.cc
    MaximizeButton.cc
    MinimizeButton.cc
    plugin.cc
)

add_library (fluentdecoration MODULE
    ${decoration_SRCS}
)

target_link_libraries (fluentdecoration
    PUBLIC
        Qt5::Core
        Qt5::Gui
        KF5::ConfigCore
        KF5::ConfigGui
        KF5::CoreAddons
        KF5::GuiAddons
        KF5::WindowSystem

    PRIVATE
        KDecoration2::KDecoration
)

install (TARGETS fluentdecoration
         DESTINATION ${PLUGIN_INSTALL_DIR}/org.kde.kdecoration2)
