#include <algorithm>
#include <cassert>
#include <iostream>
#include <initializer_list>
#include <map>

#include "map.h"

static const std::initializer_list<std::pair<const int, int>> il = {
  { 0,  0}, {-3,  9}, {-4, 16}, {-1,  1}, {-2, 4}, {2, 4}, {0, 0}, {-1, 1},
  {-3,  9}, { 5, 25}, {-3,  9}, { 7, 49}, {6, 36}
};

static const std::map<int, int> benchmark(il);

static void
test_emptiness(map<int, int>& m) {
  const map<int, int>& cb = m;
  assert(m.empty());
  assert(m.size() == 0);
  assert(m . begin() == m . end());
  assert(cb. begin() == cb. end());
  assert(m .cbegin() == m .cend());
  assert(m . rbegin() == m . rend());
  assert(cb. rbegin() == cb. rend());
  assert(m .crbegin() == m .crend());
}

template <typename Map>
static void
test_equality(map<int, int>& lhs, const Map& rhs) {

  const map<int, int>& clhs = lhs;

  // Test that iterator and const_iterator are (in)equally comparable.
  assert(   lhs.begin() == clhs.begin());
  assert(  clhs.begin() ==  lhs.begin());
  assert(!( lhs.begin() != clhs.begin()));
  assert(!(clhs.begin() !=  lhs.begin()));

  assert(lhs.empty() == rhs.empty());
  assert(lhs.size() == rhs.size());
  assert(std::equal( lhs.  begin(),  lhs.  end(), rhs.begin()));
  assert(std::equal(clhs.  begin(), clhs.  end(), rhs.begin()));
  assert(std::equal( lhs. cbegin(),  lhs. cend(), rhs.begin()));
  assert(std::equal( lhs. rbegin(),  lhs. rend(), rhs.rbegin()));
  assert(std::equal(clhs. rbegin(), clhs. rend(), rhs.rbegin()));
  assert(std::equal( lhs.crbegin(),  lhs.crend(), rhs.rbegin()));
}

static void
test_copying(const map<int, int>& m) {

  map<int, int> m1 = m;
  test_equality(m1, m);

  map<int, int> m2 = std::move(m1);
  test_equality(m2, m);
  test_emptiness(m1);

  m1 = m2;
  test_equality(m1, m);

  m2 = map<int, int>();
  test_emptiness(m2);

  m2 = std::move(m1);
  test_equality(m2, m);
  test_emptiness(m1);
}

static void
test_construction() {

  map<int, int> m1;
  test_emptiness(m1);
  test_copying(m1);

  map<int, int> m2(il.begin(), il.end());
  test_equality(m2, benchmark);
  test_copying(m2);

  map<int, int> m3(il);
  test_equality(m3, benchmark);
  test_copying(m3);
}

template <typename Ib, typename Im>
static void
test_searching(Ib b, Ib b_end, Im m, Im m_end) {
  assert((b == b_end) == (m == m_end));
  if (b != b_end)
    assert(*b == *m);
}

static void
test_insertion_erasure_and_searching() {

  map<int, int> m;
  const auto& cb = m;

  std::map<int, int> sm;
  const auto& csm = sm;

  int i = 0;

  for (const auto& x : il) {

    if (i++ & 2)
      m.insert(x);
    else
      m[x.first] = x.second;

    sm.insert(x);

    test_equality(m, sm);
    test_copying(m);

    test_searching(m .lower_bound(1), m .end(), sm .lower_bound(1), sm .end());
    test_searching(m .lower_bound(2), m .end(), sm .lower_bound(2), sm .end());

    test_searching(cb.lower_bound(1), cb.end(), csm.lower_bound(1), csm.end());
    test_searching(cb.lower_bound(2), cb.end(), csm.lower_bound(2), csm.end());

    test_searching(m .upper_bound(1), m .end(), sm .upper_bound(1), sm .end());
    test_searching(m .upper_bound(2), m .end(), sm .upper_bound(2), sm .end());

    test_searching(cb.upper_bound(1), cb.end(), csm.upper_bound(1), csm.end());
    test_searching(cb.upper_bound(2), cb.end(), csm.upper_bound(2), csm.end());

    test_searching(m .find(1), m .end(), sm .find(1), sm .end());
    test_searching(m .find(2), m .end(), sm .find(2), sm .end());

    test_searching(cb.find(1), cb.end(), csm.find(1), csm.end());
    test_searching(cb.find(2), cb.end(), csm.find(2), csm.end());
  }

  for (const auto& x : il) {

    auto im  =  m.find(x.first);
    auto ism = sm.find(x.first);

    assert((im == m.end()) == (ism == sm.end()));

    if (im != m.end()) {
      im  =  m.erase(im);
      ism = sm.erase(ism);
    }

    assert((im == m.end()) == (ism == sm.end()));
    assert(im == m.end() || *im == *ism);
  }

  m.insert(il.begin(), il.end());
  sm = il;
  test_equality(m, sm);

  assert(m.erase(m.begin(), m.end()) == m.end());
  test_emptiness(m);

  m = il;
  auto it = --m.end();
  assert(m.erase(++m.begin(), it) == it);

  sm = il;
  sm.erase(++sm.begin(), --sm.end());

  test_equality(m, sm);
}

static void
test_swap() {

  map<int, int> m1;
  map<int, int> m2;

  for (int i = 0; i < 5; ++i) {
    m1[i] = i;
    m2[i+5] = i + 6;
  }

  m1.swap(m2);

  for (int i = 0; i < 5; ++i) {
    assert(m2[i] == i);
    assert(m1[i+5] == i + 6);
  }
}

int main() {
  test_construction();
  test_insertion_erasure_and_searching();
  test_swap();
  std::cout << "OK\n";
}
