cmake_minimum_required(VERSION 3.11)

message("\nConfigure project Run...")

project(DeltaWalker)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE
        )
endif(NOT CMAKE_BUILD_TYPE)


# Общие флаги для всех сборок.
add_definitions(
    -std=c++17
    -pedantic  -pedantic-errors
    -pipe -fPIC 
    -Wall -Wno-long-long -Wno-inline 
    -finline-functions -finline-limit=2000
    --param large-function-growth=5000 --param inline-unit-growth=600 
    -D__STDC_LIMIT_MACROS=1 -I.
    -D_GLIBCXX_USE_NANOSLEEP
    -DDLL_PUBLIC=__attribute__\(\(visibility\(\"default\"\)\)\)
    )

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    #Добавление профилировки: для включения профилировки запускаем cmake -DGPROF=1.
    if(GPROF)
        add_definitions(-pg)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    endif(GPROF)

    #Добавляем флаги для построения отладочной инфомрации.
    add_definitions(-fno-inline-functions -fno-inline -O0 -ggdb3)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-O3)
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")


# Установка вспомогательных констант
set(ROOT_DIR   ${CMAKE_CURRENT_SOURCE_DIR})
set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_DIR  ${ROOT_DIR}/cmake)
set(SRC_DIR    ${ROOT_DIR}/src)
set(UTILS_DIR  ${SRC_DIR}/utils)
set(TESTS_DIR  ${SRC_DIR}/tests)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)


# Указание общих настроек
link_directories("/usr/lib")
link_directories("/usr/local/lib")


# Установка общих директорий включаемых файлов
include_directories(${SRC_DIR})
include_directories(${UTILS_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
include(${CMAKE_DIR}/TakeComponent.cmake)


# Настройка опций сборки компонент
OPTION(ENABLE_TESTS  "Enable UTESTS support [default: ON]"   ON)


if (ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    include(${CMAKE_DIR}/UnitTests.cmake)
    string(TOUPPER ${ENABLE_TESTS} UPPER_STR)
    if(NOT ${UPPER_STR} MATCHES "ON" AND NOT ${UPPER_STR} MATCHES "ALL" AND NOT ${UPPER_STR} MATCHES "1")
        separate_arguments(ENABLE_TESTS)
        foreach(TEST_NAME ${ENABLE_TESTS})
            message(STATUS "Enable utest: `${TEST_NAME}`")
            string(TOUPPER ${TEST_NAME} UPPER_TEST_NAME)
            set(UT_${UPPER_TEST_NAME} ON)
        endforeach()
    else()
        file(STRINGS "${TESTS_DIR}/CMakeLists.txt" FOUND_TESTS REGEX "^[ ]*add_gtest[ ]*\\(.*\\)")
        foreach(HIT ${FOUND_TESTS})
            string(REGEX MATCH ".*\\((.+)[ ]SOURCES+.*\\)" FOUND_FLAG ${HIT})
            string(TOUPPER ${CMAKE_MATCH_1} UPPER_MATCH)
            set(UT_${UPPER_MATCH} ON)
            message(STATUS "Enable unit test: \"${UPPER_MATCH}\"")
        endforeach()
    endif()
    include_directories(${GTEST_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
    add_subdirectory(${TESTS_DIR})
endif()


add_subdirectory("${UTILS_DIR}")


set(FLTK_SKIP_OPENGL OFF)
set(FLTK_SKIP_FORMS OFF)
set(FLTK_SKIP_FLUID OFF)
set(FLTK_SKIP_IMAGES OFF)
find_package(FLTK REQUIRED)
include_directories(${FLTK_INCLUDE_DIR})
message(STATUS "FLTK_INCLUDE_DIR: \"${FLTK_INCLUDE_DIR}\"")
add_executable(${PROJECT_NAME} "${SRC_DIR}/main.cpp")
target_link_libraries(${PROJECT_NAME} ${FLTK_LIBRARIES})
