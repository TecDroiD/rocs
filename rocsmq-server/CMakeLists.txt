cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocsmq-server)
project(${PROJECTNAME})

# in case of libraries which are needed...
find_package(SDL REQUIRED)
find_package(SDL_net REQUIRED)

set (PROJECT_LINK_LIBS librocsmq.a librocsmq-daemonizer.a ${SDL_LIBRARY} ${SDL_NET_LIBRARY} libjson-c.a)
link_directories(~/projects/rocs/rocsmq/build ~/projects/rocs/rocsmq-daemonizer/build ${SDL_LINK_DIR} ${SDL_NET_LINK_DIR})
include_directories(~/projects/rocs/rocsmq/include ~/projects/rocs/rocsmq-daemonizer/include ${SDL_INCLUDE_DIR} ${SDL_NET_INCLUDE_DIR})

# add all headers
include_directories(include)

# add all sources
file(GLOB SOURCES "src/*.c")

#configure config file
configure_file(
	${PROJECT_SOURCE_DIR}/conf/config.in 
	${PROJECT_BINARY_DIR}/conf/${PROJECTNAME}.config
)

# when project is a library
# add_library(${PROJECTNAME} STATIC ${SOURCES})

# whenn project is an executable
add_executable(${PROJECTNAME} ${SOURCES})
target_link_libraries(${PROJECTNAME} ${PROJECT_LINK_LIBS})

# install whatever you built
install(TARGETS ${PROJECTNAME} DESTINATION /usr/lib)
