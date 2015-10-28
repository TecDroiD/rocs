cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocsmq-tclclient)
project(${PROJECTNAME})

# if SDL or other packages are needed:
find_package(SDL REQUIRED)
find_package(SDL_net REQUIRED)
find_package(TCL REQUIRED)

#configure config file
configure_file(
	${PROJECT_SOURCE_DIR}/conf/config.in 
	${PROJECT_BINARY_DIR}/conf/${PROJECTNAME}.config
)
#copy scripts to build dir
file (GLOB SCRIPTS "${PROJECT_SOURCE_DIR}/script/*.tcl")
file (COPY ${SCRIPTS} DESTINATION ${PROJECT_BINARY_DIR}/script)

# in case of libraries which are needed...
set (PROJECT_LINK_LIBS librocsmq.a ${SDL_LIBRARY} ${SDL_NET_LIBRARY} ${TCL_LIBRARY} libjson-c.a libb64.a)
link_directories(~/projects/rocs/rocsmq/build ${SDL_LINK_DIR} ${SDL_NET_LINK_DIR} ${TCL_LINK_DIR})
include_directories(~/projects/rocs/rocsmq/include ${SDL_INCLUDE_DIR} ${SDL_NET_INCLUDE_DIR} ${TCL_INCLUDE_DIR})

# add all headers
include_directories(include)

# add all sources
file(GLOB SOURCES "src/*.c")

# when project is a library
# add_library(${PROJECTNAME} STATIC ${SOURCES})

# whenn project is an executable
add_executable(${PROJECTNAME} ${SOURCES})
target_link_libraries(${PROJECTNAME} ${PROJECT_LINK_LIBS})

# install whatever you built
install(TARGETS ${PROJECTNAME} DESTINATION /usr/lib)
