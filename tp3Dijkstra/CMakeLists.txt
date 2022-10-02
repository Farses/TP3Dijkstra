cmake_minimum_required(VERSION 3.20)
project(tp3Dijkstra C)

set(CMAKE_C_STANDARD 99)

add_executable(tp3Dijkstra cellule.h Graphe.h fifo.c fifo.h main.c)
