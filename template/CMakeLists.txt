cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME test)
project(${PROJECTNAME})

# if SDL or other packages are needed:
# find_package(SDL REQUIRED)

# in case of libraries which are needed...
# set (PROJECT_LINK_LIBS librocsmq.a ${SDL_LIBRARY})
# link_directories(~/projects/rocs/rocsmq/build ${SDL_LINK_DIR})
# include_directories(~/projects/rocs/rocsmq/include ${SDL_INCLUDE_DIR})

# add all headers
include_directories(include)

# add all sources
file(GLOB SOURCES "src/*.c")

# when project is a library
# add_library(${PROJECTNAME} STATIC ${SOURCES})

# whenn project is an executable
# add_executable(${PROJECTNAME} ${SOURCES})
# target_link_libraries(${PROJECTNAME} ${PROJECT_LINK_LIBS})

# install whatever you built
install(TARGETS ${PROJECTNAME} DESTINATION /usr/lib)
