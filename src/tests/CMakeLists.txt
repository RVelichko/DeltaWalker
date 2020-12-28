project(utests)
cmake_minimum_required(VERSION 3.11)

add_gtest(log_cmpltr SOURCES UtLogCompleter.cpp LIBRARIES log)
