cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocsmq-sphinx)
set(PROJECTUSER rocs)

project(${PROJECTNAME})

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
set (PROJECT_LINK_LIBS rocsmq rocsmq-daemonizer pocketsphinx sphinxbase sphinxad libjson-c.a libb64.a pthread)
link_directories( /usr/local/lib/  /usr/lib/x86_64-linux-gnu)
include_directories(../rocsmq/include ../rocsmq-daemonizer/include /usr/local/include/pocketsphinx /usr/local/include/sphinxbase  )

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
