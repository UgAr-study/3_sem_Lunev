cmake_minimum_required(VERSION 3.17)
project(MsgMes C)

set(CMAKE_C_STANDARD 99)

add_executable(MsgMes main.c messages.c message_header.h)