# Copyright 2018-2021 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.20)

project(boost_system VERSION "${BOOST_SUPERPROJECT_VERSION}" LANGUAGES CXX)

add_library(boost_system INTERFACE)
add_library(Boost::system ALIAS boost_system)

target_include_directories(boost_system INTERFACE include)

target_link_libraries(boost_system
  INTERFACE
    Boost::assert
    Boost::config
    Boost::throw_exception
    Boost::variant2
    Boost::winapi
)

if(CMAKE_VERSION VERSION_GREATER 3.18 AND CMAKE_GENERATOR MATCHES "Visual Studio")

  file(GLOB_RECURSE boost_system_IDEFILES CONFIGURE_DEPENDS include/*.hpp)
  source_group(TREE ${PROJECT_SOURCE_DIR}/include FILES ${boost_system_IDEFILES} PREFIX "Header Files")
  list(APPEND boost_system_IDEFILES extra/boost_system.natvis)
  target_sources(boost_system PRIVATE ${boost_system_IDEFILES})

endif()

if(BUILD_TESTING)

  add_subdirectory(test)

endif()
