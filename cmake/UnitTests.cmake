#find_path(LIBCPPUNITLITE_INCLUDE_DIR NAMES Test.h PATH_SUFFIXES "include" "CppUnitLite")
#find_library(LIBCPPUNITLITE_LIBRARY NAMES libCppUnitLite.a PATH_SUFFIXES "lib" "lib32" "lib64")
#
#include_directories(${LIBCPPUNITLITE_INCLUDE_DIR})
include_directories(${GTEST_INCLUDE_DIR})


# unit_test(NAME SRCS LIBS)
#  [ARGV0]     <NAME>       -- test name.
#  [SOURCES]   <NAME1 ... > -- sources files.
#  [LIBRARIES] <NAME1 ... > -- link libraryes.
macro(add_gtest)
    set(OPTIONS)
    set(ONE_VALUE_ARGS NAME)
    set(MULTI_VALUE_ARGS SOURCES LIBRARIES)
    set(MY_ARGS "NAME;${ARGN}")
    cmake_parse_arguments(GET "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${MY_ARGS})
    string(TOUPPER ${GET_NAME} TOUPPER_NAME)
    if(UT_${TOUPPER_NAME})
        set(LIBRARYES ${GET_LIBRARIES} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
        set(UT_NAME "ut_${GET_NAME}")
        add_executable(${UT_NAME} ${GET_SOURCES})
        target_link_libraries(${UT_NAME} ${LIBRARYES})
        install(TARGETS ${UT_NAME} DESTINATION bin)
        add_test(NAME ${GET_NAME} COMMAND ${UT_NAME})
    endif(UT_${TOUPPER_NAME})
endmacro(add_gtest)
 
