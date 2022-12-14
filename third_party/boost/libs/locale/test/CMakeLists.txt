# Copyright 2022 Alexander Grund
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

if(NOT TARGET tests)
  add_custom_target(tests)
endif()

# Library config as determined by configure tests
set(boost_locale_defines "")
if(BOOST_LOCALE_ENABLE_ICONV)
  list(APPEND boost_locale_defines BOOST_LOCALE_WITH_ICONV=1)
endif()
if(BOOST_LOCALE_ENABLE_ICU)
  list(APPEND boost_locale_defines BOOST_LOCALE_WITH_ICU=1)
endif()
if(NOT BOOST_LOCALE_ENABLE_STD)
  list(APPEND boost_locale_defines BOOST_LOCALE_NO_STD_BACKEND=1)
endif()
if(NOT BOOST_LOCALE_ENABLE_WINAPI)
  list(APPEND boost_locale_defines BOOST_LOCALE_NO_WINAPI_BACKEND=1)
endif()
if(NOT BOOST_LOCALE_ENABLE_POSIX)
  list(APPEND boost_locale_defines BOOST_LOCALE_NO_POSIX_BACKEND=1)
endif()

function(boost_locale_add_test name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "COMPILE_ONLY" "SRC;WORKING_DIRECTORY" "ARGS")

  if(NOT ARG_SRC)
    set(ARG_SRC ${name}.cpp)
  endif()
  set(name ${PROJECT_NAME}-${name})

  add_executable(${name} ${ARG_SRC})
  add_dependencies(tests ${name})
  target_link_libraries(${name} PRIVATE Boost::locale)
  target_compile_definitions(${name} PRIVATE ${boost_locale_defines})
  target_compile_options(${name} PRIVATE ${BOOST_LOCALE_WARNING_OPTIONS})
  if(BOOST_LOCALE_ENABLE_ICU)
    target_include_directories(${name} PRIVATE ${ICU_INCLUDE_DIR})
  endif()

  if(ARG_WORKING_DIRECTORY)
    set_target_properties(${name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${ARG_WORKING_DIRECTORY}")
  endif()

  if(NOT ARG_COMPILE_ONLY)
    add_test(NAME ${name} COMMAND ${name} ${ARG_ARGS})
    if(ARG_WORKING_DIRECTORY)
      set_tests_properties(${name} PROPERTIES WORKING_DIRECTORY "${ARG_WORKING_DIRECTORY}")
    endif()
  endif()
endfunction()

boost_locale_add_test(test_config)
# Shared
boost_locale_add_test(test_utf)
boost_locale_add_test(test_date_time)
boost_locale_add_test(test_ios_prop)
boost_locale_add_test(test_codecvt)
boost_locale_add_test(test_codepage_converter)
if(MSVC)
  boost_locale_add_test(test_codepage COMPILE_ONLY)
else()
  boost_locale_add_test(test_codepage)
endif()
boost_locale_add_test(test_message WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
boost_locale_add_test(test_generator)
# icu
boost_locale_add_test(test_collate)
boost_locale_add_test(test_convert)
if(BOOST_LOCALE_ENABLE_ICU)
  boost_locale_add_test(test_boundary COMPILE_ONLY)
  boost_locale_add_test(test_formatting COMPILE_ONLY)
else()
  boost_locale_add_test(test_boundary)
  boost_locale_add_test(test_formatting)
endif()
boost_locale_add_test(test_icu_vs_os_timezone)
# winapi
boost_locale_add_test(test_winapi_collate)
boost_locale_add_test(test_winapi_convert)
boost_locale_add_test(test_winapi_formatting)
# posix
boost_locale_add_test(test_posix_collate)
boost_locale_add_test(test_posix_convert)
boost_locale_add_test(test_posix_formatting)
# std
boost_locale_add_test(test_std_collate)
boost_locale_add_test(test_std_convert)
boost_locale_add_test(test_std_formatting)
