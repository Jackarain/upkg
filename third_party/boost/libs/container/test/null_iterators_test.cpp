//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2014-2014. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/stable_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/container/string.hpp>
#include <boost/container/list.hpp>
#include <boost/container/slist.hpp>
#include <boost/container/map.hpp>
#include <boost/container/set.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/intrusive/detail/mpl.hpp>

#include <boost/core/lightweight_test.hpp>
#include <boost/static_assert.hpp>
#include <cstring>
#include <iterator>
#include <new>

using namespace boost::container;

typedef boost::container::dtl::aligned_storage<sizeof(void*)*4>::type buffer_t;

static buffer_t buffer_0x00;
static buffer_t buffer_0xFF;

template<class Iterator>
const Iterator &on_0x00_buffer()
{
   BOOST_STATIC_ASSERT(sizeof(buffer_t) >= sizeof(Iterator));
   return * ::new(std::memset(&buffer_0x00, 0x00, sizeof(buffer_0x00))) Iterator();
}

template<class Iterator>
const Iterator &on_0xFF_buffer()
{
   BOOST_STATIC_ASSERT(sizeof(buffer_t) >= sizeof(Iterator));
   return * ::new(std::memset(&buffer_0xFF, 0xFF, sizeof(buffer_0xFF))) Iterator();
}

namespace boost {
namespace container {
namespace test {

BOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(reverse_iterator)
BOOST_INTRUSIVE_INSTANTIATE_DEFAULT_TYPE_TMPLT(const_reverse_iterator)

}}}   //namespace boost::container::test {

template<class RandomAccessIterator>
void check_plus_zero_impl(RandomAccessIterator it)
{
  RandomAccessIterator cpy(it + 0);
  BOOST_TEST(cpy == it);
}

template<class Container, class Category>
void check_plus_zero(const Category&)
{}

template<class Container>
void check_plus_zero(const std::random_access_iterator_tag&)
{
  check_plus_zero_impl(typename Container::iterator());
  check_plus_zero_impl(typename Container::const_iterator());
  check_plus_zero_impl(typename Container::reverse_iterator());
  check_plus_zero_impl(typename Container::const_reverse_iterator());
  Container c;
  check_plus_zero_impl(c.begin());
  check_plus_zero_impl(c.cbegin());
  check_plus_zero_impl(c.rbegin());
  check_plus_zero_impl(c.crbegin());
}

template<class Container>
void check_plus_zero()
{
  typedef typename Container::iterator iterator;
  typedef typename std::iterator_traits<iterator>::iterator_category category;
  category tag;
  check_plus_zero<Container>(tag);
}

template<class Container>
void check_null_iterators()
{
   typedef typename Container::iterator               iterator;
   typedef typename Container::const_iterator         const_iterator;
   typedef BOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT
      (boost::container::test::, Container
      ,reverse_iterator, iterator)                    reverse_iterator;
   typedef BOOST_INTRUSIVE_OBTAIN_TYPE_WITH_DEFAULT
      (boost::container::test::, Container
      ,const_reverse_iterator, const_iterator)        const_reverse_iterator;

   BOOST_TEST(on_0xFF_buffer<iterator>()               == on_0x00_buffer<iterator>());
   BOOST_TEST(on_0xFF_buffer<const_iterator>()         == on_0x00_buffer<const_iterator>());
   BOOST_TEST(on_0xFF_buffer<reverse_iterator>()       == on_0x00_buffer<reverse_iterator>());
   BOOST_TEST(on_0xFF_buffer<const_reverse_iterator>() == on_0x00_buffer<const_reverse_iterator>());
}

int main()
{
   check_null_iterators< vector<int> >();
   check_plus_zero< vector<int> >();
   check_null_iterators< deque<int> >();
   check_plus_zero< deque<int> >();
   check_null_iterators< stable_vector<int> >();
   check_plus_zero< stable_vector<int> >();
   check_null_iterators< static_vector<int, 1> >();
   check_plus_zero< static_vector<int, 1> >();
   check_null_iterators< string >();
   check_plus_zero< string >();
   check_null_iterators< list<int> >();
   check_plus_zero< list<int> >();
   check_null_iterators< slist<int> >();
   check_plus_zero< slist<int> >();
   check_null_iterators< map<int, int> >();
   check_plus_zero< map<int, int> >();
   check_null_iterators< multimap<int, int> >();
   check_plus_zero< multimap<int, int> >();
   check_null_iterators< set<int> >();
   check_plus_zero< set<int> >();
   check_null_iterators< multiset<int> >();
   check_plus_zero< multiset<int> >();
   check_null_iterators< flat_set<int> >();
   check_plus_zero< flat_set<int> >();
   check_null_iterators< flat_multiset<int> >();
   check_plus_zero< flat_multiset<int> >();
   check_null_iterators< flat_map<int, int> >();
   check_plus_zero< flat_map<int, int> >();
   check_null_iterators< flat_multimap<int, int> >();
   check_plus_zero< flat_multimap<int, int> >();

   return boost::report_errors();
}
