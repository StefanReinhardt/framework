cmake_minimum_required(VERSION 2.8.8)
project(cloudshop)
set( APP_NAME cloudshop )
MESSAGE( STATUS "APP_NAME  :         " ${APP_NAME} )




# QT 5.0 ===========================================
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)


# sources ==========================================

# specify app sources
file(GLOB_RECURSE app_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h )





# executeable =====================================
add_executable(${APP_NAME} ${app_sources})

qt5_use_modules(${APP_NAME} Widgets)