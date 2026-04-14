cmake_minimum_required(VERSION 3.10)
project(RonaldinhoSoccer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

find_package(OpenGL REQUIRED)

set(SOURCES 
    src/main.cpp 
    src/glad.c
    src/player.cpp
    src/goalkeeper.cpp
    src/ball.cpp
    src/match.cpp
    src/stadium.cpp
    src/hud.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    glfw 
    OpenGL::GL
    ${CMAKE_DL_LIBS}
)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
