cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocsmq-canbus)
set(PROJECTUSER rocs)

project(${PROJECTNAME})

# if SDL or other packages are needed:
find_package(SDL REQUIRED)
find_package(SDL_net REQUIRED)

#configure config file
configure_file(
	${PROJECT_SOURCE_DIR}/conf/config.in 
	${PROJECT_BINARY_DIR}/conf/${PROJECTNAME}.config
)

configure_file(
	${PROJECT_SOURCE_DIR}/conf/init.d.in 
	${PROJECT_BINARY_DIR}/etc/init.d/${PROJECTNAME}
)

# in case of libraries which are needed...
set (PROJECT_LINK_LIBS rocsmq rocsmq-daemonizer ${SDL_LIBRARY} ${SDL_NET_LIBRARY} cdb libjson-c.a libb64.a)
link_directories( ${SDL_LINK_DIR} ${SDL_NET_LINK_DIR})
include_directories(../rocsmq/include ../rocsmq-daemonizer/include ${SDL_INCLUDE_DIR} ${SDL_NET_INCLUDE_DIR})

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
install(TARGETS ${PROJECTNAME} DESTINATION /usr/local/bin)
