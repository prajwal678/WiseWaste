cmake_minimum_required(VERSION 3.10)
project(WiseWasteBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)

# Add Crow as a dependency (assuming it's in the project or installed)
# If Crow is installed system-wide, you can use find_package(Crow REQUIRED)
# For now, we'll assume it's in the project
add_subdirectory(vendor/crow)

# Add source files
set(SOURCES
    main.cpp
    model/database.cpp
    model/waste_pickup.cpp
    controller/waste_pickup_controller.cpp
)

# Add header files
set(HEADERS
    model/database.h
    model/waste_pickup.h
    controller/waste_pickup_controller.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/controller
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SQLite::SQLite3
    Threads::Threads
    Crow::Crow
)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add compiler warnings
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif() 