cmake_minimum_required(VERSION 3.12)
project(cpp_app)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/exe)

file(GLOB sourcefiles "src/*.h" "src/*.cpp" "src/*.c"  "src/m68k/*.h" "src/m68k/*.cpp" "src/utils/*.h")
add_executable(${PROJECT_NAME} ${sourcefiles}
)

find_package(OpenGL)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} SDL2_image SDL2_ttf SDL2 SDL2main)
