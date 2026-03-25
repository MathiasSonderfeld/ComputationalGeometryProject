#CMAKE FILE FOR MIN-COG
cmake_minimum_required(VERSION 3.27)
project(CgToolkit)

cmake_host_system_information(RESULT MYOS_NAME QUERY OS_NAME)
cmake_host_system_information(RESULT 64BITS QUERY IS_64BIT)
if (${64BITS})
message(STATUS "Current System: ${MYOS_NAME} 64 Bit" )
else (${64BITS})
   message(STATUS "Current System: ${MYOS_NAME} 32 Bit" )
   endif (${64BITS})


set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)


find_package(OpenGL REQUIRED)
IF (${OpenGL_FOUND})
   MESSAGE(STATUS "Found OpenGL.")
   include_directories(${GL_INCLUDE_DIRS})
ELSE (${OpenGL_FOUND})
   MESSAGE(STATUS "Could not locate OpenGL.")
ENDIF (${OpenGL_FOUND})


# in case of Windows directly use Lib in source directory and link statically
# use find_package for all other systems
IF (${MYOS_NAME} STREQUAL "Windows")
add_library(glfw STATIC IMPORTED)
if(${64BITS})
set_target_properties(glfw PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/win-libs/libglfw3-64bit.a"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/win-includes")
else(${64BITS})
   set_target_properties(glfw PROPERTIES
           IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/win-libs/libglfw3-32bit.a"
           INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/src/Dependencies/win-includes")
endif(${64BITS})
else (${MYOS_NAME} STREQUAL "Windows")
   find_package(glfw3 REQUIRED)
   IF (${glfw3_FOUND})
      MESSAGE(STATUS "Found glfw3.")
      include_directories(${glfw3_INCLUDE_DIRS})
   ELSE (${glfw3_FOUND})
      MESSAGE(STATUS "Could not locate glfw3.")
   ENDIF (${glfw3_FOUND})
endif (${MYOS_NAME} STREQUAL "Windows")


# include files for Mac
include_directories(/usr/local/include)
include_directories(/opt/local/include)
include_directories(/opt/homebrew/include)


set(IMGUI_SOURCES
    ./src/Dependencies/imgui/imgui.cpp
    ./src/Dependencies/imgui/imgui_demo.cpp
    ./src/Dependencies/imgui/imgui_draw.cpp
    ./src/Dependencies/imgui/imgui_tables.cpp
    ./src/Dependencies/imgui/imgui_widgets.cpp
    ./src/Dependencies/imgui/backends/imgui_impl_glfw.cpp
    ./src/Dependencies/imgui/backends/imgui_impl_opengl3.cpp
)

# compile glew from source directly in project
set(GLEW_SOURCES
        ./src/Dependencies/GLEW/src/glew.c
)
add_compile_definitions(GLEW_STATIC)


# find all header and source files for project dirs
file(GLOB CG_BASE CONFIGURE_DEPENDS "src/CgBase/*.cpp" "src/CgBase/*.h")
file(GLOB CG_GEOMETRY CONFIGURE_DEPENDS "src/CgGeometry/*.cpp" "src/CgGeometry/*.h")
file(GLOB CG_IMAGE_VIEWER CONFIGURE_DEPENDS "src/CgImageViewer/*.cpp" "src/CgImageViewer/*.h")
file(GLOB CG_MATH CONFIGURE_DEPENDS "src/CgMath/*.cpp" "src/CgMath/*.h")
file(GLOB CG_RENDERING_3D CONFIGURE_DEPENDS "src/CgRendering3d/*.cpp" "src/CgRendering3d/*.h")
file(GLOB CG_UTILS CONFIGURE_DEPENDS "src/CgUtils/*.cpp" "src/CgUtils/*.h")
file(GLOB CG_MAIN CONFIGURE_DEPENDS "src/main/*.cpp" "src/main/*.h")

# merge header and source files 
list(APPEND SOURCES 
 ${CG_BASE} 
 ${CG_GEOMETRY} 
 ${CG_IMAGE_VIEWER} 
 ${CG_MATH} 
 ${CG_RENDERING_3D}
 ${CG_UTILS}
 ${IMGUI_SOURCES}
 ${GLEW_SOURCES}
 ${CG_MAIN} 
)


# add all project files as include directories
include_directories(./src)
include_directories(src/CgBase)
include_directories(src/CgGeometry)
include_directories(src/CgImageViewer)
include_directories(src/CgMath)
include_directories(src/CgRendering3d)
include_directories(src/CgUtils)
include_directories(src/Dependencies/imgui)
include_directories(src/Dependencies/imgui/backends)
include_directories(src/Dependencies/)
include_directories(src/Dependencies/win-includes)
include_directories(src/Dependencies/GLEW/include)
include_directories(src/main)

# main executable
add_executable(CgToolkitMain ${SOURCES})

# link libs to executable
target_link_libraries(CgToolkitMain
        ${OPENGL_LIBRARIES}
        glfw
)
