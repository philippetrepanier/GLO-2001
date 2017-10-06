cmake_minimum_required(VERSION 3.4)
project(TP1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}) #for the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}) #for static library
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}) #for shared library

set(SOURCE_FILES
    arret.cpp
    auxiliaires.cpp
    coordonnees.cpp
    ligne.cpp
    station.cpp
    voyage.cpp
        DonneesGTFS.cpp)

add_library(TP1 STATIC ${SOURCE_FILES})
#add_library(TP1 SHARED ${SOURCE_FILES})

add_executable(main main.cpp)
target_link_libraries(main TP1)