#This file should be seen as a -*-mode:cmake-*-
#
# This file could be used as a template to create cmake projects for 
# creating webots / libamarsi-quad controllers.
# 
# This template relies on the FindWebots.cmake macro. So in order to
# use this in your own controllers you should either
#
# - Copy this file and Modules/FindWebots.cmake in your own controller
#   directory
# - Copy FindWebots.cmake in a standard location (for example its
#   /usr/share/cmake-2.6/Modules on Debian/Ubuntu ), or in a location
#   within your CMAKE_MODULE_PATH
#
#
# Created on : 15 jum. 2010
# author : Alexandre Tuleu
# Copyright 2010 EPFL Ecole Polytechnique Federale de Lausanne
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

#You need to specify a project name
project(basic CXX)
cmake_minimum_required(VERSION 2.6)

#Indicate here the name of your controller. Reminder : Webots want the
#name of the controller to be the same of the directory for example if
#you want to create the controller foo, it should be placed in
#directory controllers/foo and the files controllers/foo/foo.cpp
#should exist
set(CONTROLLER_NAME basic)

# Just add your sources file here, please remind that you need to have
# a file called your_controller_name.cpp
set(SOURCES BasicController.cpp basic.cpp)
# Just add your header files here
set(HEADERS BasicController.h)

#If you want to link against other libraries, please use these
#variables (For libraries, you should provide the full path on your
#system, without flags) for exemple if you want to use libfoo.so.1.2.3
#and libbar.4.5.6 you should do :
#set(EXTERNAL_INCLUDE /usr/include/foo-1.2.3 /usr/local/include/bar-4.5.6)
#set(EXTERNAL_LIBRARIES /usr/lib/libfoo.so.1.2.3 /usr/local/lib/libbar.so.4.5.6)
set(EXTERNAL_INCLUDE )
set(EXTERNAL_LIBRARIES )


#this line could be removed if you install FindWebots.cmake somewhere
#inside your CMAKE_MODULE_PATH. If you don't, please remind to also
#copy Modules/FindWebots.cmake in your controller directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/Modules)
find_package(Webots REQUIRED)

################################################################################
# DO NOT MODIFY BELOW, PATHS DEPENDS ON YOUR INSTALLATION
################################################################################
find_library(LIBAMARSI_QUAD_LIBRARY libamarsi-quad 
             PATHS "lib")

if(NOT LIBAMARSI_QUAD_LIBRARY)
  message(FATAL_ERROR "Could not found libamarsi-quad library in lib, please check your installation")
endif(NOT LIBAMARSI_QUAD_LIBRARY)

include_directories(${WEBOTS_INCLUDE_CPP_DIR} "include/libamarsi-quad-0.3" 
                    ${EXTERNAL_INCLUDE})

add_executable(${CONTROLLER_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${CONTROLLER_NAME} ${WEBOTS_LIBRARIES} 
                      ${LIBAMARSI_QUAD_LIBRARY} ${EXTERNAL_LIBRARIES})
################################################################################
# DO NOT MODIFY ABOVE, PATHS DEPENDS ON YOUR INSTALLATION
################################################################################
