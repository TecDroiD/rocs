cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocsmq-daemonizer)
set(PROJECTUSER rocs)

project(${PROJECTNAME})

# in case of libraries which are needed...
set (PROJECT_LINK_LIBS rocsmq libjson-c.a)
include_directories(../rocsmq/include)

# add all headers
include_directories(include)

# add all sources
file(GLOB SOURCES "src/*.c")

# when project is a library
add_library(${PROJECTNAME} STATIC ${SOURCES})

# whenn project is an executable
# add_executable(${PROJECTNAME} S{SOURCES})

# install whatever you built
install(TARGETS ${PROJECTNAME} DESTINATION /usr/lib)
