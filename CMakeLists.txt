cmake_minimum_required(VERSION 3.0.0)
set(PROJECTNAME rocs)
set(PROJECTUSER rocs)

project(${PROJECTNAME})

add_subdirectory(rocsmq)
add_subdirectory(rocsmq-daemonizer)

add_subdirectory(rocsmq-server)
add_subdirectory(rocsmq-cron)
add_subdirectory(rocsmq-speech)
add_subdirectory(rocsmq-sphinx)
add_subdirectory(rocsmq-tclclient)
add_subdirectory(rocsmq-luaclient)
add_subdirectory(rocsmq-cli)
add_subdirectory(rocsmq-canbus)
add_subdirectory(rocsmq-i2cbus)
add_subdirectory(rocsmq-gpio)

SET(CPACK_GENERATOR "RPM")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jens Rapp")
SET(CPACK_INSTALL_PREFIX "/opt/hallowelt")

INCLUDE (CPack)
