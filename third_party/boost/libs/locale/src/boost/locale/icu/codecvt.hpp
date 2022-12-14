//
// Copyright (c) 2009-2011 Artyom Beilis (Tonkikh)
//
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_LOCALE_IMPL_ICU_CODECVT_HPP
#define BOOST_LOCALE_IMPL_ICU_CODECVT_HPP

#include <boost/locale/config.hpp>
#include <boost/locale/util.hpp>
#include <string>

namespace boost {
namespace locale {
namespace impl_icu {
    BOOST_LOCALE_DECL
    util::base_converter *create_uconv_converter(std::string const &encoding);

} // impl_icu
} // locale
} // boost

#endif
