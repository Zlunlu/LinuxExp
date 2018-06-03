cmake_minimum_required(VERSION 3.10)
project(LinuxExp C)

set(CMAKE_C_STANDARD 99)

add_executable(LinuxExp exp1/ls.c exp2/message/sender.c exp2/message/receiver.c exp3/pipe.c exp3/fifo.c exp4/server.c exp4/client.c)
