cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Sidcraft VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the include directories & Imgui
include_directories(imgui)
include_directories(/Users/siddharth/Code/C++/Sidcraft/dependencies/include)

# Set the source and dependencies directories
set(PROJECT_PATH /Users/siddharth/Code/C++/Sidcraft)
set(DEPENDENCIES_PATH ${PROJECT_PATH}/dependencies)

# Add source files
file(GLOB_RECURSE CXX_SOURCES "${PROJECT_PATH}/src/*.cpp")

# Add glad.c as a separate source file
set(GLAD_SOURCE "${PROJECT_PATH}/src/glad.c")

# Add imgui files
set(IMGUI_SOURCES 
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

# Add the executable
add_executable(${PROJECT_NAME} ${IMGUI_SOURCES} ${CXX_SOURCES} ${GLAD_SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${PROJECT_PATH}/imgui
    ${PROJECT_PATH}/imgui/backends
)

# Link the necessary libraries
target_link_libraries(${PROJECT_NAME}
    ${DEPENDENCIES_PATH}/lib/libglfw.3.3.dylib
)

# Specify additional compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE -fdiagnostics-color=always -Wall -Wdeprecated-declarations -Wno-deprecated)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Find and link the OpenGL framework
find_library(OpenGL_FRAMEWORK OpenGL)
if(OpenGL_FRAMEWORK)
    target_link_libraries(${PROJECT_NAME} ${OpenGL_FRAMEWORK})
else()
    message(FATAL_ERROR "OpenGL framework not found")
endif()

# Custom command to compile GLAD
add_custom_command(
    OUTPUT ${PROJECT_PATH}/glad.o
    COMMAND gcc -c ${GLAD_SOURCE} -o ${PROJECT_PATH}/glad.o
    DEPENDS ${GLAD_SOURCE}
)
add_custom_target(GLAD ALL DEPENDS ${PROJECT_PATH}/glad.o)
add_dependencies(${PROJECT_NAME} GLAD)

# Define the clean target to remove object files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${PROJECT_PATH}/*.o)