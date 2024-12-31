cmake_minimum_required(VERSION 3.14)

project(sudoku)

set(SERVER_EXE server/server.c server/config/config.c)
# set(CLIENT_EXE client/client.c)

add_executable(server.out ${SERVER_EXE})
# add_executable(client.out ${CLIENT_EXE})