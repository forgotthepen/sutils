#include "string_utilities.hpp"

#include <iostream>
#include <cassert>
#include <chrono>

void test_str_weak_iterator() {
  constexpr auto s_4 = sutils::helpers::str_weak_ref("acd78");

  static_assert( s_4.begin() == s_4.begin(), "error");
  static_assert( s_4.rbegin() == s_4.rbegin(), "error");
  static_assert( s_4.end() == s_4.end(), "error");
  static_assert( s_4.rend() == s_4.rend(), "error");
  
  static_assert( s_4.begin() != s_4.end(), "error");
  static_assert( s_4.rbegin() != s_4.rend(), "error");

  static_assert( (s_4.begin() - s_4.begin()) == 0, "error");
  static_assert( (s_4.end() - s_4.end()) == 0, "error");
  static_assert( (s_4.rbegin() - s_4.rbegin()) == 0, "error");
  static_assert( (s_4.rend() - s_4.rend()) == 0, "error");
  
  static_assert( (s_4.rbegin() - s_4.rend()) == -5, "error"); ////////////////////////////

  static_assert( (s_4.begin() - 1) == s_4.begin(), "error");
  static_assert( (s_4.begin() - 2) == s_4.begin(), "error");
  static_assert( (s_4.begin() - 500) == s_4.begin(), "error");

  static_assert( (s_4.begin() + 0) == s_4.begin(), "error");
  static_assert( (s_4.begin() - 0) == s_4.begin(), "error");
  static_assert( (s_4.begin() + 5) == s_4.end(), "error");
  static_assert( (s_4.begin() + 6) == s_4.end(), "error");
  static_assert( (s_4.begin() + 7) == s_4.end(), "error");
  static_assert( (s_4.begin() + 500) == s_4.end(), "error");

  static_assert( (s_4.end() + 0) == s_4.end(), "error");
  static_assert( (s_4.end() - 0) == s_4.end(), "error");
  static_assert( (s_4.end() + 1) == s_4.end(), "error");
  static_assert( (s_4.end() + 500) == s_4.end(), "error");

  static_assert( (s_4.end() - 5) == s_4.begin(), "error");
  static_assert( (s_4.end() - 500) == s_4.begin(), "error");
    
  static_assert( (s_4.rbegin() + 0) == s_4.rbegin(), "error");
  static_assert( (s_4.rbegin() - 0) == s_4.rbegin(), "error");
  static_assert( (s_4.rbegin() - 1) == s_4.rbegin(), "error");
  static_assert( (s_4.rbegin() - 500) == s_4.rbegin(), "error");

  static_assert( (s_4.rend() + 0) == s_4.rend(), "error");
  static_assert( (s_4.rend() - 0) == s_4.rend(), "error");
  static_assert( (s_4.rend() + 1) == s_4.rend(), "error");
  static_assert( (s_4.rend() + 500) == s_4.rend(), "error");


  static_assert(s_4.begin().operator->() == s_4.data(), "error");
  static_assert(s_4.rbegin().operator->() == (s_4.data() + s_4.size() - 1), "error");

  static_assert(s_4.end().operator->() == (s_4.data() + s_4.size()), "error");
  static_assert((--s_4.rend()).operator->() == s_4.data(), "error");
  static_assert((s_4.rend() - 1).operator->() == s_4.data(), "error");

  static_assert( *s_4.rbegin() == '8', "error");
  static_assert( *(s_4.rbegin() + 1) == '7', "error");
  static_assert( *(s_4.rend() - 1) == 'a', "error");
  constexpr auto xb1 = *s_4.begin()++;

  static_assert( *s_4.begin()++ == 'a', "error");
  static_assert( *++s_4.begin() == 'c', "error");

  static_assert( *s_4.rbegin()++ == '8', "error");
  static_assert( *++s_4.rbegin() == '7', "error");

  static_assert( s_4.begin() + 1 == ++s_4.begin(), "error");
  static_assert( s_4.rbegin() + 1 == ++s_4.rbegin(), "error");
  static_assert( *(s_4.begin() + 1) == *++s_4.begin(), "error");

  static_assert( (s_4.rend() - s_4.rbegin()) == 5, "error");
  static_assert( s_4.rend() - (s_4.rbegin() + 2) == 3, "error");
  
  static_assert( (s_4.rbegin() + 3) - s_4.rbegin() == 3, "error");
  static_assert( (++s_4.rbegin() + 3) - s_4.rbegin() == 4, "error");
  static_assert( ++s_4.rbegin() - s_4.rbegin() == 1, "error");
  static_assert( (s_4.rbegin() - s_4.rend()) == -5, "error");
  static_assert( s_4.rbegin() - (s_4.rend() - 2) == -3, "error");
  
  constexpr auto s_empty = sutils::helpers::str_weak_ref("");
  static_assert( s_empty.begin() == s_empty.end(), "error");
  static_assert( s_empty.rbegin() == s_empty.rend(), "error");

}

void test_substr() {
  constexpr auto asd = static_cast<std::ptrdiff_t>(static_cast<size_t>(-1));
  constexpr auto s_1 = sutils::helpers::str_weak_ref("abc");
  static_assert(s_1.size() == 3, "error");
  static_assert(s_1.substr(0).size() == 3, "error");
  static_assert(s_1.substr(1).size() == 2, "error");
  static_assert(s_1.substr(2).size() == 1, "error");
  static_assert(s_1.substr(3).size() == 0, "error");
  static_assert(s_1.substr(500).size() == 0, "error");
  
  static_assert(*s_1.substr(0).data() == 'a', "error");
  static_assert(*s_1.substr(1).data() == 'b', "error");
  static_assert(*s_1.substr(2).data() == 'c', "error");
  static_assert(*s_1.substr(3).data() == '\0', "error");
  static_assert(*s_1.substr(500).data() == '\0', "error");

  static_assert(s_1.substr(0, 500).size() == 3, "error");
  static_assert(s_1.substr(2, 500).size() == 1, "error");
  
  static_assert(*s_1.substr(0, 1).data() == 'a', "error");
  static_assert(*s_1.substr(1, 1).data() == 'b', "error");
  static_assert(*s_1.substr(2, 1).data() == 'c', "error");
  static_assert(*s_1.substr(2, 500).data() == 'c', "error");
  static_assert(*s_1.substr(3, 1).data() == '\0', "error");
  static_assert(*s_1.substr(3, 100).data() == '\0', "error");
  static_assert(*s_1.substr(500, 100).data() == '\0', "error");
  
  static_assert(*s_1.substr(2).data() == 'c', "error");

  static_assert(s_1.substr(100, 500).size() == 0, "error");
  static_assert(s_1.substr(0, 1).size() == 1, "error");
  static_assert(s_1.substr(1, 1).size() == 1, "error");
  static_assert(s_1.substr(2, 1).size() == 1, "error");
  
  static_assert(s_1.substr(100, 0).size() == 0, "error");
  static_assert(s_1.substr(0, 0).size() == 0, "error");
  static_assert(s_1.substr(1, 0).size() == 0, "error");
  static_assert(s_1.substr(2, 0).size() == 0, "error");
  static_assert(s_1.substr(3, 0).size() == 0, "error");
  static_assert(s_1.substr(4, 0).size() == 0, "error");
  
  static_assert(*s_1.substr(100, 0).data() == '\0', "error");
  static_assert(*s_1.substr(0, 0).data() == '\0', "error");
  static_assert(*s_1.substr(1, 0).data() == '\0', "error");
  static_assert(*s_1.substr(2, 0).data() == '\0', "error");
  static_assert(*s_1.substr(3, 0).data() == '\0', "error");
  static_assert(*s_1.substr(4, 0).data() == '\0', "error");

  static_assert(s_1.substr(0, 2222222222222222222ULL).size() == 3, "error");

  static_assert(s_1.substr(-1).size() == 1, "error");
  static_assert(s_1.substr(-2).size() == 2, "error");
  static_assert(s_1.substr(-3).size() == 3, "error");
  static_assert(s_1.substr(-4).size() == 3, "error");
  static_assert(s_1.substr(-5).size() == 3, "error");
  static_assert(s_1.substr(-500).size() == 3, "error");
  static_assert(s_1.substr(-500, 3).size() == 3, "error");
  static_assert(s_1.substr(-500, 4).size() == 3, "error");
  static_assert(s_1.substr(-500, 5).size() == 3, "error");
  static_assert(s_1.substr(-500, 500).size() == 3, "error");
  static_assert(s_1.substr(0, 0).size() == 0, "error");
  static_assert(s_1.substr(-1, 0).size() == 0, "error");

}

void test_cmp() {
  constexpr auto s_1 = sutils::helpers::str_weak_ref("abc");
  auto aa = s_1.begin();
#ifdef SUTILS_HAS_STRVIEW
  constexpr auto s_2 = sutils::helpers::str_weak_ref(std::string_view("abc"));

  assert(sutils::cmp(s_1, s_2) && "error");
#endif
  // assert(!sutils::cmp("hello", L"hello") && "error");
  assert(sutils::cmp("hello", "hello") && "error");
  assert(sutils::cmp("hello", "Hello", true) && "error");
  assert(sutils::cmp("hello", "HEllo", true) && "error");
  assert(sutils::cmp("hello", "hellO", true) && "error");
  assert(sutils::cmp("hello", "heLLo", true) && "error");
  assert(sutils::cmp("hello", "heLLO", true) && "error");
  assert(sutils::cmp("hello", "HELLO", true) && "error");
  assert(!sutils::cmp("hello", "hell") && "error");
  assert(!sutils::cmp("hello", "he") && "error");
  assert(!sutils::cmp("hello", "h") && "error");
  assert(!sutils::cmp("hello", "") && "error");
  assert(sutils::cmp("", "") && "error");
  assert(!sutils::cmp("", "hee") && "error");
  assert(!sutils::cmp("hee", "") && "error");
  assert(!sutils::cmp("hel", "hee") && "error");
  
}

void test_starts() {
  // assert(!sutils::starts("hello", L"hello") && "error");
  assert(sutils::starts("hello", "hello") && "error");
  assert(sutils::starts("hello", "hell") && "error");
  assert(sutils::starts("hello", "he") && "error");
  assert(sutils::starts("hello", "h") && "error");
  assert(!sutils::starts("hello", "ell") && "error");
  assert(!sutils::starts("hello", "") && "error");
  assert(!sutils::starts("", "hello") && "error");
  assert(!sutils::starts("hello", "llo") && "error");
  assert(!sutils::starts("hello", "9ello") && "error");
}

void test_ends() {
  // assert(!sutils::ends("hello", L"hello") && "error");
  assert(sutils::ends("hello", "hello") && "error");
  assert(sutils::ends("hello", "ello") && "error");
  assert(sutils::ends("hello", "lo") && "error");
  assert(sutils::ends("hello", "o") && "error");
  assert(!sutils::ends("hello", "hel") && "error");
  assert(!sutils::ends("hello", "") && "error");
  assert(!sutils::ends("", "hello") && "error");
  assert(!sutils::ends("hello", "h") && "error");
  assert(!sutils::ends("hello", "9ello") && "error");
  assert(!sutils::ends("hello", "hell3") && "error");
}

void test_replace_all() {
  {
    auto result = sutils::replace_all(L"aa||aaa||aaaa||a||aaaaaaa||zzz", L"||", L"-NICE!-");
    assert(result == L"aa-NICE!-aaa-NICE!-aaaa-NICE!-a-NICE!-aaaaaaa-NICE!-zzz" && "error");
  }

  {
    auto result = sutils::replace_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "||", "");
    assert(result == "aaaaaaaaaaaaaaaaazzz" && "error");
  }

  {
    auto result = sutils::replace_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "||", "(");
    assert(result == "aa(aaa(aaaa(a(aaaaaaa(zzz" && "error");
  }
  
  {
    auto result = sutils::replace_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "", "==");
    assert(result == "aa||aaa||aaaa||a||aaaaaaa||zzz" && "error");
  }
  
  {
    auto result = sutils::replace_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "===", "@@");
    assert(result == "aa||aaa||aaaa||a||aaaaaaa||zzz" && "error");
  }
  
  {
    auto result = sutils::replace_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "", "");
    assert(result == "aa||aaa||aaaa||a||aaaaaaa||zzz" && "error");
  }
  
  {
    auto result = sutils::replace_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "", "AAAA");
    assert(result == "aa||aaa||aaaa||a||aaaaaaa||zzz" && "error");
  }
  
}

void test_replace_all_backward() {
/////////////////////////////////////////////////// TODO
}

void test_find_all() {
/////////////////////////////////////////////////// TODO
}

void test_find_all_backwards() {
/////////////////////////////////////////////////// TODO
  {
    auto result = sutils::find_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "||");
    assert((result.size() == 5) && "error");
  }

  {
    auto result = sutils::find_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "");
    assert((result.size() == 0) && "error");
  }

  {
    auto result = sutils::find_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "nope");
    assert((result.size() == 0) && "error");
  }

  {
    auto result = sutils::find_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "aa||aaa||aaaa||a||aaaaaaa||zzz");
    assert((result.size() == 1) && "error");
  }

  {
    auto result = sutils::find_all("aa||aaa||aaaa||a||aaaaaaa||zzz", "aa||aaa||aaaa||a||aaaaaaa||zzz1111111111");
    assert((result.size() == 0) && "error");
  }

}

void test_split() {
/////////////////////////////////////////////////// TODO
  {
    auto result = sutils::split("abc||zx||dddd||zzz", "||");
    assert((result.size() == 4) && "error");
    assert(sutils::cmp(result[result.size() - 1], "zzz") && "error");
    assert(sutils::cmp(result[0], "abc") && "error");
  }

  {
    auto result = sutils::split("abc||zx||dddd||", "||");
    assert((result.size() == 3) && "error");
    assert(sutils::cmp(result[result.size() - 1], "dddd") && "error");
    assert(sutils::cmp(result[0], "abc") && "error");
  }

  {
    auto result = sutils::split("abc||zx||dddd||", "||", true);
    assert((result.size() == 4) && "error");
    assert(sutils::cmp(result[result.size() - 1], "") && "error");
    assert(sutils::cmp(result[0], "abc") && "error");
  }

  {
    auto result = sutils::split("abc||zx||||", "||", true);
    assert((result.size() == 4) && "error");
    assert(sutils::cmp(result[result.size() - 1], "") && "error");
    assert(sutils::cmp(result[0], "abc") && "error");
    assert(sutils::cmp(result[2], "") && "error");
  }

  {
    auto result = sutils::split("abc||zx||dddd", "||");
    assert((result.size() == 3) && "error");
    assert(sutils::cmp(result[result.size() - 1], "dddd") && "error");
    assert(sutils::cmp(result[0], "abc") && "error");
  }
  
}

int main() {
  auto t1 = std::chrono::high_resolution_clock::now();
  
  test_str_weak_iterator();
  test_substr();
  test_cmp();
  test_starts();
  test_replace_all();
  test_replace_all_backward();
  test_find_all();
  test_find_all_backwards();
  test_split();

  auto t2 = std::chrono::high_resolution_clock::now();
  auto d_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  auto d_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
  std::cout << "done in " << d_us.count() << " us, " << d_ms.count() << " ms" << std::endl;
  return 0;
}
