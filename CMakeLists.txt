cmake_minimum_required(VERSION 3.18)

#
# Define the library
#

project(spatula VERSION 1.0.0
        DESCRIPTION "Spatial manipulation with modern C++"
        HOMEPAGE_URL "https://github.com/josiest/spatula"
        LANGUAGES CXX)

add_library(spatula INTERFACE)
add_library(sp::spatula ALIAS spatula)

option(run_tests OFF)
if (run_tests)
    add_subdirectory(test)
endif()

#
# Export the library package
#

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# define the include directory for the library
target_include_directories(spatula INTERFACE
    $<BUILD_INTERFACE:${spatula_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# define the export targets
install(TARGETS spatula
        EXPORT spatula-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# generate a version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/spatula-config-version.cmake
    VERSION ${spatula_VERSION}
    COMPATIBILITY AnyNewerVersion)

# generate the package configuration file
configure_package_config_file(
    spatula-config.cmake.in spatula-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spatula)

# allow exporting from build tree
export(EXPORT spatula-targets
       FILE ${CMAKE_CURRENT_BINARY_DIR}/spatula-targets.cmake
       NAMESPACE sp::)

# install the exported targets
install(EXPORT spatula-targets
        FILE spatula-targets.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spatula
        NAMESPACE sp::)

# install the package files
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/spatula-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/spatula-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/spatula)

# install the include files
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp")

export(PACKAGE spatula)
