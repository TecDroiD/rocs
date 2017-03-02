cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocsmq-opencv)
set(PROJECTUSER rocs)

project(${PROJECTNAME})

<<<<<<< HEAD
# if SDL or other packages are needed:
find_package(SDL REQUIRED)
find_package(SDL_net REQUIRED)
find_package(OpenCV REQUIRED)
=======
>>>>>>> nosdl
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
<<<<<<< HEAD
set (PROJECT_LINK_LIBS rocsmq rocsmq-daemonizer ${SDL_LIBRARY} ${SDL_NET_LIBRARY} ${OpenCV_LIBS} libjson-c.a libb64.a)
link_directories( ${SDL_LINK_DIR} ${SDL_NET_LINK_DIR})
include_directories(../rocsmq/include ../rocsmq-daemonizer/include ${SDL_INCLUDE_DIR} ${SDL_NET_INCLUDE_DIR})
=======
set (PROJECT_LINK_LIBS rocsmq rocsmq-daemonizer libjson-c.a libb64.a pthread)
link_directories( )
include_directories(../rocsmq/include ../rocsmq-daemonizer/include )
>>>>>>> nosdl

# add all headers
include_directories(include)

# add all sources
file(GLOB SOURCES "src/*.cpp")

# when project is a library
# add_library(${PROJECTNAME} STATIC ${SOURCES})

# whenn project is an executable
add_executable(${PROJECTNAME} ${SOURCES})
target_link_libraries(${PROJECTNAME} ${PROJECT_LINK_LIBS})

# install whatever you built
install(TARGETS ${PROJECTNAME} DESTINATION /usr/local/bin)
