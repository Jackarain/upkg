
// Copyright 2017, 2019 Peter Dimov.
//
// Distributed under the Boost Software License, Version 1.0.
//
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt

// See library home page at http://www.boost.org/libs/system

#if defined(__GNUC__) && __GNUC__ >= 5 && __cplusplus >= 201103L
# pragma GCC diagnostic error "-Wsuggest-override"
#endif

#include <boost/config.hpp>

#if defined( BOOST_GCC ) && BOOST_GCC < 40600
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

#include <boost/system/system_error.hpp>
#include <boost/system/error_code.hpp>
#include <boost/core/lightweight_test.hpp>
#include <cerrno>

int main()
{
    boost::system::error_category const & bt = boost::system::generic_category();

    int ev = ENOENT;

    boost::system::error_code bc( ev, bt );

    BOOST_TEST_EQ( bc.value(), ev );
    BOOST_TEST_EQ( &bc.category(), &bt );

    boost::system::error_condition bn = bt.default_error_condition( ev );

    BOOST_TEST_EQ( bn.value(), ev );
    BOOST_TEST_EQ( &bn.category(), &bt );

    BOOST_TEST( bt.equivalent( ev, bn ) );

    BOOST_TEST( bc == bn );

    boost::system::system_error x( bc, "prefix" );

    BOOST_TEST_EQ( x.code(), bc );
    BOOST_TEST_EQ( std::string( x.what() ), "prefix: " + bc.what() );

    return boost::report_errors();
}
