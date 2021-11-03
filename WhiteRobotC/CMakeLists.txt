cmake_minimum_required(VERSION 3.17)
project(WhiteRobotC)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(WhiteRobotC
        RobotMenu.cpp
        RobotMenu.h
        Signal_Generator.cpp
        Signal_Generator.h
        WhiteRobot.cpp
        WhiteRobot.h
        WhiteRobotC.cpp
        WhiteStrategy.cpp
        WhiteStrategy.h)
