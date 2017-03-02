cmake_minimum_required(VERSION 3.0.0)
# configure project 
set(PROJECTNAME rocsmq-luaclient)
set(PROJECTUSER rocs)

project(${PROJECTNAME})

# if SDL or other packages are needed:
find_package(Lua REQUIRED)

#configure config file
configure_file(
	${PROJECT_SOURCE_DIR}/conf/config.in 
	${PROJECT_BINARY_DIR}/conf/${PROJECTNAME}.config
)

configure_file(
	${PROJECT_SOURCE_DIR}/conf/init.d.in 
	${PROJECT_BINARY_DIR}/etc/init.d/${PROJECTNAME}
)

#copy scripts to build dir
file (GLOB SCRIPTS "${PROJECT_SOURCE_DIR}/script/*.lua")
file (COPY ${SCRIPTS} DESTINATION ${PROJECT_BINARY_DIR}/script)

# in case of libraries which are needed...
set (PROJECT_LINK_LIBS rocsmq rocsmq-daemonizer ${LUA_LIBRARIES} cdb libjson-c.a libb64.a pthread)
link_directories( ${LUA_LINK_DIR})
include_directories(../rocsmq/include  ${LUA_INCLUDE_DIR})

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
