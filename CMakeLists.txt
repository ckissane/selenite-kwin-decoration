cmake_minimum_required (VERSION 3.10)
project (selenite-kwin-decoration)

add_definitions (-Wall -Werror)

include (FeatureSummary)
find_package (ECM 0.0.9 REQUIRED NO_MODULE)

set (CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
    ${ECM_KDE_MODULE_DIR}
)

include (ECMInstallIcons)
include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings NO_POLICY_SCOPE)

add_subdirectory (src)

feature_summary(WHAT ALL)
