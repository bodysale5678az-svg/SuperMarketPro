cmake_minimum_required(VERSION 3.16)

project(SuperMarketPro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Treat warnings strictly — this is a learning project, keep it clean.
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(SuperMarketPro
    src/main.cpp
    src/Application.cpp
    src/SupermarketService.cpp
)

target_include_directories(SuperMarketPro PRIVATE include)

# Copy the data folder next to the binary so the app finds ./data
if (WIN32)
    add_custom_command(TARGET SuperMarketPro POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:SuperMarketPro>/data")
endif()
