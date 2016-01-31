cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocs)
set(PROJECTUSER rocs)

project(${PROJECTNAME})

add_subdirectory(rocsmq)
add_subdirectory(rocsmq-daemonizer)

add_subdirectory(rocsmq-server)
add_subdirectory(rocsmq-tclclient)
add_subdirectory(rocsmq-testclient)
add_subdirectory(rocsmq-canbus)
add_subdirectory(rocsmq-i2cbus)

SET(CPACK_GENERATOR "RPM")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jens Rapp")
SET(CPACK_INSTALL_PREFIX "/opt/hallowelt")

INCLUDE (CPack)
