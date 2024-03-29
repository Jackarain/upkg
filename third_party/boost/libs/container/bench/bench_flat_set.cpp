//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2013-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include "boost/container/set.hpp"
#include "boost/container/flat_set.hpp"
#include "boost/container/devector.hpp"
#include "bench_set.hpp"

int main()
{
   using namespace boost::container;

   fill_range_ints();
   fill_range_strings();

   //flat_set vs set
   launch_tests< flat_set<int> , set<int> >
      ("flat_set<int>", "set<int>");
   launch_tests< flat_set<string> , set<string> >
      ("flat_set<string>", "set<string>");
   //flat_multiset vs flat_multiset(devector)
   launch_tests< flat_set<int>, flat_set<int, std::less<int>, devector<int> > >
      ("flat_set<int>", "flat_set(devector)<int>");
   launch_tests< flat_set<string>, flat_set<string, std::less<string>, devector<string> > >
      ("flat_set<string>", "flat_set(devector)<string>");

   return 0;
}
