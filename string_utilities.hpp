
#ifndef SUTILS_CPP_VERSION
  #define SUTILS_CPP_VERSION __cplusplus
#endif

// https://en.cppreference.com/w/cpp/preprocessor/replace
#if SUTILS_CPP_VERSION < 201402L
  #error "C++14 or greater is required"
#endif

#include <cstdint>
#include <algorithm>
#include <cctype>
#include <utility>
#include <type_traits>
#include <iterator>
#include <cstddef>
#include <limits>
#include <vector>
#include <string>

#if SUTILS_CPP_VERSION >= 201703L
  #include <string_view>
  #define SUTILS_HAS_STRVIEW
#endif

#if SUTILS_CPP_VERSION >= 202002L
  #define SUTILS_CONSTEXPR_DTOR constexpr
#else
  #define SUTILS_CONSTEXPR_DTOR
#endif


namespace sutils {
namespace helpers {

  // alternative to std::basic_stringview
  // container implementation:
  // https://en.cppreference.com/w/cpp/container
  template<class TC>
  class str_weak_ref_basic {
  public:
    // https://en.cppreference.com/w/cpp/named_req/Contfainer
    using value_type = typename std::remove_cv<TC>::type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using difference_type = std::ptrdiff_t; // must be signed
    using size_type = size_t; // must be unsigned
    
    static constexpr auto diff_max = static_cast<unsigned long long>(std::numeric_limits<difference_type>::max());
    static constexpr auto size_max = static_cast<unsigned long long>(std::numeric_limits<size_type>::max());
    static_assert(diff_max < size_max, "max value of difference_type must be < max value of size_type");
    
    // iterator implementation:
    // https://en.cppreference.com/w/cpp/iterator
    template<bool reverse>
    class str_weak_iterator {
    public:
      // https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
      // https://en.cppreference.com/w/cpp/iterator/iterator_traits
      // https://en.cppreference.com/w/cpp/iterator/iterator_tags
      using iterator_category = std::random_access_iterator_tag;
#if SUTILS_CPP_VERSION >= 202002L
      using iterator_concept = std::contiguous_iterator_tag;
#endif

      // already defined by parent class
      using difference_type = str_weak_ref_basic::difference_type;
      // already defined by parent class
      using value_type = str_weak_ref_basic::value_type;
      using pointer = const value_type*;
      using reference = const_reference;

    private:
      size_type m_count;
      pointer m_data_base;
      pointer m_data;

      constexpr void add_offset(difference_type offset) noexcept {
        auto absolute_offset = static_cast<difference_type>(
          (m_data - m_data_base) + (reverse ? -offset : offset)
        );
        if (absolute_offset < 0) {
          m_data = m_data_base;
        } else if (absolute_offset > m_count) {
          m_data = m_data_base + m_count;
        } else {
          m_data = m_data_base + absolute_offset;
        }
      }

    public:
      constexpr str_weak_iterator() noexcept = delete;
      constexpr str_weak_iterator(size_type count, pointer base, pointer data) noexcept :
        m_count(count),
        m_data_base(base),
        m_data(data)
      { }

      // ------------------------------------------------ LegacyIterator: https://en.cppreference.com/w/cpp/iterator
      constexpr auto& operator*() const noexcept {
        return *( m_data - (reverse ? 1 : 0) );
      }

      constexpr auto& operator++() noexcept { // ++it
        add_offset(1);
        return *this;
      }

      // ------------------------------------------------ LegacyInputIterator: https://en.cppreference.com/w/cpp/named_req/InputIterator
      constexpr bool operator==(const str_weak_iterator &other) const noexcept {
        return m_data_base == other.m_data_base &&
               m_count == other.m_count &&
               m_data == other.m_data;
      }
      
      constexpr bool operator!=(const str_weak_iterator &other) const noexcept {
        return !( *this == other );
      }
      
      constexpr auto operator->() const noexcept {
        return &( *(*this) );
      }

      // ------------------------------------------------ LegacyForwardIterator: https://en.cppreference.com/w/cpp/named_req/ForwardIterator
      constexpr auto operator++(int) noexcept { // it++
        auto tmp = *this;
        ++(*this);
        return tmp;
      }

      // ------------------------------------------------ LegacyBidirectionalIterator: https://en.cppreference.com/w/cpp/named_req/BidirectionalIterator
      constexpr auto& operator--() noexcept { // --it
        add_offset(-1);
        return *this;
      }

      constexpr auto operator--(int) noexcept { // it--
        auto tmp = *this;
        --(*this);
        return tmp;
      }
      
      // ------------------------------------------------ LegacyRandomAccessIterator: https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
      constexpr auto& operator+=(difference_type offset) noexcept { // it += 5
        add_offset(offset);
        return *this;
      }

      constexpr auto operator+(difference_type offset) const noexcept { // it + 5
        auto tmp = *this;
        tmp += offset;
        return tmp;
      }
      
      friend constexpr auto operator+(difference_type offset, str_weak_iterator right) noexcept { // 5 + it
        right += offset; // we already have a copy (lvalue or rvalue), safe to modify
        return right;
      }
      
      constexpr auto& operator-=(difference_type offset) noexcept { // it -= 5
        add_offset(-offset);
        return *this;
      }

      constexpr auto operator-(difference_type offset) const noexcept { // it - 5
        auto tmp = *this;
        tmp -= offset;
        return tmp;
      }
      
      // this is not required
      // friend constexpr auto operator-(difference_type offset, str_weak_iterator right) noexcept; // 5 - it
      
      constexpr auto operator-(const str_weak_iterator &other) const noexcept { // it - other
        return static_cast<difference_type>(
          reverse ? (other.m_data - m_data) : (m_data - other.m_data)
        );
      }
      
      constexpr auto& operator[](difference_type offset) const noexcept { // it[5]
        return *(*this + offset);
      }
      
      constexpr bool operator<(const str_weak_iterator &other) const noexcept { // it < other
        return reverse ? (other.m_data < m_data) : (m_data < other.m_data);
      }
      
      constexpr bool operator>(const str_weak_iterator &other) const noexcept { // it > other
        return reverse ? (other.m_data > m_data) : (m_data > other.m_data);
      }
      
      constexpr bool operator>=(const str_weak_iterator &other) const noexcept { // it >= other
        return !( *this < other );
      }
      
      constexpr bool operator<=(const str_weak_iterator &other) const noexcept { // it <= other
        return !( *this > other );
      }

      // ------------------------------------------------ LegacyContiguousIterator: https://en.cppreference.com/w/cpp/named_req/ContiguousIterator
      // already implemented

      // ------------------------------------------------ own implementation
      constexpr auto data() const noexcept {
        return m_data;
      }
    };

    using const_iterator = str_weak_iterator<false>;
    using iterator = const_iterator;
    // https://en.cppreference.com/w/cpp/named_req/ReversibleContainer
    using const_reverse_iterator = str_weak_iterator<true>;
    using reverse_iterator = const_reverse_iterator;
    
    constexpr str_weak_ref_basic() noexcept = delete;
    constexpr str_weak_ref_basic(const value_type *data, size_type count) noexcept :
      m_data(data),
      m_count(count)
    { }

    constexpr auto size() const noexcept {
      return m_count;
    }

    constexpr auto max_size() const noexcept {
      return diff_max < size_max ? diff_max : size_max;
    }

    constexpr bool empty() const noexcept {
      return m_count == 0;
    }

    constexpr const auto& data() const noexcept {
      return m_data;
    }

    constexpr auto begin() const noexcept {
      return iterator(m_count, m_data, m_data);
    }

    constexpr auto end() const noexcept {
      return iterator(m_count, m_data, m_data + m_count);
    }

    constexpr auto cbegin() const noexcept {
      return begin();
    }

    constexpr auto cend() const noexcept {
      return end();
    }

    constexpr auto rbegin() const noexcept {
      return reverse_iterator(m_count, m_data, m_data + m_count);
    }

    constexpr auto rend() const noexcept {
      return reverse_iterator(m_count, m_data, m_data);
    }

    constexpr auto crbegin() const noexcept {
      return rbegin();
    }
    
    constexpr auto crend() const noexcept {
      return rend();
    }

    constexpr auto substr(difference_type start, size_type len = static_cast<size_type>(-1)) const noexcept {
      if (m_count == 0) {
        return *this;
      } else if (len == 0) {
        return str_weak_ref_basic(m_data + m_count, 0);
      }
      
      if (start < 0) {
        start = static_cast<difference_type>(m_count + start);
        if (start < 0) { // too negative
          start = 0;
        }
      } else if (static_cast<size_type>(start) >= m_count) {
        return str_weak_ref_basic(m_data + m_count, 0);
      }

      const auto remaining_len = m_count - static_cast<size_type>(start);
      if (remaining_len < len) {
        len = remaining_len;
      }
      return str_weak_ref_basic(m_data + static_cast<size_type>(start), len);
    }

  private:
    const value_type *m_data;
    size_type m_count;
    
  };

  // https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
  // class template deduction from constructor is available in C++17 and above
  // these overloads take care of that
  template<class TC>
  constexpr auto str_weak_ref(const str_weak_ref_basic<TC> &str) noexcept {
    return str_weak_ref_basic<TC>(str.data(), str.size());
  }

  template<class TC, size_t N>
  constexpr auto str_weak_ref(const TC (&str)[N]) noexcept {
    return str_weak_ref_basic<TC>(str, N - 1);
  }

  template<class TC>
  auto str_weak_ref(const std::basic_string<TC> &str) noexcept {
    return str_weak_ref_basic<TC>(str.data(), str.size());
  }

#ifdef SUTILS_HAS_STRVIEW
  template<class TC>
  constexpr auto str_weak_ref(std::basic_string_view<TC> str) noexcept {
    return str_weak_ref_basic<TC>(str.data(), str.size());
  }
#endif

} // helpers
} // sutils


namespace sutils {
  template<class TC, bool reverse = false>
  bool cmp(
    const typename helpers::str_weak_ref_basic<TC>::str_weak_iterator<reverse> &st1_begin,
    const typename helpers::str_weak_ref_basic<TC>::str_weak_iterator<reverse> &st1_end,
    const typename helpers::str_weak_ref_basic<TC>::str_weak_iterator<reverse> &st2_begin,
    const typename helpers::str_weak_ref_basic<TC>::str_weak_iterator<reverse> &st2_end,
    bool case_insensitive = false
  ) {
    if (case_insensitive) {
      return std::equal(st1_begin, st1_end, st2_begin, st2_end, [](auto cc1, auto cc2){
        return std::toupper(static_cast<int>(cc1)) == std::toupper(static_cast<int>(cc2));
      });
    } else {
      return std::equal(st1_begin, st1_end, st2_begin, st2_end, [](auto cc1, auto cc2){
        return cc1 == cc2;
      });
    }
  }

  template<class TStr1, class TStr2>
  bool cmp(const TStr1 &s1, const TStr2 &s2, bool case_insensitive = false) {
    const auto hs1 = helpers::str_weak_ref(s1);
    const auto hs2 = helpers::str_weak_ref(s2);

    using TC1 = typename decltype(hs1)::value_type;
    using TC2 = typename decltype(hs2)::value_type;

    static_assert(std::is_same<TC1, TC2>::value, "mismatching char type");

    return cmp<TC1, false>(hs1.begin(), hs1.end(), hs2.begin(), hs2.end(), case_insensitive);
  }

  template<class TStr1, class TStr2>
  bool starts(const TStr1 &str, const TStr2 &search, bool case_insensitive = false) {
    const auto hstr = helpers::str_weak_ref(str);
    const auto hsearch = helpers::str_weak_ref(search);

    using TC1 = typename decltype(hstr)::value_type;
    using TC2 = typename decltype(hsearch)::value_type;

    static_assert(std::is_same<TC1, TC2>::value, "mismatching char type");

    // only allow:
    // "" >><< ""
    // "not empty" >><< "also not empty"
    if (hstr.empty() != hsearch.empty()) {
      return false;
    }

    auto hstr_begin = hstr.begin();
    return cmp<TC1>(hstr_begin, hstr_begin + hsearch.size(), hsearch.begin(), hsearch.end(), case_insensitive);
  }

  template<class TStr1, class TStr2>
  bool ends(const TStr1 &str, const TStr2 &search, bool case_insensitive = false) {
    const auto hstr = helpers::str_weak_ref(str);
    const auto hsearch = helpers::str_weak_ref(search);

    using TC1 = typename decltype(hstr)::value_type;
    using TC2 = typename decltype(hsearch)::value_type;

    static_assert(std::is_same<TC1, TC2>::value, "mismatching char type");

    // only allow:
    // "" >><< ""
    // "not empty" >><< "also not empty"
    if (hstr.empty() != hsearch.empty()) {
      return false;
    }

    auto hstr_rbegin = hstr.rbegin();
    return cmp<TC1, true>(hstr_rbegin, hstr_rbegin + hsearch.size(), hsearch.rbegin(), hsearch.rend(), case_insensitive);
  }

  template<class TStr1, class TStr2>
  std::vector<size_t> find_all(const TStr1 &str, const TStr2 &search, bool backward = false, size_t max_finds = static_cast<size_t>(-1), bool case_insensitive = false) {
    const auto hstr = helpers::str_weak_ref(str);
    const auto hsearch = helpers::str_weak_ref(search);

    using TC1 = typename decltype(hstr)::value_type;
    using TC2 = typename decltype(hsearch)::value_type;

    static_assert(std::is_same<TC1, TC2>::value, "mismatching char type");

    if (hstr.empty() || hsearch.empty() || (hstr.size() < hsearch.size()) || (max_finds == 0)) {
      return {};
    }

    std::vector<size_t> results{};
    if (backward) {
      for (size_t len = hstr.size(); (len > 0) && (max_finds > 0); --max_finds) {
        if (ends(hstr.substr(0, len), hsearch, case_insensitive)) {
          results.emplace_back(len - hsearch.size());
          len -= hsearch.size();
        } else {
          --len;
        }
      }
    } else {
      for (size_t idx = 0; (idx < hstr.size()) && (max_finds > 0); --max_finds) {
        if (starts(hstr.substr(idx), hsearch, case_insensitive)) {
          results.emplace_back(idx);
          idx += hsearch.size();
        } else {
          ++idx;
        }
      }
    }
    return results;
  }

  template<class TStr1, class TStr2, class TStr3>
  auto replace_all(const TStr1 &str, const TStr2 &search, const TStr3 &replace, bool backward = false, size_t max_replaces = static_cast<size_t>(-1), bool case_insensitive = false) {
    const auto hstr = helpers::str_weak_ref(str);
    const auto hsearch = helpers::str_weak_ref(search);
    const auto hreplace = helpers::str_weak_ref(replace);

    using TC1 = typename decltype(hstr)::value_type;
    using TC2 = typename decltype(hsearch)::value_type;
    using TC3 = typename decltype(hreplace)::value_type;

    static_assert(std::is_same<TC1, TC2>::value && std::is_same<TC1, TC3>::value, "mismatching char type");

    if (max_replaces == 0) {
      return std::basic_string<TC1>(hstr.data(), hstr.size());
    }

    const auto all_places = find_all(hstr, hsearch, backward, max_replaces, case_insensitive);
    if (all_places.empty()) {
      return std::basic_string<TC1>(hstr.data(), hstr.size());
    }

    size_t total_count = hstr.size() - all_places.size() * hsearch.size() + all_places.size() * hreplace.size();
    
    std::basic_string<TC1> result{};
    result.reserve(total_count + 1); // +1 for null
    size_t start = 0;
    if (backward) {
      for (size_t idx = all_places.size(); idx > 0; --idx) {
        size_t offset = all_places[idx - 1];
        const auto original_length = offset - start;
        const auto original = hstr.substr(start, original_length);
        result.append(original.data(), original.size())
              .append(hreplace.data(), hreplace.size());
        start = offset + hsearch.size();
      }
    } else {
      for (size_t offset : all_places) {
        const auto original_length = offset - start;
        const auto original = hstr.substr(start, original_length);
        result.append(original.data(), original.size())
              .append(hreplace.data(), hreplace.size());
        start = offset + hsearch.size();
      }
    }
    // copy remaining chars after last match
    const auto original = hstr.substr(start);
    result.append(original.data(), original.size());

    return result;
  }

  template<class TStr1, class TStr2>
  long long first(const TStr1 &str, const TStr2 &search, bool case_insensitive = false) {
    const auto results = find_all(str, search, false, 1, case_insensitive);
    return results.empty() ? -1 : static_cast<long long>(results[0]);
  }

  template<class TStr1, class TStr2>
  long long last(const TStr1 &str, const TStr2 &search, bool case_insensitive = false) {
    const auto results = find_all(str, search, true, 1, case_insensitive);
    return results.empty() ? -1 : static_cast<long long>(results[0]);
  }

  template<class TStr1, class TStr2>
  auto split(const TStr1 &str, const TStr2 &splitter, bool keep_empty = false, bool backward = false, size_t max_tokens = static_cast<size_t>(-1), bool case_insensitive = false) {
    const auto hstr = helpers::str_weak_ref(str);
    const auto hsplitter = helpers::str_weak_ref(splitter);

    using TC1 = typename decltype(hstr)::value_type;
    using TC2 = typename decltype(hsplitter)::value_type;

    static_assert(std::is_same<TC1, TC2>::value, "mismatching char type");

    if (max_tokens == 0 || hstr.empty()) {
      return std::vector<std::basic_string<TC1>>{};
    }

    if (max_tokens == 1 || hsplitter.empty() || hstr.size() < hsplitter.size()) {
      return std::vector<std::basic_string<TC1>>{ std::basic_string<TC1>(hstr.data(), hstr.size()) };
    }

    const auto all_places = find_all(hstr, hsplitter, backward, max_tokens - 1 /*splitter count*/, case_insensitive);
    if (all_places.empty()) {
      return std::vector<std::basic_string<TC1>>{ std::basic_string<TC1>(hstr.data(), hstr.size()) };
    }

    std::vector<std::basic_string<TC1>> tokens{};
    tokens.reserve(all_places.size() + 1 /*tokens count*/);
    size_t start = 0;
    if (backward) {
      for (size_t idx = all_places.size(); idx > 0; --idx) {
        size_t offset = all_places[idx - 1];
        const auto token_len = offset - start;
        if (token_len > 0 || keep_empty) {
          const auto token_str = hstr.substr(start, token_len);
          tokens.emplace_back( std::basic_string<TC1>(token_str.data(), token_str.size()) );
        }
        start = offset + hsplitter.size();
      }
    } else {
      for (size_t offset : all_places) {
        const auto token_len = offset - start;
        if (token_len > 0 || keep_empty) {
          const auto token_str = hstr.substr(start, token_len);
          tokens.emplace_back( std::basic_string<TC1>(token_str.data(), token_str.size()) );
        }
        start = offset + hsplitter.size();
      }
    }
    // add last/remaining part of the string (after last splitter)
    const auto token_len = hstr.size() - start;
    if (token_len > 0 || keep_empty) {
      const auto current_str = hstr.substr(start, token_len);
      tokens.emplace_back( std::basic_string<TC1>(current_str.data(), current_str.size()) );
    }

    return tokens;
  }
}
