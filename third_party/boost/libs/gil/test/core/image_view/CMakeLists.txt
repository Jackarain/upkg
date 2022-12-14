#
# Copyright (c) 2018 Mateusz Loskot <mateusz at loskot dot net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
foreach(_name
  axis_iterator
  collection
  concepts
  derived_view_type
  dynamic_step
  is_planar
  is_1d_traversable
  iterator
  planar_rgba_view
  reverse_iterator
  subimage_view
  view_is_basic
  view_is_mutable
  view_is_step
  view_type
  x_iterator
  xy_locator
  y_iterator
  view_type_from_pixel)
  set(_test t_core_image_view_${_name})
  set(_target test_core_image_view_${_name})

  add_executable(${_target} "")
  target_sources(${_target} PRIVATE ${_name}.cpp)
  target_link_libraries(${_target}
    PRIVATE
      gil_compile_options
      gil_include_directories
      gil_dependencies)
  target_compile_definitions(${_target} PRIVATE BOOST_GIL_USE_CONCEPT_CHECK)
  add_test(NAME ${_test} COMMAND ${_target})

  unset(_name)
  unset(_target)
  unset(_test)
endforeach()
