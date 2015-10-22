cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocs)
project(${PROJECTNAME})

add_subdirectory(rocsmq)
add_subdirectory(rocsmq-daemonizer)

add_subdirectory(rocsmq-server)
add_subdirectory(rocsmq-testclient)
add_subdirectory(rocsmq-canbus)
add_subdirectory(rocsmq-i2cbus)
