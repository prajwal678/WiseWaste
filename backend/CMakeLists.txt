cmake_minimum_required(VERSION 3.10)
project(WiseWasteBackend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)


add_subdirectory(vendor/Crow)

set(SOURCES
    main.cpp
    model/database.cpp
    model/waste_pickup.cpp
    controller/waste_pickup_controller.cpp
)

set(HEADERS
    model/database.h
    model/waste_pickup.h
    controller/waste_pickup_controller.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/model
    ${CMAKE_CURRENT_SOURCE_DIR}/controller
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SQLite::SQLite3
    Threads::Threads
    Crow::Crow
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif() 