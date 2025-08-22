/*
  Formatting library for C++

  Copyright (c) 2012 - present, Victor Zverovich

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  --- Optional exception to the license ---

  As an exception, if, as a result of your compiling your source code, portions
  of this Software are embedded into a machine-executable object form of such
  source code, you may redistribute such embedded portions in such object form
  without including the above copyright and permission notices.
 */

#ifndef FMT_FORMAT_H_
#define FMT_FORMAT_H_

#ifndef _LIBCPP_REMOVE_TRANSITIVE_INCLUDES
#  define _LIBCPP_REMOVE_TRANSITIVE_INCLUDES
#  define FMT_REMOVE_TRANSITIVE_INCLUDES
#endif

// Formatting library for C++ - the base API for char/UTF-8
//
// Copyright (c) 2012 - present, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_BASE_H_
#define FMT_BASE_H_

#if defined(FMT_IMPORT_STD) && !defined(FMT_MODULE)
#  define FMT_MODULE
#endif

#ifndef FMT_MODULE
#  include <limits.h>  // CHAR_BIT
#  include <stdio.h>   // FILE
#  include <string.h>  // memcmp

#  include <type_traits>  // std::enable_if
#endif

// The fmt library version in the form major * 10000 + minor * 100 + patch.
#define FMT_VERSION 110201

// Detect compiler versions.
#if defined(__clang__) && !defined(__ibmxl__)
#  define FMT_CLANG_VERSION (__clang_major__ * 100 + __clang_minor__)
#else
#  define FMT_CLANG_VERSION 0
#endif
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#  define FMT_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#else
#  define FMT_GCC_VERSION 0
#endif
#if defined(__ICL)
#  define FMT_ICC_VERSION __ICL
#elif defined(__INTEL_COMPILER)
#  define FMT_ICC_VERSION __INTEL_COMPILER
#else
#  define FMT_ICC_VERSION 0
#endif
#if defined(_MSC_VER)
#  define FMT_MSC_VERSION _MSC_VER
#else
#  define FMT_MSC_VERSION 0
#endif

// Detect standard library versions.
#ifdef _GLIBCXX_RELEASE
#  define FMT_GLIBCXX_RELEASE _GLIBCXX_RELEASE
#else
#  define FMT_GLIBCXX_RELEASE 0
#endif
#ifdef _LIBCPP_VERSION
#  define FMT_LIBCPP_VERSION _LIBCPP_VERSION
#else
#  define FMT_LIBCPP_VERSION 0
#endif

#ifdef _MSVC_LANG
#  define FMT_CPLUSPLUS _MSVC_LANG
#else
#  define FMT_CPLUSPLUS __cplusplus
#endif

// Detect __has_*.
#ifdef __has_feature
#  define FMT_HAS_FEATURE(x) __has_feature(x)
#else
#  define FMT_HAS_FEATURE(x) 0
#endif
#ifdef __has_include
#  define FMT_HAS_INCLUDE(x) __has_include(x)
#else
#  define FMT_HAS_INCLUDE(x) 0
#endif
#ifdef __has_builtin
#  define FMT_HAS_BUILTIN(x) __has_builtin(x)
#else
#  define FMT_HAS_BUILTIN(x) 0
#endif
#ifdef __has_cpp_attribute
#  define FMT_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#  define FMT_HAS_CPP_ATTRIBUTE(x) 0
#endif

#define FMT_HAS_CPP14_ATTRIBUTE(attribute) \
  (FMT_CPLUSPLUS >= 201402L && FMT_HAS_CPP_ATTRIBUTE(attribute))

#define FMT_HAS_CPP17_ATTRIBUTE(attribute) \
  (FMT_CPLUSPLUS >= 201703L && FMT_HAS_CPP_ATTRIBUTE(attribute))

// Detect C++14 relaxed constexpr.
#ifdef FMT_USE_CONSTEXPR
// Use the provided definition.
#elif FMT_GCC_VERSION >= 702 && FMT_CPLUSPLUS >= 201402L
// GCC only allows constexpr member functions in non-literal types since 7.2:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66297.
#  define FMT_USE_CONSTEXPR 1
#elif FMT_ICC_VERSION
#  define FMT_USE_CONSTEXPR 0  // https://github.com/fmtlib/fmt/issues/1628
#elif FMT_HAS_FEATURE(cxx_relaxed_constexpr) || FMT_MSC_VERSION >= 1912
#  define FMT_USE_CONSTEXPR 1
#else
#  define FMT_USE_CONSTEXPR 0
#endif
#if FMT_USE_CONSTEXPR
#  define FMT_CONSTEXPR constexpr
#else
#  define FMT_CONSTEXPR
#endif

// Detect consteval, C++20 constexpr extensions and std::is_constant_evaluated.
#if !defined(__cpp_lib_is_constant_evaluated)
#  define FMT_USE_CONSTEVAL 0
#elif FMT_CPLUSPLUS < 201709L
#  define FMT_USE_CONSTEVAL 0
#elif FMT_GLIBCXX_RELEASE && FMT_GLIBCXX_RELEASE < 10
#  define FMT_USE_CONSTEVAL 0
#elif FMT_LIBCPP_VERSION && FMT_LIBCPP_VERSION < 10000
#  define FMT_USE_CONSTEVAL 0
#elif defined(__apple_build_version__) && __apple_build_version__ < 14000029L
#  define FMT_USE_CONSTEVAL 0  // consteval is broken in Apple clang < 14.
#elif FMT_MSC_VERSION && FMT_MSC_VERSION < 1929
#  define FMT_USE_CONSTEVAL 0  // consteval is broken in MSVC VS2019 < 16.10.
#elif defined(__cpp_consteval)
#  define FMT_USE_CONSTEVAL 1
#elif FMT_GCC_VERSION >= 1002 || FMT_CLANG_VERSION >= 1101
#  define FMT_USE_CONSTEVAL 1
#else
#  define FMT_USE_CONSTEVAL 0
#endif
#if FMT_USE_CONSTEVAL
#  define FMT_CONSTEVAL consteval
#  define FMT_CONSTEXPR20 constexpr
#else
#  define FMT_CONSTEVAL
#  define FMT_CONSTEXPR20
#endif

// Check if exceptions are disabled.
#ifdef FMT_USE_EXCEPTIONS
// Use the provided definition.
#elif defined(__GNUC__) && !defined(__EXCEPTIONS)
#  define FMT_USE_EXCEPTIONS 0
#elif defined(__clang__) && !defined(__cpp_exceptions)
#  define FMT_USE_EXCEPTIONS 0
#elif FMT_MSC_VERSION && !_HAS_EXCEPTIONS
#  define FMT_USE_EXCEPTIONS 0
#else
#  define FMT_USE_EXCEPTIONS 1
#endif
#if FMT_USE_EXCEPTIONS
#  define FMT_TRY try
#  define FMT_CATCH(x) catch (x)
#else
#  define FMT_TRY if (true)
#  define FMT_CATCH(x) if (false)
#endif

#ifdef FMT_NO_UNIQUE_ADDRESS
// Use the provided definition.
#elif FMT_CPLUSPLUS < 202002L
// Not supported.
#elif FMT_HAS_CPP_ATTRIBUTE(no_unique_address)
#  define FMT_NO_UNIQUE_ADDRESS [[no_unique_address]]
// VS2019 v16.10 and later except clang-cl (https://reviews.llvm.org/D110485).
#elif FMT_MSC_VERSION >= 1929 && !FMT_CLANG_VERSION
#  define FMT_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#endif
#ifndef FMT_NO_UNIQUE_ADDRESS
#  define FMT_NO_UNIQUE_ADDRESS
#endif

#if FMT_HAS_CPP17_ATTRIBUTE(fallthrough)
#  define FMT_FALLTHROUGH [[fallthrough]]
#elif defined(__clang__)
#  define FMT_FALLTHROUGH [[clang::fallthrough]]
#elif FMT_GCC_VERSION >= 700 && \
    (!defined(__EDG_VERSION__) || __EDG_VERSION__ >= 520)
#  define FMT_FALLTHROUGH [[gnu::fallthrough]]
#else
#  define FMT_FALLTHROUGH
#endif

// Disable [[noreturn]] on MSVC/NVCC because of bogus unreachable code warnings.
#if FMT_HAS_CPP_ATTRIBUTE(noreturn) && !FMT_MSC_VERSION && !defined(__NVCC__)
#  define FMT_NORETURN [[noreturn]]
#else
#  define FMT_NORETURN
#endif

#ifdef FMT_NODISCARD
// Use the provided definition.
#elif FMT_HAS_CPP17_ATTRIBUTE(nodiscard)
#  define FMT_NODISCARD [[nodiscard]]
#else
#  define FMT_NODISCARD
#endif

#if FMT_GCC_VERSION || FMT_CLANG_VERSION
#  define FMT_VISIBILITY(value) __attribute__((visibility(value)))
#else
#  define FMT_VISIBILITY(value)
#endif

// Detect pragmas.
#define FMT_PRAGMA_IMPL(x) _Pragma(#x)
#if FMT_GCC_VERSION >= 504 && !defined(__NVCOMPILER)
// Workaround a _Pragma bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=59884
// and an nvhpc warning: https://github.com/fmtlib/fmt/pull/2582.
#  define FMT_PRAGMA_GCC(x) FMT_PRAGMA_IMPL(GCC x)
#else
#  define FMT_PRAGMA_GCC(x)
#endif
#if FMT_CLANG_VERSION
#  define FMT_PRAGMA_CLANG(x) FMT_PRAGMA_IMPL(clang x)
#else
#  define FMT_PRAGMA_CLANG(x)
#endif
#if FMT_MSC_VERSION
#  define FMT_MSC_WARNING(...) __pragma(warning(__VA_ARGS__))
#else
#  define FMT_MSC_WARNING(...)
#endif

// Enable minimal optimizations for more compact code in debug mode.
FMT_PRAGMA_GCC(push_options)
#if !defined(__OPTIMIZE__) && !defined(__CUDACC__) && !defined(FMT_MODULE)
FMT_PRAGMA_GCC(optimize("Og"))
#  define FMT_GCC_OPTIMIZED
#endif
FMT_PRAGMA_CLANG(diagnostic push)

#ifdef FMT_ALWAYS_INLINE
// Use the provided definition.
#elif FMT_GCC_VERSION || FMT_CLANG_VERSION
#  define FMT_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#  define FMT_ALWAYS_INLINE inline
#endif
// A version of FMT_ALWAYS_INLINE to prevent code bloat in debug mode.
#if defined(NDEBUG) || defined(FMT_GCC_OPTIMIZED)
#  define FMT_INLINE FMT_ALWAYS_INLINE
#else
#  define FMT_INLINE inline
#endif

#ifndef FMT_BEGIN_NAMESPACE
#  define FMT_BEGIN_NAMESPACE \
    namespace fmt {           \
    inline namespace v11 {
#  define FMT_END_NAMESPACE \
    }                       \
    }
#endif

#ifndef FMT_EXPORT
#  define FMT_EXPORT
#  define FMT_BEGIN_EXPORT
#  define FMT_END_EXPORT
#endif

#ifdef _WIN32
#  define FMT_WIN32 1
#else
#  define FMT_WIN32 0
#endif

#if !defined(FMT_HEADER_ONLY) && FMT_WIN32
#  if defined(FMT_LIB_EXPORT)
#    define FMT_API __declspec(dllexport)
#  elif defined(FMT_SHARED)
#    define FMT_API __declspec(dllimport)
#  endif
#elif defined(FMT_LIB_EXPORT) || defined(FMT_SHARED)
#  define FMT_API FMT_VISIBILITY("default")
#endif
#ifndef FMT_API
#  define FMT_API
#endif

#ifndef FMT_OPTIMIZE_SIZE
#  define FMT_OPTIMIZE_SIZE 0
#endif

// FMT_BUILTIN_TYPE=0 may result in smaller library size at the cost of higher
// per-call binary size by passing built-in types through the extension API.
#ifndef FMT_BUILTIN_TYPES
#  define FMT_BUILTIN_TYPES 1
#endif

#define FMT_APPLY_VARIADIC(expr) \
  using unused = int[];          \
  (void)unused { 0, (expr, 0)... }

FMT_BEGIN_NAMESPACE

// Implementations of enable_if_t and other metafunctions for older systems.
template <bool B, typename T = void>
using enable_if_t = typename std::enable_if<B, T>::type;
template <bool B, typename T, typename F>
using conditional_t = typename std::conditional<B, T, F>::type;
template <bool B> using bool_constant = std::integral_constant<bool, B>;
template <typename T>
using remove_reference_t = typename std::remove_reference<T>::type;
template <typename T>
using remove_const_t = typename std::remove_const<T>::type;
template <typename T>
using remove_cvref_t = typename std::remove_cv<remove_reference_t<T>>::type;
template <typename T>
using make_unsigned_t = typename std::make_unsigned<T>::type;
template <typename T>
using underlying_t = typename std::underlying_type<T>::type;
template <typename T> using decay_t = typename std::decay<T>::type;
using nullptr_t = decltype(nullptr);

#if (FMT_GCC_VERSION && FMT_GCC_VERSION < 500) || FMT_MSC_VERSION
// A workaround for gcc 4.9 & MSVC v141 to make void_t work in a SFINAE context.
template <typename...> struct void_t_impl {
  using type = void;
};
template <typename... T> using void_t = typename void_t_impl<T...>::type;
#else
template <typename...> using void_t = void;
#endif

struct monostate {
  constexpr monostate() {}
};

// An enable_if helper to be used in template parameters which results in much
// shorter symbols: https://godbolt.org/z/sWw4vP. Extra parentheses are needed
// to workaround a bug in MSVC 2019 (see #1140 and #1186).
#ifdef FMT_DOC
#  define FMT_ENABLE_IF(...)
#else
#  define FMT_ENABLE_IF(...) fmt::enable_if_t<(__VA_ARGS__), int> = 0
#endif

template <typename T> constexpr auto min_of(T a, T b) -> T {
  return a < b ? a : b;
}
template <typename T> constexpr auto max_of(T a, T b) -> T {
  return a > b ? a : b;
}

namespace detail {
// Suppresses "unused variable" warnings with the method described in
// https://herbsutter.com/2009/10/18/mailbag-shutting-up-compiler-warnings/.
// (void)var does not work on many Intel compilers.
template <typename... T> FMT_CONSTEXPR void ignore_unused(const T&...) {}

constexpr auto is_constant_evaluated(bool default_value = false) noexcept
    -> bool {
// Workaround for incompatibility between clang 14 and libstdc++ consteval-based
// std::is_constant_evaluated: https://github.com/fmtlib/fmt/issues/3247.
#if FMT_CPLUSPLUS >= 202002L && FMT_GLIBCXX_RELEASE >= 12 && \
    (FMT_CLANG_VERSION >= 1400 && FMT_CLANG_VERSION < 1500)
  ignore_unused(default_value);
  return __builtin_is_constant_evaluated();
#elif defined(__cpp_lib_is_constant_evaluated)
  ignore_unused(default_value);
  return std::is_constant_evaluated();
#else
  return default_value;
#endif
}

// Suppresses "conditional expression is constant" warnings.
template <typename T> FMT_ALWAYS_INLINE constexpr auto const_check(T val) -> T {
  return val;
}

FMT_NORETURN FMT_API void assert_fail(const char* file, int line,
                                      const char* message);

#if defined(FMT_ASSERT)
// Use the provided definition.
#elif defined(NDEBUG)
// FMT_ASSERT is not empty to avoid -Wempty-body.
#  define FMT_ASSERT(condition, message) \
    fmt::detail::ignore_unused((condition), (message))
#else
#  define FMT_ASSERT(condition, message)                                    \
    ((condition) /* void() fails with -Winvalid-constexpr on clang 4.0.1 */ \
         ? (void)0                                                          \
         : fmt::detail::assert_fail(__FILE__, __LINE__, (message)))
#endif

#ifdef FMT_USE_INT128
// Use the provided definition.
#elif defined(__SIZEOF_INT128__) && !defined(__NVCC__) && \
    !(FMT_CLANG_VERSION && FMT_MSC_VERSION)
#  define FMT_USE_INT128 1
using int128_opt = __int128_t;  // An optional native 128-bit integer.
using uint128_opt = __uint128_t;
inline auto map(int128_opt x) -> int128_opt { return x; }
inline auto map(uint128_opt x) -> uint128_opt { return x; }
#else
#  define FMT_USE_INT128 0
#endif
#if !FMT_USE_INT128
enum class int128_opt {};
enum class uint128_opt {};
// Reduce template instantiations.
inline auto map(int128_opt) -> monostate { return {}; }
inline auto map(uint128_opt) -> monostate { return {}; }
#endif

#ifndef FMT_USE_BITINT
#  define FMT_USE_BITINT (FMT_CLANG_VERSION >= 1500)
#endif

#if FMT_USE_BITINT
FMT_PRAGMA_CLANG(diagnostic ignored "-Wbit-int-extension")
template <int N> using bitint = _BitInt(N);
template <int N> using ubitint = unsigned _BitInt(N);
#else
template <int N> struct bitint {};
template <int N> struct ubitint {};
#endif  // FMT_USE_BITINT

// Casts a nonnegative integer to unsigned.
template <typename Int>
FMT_CONSTEXPR auto to_unsigned(Int value) -> make_unsigned_t<Int> {
  FMT_ASSERT(std::is_unsigned<Int>::value || value >= 0, "negative value");
  return static_cast<make_unsigned_t<Int>>(value);
}

template <typename Char>
using unsigned_char = conditional_t<sizeof(Char) == 1, unsigned char, unsigned>;

// A heuristic to detect std::string and std::[experimental::]string_view.
// It is mainly used to avoid dependency on <[experimental/]string_view>.
template <typename T, typename Enable = void>
struct is_std_string_like : std::false_type {};
template <typename T>
struct is_std_string_like<T, void_t<decltype(std::declval<T>().find_first_of(
                                 typename T::value_type(), 0))>>
    : std::is_convertible<decltype(std::declval<T>().data()),
                          const typename T::value_type*> {};

// Check if the literal encoding is UTF-8.
enum { is_utf8_enabled = "\u00A7"[1] == '\xA7' };
enum { use_utf8 = !FMT_WIN32 || is_utf8_enabled };

#ifndef FMT_UNICODE
#  define FMT_UNICODE 1
#endif

static_assert(!FMT_UNICODE || use_utf8,
              "Unicode support requires compiling with /utf-8");

template <typename T> constexpr const char* narrow(const T*) { return nullptr; }
constexpr FMT_ALWAYS_INLINE const char* narrow(const char* s) { return s; }

template <typename Char>
FMT_CONSTEXPR auto compare(const Char* s1, const Char* s2, size_t n) -> int {
  if (!is_constant_evaluated() && sizeof(Char) == 1) return memcmp(s1, s2, n);
  for (; n != 0; ++s1, ++s2, --n) {
    if (*s1 < *s2) return -1;
    if (*s1 > *s2) return 1;
  }
  return 0;
}

namespace adl {
using namespace std;

template <typename Container>
auto invoke_back_inserter()
    -> decltype(back_inserter(std::declval<Container&>()));
}  // namespace adl

template <typename It, typename Enable = std::true_type>
struct is_back_insert_iterator : std::false_type {};

template <typename It>
struct is_back_insert_iterator<
    It, bool_constant<std::is_same<
            decltype(adl::invoke_back_inserter<typename It::container_type>()),
            It>::value>> : std::true_type {};

// Extracts a reference to the container from *insert_iterator.
template <typename OutputIt>
inline FMT_CONSTEXPR20 auto get_container(OutputIt it) ->
    typename OutputIt::container_type& {
  struct accessor : OutputIt {
    FMT_CONSTEXPR20 accessor(OutputIt base) : OutputIt(base) {}
    using OutputIt::container;
  };
  return *accessor(it).container;
}
}  // namespace detail

// Parsing-related public API and forward declarations.
FMT_BEGIN_EXPORT

/**
 * An implementation of `std::basic_string_view` for pre-C++17. It provides a
 * subset of the API. `fmt::basic_string_view` is used for format strings even
 * if `std::basic_string_view` is available to prevent issues when a library is
 * compiled with a different `-std` option than the client code (which is not
 * recommended).
 */
template <typename Char> class basic_string_view {
 private:
  const Char* data_;
  size_t size_;

 public:
  using value_type = Char;
  using iterator = const Char*;

  constexpr basic_string_view() noexcept : data_(nullptr), size_(0) {}

  /// Constructs a string view object from a C string and a size.
  constexpr basic_string_view(const Char* s, size_t count) noexcept
      : data_(s), size_(count) {}

  constexpr basic_string_view(nullptr_t) = delete;

  /// Constructs a string view object from a C string.
#if FMT_GCC_VERSION
  FMT_ALWAYS_INLINE
#endif
  FMT_CONSTEXPR20 basic_string_view(const Char* s) : data_(s) {
#if FMT_HAS_BUILTIN(__builtin_strlen) || FMT_GCC_VERSION || FMT_CLANG_VERSION
    if (std::is_same<Char, char>::value && !detail::is_constant_evaluated()) {
      size_ = __builtin_strlen(detail::narrow(s));  // strlen is not constexpr.
      return;
    }
#endif
    size_t len = 0;
    while (*s++) ++len;
    size_ = len;
  }

  /// Constructs a string view from a `std::basic_string` or a
  /// `std::basic_string_view` object.
  template <typename S,
            FMT_ENABLE_IF(detail::is_std_string_like<S>::value&& std::is_same<
                          typename S::value_type, Char>::value)>
  FMT_CONSTEXPR basic_string_view(const S& s) noexcept
      : data_(s.data()), size_(s.size()) {}

  /// Returns a pointer to the string data.
  constexpr auto data() const noexcept -> const Char* { return data_; }

  /// Returns the string size.
  constexpr auto size() const noexcept -> size_t { return size_; }

  constexpr auto begin() const noexcept -> iterator { return data_; }
  constexpr auto end() const noexcept -> iterator { return data_ + size_; }

  constexpr auto operator[](size_t pos) const noexcept -> const Char& {
    return data_[pos];
  }

  FMT_CONSTEXPR void remove_prefix(size_t n) noexcept {
    data_ += n;
    size_ -= n;
  }

  FMT_CONSTEXPR auto starts_with(basic_string_view<Char> sv) const noexcept
      -> bool {
    return size_ >= sv.size_ && detail::compare(data_, sv.data_, sv.size_) == 0;
  }
  FMT_CONSTEXPR auto starts_with(Char c) const noexcept -> bool {
    return size_ >= 1 && *data_ == c;
  }
  FMT_CONSTEXPR auto starts_with(const Char* s) const -> bool {
    return starts_with(basic_string_view<Char>(s));
  }

  FMT_CONSTEXPR auto compare(basic_string_view other) const -> int {
    int result =
        detail::compare(data_, other.data_, min_of(size_, other.size_));
    if (result != 0) return result;
    return size_ == other.size_ ? 0 : (size_ < other.size_ ? -1 : 1);
  }

  FMT_CONSTEXPR friend auto operator==(basic_string_view lhs,
                                       basic_string_view rhs) -> bool {
    return lhs.compare(rhs) == 0;
  }
  friend auto operator!=(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) != 0;
  }
  friend auto operator<(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) < 0;
  }
  friend auto operator<=(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) <= 0;
  }
  friend auto operator>(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) > 0;
  }
  friend auto operator>=(basic_string_view lhs, basic_string_view rhs) -> bool {
    return lhs.compare(rhs) >= 0;
  }
};

using string_view = basic_string_view<char>;

// DEPRECATED! Will be merged with is_char and moved to detail.
template <typename T> struct is_xchar : std::false_type {};
template <> struct is_xchar<wchar_t> : std::true_type {};
template <> struct is_xchar<char16_t> : std::true_type {};
template <> struct is_xchar<char32_t> : std::true_type {};
#ifdef __cpp_char8_t
template <> struct is_xchar<char8_t> : std::true_type {};
#endif

// Specifies if `T` is a character (code unit) type.
template <typename T> struct is_char : is_xchar<T> {};
template <> struct is_char<char> : std::true_type {};

template <typename T> class basic_appender;
using appender = basic_appender<char>;

// Checks whether T is a container with contiguous storage.
template <typename T> struct is_contiguous : std::false_type {};

class context;
template <typename OutputIt, typename Char> class generic_context;
template <typename Char> class parse_context;

// Longer aliases for C++20 compatibility.
template <typename Char> using basic_format_parse_context = parse_context<Char>;
using format_parse_context = parse_context<char>;
template <typename OutputIt, typename Char>
using basic_format_context =
    conditional_t<std::is_same<OutputIt, appender>::value, context,
                  generic_context<OutputIt, Char>>;
using format_context = context;

template <typename Char>
using buffered_context =
    conditional_t<std::is_same<Char, char>::value, context,
                  generic_context<basic_appender<Char>, Char>>;

template <typename Context> class basic_format_arg;
template <typename Context> class basic_format_args;

// A separate type would result in shorter symbols but break ABI compatibility
// between clang and gcc on ARM (#1919).
using format_args = basic_format_args<context>;

// A formatter for objects of type T.
template <typename T, typename Char = char, typename Enable = void>
struct formatter {
  // A deleted default constructor indicates a disabled formatter.
  formatter() = delete;
};

/// Reports a format error at compile time or, via a `format_error` exception,
/// at runtime.
// This function is intentionally not constexpr to give a compile-time error.
FMT_NORETURN FMT_API void report_error(const char* message);

enum class presentation_type : unsigned char {
  // Common specifiers:
  none = 0,
  debug = 1,   // '?'
  string = 2,  // 's' (string, bool)

  // Integral, bool and character specifiers:
  dec = 3,  // 'd'
  hex,      // 'x' or 'X'
  oct,      // 'o'
  bin,      // 'b' or 'B'
  chr,      // 'c'

  // String and pointer specifiers:
  pointer = 3,  // 'p'

  // Floating-point specifiers:
  exp = 1,  // 'e' or 'E' (1 since there is no FP debug presentation)
  fixed,    // 'f' or 'F'
  general,  // 'g' or 'G'
  hexfloat  // 'a' or 'A'
};

enum class align { none, left, right, center, numeric };
enum class sign { none, minus, plus, space };
enum class arg_id_kind { none, index, name };

// Basic format specifiers for built-in and string types.
class basic_specs {
 private:
  // Data is arranged as follows:
  //
  //  0                   1                   2                   3
  //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  // +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  // |type |align| w | p | s |u|#|L|  f  |          unused           |
  // +-----+-----+---+---+---+-+-+-+-----+---------------------------+
  //
  //   w - dynamic width info
  //   p - dynamic precision info
  //   s - sign
  //   u - uppercase (e.g. 'X' for 'x')
  //   # - alternate form ('#')
  //   L - localized
  //   f - fill size
  //
  // Bitfields are not used because of compiler bugs such as gcc bug 61414.
  enum : unsigned {
    type_mask = 0x00007,
    align_mask = 0x00038,
    width_mask = 0x000C0,
    precision_mask = 0x00300,
    sign_mask = 0x00C00,
    uppercase_mask = 0x01000,
    alternate_mask = 0x02000,
    localized_mask = 0x04000,
    fill_size_mask = 0x38000,

    align_shift = 3,
    width_shift = 6,
    precision_shift = 8,
    sign_shift = 10,
    fill_size_shift = 15,

    max_fill_size = 4
  };

  unsigned data_ = 1 << fill_size_shift;
  static_assert(sizeof(basic_specs::data_) * CHAR_BIT >= 18, "");

  // Character (code unit) type is erased to prevent template bloat.
  char fill_data_[max_fill_size] = {' '};

  FMT_CONSTEXPR void set_fill_size(size_t size) {
    data_ = (data_ & ~fill_size_mask) |
            (static_cast<unsigned>(size) << fill_size_shift);
  }

 public:
  constexpr auto type() const -> presentation_type {
    return static_cast<presentation_type>(data_ & type_mask);
  }
  FMT_CONSTEXPR void set_type(presentation_type t) {
    data_ = (data_ & ~type_mask) | static_cast<unsigned>(t);
  }

  constexpr auto align() const -> align {
    return static_cast<fmt::align>((data_ & align_mask) >> align_shift);
  }
  FMT_CONSTEXPR void set_align(fmt::align a) {
    data_ = (data_ & ~align_mask) | (static_cast<unsigned>(a) << align_shift);
  }

  constexpr auto dynamic_width() const -> arg_id_kind {
    return static_cast<arg_id_kind>((data_ & width_mask) >> width_shift);
  }
  FMT_CONSTEXPR void set_dynamic_width(arg_id_kind w) {
    data_ = (data_ & ~width_mask) | (static_cast<unsigned>(w) << width_shift);
  }

  FMT_CONSTEXPR auto dynamic_precision() const -> arg_id_kind {
    return static_cast<arg_id_kind>((data_ & precision_mask) >>
                                    precision_shift);
  }
  FMT_CONSTEXPR void set_dynamic_precision(arg_id_kind p) {
    data_ = (data_ & ~precision_mask) |
            (static_cast<unsigned>(p) << precision_shift);
  }

  constexpr bool dynamic() const {
    return (data_ & (width_mask | precision_mask)) != 0;
  }

  constexpr auto sign() const -> sign {
    return static_cast<fmt::sign>((data_ & sign_mask) >> sign_shift);
  }
  FMT_CONSTEXPR void set_sign(fmt::sign s) {
    data_ = (data_ & ~sign_mask) | (static_cast<unsigned>(s) << sign_shift);
  }

  constexpr auto upper() const -> bool { return (data_ & uppercase_mask) != 0; }
  FMT_CONSTEXPR void set_upper() { data_ |= uppercase_mask; }

  constexpr auto alt() const -> bool { return (data_ & alternate_mask) != 0; }
  FMT_CONSTEXPR void set_alt() { data_ |= alternate_mask; }
  FMT_CONSTEXPR void clear_alt() { data_ &= ~alternate_mask; }

  constexpr auto localized() const -> bool {
    return (data_ & localized_mask) != 0;
  }
  FMT_CONSTEXPR void set_localized() { data_ |= localized_mask; }

  constexpr auto fill_size() const -> size_t {
    return (data_ & fill_size_mask) >> fill_size_shift;
  }

  template <typename Char, FMT_ENABLE_IF(std::is_same<Char, char>::value)>
  constexpr auto fill() const -> const Char* {
    return fill_data_;
  }
  template <typename Char, FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
  constexpr auto fill() const -> const Char* {
    return nullptr;
  }

  template <typename Char> constexpr auto fill_unit() const -> Char {
    using uchar = unsigned char;
    return static_cast<Char>(static_cast<uchar>(fill_data_[0]) |
                             (static_cast<uchar>(fill_data_[1]) << 8) |
                             (static_cast<uchar>(fill_data_[2]) << 16));
  }

  FMT_CONSTEXPR void set_fill(char c) {
    fill_data_[0] = c;
    set_fill_size(1);
  }

  template <typename Char>
  FMT_CONSTEXPR void set_fill(basic_string_view<Char> s) {
    auto size = s.size();
    set_fill_size(size);
    if (size == 1) {
      unsigned uchar = static_cast<detail::unsigned_char<Char>>(s[0]);
      fill_data_[0] = static_cast<char>(uchar);
      fill_data_[1] = static_cast<char>(uchar >> 8);
      fill_data_[2] = static_cast<char>(uchar >> 16);
      return;
    }
    FMT_ASSERT(size <= max_fill_size, "invalid fill");
    for (size_t i = 0; i < size; ++i)
      fill_data_[i & 3] = static_cast<char>(s[i]);
  }

  FMT_CONSTEXPR void copy_fill_from(const basic_specs& specs) {
    set_fill_size(specs.fill_size());
    for (size_t i = 0; i < max_fill_size; ++i)
      fill_data_[i] = specs.fill_data_[i];
  }
};

// Format specifiers for built-in and string types.
struct format_specs : basic_specs {
  int width;
  int precision;

  constexpr format_specs() : width(0), precision(-1) {}
};

/**
 * Parsing context consisting of a format string range being parsed and an
 * argument counter for automatic indexing.
 */
template <typename Char = char> class parse_context {
 private:
  basic_string_view<Char> fmt_;
  int next_arg_id_;

  enum { use_constexpr_cast = !FMT_GCC_VERSION || FMT_GCC_VERSION >= 1200 };

  FMT_CONSTEXPR void do_check_arg_id(int arg_id);

 public:
  using char_type = Char;
  using iterator = const Char*;

  constexpr explicit parse_context(basic_string_view<Char> fmt,
                                   int next_arg_id = 0)
      : fmt_(fmt), next_arg_id_(next_arg_id) {}

  /// Returns an iterator to the beginning of the format string range being
  /// parsed.
  constexpr auto begin() const noexcept -> iterator { return fmt_.begin(); }

  /// Returns an iterator past the end of the format string range being parsed.
  constexpr auto end() const noexcept -> iterator { return fmt_.end(); }

  /// Advances the begin iterator to `it`.
  FMT_CONSTEXPR void advance_to(iterator it) {
    fmt_.remove_prefix(detail::to_unsigned(it - begin()));
  }

  /// Reports an error if using the manual argument indexing; otherwise returns
  /// the next argument index and switches to the automatic indexing.
  FMT_CONSTEXPR auto next_arg_id() -> int {
    if (next_arg_id_ < 0) {
      report_error("cannot switch from manual to automatic argument indexing");
      return 0;
    }
    int id = next_arg_id_++;
    do_check_arg_id(id);
    return id;
  }

  /// Reports an error if using the automatic argument indexing; otherwise
  /// switches to the manual indexing.
  FMT_CONSTEXPR void check_arg_id(int id) {
    if (next_arg_id_ > 0) {
      report_error("cannot switch from automatic to manual argument indexing");
      return;
    }
    next_arg_id_ = -1;
    do_check_arg_id(id);
  }
  FMT_CONSTEXPR void check_arg_id(basic_string_view<Char>) {
    next_arg_id_ = -1;
  }
  FMT_CONSTEXPR void check_dynamic_spec(int arg_id);
};

FMT_END_EXPORT

namespace detail {

// Constructs fmt::basic_string_view<Char> from types implicitly convertible
// to it, deducing Char. Explicitly convertible types such as the ones returned
// from FMT_STRING are intentionally excluded.
template <typename Char, FMT_ENABLE_IF(is_char<Char>::value)>
constexpr auto to_string_view(const Char* s) -> basic_string_view<Char> {
  return s;
}
template <typename T, FMT_ENABLE_IF(is_std_string_like<T>::value)>
constexpr auto to_string_view(const T& s)
    -> basic_string_view<typename T::value_type> {
  return s;
}
template <typename Char>
constexpr auto to_string_view(basic_string_view<Char> s)
    -> basic_string_view<Char> {
  return s;
}

template <typename T, typename Enable = void>
struct has_to_string_view : std::false_type {};
// detail:: is intentional since to_string_view is not an extension point.
template <typename T>
struct has_to_string_view<
    T, void_t<decltype(detail::to_string_view(std::declval<T>()))>>
    : std::true_type {};

/// String's character (code unit) type. detail:: is intentional to prevent ADL.
template <typename S,
          typename V = decltype(detail::to_string_view(std::declval<S>()))>
using char_t = typename V::value_type;

enum class type {
  none_type,
  // Integer types should go first,
  int_type,
  uint_type,
  long_long_type,
  ulong_long_type,
  int128_type,
  uint128_type,
  bool_type,
  char_type,
  last_integer_type = char_type,
  // followed by floating-point types.
  float_type,
  double_type,
  long_double_type,
  last_numeric_type = long_double_type,
  cstring_type,
  string_type,
  pointer_type,
  custom_type
};

// Maps core type T to the corresponding type enum constant.
template <typename T, typename Char>
struct type_constant : std::integral_constant<type, type::custom_type> {};

#define FMT_TYPE_CONSTANT(Type, constant) \
  template <typename Char>                \
  struct type_constant<Type, Char>        \
      : std::integral_constant<type, type::constant> {}

FMT_TYPE_CONSTANT(int, int_type);
FMT_TYPE_CONSTANT(unsigned, uint_type);
FMT_TYPE_CONSTANT(long long, long_long_type);
FMT_TYPE_CONSTANT(unsigned long long, ulong_long_type);
FMT_TYPE_CONSTANT(int128_opt, int128_type);
FMT_TYPE_CONSTANT(uint128_opt, uint128_type);
FMT_TYPE_CONSTANT(bool, bool_type);
FMT_TYPE_CONSTANT(Char, char_type);
FMT_TYPE_CONSTANT(float, float_type);
FMT_TYPE_CONSTANT(double, double_type);
FMT_TYPE_CONSTANT(long double, long_double_type);
FMT_TYPE_CONSTANT(const Char*, cstring_type);
FMT_TYPE_CONSTANT(basic_string_view<Char>, string_type);
FMT_TYPE_CONSTANT(const void*, pointer_type);

constexpr auto is_integral_type(type t) -> bool {
  return t > type::none_type && t <= type::last_integer_type;
}
constexpr auto is_arithmetic_type(type t) -> bool {
  return t > type::none_type && t <= type::last_numeric_type;
}

constexpr auto set(type rhs) -> int { return 1 << static_cast<int>(rhs); }
constexpr auto in(type t, int set) -> bool {
  return ((set >> static_cast<int>(t)) & 1) != 0;
}

// Bitsets of types.
enum {
  sint_set =
      set(type::int_type) | set(type::long_long_type) | set(type::int128_type),
  uint_set = set(type::uint_type) | set(type::ulong_long_type) |
             set(type::uint128_type),
  bool_set = set(type::bool_type),
  char_set = set(type::char_type),
  float_set = set(type::float_type) | set(type::double_type) |
              set(type::long_double_type),
  string_set = set(type::string_type),
  cstring_set = set(type::cstring_type),
  pointer_set = set(type::pointer_type)
};

struct view {};

template <typename T, typename Enable = std::true_type>
struct is_view : std::false_type {};
template <typename T>
struct is_view<T, bool_constant<sizeof(T) != 0>> : std::is_base_of<view, T> {};

template <typename Char, typename T> struct named_arg;
template <typename T> struct is_named_arg : std::false_type {};
template <typename T> struct is_static_named_arg : std::false_type {};

template <typename Char, typename T>
struct is_named_arg<named_arg<Char, T>> : std::true_type {};

template <typename Char, typename T> struct named_arg : view {
  const Char* name;
  const T& value;

  named_arg(const Char* n, const T& v) : name(n), value(v) {}
  static_assert(!is_named_arg<T>::value, "nested named arguments");
};

template <bool B = false> constexpr auto count() -> int { return B ? 1 : 0; }
template <bool B1, bool B2, bool... Tail> constexpr auto count() -> int {
  return (B1 ? 1 : 0) + count<B2, Tail...>();
}

template <typename... Args> constexpr auto count_named_args() -> int {
  return count<is_named_arg<Args>::value...>();
}
template <typename... Args> constexpr auto count_static_named_args() -> int {
  return count<is_static_named_arg<Args>::value...>();
}

template <typename Char> struct named_arg_info {
  const Char* name;
  int id;
};

// named_args is non-const to suppress a bogus -Wmaybe-uninitialized in gcc 13.
template <typename Char>
FMT_CONSTEXPR void check_for_duplicate(named_arg_info<Char>* named_args,
                                       int named_arg_index,
                                       basic_string_view<Char> arg_name) {
  for (int i = 0; i < named_arg_index; ++i) {
    if (named_args[i].name == arg_name) report_error("duplicate named arg");
  }
}

template <typename Char, typename T, FMT_ENABLE_IF(!is_named_arg<T>::value)>
void init_named_arg(named_arg_info<Char>*, int& arg_index, int&, const T&) {
  ++arg_index;
}
template <typename Char, typename T, FMT_ENABLE_IF(is_named_arg<T>::value)>
void init_named_arg(named_arg_info<Char>* named_args, int& arg_index,
                    int& named_arg_index, const T& arg) {
  check_for_duplicate<Char>(named_args, named_arg_index, arg.name);
  named_args[named_arg_index++] = {arg.name, arg_index++};
}

template <typename T, typename Char,
          FMT_ENABLE_IF(!is_static_named_arg<T>::value)>
FMT_CONSTEXPR void init_static_named_arg(named_arg_info<Char>*, int& arg_index,
                                         int&) {
  ++arg_index;
}
template <typename T, typename Char,
          FMT_ENABLE_IF(is_static_named_arg<T>::value)>
FMT_CONSTEXPR void init_static_named_arg(named_arg_info<Char>* named_args,
                                         int& arg_index, int& named_arg_index) {
  check_for_duplicate<Char>(named_args, named_arg_index, T::name);
  named_args[named_arg_index++] = {T::name, arg_index++};
}

// To minimize the number of types we need to deal with, long is translated
// either to int or to long long depending on its size.
enum { long_short = sizeof(long) == sizeof(int) && FMT_BUILTIN_TYPES };
using long_type = conditional_t<long_short, int, long long>;
using ulong_type = conditional_t<long_short, unsigned, unsigned long long>;

template <typename T>
using format_as_result =
    remove_cvref_t<decltype(format_as(std::declval<const T&>()))>;
template <typename T>
using format_as_member_result =
    remove_cvref_t<decltype(formatter<T>::format_as(std::declval<const T&>()))>;

template <typename T, typename Enable = std::true_type>
struct use_format_as : std::false_type {};
// format_as member is only used to avoid injection into the std namespace.
template <typename T, typename Enable = std::true_type>
struct use_format_as_member : std::false_type {};

// Only map owning types because mapping views can be unsafe.
template <typename T>
struct use_format_as<
    T, bool_constant<std::is_arithmetic<format_as_result<T>>::value>>
    : std::true_type {};
template <typename T>
struct use_format_as_member<
    T, bool_constant<std::is_arithmetic<format_as_member_result<T>>::value>>
    : std::true_type {};

template <typename T, typename U = remove_const_t<T>>
using use_formatter =
    bool_constant<(std::is_class<T>::value || std::is_enum<T>::value ||
                   std::is_union<T>::value || std::is_array<T>::value) &&
                  !has_to_string_view<T>::value && !is_named_arg<T>::value &&
                  !use_format_as<T>::value && !use_format_as_member<U>::value>;

template <typename Char, typename T, typename U = remove_const_t<T>>
auto has_formatter_impl(T* p, buffered_context<Char>* ctx = nullptr)
    -> decltype(formatter<U, Char>().format(*p, *ctx), std::true_type());
template <typename Char> auto has_formatter_impl(...) -> std::false_type;

// T can be const-qualified to check if it is const-formattable.
template <typename T, typename Char> constexpr auto has_formatter() -> bool {
  return decltype(has_formatter_impl<Char>(static_cast<T*>(nullptr)))::value;
}

// Maps formatting argument types to natively supported types or user-defined
// types with formatters. Returns void on errors to be SFINAE-friendly.
template <typename Char> struct type_mapper {
  static auto map(signed char) -> int;
  static auto map(unsigned char) -> unsigned;
  static auto map(short) -> int;
  static auto map(unsigned short) -> unsigned;
  static auto map(int) -> int;
  static auto map(unsigned) -> unsigned;
  static auto map(long) -> long_type;
  static auto map(unsigned long) -> ulong_type;
  static auto map(long long) -> long long;
  static auto map(unsigned long long) -> unsigned long long;
  static auto map(int128_opt) -> int128_opt;
  static auto map(uint128_opt) -> uint128_opt;
  static auto map(bool) -> bool;

  template <int N>
  static auto map(bitint<N>) -> conditional_t<N <= 64, long long, void>;
  template <int N>
  static auto map(ubitint<N>)
      -> conditional_t<N <= 64, unsigned long long, void>;

  template <typename T, FMT_ENABLE_IF(is_char<T>::value)>
  static auto map(T) -> conditional_t<
      std::is_same<T, char>::value || std::is_same<T, Char>::value, Char, void>;

  static auto map(float) -> float;
  static auto map(double) -> double;
  static auto map(long double) -> long double;

  static auto map(Char*) -> const Char*;
  static auto map(const Char*) -> const Char*;
  template <typename T, typename C = char_t<T>,
            FMT_ENABLE_IF(!std::is_pointer<T>::value)>
  static auto map(const T&) -> conditional_t<std::is_same<C, Char>::value,
                                             basic_string_view<C>, void>;

  static auto map(void*) -> const void*;
  static auto map(const void*) -> const void*;
  static auto map(volatile void*) -> const void*;
  static auto map(const volatile void*) -> const void*;
  static auto map(nullptr_t) -> const void*;
  template <typename T, FMT_ENABLE_IF(std::is_pointer<T>::value ||
                                      std::is_member_pointer<T>::value)>
  static auto map(const T&) -> void;

  template <typename T, FMT_ENABLE_IF(use_format_as<T>::value)>
  static auto map(const T& x) -> decltype(map(format_as(x)));
  template <typename T, FMT_ENABLE_IF(use_format_as_member<T>::value)>
  static auto map(const T& x) -> decltype(map(formatter<T>::format_as(x)));

  template <typename T, FMT_ENABLE_IF(use_formatter<T>::value)>
  static auto map(T&) -> conditional_t<has_formatter<T, Char>(), T&, void>;

  template <typename T, FMT_ENABLE_IF(is_named_arg<T>::value)>
  static auto map(const T& named_arg) -> decltype(map(named_arg.value));
};

// detail:: is used to workaround a bug in MSVC 2017.
template <typename T, typename Char>
using mapped_t = decltype(detail::type_mapper<Char>::map(std::declval<T&>()));

// A type constant after applying type_mapper.
template <typename T, typename Char = char>
using mapped_type_constant = type_constant<mapped_t<T, Char>, Char>;

template <typename T, typename Context,
          type TYPE =
              mapped_type_constant<T, typename Context::char_type>::value>
using stored_type_constant = std::integral_constant<
    type, Context::builtin_types || TYPE == type::int_type ? TYPE
                                                           : type::custom_type>;
// A parse context with extra data used only in compile-time checks.
template <typename Char>
class compile_parse_context : public parse_context<Char> {
 private:
  int num_args_;
  const type* types_;
  using base = parse_context<Char>;

 public:
  FMT_CONSTEXPR explicit compile_parse_context(basic_string_view<Char> fmt,
                                               int num_args, const type* types,
                                               int next_arg_id = 0)
      : base(fmt, next_arg_id), num_args_(num_args), types_(types) {}

  constexpr auto num_args() const -> int { return num_args_; }
  constexpr auto arg_type(int id) const -> type { return types_[id]; }

  FMT_CONSTEXPR auto next_arg_id() -> int {
    int id = base::next_arg_id();
    if (id >= num_args_) report_error("argument not found");
    return id;
  }

  FMT_CONSTEXPR void check_arg_id(int id) {
    base::check_arg_id(id);
    if (id >= num_args_) report_error("argument not found");
  }
  using base::check_arg_id;

  FMT_CONSTEXPR void check_dynamic_spec(int arg_id) {
    ignore_unused(arg_id);
    if (arg_id < num_args_ && types_ && !is_integral_type(types_[arg_id]))
      report_error("width/precision is not integer");
  }
};

// An argument reference.
template <typename Char> union arg_ref {
  FMT_CONSTEXPR arg_ref(int idx = 0) : index(idx) {}
  FMT_CONSTEXPR arg_ref(basic_string_view<Char> n) : name(n) {}

  int index;
  basic_string_view<Char> name;
};

// Format specifiers with width and precision resolved at formatting rather
// than parsing time to allow reusing the same parsed specifiers with
// different sets of arguments (precompilation of format strings).
template <typename Char = char> struct dynamic_format_specs : format_specs {
  arg_ref<Char> width_ref;
  arg_ref<Char> precision_ref;
};

// Converts a character to ASCII. Returns '\0' on conversion failure.
template <typename Char, FMT_ENABLE_IF(std::is_integral<Char>::value)>
constexpr auto to_ascii(Char c) -> char {
  return c <= 0xff ? static_cast<char>(c) : '\0';
}

// Returns the number of code units in a code point or 1 on error.
template <typename Char>
FMT_CONSTEXPR auto code_point_length(const Char* begin) -> int {
  if (const_check(sizeof(Char) != 1)) return 1;
  auto c = static_cast<unsigned char>(*begin);
  return static_cast<int>((0x3a55000000000000ull >> (2 * (c >> 3))) & 3) + 1;
}

// Parses the range [begin, end) as an unsigned integer. This function assumes
// that the range is non-empty and the first character is a digit.
template <typename Char>
FMT_CONSTEXPR auto parse_nonnegative_int(const Char*& begin, const Char* end,
                                         int error_value) noexcept -> int {
  FMT_ASSERT(begin != end && '0' <= *begin && *begin <= '9', "");
  unsigned value = 0, prev = 0;
  auto p = begin;
  do {
    prev = value;
    value = value * 10 + unsigned(*p - '0');
    ++p;
  } while (p != end && '0' <= *p && *p <= '9');
  auto num_digits = p - begin;
  begin = p;
  int digits10 = static_cast<int>(sizeof(int) * CHAR_BIT * 3 / 10);
  if (num_digits <= digits10) return static_cast<int>(value);
  // Check for overflow.
  unsigned max = INT_MAX;
  return num_digits == digits10 + 1 &&
                 prev * 10ull + unsigned(p[-1] - '0') <= max
             ? static_cast<int>(value)
             : error_value;
}

FMT_CONSTEXPR inline auto parse_align(char c) -> align {
  switch (c) {
  case '<': return align::left;
  case '>': return align::right;
  case '^': return align::center;
  }
  return align::none;
}

template <typename Char> constexpr auto is_name_start(Char c) -> bool {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

template <typename Char, typename Handler>
FMT_CONSTEXPR auto parse_arg_id(const Char* begin, const Char* end,
                                Handler&& handler) -> const Char* {
  Char c = *begin;
  if (c >= '0' && c <= '9') {
    int index = 0;
    if (c != '0')
      index = parse_nonnegative_int(begin, end, INT_MAX);
    else
      ++begin;
    if (begin == end || (*begin != '}' && *begin != ':'))
      report_error("invalid format string");
    else
      handler.on_index(index);
    return begin;
  }
  if (FMT_OPTIMIZE_SIZE > 1 || !is_name_start(c)) {
    report_error("invalid format string");
    return begin;
  }
  auto it = begin;
  do {
    ++it;
  } while (it != end && (is_name_start(*it) || ('0' <= *it && *it <= '9')));
  handler.on_name({begin, to_unsigned(it - begin)});
  return it;
}

template <typename Char> struct dynamic_spec_handler {
  parse_context<Char>& ctx;
  arg_ref<Char>& ref;
  arg_id_kind& kind;

  FMT_CONSTEXPR void on_index(int id) {
    ref = id;
    kind = arg_id_kind::index;
    ctx.check_arg_id(id);
    ctx.check_dynamic_spec(id);
  }
  FMT_CONSTEXPR void on_name(basic_string_view<Char> id) {
    ref = id;
    kind = arg_id_kind::name;
    ctx.check_arg_id(id);
  }
};

template <typename Char> struct parse_dynamic_spec_result {
  const Char* end;
  arg_id_kind kind;
};

// Parses integer | "{" [arg_id] "}".
template <typename Char>
FMT_CONSTEXPR auto parse_dynamic_spec(const Char* begin, const Char* end,
                                      int& value, arg_ref<Char>& ref,
                                      parse_context<Char>& ctx)
    -> parse_dynamic_spec_result<Char> {
  FMT_ASSERT(begin != end, "");
  auto kind = arg_id_kind::none;
  if ('0' <= *begin && *begin <= '9') {
    int val = parse_nonnegative_int(begin, end, -1);
    if (val == -1) report_error("number is too big");
    value = val;
  } else {
    if (*begin == '{') {
      ++begin;
      if (begin != end) {
        Char c = *begin;
        if (c == '}' || c == ':') {
          int id = ctx.next_arg_id();
          ref = id;
          kind = arg_id_kind::index;
          ctx.check_dynamic_spec(id);
        } else {
          begin = parse_arg_id(begin, end,
                               dynamic_spec_handler<Char>{ctx, ref, kind});
        }
      }
      if (begin != end && *begin == '}') return {++begin, kind};
    }
    report_error("invalid format string");
  }
  return {begin, kind};
}

template <typename Char>
FMT_CONSTEXPR auto parse_width(const Char* begin, const Char* end,
                               format_specs& specs, arg_ref<Char>& width_ref,
                               parse_context<Char>& ctx) -> const Char* {
  auto result = parse_dynamic_spec(begin, end, specs.width, width_ref, ctx);
  specs.set_dynamic_width(result.kind);
  return result.end;
}

template <typename Char>
FMT_CONSTEXPR auto parse_precision(const Char* begin, const Char* end,
                                   format_specs& specs,
                                   arg_ref<Char>& precision_ref,
                                   parse_context<Char>& ctx) -> const Char* {
  ++begin;
  if (begin == end) {
    report_error("invalid precision");
    return begin;
  }
  auto result =
      parse_dynamic_spec(begin, end, specs.precision, precision_ref, ctx);
  specs.set_dynamic_precision(result.kind);
  return result.end;
}

enum class state { start, align, sign, hash, zero, width, precision, locale };

// Parses standard format specifiers.
template <typename Char>
FMT_CONSTEXPR auto parse_format_specs(const Char* begin, const Char* end,
                                      dynamic_format_specs<Char>& specs,
                                      parse_context<Char>& ctx, type arg_type)
    -> const Char* {
  auto c = '\0';
  if (end - begin > 1) {
    auto next = to_ascii(begin[1]);
    c = parse_align(next) == align::none ? to_ascii(*begin) : '\0';
  } else {
    if (begin == end) return begin;
    c = to_ascii(*begin);
  }

  struct {
    state current_state = state::start;
    FMT_CONSTEXPR void operator()(state s, bool valid = true) {
      if (current_state >= s || !valid)
        report_error("invalid format specifier");
      current_state = s;
    }
  } enter_state;

  using pres = presentation_type;
  constexpr auto integral_set = sint_set | uint_set | bool_set | char_set;
  struct {
    const Char*& begin;
    format_specs& specs;
    type arg_type;

    FMT_CONSTEXPR auto operator()(pres pres_type, int set) -> const Char* {
      if (!in(arg_type, set)) report_error("invalid format specifier");
      specs.set_type(pres_type);
      return begin + 1;
    }
  } parse_presentation_type{begin, specs, arg_type};

  for (;;) {
    switch (c) {
    case '<':
    case '>':
    case '^':
      enter_state(state::align);
      specs.set_align(parse_align(c));
      ++begin;
      break;
    case '+':
    case ' ':
      specs.set_sign(c == ' ' ? sign::space : sign::plus);
      FMT_FALLTHROUGH;
    case '-':
      enter_state(state::sign, in(arg_type, sint_set | float_set));
      ++begin;
      break;
    case '#':
      enter_state(state::hash, is_arithmetic_type(arg_type));
      specs.set_alt();
      ++begin;
      break;
    case '0':
      enter_state(state::zero);
      if (!is_arithmetic_type(arg_type))
        report_error("format specifier requires numeric argument");
      if (specs.align() == align::none) {
        // Ignore 0 if align is specified for compatibility with std::format.
        specs.set_align(align::numeric);
        specs.set_fill('0');
      }
      ++begin;
      break;
      // clang-format off
    case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9': case '{':
      // clang-format on
      enter_state(state::width);
      begin = parse_width(begin, end, specs, specs.width_ref, ctx);
      break;
    case '.':
      enter_state(state::precision,
                  in(arg_type, float_set | string_set | cstring_set));
      begin = parse_precision(begin, end, specs, specs.precision_ref, ctx);
      break;
    case 'L':
      enter_state(state::locale, is_arithmetic_type(arg_type));
      specs.set_localized();
      ++begin;
      break;
    case 'd': return parse_presentation_type(pres::dec, integral_set);
    case 'X': specs.set_upper(); FMT_FALLTHROUGH;
    case 'x': return parse_presentation_type(pres::hex, integral_set);
    case 'o': return parse_presentation_type(pres::oct, integral_set);
    case 'B': specs.set_upper(); FMT_FALLTHROUGH;
    case 'b': return parse_presentation_type(pres::bin, integral_set);
    case 'E': specs.set_upper(); FMT_FALLTHROUGH;
    case 'e': return parse_presentation_type(pres::exp, float_set);
    case 'F': specs.set_upper(); FMT_FALLTHROUGH;
    case 'f': return parse_presentation_type(pres::fixed, float_set);
    case 'G': specs.set_upper(); FMT_FALLTHROUGH;
    case 'g': return parse_presentation_type(pres::general, float_set);
    case 'A': specs.set_upper(); FMT_FALLTHROUGH;
    case 'a': return parse_presentation_type(pres::hexfloat, float_set);
    case 'c':
      if (arg_type == type::bool_type) report_error("invalid format specifier");
      return parse_presentation_type(pres::chr, integral_set);
    case 's':
      return parse_presentation_type(pres::string,
                                     bool_set | string_set | cstring_set);
    case 'p':
      return parse_presentation_type(pres::pointer, pointer_set | cstring_set);
    case '?':
      return parse_presentation_type(pres::debug,
                                     char_set | string_set | cstring_set);
    case '}': return begin;
    default:  {
      if (*begin == '}') return begin;
      // Parse fill and alignment.
      auto fill_end = begin + code_point_length(begin);
      if (end - fill_end <= 0) {
        report_error("invalid format specifier");
        return begin;
      }
      if (*begin == '{') {
        report_error("invalid fill character '{'");
        return begin;
      }
      auto alignment = parse_align(to_ascii(*fill_end));
      enter_state(state::align, alignment != align::none);
      specs.set_fill(
          basic_string_view<Char>(begin, to_unsigned(fill_end - begin)));
      specs.set_align(alignment);
      begin = fill_end + 1;
    }
    }
    if (begin == end) return begin;
    c = to_ascii(*begin);
  }
}

template <typename Char, typename Handler>
FMT_CONSTEXPR FMT_INLINE auto parse_replacement_field(const Char* begin,
                                                      const Char* end,
                                                      Handler&& handler)
    -> const Char* {
  ++begin;
  if (begin == end) {
    handler.on_error("invalid format string");
    return end;
  }
  int arg_id = 0;
  switch (*begin) {
  case '}':
    handler.on_replacement_field(handler.on_arg_id(), begin);
    return begin + 1;
  case '{': handler.on_text(begin, begin + 1); return begin + 1;
  case ':': arg_id = handler.on_arg_id(); break;
  default:  {
    struct id_adapter {
      Handler& handler;
      int arg_id;

      FMT_CONSTEXPR void on_index(int id) { arg_id = handler.on_arg_id(id); }
      FMT_CONSTEXPR void on_name(basic_string_view<Char> id) {
        arg_id = handler.on_arg_id(id);
      }
    } adapter = {handler, 0};
    begin = parse_arg_id(begin, end, adapter);
    arg_id = adapter.arg_id;
    Char c = begin != end ? *begin : Char();
    if (c == '}') {
      handler.on_replacement_field(arg_id, begin);
      return begin + 1;
    }
    if (c != ':') {
      handler.on_error("missing '}' in format string");
      return end;
    }
    break;
  }
  }
  begin = handler.on_format_specs(arg_id, begin + 1, end);
  if (begin == end || *begin != '}')
    return handler.on_error("unknown format specifier"), end;
  return begin + 1;
}

template <typename Char, typename Handler>
FMT_CONSTEXPR void parse_format_string(basic_string_view<Char> fmt,
                                       Handler&& handler) {
  auto begin = fmt.data(), end = begin + fmt.size();
  auto p = begin;
  while (p != end) {
    auto c = *p++;
    if (c == '{') {
      handler.on_text(begin, p - 1);
      begin = p = parse_replacement_field(p - 1, end, handler);
    } else if (c == '}') {
      if (p == end || *p != '}')
        return handler.on_error("unmatched '}' in format string");
      handler.on_text(begin, p);
      begin = ++p;
    }
  }
  handler.on_text(begin, end);
}

// Checks char specs and returns true iff the presentation type is char-like.
FMT_CONSTEXPR inline auto check_char_specs(const format_specs& specs) -> bool {
  auto type = specs.type();
  if (type != presentation_type::none && type != presentation_type::chr &&
      type != presentation_type::debug) {
    return false;
  }
  if (specs.align() == align::numeric || specs.sign() != sign::none ||
      specs.alt()) {
    report_error("invalid format specifier for char");
  }
  return true;
}

// A base class for compile-time strings.
struct compile_string {};

template <typename T, typename Char>
FMT_VISIBILITY("hidden")  // Suppress an ld warning on macOS (#3769).
FMT_CONSTEXPR auto invoke_parse(parse_context<Char>& ctx) -> const Char* {
  using mapped_type = remove_cvref_t<mapped_t<T, Char>>;
  constexpr bool formattable =
      std::is_constructible<formatter<mapped_type, Char>>::value;
  if (!formattable) return ctx.begin();  // Error is reported in the value ctor.
  using formatted_type = conditional_t<formattable, mapped_type, int>;
  return formatter<formatted_type, Char>().parse(ctx);
}

template <typename... T> struct arg_pack {};

template <typename Char, int NUM_ARGS, int NUM_NAMED_ARGS, bool DYNAMIC_NAMES>
class format_string_checker {
 private:
  type types_[max_of<size_t>(1, NUM_ARGS)];
  named_arg_info<Char> named_args_[max_of<size_t>(1, NUM_NAMED_ARGS)];
  compile_parse_context<Char> context_;

  using parse_func = auto (*)(parse_context<Char>&) -> const Char*;
  parse_func parse_funcs_[max_of<size_t>(1, NUM_ARGS)];

 public:
  template <typename... T>
  FMT_CONSTEXPR explicit format_string_checker(basic_string_view<Char> fmt,
                                               arg_pack<T...>)
      : types_{mapped_type_constant<T, Char>::value...},
        named_args_{},
        context_(fmt, NUM_ARGS, types_),
        parse_funcs_{&invoke_parse<T, Char>...} {
    int arg_index = 0, named_arg_index = 0;
    FMT_APPLY_VARIADIC(
        init_static_named_arg<T>(named_args_, arg_index, named_arg_index));
    ignore_unused(arg_index, named_arg_index);
  }

  FMT_CONSTEXPR void on_text(const Char*, const Char*) {}

  FMT_CONSTEXPR auto on_arg_id() -> int { return context_.next_arg_id(); }
  FMT_CONSTEXPR auto on_arg_id(int id) -> int {
    context_.check_arg_id(id);
    return id;
  }
  FMT_CONSTEXPR auto on_arg_id(basic_string_view<Char> id) -> int {
    for (int i = 0; i < NUM_NAMED_ARGS; ++i) {
      if (named_args_[i].name == id) return named_args_[i].id;
    }
    if (!DYNAMIC_NAMES) on_error("argument not found");
    return -1;
  }

  FMT_CONSTEXPR void on_replacement_field(int id, const Char* begin) {
    on_format_specs(id, begin, begin);  // Call parse() on empty specs.
  }

  FMT_CONSTEXPR auto on_format_specs(int id, const Char* begin, const Char* end)
      -> const Char* {
    context_.advance_to(begin);
    if (id >= 0 && id < NUM_ARGS) return parse_funcs_[id](context_);

    // If id is out of range, it means we do not know the type and cannot parse
    // the format at compile time. Instead, skip over content until we finish
    // the format spec, accounting for any nested replacements.
    for (int bracket_count = 0;
         begin != end && (bracket_count > 0 || *begin != '}'); ++begin) {
      if (*begin == '{')
        ++bracket_count;
      else if (*begin == '}')
        --bracket_count;
    }
    return begin;
  }

  FMT_NORETURN FMT_CONSTEXPR void on_error(const char* message) {
    report_error(message);
  }
};

/// A contiguous memory buffer with an optional growing ability. It is an
/// internal class and shouldn't be used directly, only via `memory_buffer`.
template <typename T> class buffer {
 private:
  T* ptr_;
  size_t size_;
  size_t capacity_;

  using grow_fun = void (*)(buffer& buf, size_t capacity);
  grow_fun grow_;

 protected:
  // Don't initialize ptr_ since it is not accessed to save a few cycles.
  FMT_MSC_WARNING(suppress : 26495)
  FMT_CONSTEXPR buffer(grow_fun grow, size_t sz) noexcept
      : size_(sz), capacity_(sz), grow_(grow) {}

  constexpr buffer(grow_fun grow, T* p = nullptr, size_t sz = 0,
                   size_t cap = 0) noexcept
      : ptr_(p), size_(sz), capacity_(cap), grow_(grow) {}

  FMT_CONSTEXPR20 ~buffer() = default;
  buffer(buffer&&) = default;

  /// Sets the buffer data and capacity.
  FMT_CONSTEXPR void set(T* buf_data, size_t buf_capacity) noexcept {
    ptr_ = buf_data;
    capacity_ = buf_capacity;
  }

 public:
  using value_type = T;
  using const_reference = const T&;

  buffer(const buffer&) = delete;
  void operator=(const buffer&) = delete;

  auto begin() noexcept -> T* { return ptr_; }
  auto end() noexcept -> T* { return ptr_ + size_; }

  auto begin() const noexcept -> const T* { return ptr_; }
  auto end() const noexcept -> const T* { return ptr_ + size_; }

  /// Returns the size of this buffer.
  constexpr auto size() const noexcept -> size_t { return size_; }

  /// Returns the capacity of this buffer.
  constexpr auto capacity() const noexcept -> size_t { return capacity_; }

  /// Returns a pointer to the buffer data (not null-terminated).
  FMT_CONSTEXPR auto data() noexcept -> T* { return ptr_; }
  FMT_CONSTEXPR auto data() const noexcept -> const T* { return ptr_; }

  /// Clears this buffer.
  FMT_CONSTEXPR void clear() { size_ = 0; }

  // Tries resizing the buffer to contain `count` elements. If T is a POD type
  // the new elements may not be initialized.
  FMT_CONSTEXPR void try_resize(size_t count) {
    try_reserve(count);
    size_ = min_of(count, capacity_);
  }

  // Tries increasing the buffer capacity to `new_capacity`. It can increase the
  // capacity by a smaller amount than requested but guarantees there is space
  // for at least one additional element either by increasing the capacity or by
  // flushing the buffer if it is full.
  FMT_CONSTEXPR void try_reserve(size_t new_capacity) {
    if (new_capacity > capacity_) grow_(*this, new_capacity);
  }

  FMT_CONSTEXPR void push_back(const T& value) {
    try_reserve(size_ + 1);
    ptr_[size_++] = value;
  }

  /// Appends data to the end of the buffer.
  template <typename U>
// Workaround for MSVC2019 to fix error C2893: Failed to specialize function
// template 'void fmt::v11::detail::buffer<T>::append(const U *,const U *)'.
#if !FMT_MSC_VERSION || FMT_MSC_VERSION >= 1940
  FMT_CONSTEXPR20
#endif
      void
      append(const U* begin, const U* end) {
    while (begin != end) {
      auto count = to_unsigned(end - begin);
      try_reserve(size_ + count);
      auto free_cap = capacity_ - size_;
      if (free_cap < count) count = free_cap;
      // A loop is faster than memcpy on small sizes.
      T* out = ptr_ + size_;
      for (size_t i = 0; i < count; ++i) out[i] = begin[i];
      size_ += count;
      begin += count;
    }
  }

  template <typename Idx> FMT_CONSTEXPR auto operator[](Idx index) -> T& {
    return ptr_[index];
  }
  template <typename Idx>
  FMT_CONSTEXPR auto operator[](Idx index) const -> const T& {
    return ptr_[index];
  }
};

struct buffer_traits {
  constexpr explicit buffer_traits(size_t) {}
  constexpr auto count() const -> size_t { return 0; }
  constexpr auto limit(size_t size) const -> size_t { return size; }
};

class fixed_buffer_traits {
 private:
  size_t count_ = 0;
  size_t limit_;

 public:
  constexpr explicit fixed_buffer_traits(size_t limit) : limit_(limit) {}
  constexpr auto count() const -> size_t { return count_; }
  FMT_CONSTEXPR auto limit(size_t size) -> size_t {
    size_t n = limit_ > count_ ? limit_ - count_ : 0;
    count_ += size;
    return min_of(size, n);
  }
};

// A buffer that writes to an output iterator when flushed.
template <typename OutputIt, typename T, typename Traits = buffer_traits>
class iterator_buffer : public Traits, public buffer<T> {
 private:
  OutputIt out_;
  enum { buffer_size = 256 };
  T data_[buffer_size];

  static FMT_CONSTEXPR void grow(buffer<T>& buf, size_t) {
    if (buf.size() == buffer_size) static_cast<iterator_buffer&>(buf).flush();
  }

  void flush() {
    auto size = this->size();
    this->clear();
    const T* begin = data_;
    const T* end = begin + this->limit(size);
    while (begin != end) *out_++ = *begin++;
  }

 public:
  explicit iterator_buffer(OutputIt out, size_t n = buffer_size)
      : Traits(n), buffer<T>(grow, data_, 0, buffer_size), out_(out) {}
  iterator_buffer(iterator_buffer&& other) noexcept
      : Traits(other),
        buffer<T>(grow, data_, 0, buffer_size),
        out_(other.out_) {}
  ~iterator_buffer() {
    // Don't crash if flush fails during unwinding.
    FMT_TRY { flush(); }
    FMT_CATCH(...) {}
  }

  auto out() -> OutputIt {
    flush();
    return out_;
  }
  auto count() const -> size_t { return Traits::count() + this->size(); }
};

template <typename T>
class iterator_buffer<T*, T, fixed_buffer_traits> : public fixed_buffer_traits,
                                                    public buffer<T> {
 private:
  T* out_;
  enum { buffer_size = 256 };
  T data_[buffer_size];

  static FMT_CONSTEXPR void grow(buffer<T>& buf, size_t) {
    if (buf.size() == buf.capacity())
      static_cast<iterator_buffer&>(buf).flush();
  }

  void flush() {
    size_t n = this->limit(this->size());
    if (this->data() == out_) {
      out_ += n;
      this->set(data_, buffer_size);
    }
    this->clear();
  }

 public:
  explicit iterator_buffer(T* out, size_t n = buffer_size)
      : fixed_buffer_traits(n), buffer<T>(grow, out, 0, n), out_(out) {}
  iterator_buffer(iterator_buffer&& other) noexcept
      : fixed_buffer_traits(other),
        buffer<T>(static_cast<iterator_buffer&&>(other)),
        out_(other.out_) {
    if (this->data() != out_) {
      this->set(data_, buffer_size);
      this->clear();
    }
  }
  ~iterator_buffer() { flush(); }

  auto out() -> T* {
    flush();
    return out_;
  }
  auto count() const -> size_t {
    return fixed_buffer_traits::count() + this->size();
  }
};

template <typename T> class iterator_buffer<T*, T> : public buffer<T> {
 public:
  explicit iterator_buffer(T* out, size_t = 0)
      : buffer<T>([](buffer<T>&, size_t) {}, out, 0, ~size_t()) {}

  auto out() -> T* { return &*this->end(); }
};

template <typename Container>
class container_buffer : public buffer<typename Container::value_type> {
 private:
  using value_type = typename Container::value_type;

  static FMT_CONSTEXPR void grow(buffer<value_type>& buf, size_t capacity) {
    auto& self = static_cast<container_buffer&>(buf);
    self.container.resize(capacity);
    self.set(&self.container[0], capacity);
  }

 public:
  Container& container;

  explicit container_buffer(Container& c)
      : buffer<value_type>(grow, c.size()), container(c) {}
};

// A buffer that writes to a container with the contiguous storage.
template <typename OutputIt>
class iterator_buffer<
    OutputIt,
    enable_if_t<is_back_insert_iterator<OutputIt>::value &&
                    is_contiguous<typename OutputIt::container_type>::value,
                typename OutputIt::container_type::value_type>>
    : public container_buffer<typename OutputIt::container_type> {
 private:
  using base = container_buffer<typename OutputIt::container_type>;

 public:
  explicit iterator_buffer(typename OutputIt::container_type& c) : base(c) {}
  explicit iterator_buffer(OutputIt out, size_t = 0)
      : base(get_container(out)) {}

  auto out() -> OutputIt { return OutputIt(this->container); }
};

// A buffer that counts the number of code units written discarding the output.
template <typename T = char> class counting_buffer : public buffer<T> {
 private:
  enum { buffer_size = 256 };
  T data_[buffer_size];
  size_t count_ = 0;

  static FMT_CONSTEXPR void grow(buffer<T>& buf, size_t) {
    if (buf.size() != buffer_size) return;
    static_cast<counting_buffer&>(buf).count_ += buf.size();
    buf.clear();
  }

 public:
  FMT_CONSTEXPR counting_buffer() : buffer<T>(grow, data_, 0, buffer_size) {}

  constexpr auto count() const noexcept -> size_t {
    return count_ + this->size();
  }
};

template <typename T>
struct is_back_insert_iterator<basic_appender<T>> : std::true_type {};

template <typename OutputIt, typename InputIt, typename = void>
struct has_back_insert_iterator_container_append : std::false_type {};
template <typename OutputIt, typename InputIt>
struct has_back_insert_iterator_container_append<
    OutputIt, InputIt,
    void_t<decltype(get_container(std::declval<OutputIt>())
                        .append(std::declval<InputIt>(),
                                std::declval<InputIt>()))>> : std::true_type {};

template <typename OutputIt, typename InputIt, typename = void>
struct has_back_insert_iterator_container_insert_at_end : std::false_type {};

template <typename OutputIt, typename InputIt>
struct has_back_insert_iterator_container_insert_at_end<
    OutputIt, InputIt,
    void_t<decltype(get_container(std::declval<OutputIt>())
                        .insert(get_container(std::declval<OutputIt>()).end(),
                                std::declval<InputIt>(),
                                std::declval<InputIt>()))>> : std::true_type {};

// An optimized version of std::copy with the output value type (T).
template <typename T, typename InputIt, typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value&&
                            has_back_insert_iterator_container_append<
                                OutputIt, InputIt>::value)>
FMT_CONSTEXPR20 auto copy(InputIt begin, InputIt end, OutputIt out)
    -> OutputIt {
  get_container(out).append(begin, end);
  return out;
}

template <typename T, typename InputIt, typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value &&
                        !has_back_insert_iterator_container_append<
                            OutputIt, InputIt>::value &&
                        has_back_insert_iterator_container_insert_at_end<
                            OutputIt, InputIt>::value)>
FMT_CONSTEXPR20 auto copy(InputIt begin, InputIt end, OutputIt out)
    -> OutputIt {
  auto& c = get_container(out);
  c.insert(c.end(), begin, end);
  return out;
}

template <typename T, typename InputIt, typename OutputIt,
          FMT_ENABLE_IF(!(is_back_insert_iterator<OutputIt>::value &&
                          (has_back_insert_iterator_container_append<
                               OutputIt, InputIt>::value ||
                           has_back_insert_iterator_container_insert_at_end<
                               OutputIt, InputIt>::value)))>
FMT_CONSTEXPR auto copy(InputIt begin, InputIt end, OutputIt out) -> OutputIt {
  while (begin != end) *out++ = static_cast<T>(*begin++);
  return out;
}

template <typename T, typename V, typename OutputIt>
FMT_CONSTEXPR auto copy(basic_string_view<V> s, OutputIt out) -> OutputIt {
  return copy<T>(s.begin(), s.end(), out);
}

template <typename It, typename Enable = std::true_type>
struct is_buffer_appender : std::false_type {};
template <typename It>
struct is_buffer_appender<
    It, bool_constant<
            is_back_insert_iterator<It>::value &&
            std::is_base_of<buffer<typename It::container_type::value_type>,
                            typename It::container_type>::value>>
    : std::true_type {};

// Maps an output iterator to a buffer.
template <typename T, typename OutputIt,
          FMT_ENABLE_IF(!is_buffer_appender<OutputIt>::value)>
auto get_buffer(OutputIt out) -> iterator_buffer<OutputIt, T> {
  return iterator_buffer<OutputIt, T>(out);
}
template <typename T, typename OutputIt,
          FMT_ENABLE_IF(is_buffer_appender<OutputIt>::value)>
auto get_buffer(OutputIt out) -> buffer<T>& {
  return get_container(out);
}

template <typename Buf, typename OutputIt>
auto get_iterator(Buf& buf, OutputIt) -> decltype(buf.out()) {
  return buf.out();
}
template <typename T, typename OutputIt>
auto get_iterator(buffer<T>&, OutputIt out) -> OutputIt {
  return out;
}

// This type is intentionally undefined, only used for errors.
template <typename T, typename Char> struct type_is_unformattable_for;

template <typename Char> struct string_value {
  const Char* data;
  size_t size;
  auto str() const -> basic_string_view<Char> { return {data, size}; }
};

template <typename Context> struct custom_value {
  using char_type = typename Context::char_type;
  void* value;
  void (*format)(void* arg, parse_context<char_type>& parse_ctx, Context& ctx);
};

template <typename Char> struct named_arg_value {
  const named_arg_info<Char>* data;
  size_t size;
};

struct custom_tag {};

#if !FMT_BUILTIN_TYPES
#  define FMT_BUILTIN , monostate
#else
#  define FMT_BUILTIN
#endif

// A formatting argument value.
template <typename Context> class value {
 public:
  using char_type = typename Context::char_type;

  union {
    monostate no_value;
    int int_value;
    unsigned uint_value;
    long long long_long_value;
    unsigned long long ulong_long_value;
    int128_opt int128_value;
    uint128_opt uint128_value;
    bool bool_value;
    char_type char_value;
    float float_value;
    double double_value;
    long double long_double_value;
    const void* pointer;
    string_value<char_type> string;
    custom_value<Context> custom;
    named_arg_value<char_type> named_args;
  };

  constexpr FMT_INLINE value() : no_value() {}
  constexpr FMT_INLINE value(signed char x) : int_value(x) {}
  constexpr FMT_INLINE value(unsigned char x FMT_BUILTIN) : uint_value(x) {}
  constexpr FMT_INLINE value(signed short x) : int_value(x) {}
  constexpr FMT_INLINE value(unsigned short x FMT_BUILTIN) : uint_value(x) {}
  constexpr FMT_INLINE value(int x) : int_value(x) {}
  constexpr FMT_INLINE value(unsigned x FMT_BUILTIN) : uint_value(x) {}
  FMT_CONSTEXPR FMT_INLINE value(long x FMT_BUILTIN) : value(long_type(x)) {}
  FMT_CONSTEXPR FMT_INLINE value(unsigned long x FMT_BUILTIN)
      : value(ulong_type(x)) {}
  constexpr FMT_INLINE value(long long x FMT_BUILTIN) : long_long_value(x) {}
  constexpr FMT_INLINE value(unsigned long long x FMT_BUILTIN)
      : ulong_long_value(x) {}
  FMT_INLINE value(int128_opt x FMT_BUILTIN) : int128_value(x) {}
  FMT_INLINE value(uint128_opt x FMT_BUILTIN) : uint128_value(x) {}
  constexpr FMT_INLINE value(bool x FMT_BUILTIN) : bool_value(x) {}

  template <int N>
  constexpr FMT_INLINE value(bitint<N> x FMT_BUILTIN) : long_long_value(x) {
    static_assert(N <= 64, "unsupported _BitInt");
  }
  template <int N>
  constexpr FMT_INLINE value(ubitint<N> x FMT_BUILTIN) : ulong_long_value(x) {
    static_assert(N <= 64, "unsupported _BitInt");
  }

  template <typename T, FMT_ENABLE_IF(is_char<T>::value)>
  constexpr FMT_INLINE value(T x FMT_BUILTIN) : char_value(x) {
    static_assert(
        std::is_same<T, char>::value || std::is_same<T, char_type>::value,
        "mixing character types is disallowed");
  }

  constexpr FMT_INLINE value(float x FMT_BUILTIN) : float_value(x) {}
  constexpr FMT_INLINE value(double x FMT_BUILTIN) : double_value(x) {}
  FMT_INLINE value(long double x FMT_BUILTIN) : long_double_value(x) {}

  FMT_CONSTEXPR FMT_INLINE value(char_type* x FMT_BUILTIN) {
    string.data = x;
    if (is_constant_evaluated()) string.size = 0;
  }
  FMT_CONSTEXPR FMT_INLINE value(const char_type* x FMT_BUILTIN) {
    string.data = x;
    if (is_constant_evaluated()) string.size = 0;
  }
  template <typename T, typename C = char_t<T>,
            FMT_ENABLE_IF(!std::is_pointer<T>::value)>
  FMT_CONSTEXPR value(const T& x FMT_BUILTIN) {
    static_assert(std::is_same<C, char_type>::value,
                  "mixing character types is disallowed");
    auto sv = to_string_view(x);
    string.data = sv.data();
    string.size = sv.size();
  }
  FMT_INLINE value(void* x FMT_BUILTIN) : pointer(x) {}
  FMT_INLINE value(const void* x FMT_BUILTIN) : pointer(x) {}
  FMT_INLINE value(volatile void* x FMT_BUILTIN)
      : pointer(const_cast<const void*>(x)) {}
  FMT_INLINE value(const volatile void* x FMT_BUILTIN)
      : pointer(const_cast<const void*>(x)) {}
  FMT_INLINE value(nullptr_t) : pointer(nullptr) {}

  template <typename T, FMT_ENABLE_IF(std::is_pointer<T>::value ||
                                      std::is_member_pointer<T>::value)>
  value(const T&) {
    // Formatting of arbitrary pointers is disallowed. If you want to format a
    // pointer cast it to `void*` or `const void*`. In particular, this forbids
    // formatting of `[const] volatile char*` printed as bool by iostreams.
    static_assert(sizeof(T) == 0,
                  "formatting of non-void pointers is disallowed");
  }

  template <typename T, FMT_ENABLE_IF(use_format_as<T>::value)>
  value(const T& x) : value(format_as(x)) {}
  template <typename T, FMT_ENABLE_IF(use_format_as_member<T>::value)>
  value(const T& x) : value(formatter<T>::format_as(x)) {}

  template <typename T, FMT_ENABLE_IF(is_named_arg<T>::value)>
  value(const T& named_arg) : value(named_arg.value) {}

  template <typename T,
            FMT_ENABLE_IF(use_formatter<T>::value || !FMT_BUILTIN_TYPES)>
  FMT_CONSTEXPR20 FMT_INLINE value(T& x) : value(x, custom_tag()) {}

  FMT_ALWAYS_INLINE value(const named_arg_info<char_type>* args, size_t size)
      : named_args{args, size} {}

 private:
  template <typename T, FMT_ENABLE_IF(has_formatter<T, char_type>())>
  FMT_CONSTEXPR value(T& x, custom_tag) {
    using value_type = remove_const_t<T>;
    // T may overload operator& e.g. std::vector<bool>::reference in libc++.
    if (!is_constant_evaluated()) {
      custom.value =
          const_cast<char*>(&reinterpret_cast<const volatile char&>(x));
    } else {
      custom.value = nullptr;
#if defined(__cpp_if_constexpr)
      if constexpr (std::is_same<decltype(&x), remove_reference_t<T>*>::value)
        custom.value = const_cast<value_type*>(&x);
#endif
    }
    custom.format = format_custom<value_type, formatter<value_type, char_type>>;
  }

  template <typename T, FMT_ENABLE_IF(!has_formatter<T, char_type>())>
  FMT_CONSTEXPR value(const T&, custom_tag) {
    // Cannot format an argument; to make type T formattable provide a
    // formatter<T> specialization: https://fmt.dev/latest/api.html#udt.
    type_is_unformattable_for<T, char_type> _;
  }

  // Formats an argument of a custom type, such as a user-defined class.
  // DEPRECATED! Formatter template parameter will be removed.
  template <typename T, typename Formatter>
  static void format_custom(void* arg, parse_context<char_type>& parse_ctx,
                            Context& ctx) {
    auto f = Formatter();
    parse_ctx.advance_to(f.parse(parse_ctx));
    using qualified_type =
        conditional_t<has_formatter<const T, char_type>(), const T, T>;
    // format must be const for compatibility with std::format and compilation.
    const auto& cf = f;
    ctx.advance_to(cf.format(*static_cast<qualified_type*>(arg), ctx));
  }
};

enum { packed_arg_bits = 4 };
// Maximum number of arguments with packed types.
enum { max_packed_args = 62 / packed_arg_bits };
enum : unsigned long long { is_unpacked_bit = 1ULL << 63 };
enum : unsigned long long { has_named_args_bit = 1ULL << 62 };

template <typename It, typename T, typename Enable = void>
struct is_output_iterator : std::false_type {};

template <> struct is_output_iterator<appender, char> : std::true_type {};

template <typename It, typename T>
struct is_output_iterator<
    It, T,
    enable_if_t<std::is_assignable<decltype(*std::declval<decay_t<It>&>()++),
                                   T>::value>> : std::true_type {};

#ifndef FMT_USE_LOCALE
#  define FMT_USE_LOCALE (FMT_OPTIMIZE_SIZE <= 1)
#endif

// A type-erased reference to an std::locale to avoid a heavy <locale> include.
class locale_ref {
#if FMT_USE_LOCALE
 private:
  const void* locale_;  // A type-erased pointer to std::locale.

 public:
  constexpr locale_ref() : locale_(nullptr) {}
  template <typename Locale> locale_ref(const Locale& loc);

  inline explicit operator bool() const noexcept { return locale_ != nullptr; }
#endif  // FMT_USE_LOCALE

 public:
  template <typename Locale> auto get() const -> Locale;
};

template <typename> constexpr auto encode_types() -> unsigned long long {
  return 0;
}

template <typename Context, typename Arg, typename... Args>
constexpr auto encode_types() -> unsigned long long {
  return static_cast<unsigned>(stored_type_constant<Arg, Context>::value) |
         (encode_types<Context, Args...>() << packed_arg_bits);
}

template <typename Context, typename... T, size_t NUM_ARGS = sizeof...(T)>
constexpr auto make_descriptor() -> unsigned long long {
  return NUM_ARGS <= max_packed_args ? encode_types<Context, T...>()
                                     : is_unpacked_bit | NUM_ARGS;
}

template <typename Context, int NUM_ARGS>
using arg_t = conditional_t<NUM_ARGS <= max_packed_args, value<Context>,
                            basic_format_arg<Context>>;

template <typename Context, int NUM_ARGS, int NUM_NAMED_ARGS,
          unsigned long long DESC>
struct named_arg_store {
  // args_[0].named_args points to named_args to avoid bloating format_args.
  arg_t<Context, NUM_ARGS> args[1u + NUM_ARGS];
  named_arg_info<typename Context::char_type>
      named_args[static_cast<size_t>(NUM_NAMED_ARGS)];

  template <typename... T>
  FMT_CONSTEXPR FMT_ALWAYS_INLINE named_arg_store(T&... values)
      : args{{named_args, NUM_NAMED_ARGS}, values...} {
    int arg_index = 0, named_arg_index = 0;
    FMT_APPLY_VARIADIC(
        init_named_arg(named_args, arg_index, named_arg_index, values));
  }

  named_arg_store(named_arg_store&& rhs) {
    args[0] = {named_args, NUM_NAMED_ARGS};
    for (size_t i = 1; i < sizeof(args) / sizeof(*args); ++i)
      args[i] = rhs.args[i];
    for (size_t i = 0; i < NUM_NAMED_ARGS; ++i)
      named_args[i] = rhs.named_args[i];
  }

  named_arg_store(const named_arg_store& rhs) = delete;
  named_arg_store& operator=(const named_arg_store& rhs) = delete;
  named_arg_store& operator=(named_arg_store&& rhs) = delete;
  operator const arg_t<Context, NUM_ARGS>*() const { return args + 1; }
};

// An array of references to arguments. It can be implicitly converted to
// `basic_format_args` for passing into type-erased formatting functions
// such as `vformat`. It is a plain struct to reduce binary size in debug mode.
template <typename Context, int NUM_ARGS, int NUM_NAMED_ARGS,
          unsigned long long DESC>
struct format_arg_store {
  // +1 to workaround a bug in gcc 7.5 that causes duplicated-branches warning.
  using type =
      conditional_t<NUM_NAMED_ARGS == 0,
                    arg_t<Context, NUM_ARGS>[max_of<size_t>(1, NUM_ARGS)],
                    named_arg_store<Context, NUM_ARGS, NUM_NAMED_ARGS, DESC>>;
  type args;
};

// TYPE can be different from type_constant<T>, e.g. for __float128.
template <typename T, typename Char, type TYPE> struct native_formatter {
 private:
  dynamic_format_specs<Char> specs_;

 public:
  using nonlocking = void;

  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    if (ctx.begin() == ctx.end() || *ctx.begin() == '}') return ctx.begin();
    auto end = parse_format_specs(ctx.begin(), ctx.end(), specs_, ctx, TYPE);
    if (const_check(TYPE == type::char_type)) check_char_specs(specs_);
    return end;
  }

  template <type U = TYPE,
            FMT_ENABLE_IF(U == type::string_type || U == type::cstring_type ||
                          U == type::char_type)>
  FMT_CONSTEXPR void set_debug_format(bool set = true) {
    specs_.set_type(set ? presentation_type::debug : presentation_type::none);
  }

  FMT_PRAGMA_CLANG(diagnostic ignored "-Wundefined-inline")
  template <typename FormatContext>
  FMT_CONSTEXPR auto format(const T& val, FormatContext& ctx) const
      -> decltype(ctx.out());
};

template <typename T, typename Enable = void>
struct locking
    : bool_constant<mapped_type_constant<T>::value == type::custom_type> {};
template <typename T>
struct locking<T, void_t<typename formatter<remove_cvref_t<T>>::nonlocking>>
    : std::false_type {};

template <typename T = int> FMT_CONSTEXPR inline auto is_locking() -> bool {
  return locking<T>::value;
}
template <typename T1, typename T2, typename... Tail>
FMT_CONSTEXPR inline auto is_locking() -> bool {
  return locking<T1>::value || is_locking<T2, Tail...>();
}

FMT_API void vformat_to(buffer<char>& buf, string_view fmt, format_args args,
                        locale_ref loc = {});

#if FMT_WIN32
FMT_API void vprint_mojibake(FILE*, string_view, format_args, bool);
#else  // format_args is passed by reference since it is defined later.
inline void vprint_mojibake(FILE*, string_view, const format_args&, bool) {}
#endif
}  // namespace detail

// The main public API.

template <typename Char>
FMT_CONSTEXPR void parse_context<Char>::do_check_arg_id(int arg_id) {
  // Argument id is only checked at compile time during parsing because
  // formatting has its own validation.
  if (detail::is_constant_evaluated() && use_constexpr_cast) {
    auto ctx = static_cast<detail::compile_parse_context<Char>*>(this);
    if (arg_id >= ctx->num_args()) report_error("argument not found");
  }
}

template <typename Char>
FMT_CONSTEXPR void parse_context<Char>::check_dynamic_spec(int arg_id) {
  using detail::compile_parse_context;
  if (detail::is_constant_evaluated() && use_constexpr_cast)
    static_cast<compile_parse_context<Char>*>(this)->check_dynamic_spec(arg_id);
}

FMT_BEGIN_EXPORT

// An output iterator that appends to a buffer. It is used instead of
// back_insert_iterator to reduce symbol sizes and avoid <iterator> dependency.
template <typename T> class basic_appender {
 protected:
  detail::buffer<T>* container;

 public:
  using container_type = detail::buffer<T>;

  FMT_CONSTEXPR basic_appender(detail::buffer<T>& buf) : container(&buf) {}

  FMT_CONSTEXPR20 auto operator=(T c) -> basic_appender& {
    container->push_back(c);
    return *this;
  }
  FMT_CONSTEXPR20 auto operator*() -> basic_appender& { return *this; }
  FMT_CONSTEXPR20 auto operator++() -> basic_appender& { return *this; }
  FMT_CONSTEXPR20 auto operator++(int) -> basic_appender { return *this; }
};

// A formatting argument. Context is a template parameter for the compiled API
// where output can be unbuffered.
template <typename Context> class basic_format_arg {
 private:
  detail::value<Context> value_;
  detail::type type_;

  friend class basic_format_args<Context>;

  using char_type = typename Context::char_type;

 public:
  class handle {
   private:
    detail::custom_value<Context> custom_;

   public:
    explicit handle(detail::custom_value<Context> custom) : custom_(custom) {}

    void format(parse_context<char_type>& parse_ctx, Context& ctx) const {
      custom_.format(custom_.value, parse_ctx, ctx);
    }
  };

  constexpr basic_format_arg() : type_(detail::type::none_type) {}
  basic_format_arg(const detail::named_arg_info<char_type>* args, size_t size)
      : value_(args, size) {}
  template <typename T>
  basic_format_arg(T&& val)
      : value_(val), type_(detail::stored_type_constant<T, Context>::value) {}

  constexpr explicit operator bool() const noexcept {
    return type_ != detail::type::none_type;
  }
  auto type() const -> detail::type { return type_; }

  /**
   * Visits an argument dispatching to the appropriate visit method based on
   * the argument type. For example, if the argument type is `double` then
   * `vis(value)` will be called with the value of type `double`.
   */
  template <typename Visitor>
  FMT_CONSTEXPR FMT_INLINE auto visit(Visitor&& vis) const -> decltype(vis(0)) {
    using detail::map;
    switch (type_) {
    case detail::type::none_type:        break;
    case detail::type::int_type:         return vis(value_.int_value);
    case detail::type::uint_type:        return vis(value_.uint_value);
    case detail::type::long_long_type:   return vis(value_.long_long_value);
    case detail::type::ulong_long_type:  return vis(value_.ulong_long_value);
    case detail::type::int128_type:      return vis(map(value_.int128_value));
    case detail::type::uint128_type:     return vis(map(value_.uint128_value));
    case detail::type::bool_type:        return vis(value_.bool_value);
    case detail::type::char_type:        return vis(value_.char_value);
    case detail::type::float_type:       return vis(value_.float_value);
    case detail::type::double_type:      return vis(value_.double_value);
    case detail::type::long_double_type: return vis(value_.long_double_value);
    case detail::type::cstring_type:     return vis(value_.string.data);
    case detail::type::string_type:      return vis(value_.string.str());
    case detail::type::pointer_type:     return vis(value_.pointer);
    case detail::type::custom_type:      return vis(handle(value_.custom));
    }
    return vis(monostate());
  }

  auto format_custom(const char_type* parse_begin,
                     parse_context<char_type>& parse_ctx, Context& ctx)
      -> bool {
    if (type_ != detail::type::custom_type) return false;
    parse_ctx.advance_to(parse_begin);
    value_.custom.format(value_.custom.value, parse_ctx, ctx);
    return true;
  }
};

/**
 * A view of a collection of formatting arguments. To avoid lifetime issues it
 * should only be used as a parameter type in type-erased functions such as
 * `vformat`:
 *
 *     void vlog(fmt::string_view fmt, fmt::format_args args);  // OK
 *     fmt::format_args args = fmt::make_format_args();  // Dangling reference
 */
template <typename Context> class basic_format_args {
 private:
  // A descriptor that contains information about formatting arguments.
  // If the number of arguments is less or equal to max_packed_args then
  // argument types are passed in the descriptor. This reduces binary code size
  // per formatting function call.
  unsigned long long desc_;
  union {
    // If is_packed() returns true then argument values are stored in values_;
    // otherwise they are stored in args_. This is done to improve cache
    // locality and reduce compiled code size since storing larger objects
    // may require more code (at least on x86-64) even if the same amount of
    // data is actually copied to stack. It saves ~10% on the bloat test.
    const detail::value<Context>* values_;
    const basic_format_arg<Context>* args_;
  };

  constexpr auto is_packed() const -> bool {
    return (desc_ & detail::is_unpacked_bit) == 0;
  }
  constexpr auto has_named_args() const -> bool {
    return (desc_ & detail::has_named_args_bit) != 0;
  }

  FMT_CONSTEXPR auto type(int index) const -> detail::type {
    int shift = index * detail::packed_arg_bits;
    unsigned mask = (1 << detail::packed_arg_bits) - 1;
    return static_cast<detail::type>((desc_ >> shift) & mask);
  }

  template <int NUM_ARGS, int NUM_NAMED_ARGS, unsigned long long DESC>
  using store =
      detail::format_arg_store<Context, NUM_ARGS, NUM_NAMED_ARGS, DESC>;

 public:
  using format_arg = basic_format_arg<Context>;

  constexpr basic_format_args() : desc_(0), args_(nullptr) {}

  /// Constructs a `basic_format_args` object from `format_arg_store`.
  template <int NUM_ARGS, int NUM_NAMED_ARGS, unsigned long long DESC,
            FMT_ENABLE_IF(NUM_ARGS <= detail::max_packed_args)>
  constexpr FMT_ALWAYS_INLINE basic_format_args(
      const store<NUM_ARGS, NUM_NAMED_ARGS, DESC>& s)
      : desc_(DESC | (NUM_NAMED_ARGS != 0 ? +detail::has_named_args_bit : 0)),
        values_(s.args) {}

  template <int NUM_ARGS, int NUM_NAMED_ARGS, unsigned long long DESC,
            FMT_ENABLE_IF(NUM_ARGS > detail::max_packed_args)>
  constexpr basic_format_args(const store<NUM_ARGS, NUM_NAMED_ARGS, DESC>& s)
      : desc_(DESC | (NUM_NAMED_ARGS != 0 ? +detail::has_named_args_bit : 0)),
        args_(s.args) {}

  /// Constructs a `basic_format_args` object from a dynamic list of arguments.
  constexpr basic_format_args(const format_arg* args, int count,
                              bool has_named = false)
      : desc_(detail::is_unpacked_bit | detail::to_unsigned(count) |
              (has_named ? +detail::has_named_args_bit : 0)),
        args_(args) {}

  /// Returns the argument with the specified id.
  FMT_CONSTEXPR auto get(int id) const -> format_arg {
    auto arg = format_arg();
    if (!is_packed()) {
      if (id < max_size()) arg = args_[id];
      return arg;
    }
    if (static_cast<unsigned>(id) >= detail::max_packed_args) return arg;
    arg.type_ = type(id);
    if (arg.type_ != detail::type::none_type) arg.value_ = values_[id];
    return arg;
  }

  template <typename Char>
  auto get(basic_string_view<Char> name) const -> format_arg {
    int id = get_id(name);
    return id >= 0 ? get(id) : format_arg();
  }

  template <typename Char>
  FMT_CONSTEXPR auto get_id(basic_string_view<Char> name) const -> int {
    if (!has_named_args()) return -1;
    const auto& named_args =
        (is_packed() ? values_[-1] : args_[-1].value_).named_args;
    for (size_t i = 0; i < named_args.size; ++i) {
      if (named_args.data[i].name == name) return named_args.data[i].id;
    }
    return -1;
  }

  auto max_size() const -> int {
    unsigned long long max_packed = detail::max_packed_args;
    return static_cast<int>(is_packed() ? max_packed
                                        : desc_ & ~detail::is_unpacked_bit);
  }
};

// A formatting context.
class context {
 private:
  appender out_;
  format_args args_;
  FMT_NO_UNIQUE_ADDRESS detail::locale_ref loc_;

 public:
  using char_type = char;  ///< The character type for the output.
  using iterator = appender;
  using format_arg = basic_format_arg<context>;
  enum { builtin_types = FMT_BUILTIN_TYPES };

  /// Constructs a `context` object. References to the arguments are stored
  /// in the object so make sure they have appropriate lifetimes.
  FMT_CONSTEXPR context(iterator out, format_args args,
                        detail::locale_ref loc = {})
      : out_(out), args_(args), loc_(loc) {}
  context(context&&) = default;
  context(const context&) = delete;
  void operator=(const context&) = delete;

  FMT_CONSTEXPR auto arg(int id) const -> format_arg { return args_.get(id); }
  inline auto arg(string_view name) const -> format_arg {
    return args_.get(name);
  }
  FMT_CONSTEXPR auto arg_id(string_view name) const -> int {
    return args_.get_id(name);
  }
  auto args() const -> const format_args& { return args_; }

  // Returns an iterator to the beginning of the output range.
  FMT_CONSTEXPR auto out() const -> iterator { return out_; }

  // Advances the begin iterator to `it`.
  FMT_CONSTEXPR void advance_to(iterator) {}

  FMT_CONSTEXPR auto locale() const -> detail::locale_ref { return loc_; }
};

template <typename Char = char> struct runtime_format_string {
  basic_string_view<Char> str;
};

/**
 * Creates a runtime format string.
 *
 * **Example**:
 *
 *     // Check format string at runtime instead of compile-time.
 *     fmt::print(fmt::runtime("{:d}"), "I am not a number");
 */
inline auto runtime(string_view s) -> runtime_format_string<> { return {{s}}; }

/// A compile-time format string. Use `format_string` in the public API to
/// prevent type deduction.
template <typename... T> struct fstring {
 private:
  static constexpr int num_static_named_args =
      detail::count_static_named_args<T...>();

  using checker = detail::format_string_checker<
      char, static_cast<int>(sizeof...(T)), num_static_named_args,
      num_static_named_args != detail::count_named_args<T...>()>;

  using arg_pack = detail::arg_pack<T...>;

 public:
  string_view str;
  using t = fstring;

  // Reports a compile-time error if S is not a valid format string for T.
  template <size_t N>
  FMT_CONSTEVAL FMT_ALWAYS_INLINE fstring(const char (&s)[N]) : str(s, N - 1) {
    using namespace detail;
    static_assert(count<(is_view<remove_cvref_t<T>>::value &&
                         std::is_reference<T>::value)...>() == 0,
                  "passing views as lvalues is disallowed");
    if (FMT_USE_CONSTEVAL) parse_format_string<char>(s, checker(s, arg_pack()));
#ifdef FMT_ENFORCE_COMPILE_STRING
    static_assert(
        FMT_USE_CONSTEVAL && sizeof(s) != 0,
        "FMT_ENFORCE_COMPILE_STRING requires format strings to use FMT_STRING");
#endif
  }
  template <typename S,
            FMT_ENABLE_IF(std::is_convertible<const S&, string_view>::value)>
  FMT_CONSTEVAL FMT_ALWAYS_INLINE fstring(const S& s) : str(s) {
    auto sv = string_view(str);
    if (FMT_USE_CONSTEVAL)
      detail::parse_format_string<char>(sv, checker(sv, arg_pack()));
#ifdef FMT_ENFORCE_COMPILE_STRING
    static_assert(
        FMT_USE_CONSTEVAL && sizeof(s) != 0,
        "FMT_ENFORCE_COMPILE_STRING requires format strings to use FMT_STRING");
#endif
  }
  template <typename S,
            FMT_ENABLE_IF(std::is_base_of<detail::compile_string, S>::value&&
                              std::is_same<typename S::char_type, char>::value)>
  FMT_ALWAYS_INLINE fstring(const S&) : str(S()) {
    FMT_CONSTEXPR auto sv = string_view(S());
    FMT_CONSTEXPR int unused =
        (parse_format_string(sv, checker(sv, arg_pack())), 0);
    detail::ignore_unused(unused);
  }
  fstring(runtime_format_string<> fmt) : str(fmt.str) {}

  // Returning by reference generates better code in debug mode.
  FMT_ALWAYS_INLINE operator const string_view&() const { return str; }
  auto get() const -> string_view { return str; }
};

template <typename... T> using format_string = typename fstring<T...>::t;

template <typename T, typename Char = char>
using is_formattable = bool_constant<!std::is_same<
    detail::mapped_t<conditional_t<std::is_void<T>::value, int*, T>, Char>,
    void>::value>;
#ifdef __cpp_concepts
template <typename T, typename Char = char>
concept formattable = is_formattable<remove_reference_t<T>, Char>::value;
#endif

// A formatter specialization for natively supported types.
template <typename T, typename Char>
struct formatter<T, Char,
                 enable_if_t<detail::type_constant<T, Char>::value !=
                             detail::type::custom_type>>
    : detail::native_formatter<T, Char, detail::type_constant<T, Char>::value> {
};

/**
 * Constructs an object that stores references to arguments and can be
 * implicitly converted to `format_args`. `Context` can be omitted in which case
 * it defaults to `context`. See `arg` for lifetime considerations.
 */
// Take arguments by lvalue references to avoid some lifetime issues, e.g.
//   auto args = make_format_args(std::string());
template <typename Context = context, typename... T,
          int NUM_ARGS = sizeof...(T),
          int NUM_NAMED_ARGS = detail::count_named_args<T...>(),
          unsigned long long DESC = detail::make_descriptor<Context, T...>()>
constexpr FMT_ALWAYS_INLINE auto make_format_args(T&... args)
    -> detail::format_arg_store<Context, NUM_ARGS, NUM_NAMED_ARGS, DESC> {
  // Suppress warnings for pathological types convertible to detail::value.
  FMT_PRAGMA_GCC(diagnostic ignored "-Wconversion")
  return {{args...}};
}

template <typename... T>
using vargs =
    detail::format_arg_store<context, sizeof...(T),
                             detail::count_named_args<T...>(),
                             detail::make_descriptor<context, T...>()>;

/**
 * Returns a named argument to be used in a formatting function.
 * It should only be used in a call to a formatting function.
 *
 * **Example**:
 *
 *     fmt::print("The answer is {answer}.", fmt::arg("answer", 42));
 */
template <typename Char, typename T>
inline auto arg(const Char* name, const T& arg) -> detail::named_arg<Char, T> {
  return {name, arg};
}

/// Formats a string and writes the output to `out`.
template <typename OutputIt,
          FMT_ENABLE_IF(detail::is_output_iterator<remove_cvref_t<OutputIt>,
                                                   char>::value)>
auto vformat_to(OutputIt&& out, string_view fmt, format_args args)
    -> remove_cvref_t<OutputIt> {
  auto&& buf = detail::get_buffer<char>(out);
  detail::vformat_to(buf, fmt, args, {});
  return detail::get_iterator(buf, out);
}

/**
 * Formats `args` according to specifications in `fmt`, writes the result to
 * the output iterator `out` and returns the iterator past the end of the output
 * range. `format_to` does not append a terminating null character.
 *
 * **Example**:
 *
 *     auto out = std::vector<char>();
 *     fmt::format_to(std::back_inserter(out), "{}", 42);
 */
template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<remove_cvref_t<OutputIt>,
                                                   char>::value)>
FMT_INLINE auto format_to(OutputIt&& out, format_string<T...> fmt, T&&... args)
    -> remove_cvref_t<OutputIt> {
  return vformat_to(out, fmt.str, vargs<T...>{{args...}});
}

template <typename OutputIt> struct format_to_n_result {
  /// Iterator past the end of the output range.
  OutputIt out;
  /// Total (not truncated) output size.
  size_t size;
};

template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
auto vformat_to_n(OutputIt out, size_t n, string_view fmt, format_args args)
    -> format_to_n_result<OutputIt> {
  using traits = detail::fixed_buffer_traits;
  auto buf = detail::iterator_buffer<OutputIt, char, traits>(out, n);
  detail::vformat_to(buf, fmt, args, {});
  return {buf.out(), buf.count()};
}

/**
 * Formats `args` according to specifications in `fmt`, writes up to `n`
 * characters of the result to the output iterator `out` and returns the total
 * (not truncated) output size and the iterator past the end of the output
 * range. `format_to_n` does not append a terminating null character.
 */
template <typename OutputIt, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
FMT_INLINE auto format_to_n(OutputIt out, size_t n, format_string<T...> fmt,
                            T&&... args) -> format_to_n_result<OutputIt> {
  return vformat_to_n(out, n, fmt.str, vargs<T...>{{args...}});
}

struct format_to_result {
  /// Pointer to just after the last successful write in the array.
  char* out;
  /// Specifies if the output was truncated.
  bool truncated;

  FMT_CONSTEXPR operator char*() const {
    // Report truncation to prevent silent data loss.
    if (truncated) report_error("output is truncated");
    return out;
  }
};

template <size_t N>
auto vformat_to(char (&out)[N], string_view fmt, format_args args)
    -> format_to_result {
  auto result = vformat_to_n(out, N, fmt, args);
  return {result.out, result.size > N};
}

template <size_t N, typename... T>
FMT_INLINE auto format_to(char (&out)[N], format_string<T...> fmt, T&&... args)
    -> format_to_result {
  auto result = vformat_to_n(out, N, fmt.str, vargs<T...>{{args...}});
  return {result.out, result.size > N};
}

/// Returns the number of chars in the output of `format(fmt, args...)`.
template <typename... T>
FMT_NODISCARD FMT_INLINE auto formatted_size(format_string<T...> fmt,
                                             T&&... args) -> size_t {
  auto buf = detail::counting_buffer<>();
  detail::vformat_to(buf, fmt.str, vargs<T...>{{args...}}, {});
  return buf.count();
}

FMT_API void vprint(string_view fmt, format_args args);
FMT_API void vprint(FILE* f, string_view fmt, format_args args);
FMT_API void vprintln(FILE* f, string_view fmt, format_args args);
FMT_API void vprint_buffered(FILE* f, string_view fmt, format_args args);

/**
 * Formats `args` according to specifications in `fmt` and writes the output
 * to `stdout`.
 *
 * **Example**:
 *
 *     fmt::print("The answer is {}.", 42);
 */
template <typename... T>
FMT_INLINE void print(format_string<T...> fmt, T&&... args) {
  vargs<T...> va = {{args...}};
  if (detail::const_check(!detail::use_utf8))
    return detail::vprint_mojibake(stdout, fmt.str, va, false);
  return detail::is_locking<T...>() ? vprint_buffered(stdout, fmt.str, va)
                                    : vprint(fmt.str, va);
}

/**
 * Formats `args` according to specifications in `fmt` and writes the
 * output to the file `f`.
 *
 * **Example**:
 *
 *     fmt::print(stderr, "Don't {}!", "panic");
 */
template <typename... T>
FMT_INLINE void print(FILE* f, format_string<T...> fmt, T&&... args) {
  vargs<T...> va = {{args...}};
  if (detail::const_check(!detail::use_utf8))
    return detail::vprint_mojibake(f, fmt.str, va, false);
  return detail::is_locking<T...>() ? vprint_buffered(f, fmt.str, va)
                                    : vprint(f, fmt.str, va);
}

/// Formats `args` according to specifications in `fmt` and writes the output
/// to the file `f` followed by a newline.
template <typename... T>
FMT_INLINE void println(FILE* f, format_string<T...> fmt, T&&... args) {
  vargs<T...> va = {{args...}};
  return detail::const_check(detail::use_utf8)
             ? vprintln(f, fmt.str, va)
             : detail::vprint_mojibake(f, fmt.str, va, true);
}

/// Formats `args` according to specifications in `fmt` and writes the output
/// to `stdout` followed by a newline.
template <typename... T>
FMT_INLINE void println(format_string<T...> fmt, T&&... args) {
  return fmt::println(stdout, fmt, static_cast<T&&>(args)...);
}

FMT_PRAGMA_CLANG(diagnostic pop)
FMT_PRAGMA_GCC(pop_options)
FMT_END_EXPORT
FMT_END_NAMESPACE

#endif  // FMT_BASE_H_

#ifndef FMT_MODULE
#  include <cmath>    // std::signbit
#  include <cstddef>  // std::byte
#  include <cstdint>  // uint32_t
#  include <cstdlib>  // std::malloc, std::free
#  include <cstring>  // std::memcpy
#  include <limits>   // std::numeric_limits
#  include <new>      // std::bad_alloc
#  if defined(__GLIBCXX__) && !defined(_GLIBCXX_USE_DUAL_ABI)
// Workaround for pre gcc 5 libstdc++.
#    include <memory>  // std::allocator_traits
#  endif
#  include <stdexcept>     // std::runtime_error
#  include <string>        // std::string
#  include <system_error>  // std::system_error

// Check FMT_CPLUSPLUS to avoid a warning in MSVC.
#  if FMT_HAS_INCLUDE(<bit>) && FMT_CPLUSPLUS > 201703L
#    include <bit>  // std::bit_cast
#  endif

// libc++ supports string_view in pre-c++17.
#  if FMT_HAS_INCLUDE(<string_view>) && \
      (FMT_CPLUSPLUS >= 201703L || defined(_LIBCPP_VERSION))
#    include <string_view>
#    define FMT_USE_STRING_VIEW
#  endif

#  if FMT_MSC_VERSION
#    include <intrin.h>  // _BitScanReverse[64], _umul128
#  endif
#endif  // FMT_MODULE

#if defined(FMT_USE_NONTYPE_TEMPLATE_ARGS)
// Use the provided definition.
#elif defined(__NVCOMPILER)
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 0
#elif FMT_GCC_VERSION >= 903 && FMT_CPLUSPLUS >= 201709L
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 1
#elif defined(__cpp_nontype_template_args) && \
    __cpp_nontype_template_args >= 201911L
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 1
#elif FMT_CLANG_VERSION >= 1200 && FMT_CPLUSPLUS >= 202002L
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 1
#else
#  define FMT_USE_NONTYPE_TEMPLATE_ARGS 0
#endif

#if defined __cpp_inline_variables && __cpp_inline_variables >= 201606L
#  define FMT_INLINE_VARIABLE inline
#else
#  define FMT_INLINE_VARIABLE
#endif

// Check if RTTI is disabled.
#ifdef FMT_USE_RTTI
// Use the provided definition.
#elif defined(__GXX_RTTI) || FMT_HAS_FEATURE(cxx_rtti) || defined(_CPPRTTI) || \
    defined(__INTEL_RTTI__) || defined(__RTTI)
// __RTTI is for EDG compilers. _CPPRTTI is for MSVC.
#  define FMT_USE_RTTI 1
#else
#  define FMT_USE_RTTI 0
#endif

// Visibility when compiled as a shared library/object.
#if defined(FMT_LIB_EXPORT) || defined(FMT_SHARED)
#  define FMT_SO_VISIBILITY(value) FMT_VISIBILITY(value)
#else
#  define FMT_SO_VISIBILITY(value)
#endif

#if FMT_GCC_VERSION || FMT_CLANG_VERSION
#  define FMT_NOINLINE __attribute__((noinline))
#else
#  define FMT_NOINLINE
#endif

#ifdef FMT_DEPRECATED
// Use the provided definition.
#elif FMT_HAS_CPP14_ATTRIBUTE(deprecated)
#  define FMT_DEPRECATED [[deprecated]]
#else
#  define FMT_DEPRECATED /* deprecated */
#endif

// Detect constexpr std::string.
#if !FMT_USE_CONSTEVAL
#  define FMT_USE_CONSTEXPR_STRING 0
#elif defined(__cpp_lib_constexpr_string) && \
    __cpp_lib_constexpr_string >= 201907L
#  if FMT_CLANG_VERSION && FMT_GLIBCXX_RELEASE
// clang + libstdc++ are able to work only starting with gcc13.3
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=113294
#    if FMT_GLIBCXX_RELEASE < 13
#      define FMT_USE_CONSTEXPR_STRING 0
#    elif FMT_GLIBCXX_RELEASE == 13 && __GLIBCXX__ < 20240521
#      define FMT_USE_CONSTEXPR_STRING 0
#    else
#      define FMT_USE_CONSTEXPR_STRING 1
#    endif
#  else
#    define FMT_USE_CONSTEXPR_STRING 1
#  endif
#else
#  define FMT_USE_CONSTEXPR_STRING 0
#endif
#if FMT_USE_CONSTEXPR_STRING
#  define FMT_CONSTEXPR_STRING constexpr
#else
#  define FMT_CONSTEXPR_STRING
#endif

// GCC 4.9 doesn't support qualified names in specializations.
namespace std {
template <typename T> struct iterator_traits<fmt::basic_appender<T>> {
  using iterator_category = output_iterator_tag;
  using value_type = T;
  using difference_type =
      decltype(static_cast<int*>(nullptr) - static_cast<int*>(nullptr));
  using pointer = void;
  using reference = void;
};
}  // namespace std

#ifndef FMT_THROW
#  if FMT_USE_EXCEPTIONS
#    if FMT_MSC_VERSION || defined(__NVCC__)
FMT_BEGIN_NAMESPACE
namespace detail {
template <typename Exception> inline void do_throw(const Exception& x) {
  // Silence unreachable code warnings in MSVC and NVCC because these
  // are nearly impossible to fix in a generic code.
  volatile bool b = true;
  if (b) throw x;
}
}  // namespace detail
FMT_END_NAMESPACE
#      define FMT_THROW(x) detail::do_throw(x)
#    else
#      define FMT_THROW(x) throw x
#    endif
#  else
#    define FMT_THROW(x) \
      ::fmt::detail::assert_fail(__FILE__, __LINE__, (x).what())
#  endif  // FMT_USE_EXCEPTIONS
#endif    // FMT_THROW

// Defining FMT_REDUCE_INT_INSTANTIATIONS to 1, will reduce the number of
// integer formatter template instantiations to just one by only using the
// largest integer type. This results in a reduction in binary size but will
// cause a decrease in integer formatting performance.
#if !defined(FMT_REDUCE_INT_INSTANTIATIONS)
#  define FMT_REDUCE_INT_INSTANTIATIONS 0
#endif

FMT_BEGIN_NAMESPACE

template <typename Char, typename Traits, typename Allocator>
struct is_contiguous<std::basic_string<Char, Traits, Allocator>>
    : std::true_type {};

namespace detail {

// __builtin_clz is broken in clang with Microsoft codegen:
// https://github.com/fmtlib/fmt/issues/519.
#if !FMT_MSC_VERSION
#  if FMT_HAS_BUILTIN(__builtin_clz) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CLZ(n) __builtin_clz(n)
#  endif
#  if FMT_HAS_BUILTIN(__builtin_clzll) || FMT_GCC_VERSION || FMT_ICC_VERSION
#    define FMT_BUILTIN_CLZLL(n) __builtin_clzll(n)
#  endif
#endif

// Some compilers masquerade as both MSVC and GCC but otherwise support
// __builtin_clz and __builtin_clzll, so only define FMT_BUILTIN_CLZ using the
// MSVC intrinsics if the clz and clzll builtins are not available.
#if FMT_MSC_VERSION && !defined(FMT_BUILTIN_CLZLL)
// Avoid Clang with Microsoft CodeGen's -Wunknown-pragmas warning.
#  ifndef __clang__
#    pragma intrinsic(_BitScanReverse)
#    ifdef _WIN64
#      pragma intrinsic(_BitScanReverse64)
#    endif
#  endif

inline auto clz(uint32_t x) -> int {
  FMT_ASSERT(x != 0, "");
  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
  unsigned long r = 0;
  _BitScanReverse(&r, x);
  return 31 ^ static_cast<int>(r);
}
#  define FMT_BUILTIN_CLZ(n) detail::clz(n)

inline auto clzll(uint64_t x) -> int {
  FMT_ASSERT(x != 0, "");
  FMT_MSC_WARNING(suppress : 6102)  // Suppress a bogus static analysis warning.
  unsigned long r = 0;
#  ifdef _WIN64
  _BitScanReverse64(&r, x);
#  else
  // Scan the high 32 bits.
  if (_BitScanReverse(&r, static_cast<uint32_t>(x >> 32)))
    return 63 ^ static_cast<int>(r + 32);
  // Scan the low 32 bits.
  _BitScanReverse(&r, static_cast<uint32_t>(x));
#  endif
  return 63 ^ static_cast<int>(r);
}
#  define FMT_BUILTIN_CLZLL(n) detail::clzll(n)
#endif  // FMT_MSC_VERSION && !defined(FMT_BUILTIN_CLZLL)

FMT_CONSTEXPR inline void abort_fuzzing_if(bool condition) {
  ignore_unused(condition);
#ifdef FMT_FUZZ
  if (condition) throw std::runtime_error("fuzzing limit reached");
#endif
}

#if defined(FMT_USE_STRING_VIEW)
template <typename Char> using std_string_view = std::basic_string_view<Char>;
#else
template <typename Char> struct std_string_view {
  operator basic_string_view<Char>() const;
};
#endif

template <typename Char, Char... C> struct string_literal {
  static constexpr Char value[sizeof...(C)] = {C...};
  constexpr operator basic_string_view<Char>() const {
    return {value, sizeof...(C)};
  }
};
#if FMT_CPLUSPLUS < 201703L
template <typename Char, Char... C>
constexpr Char string_literal<Char, C...>::value[sizeof...(C)];
#endif

// Implementation of std::bit_cast for pre-C++20.
template <typename To, typename From, FMT_ENABLE_IF(sizeof(To) == sizeof(From))>
FMT_CONSTEXPR20 auto bit_cast(const From& from) -> To {
#ifdef __cpp_lib_bit_cast
  if (is_constant_evaluated()) return std::bit_cast<To>(from);
#endif
  auto to = To();
  // The cast suppresses a bogus -Wclass-memaccess on GCC.
  std::memcpy(static_cast<void*>(&to), &from, sizeof(to));
  return to;
}

inline auto is_big_endian() -> bool {
#ifdef _WIN32
  return false;
#elif defined(__BIG_ENDIAN__)
  return true;
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__)
  return __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__;
#else
  struct bytes {
    char data[sizeof(int)];
  };
  return bit_cast<bytes>(1).data[0] == 0;
#endif
}

class uint128_fallback {
 private:
  uint64_t lo_, hi_;

 public:
  constexpr uint128_fallback(uint64_t hi, uint64_t lo) : lo_(lo), hi_(hi) {}
  constexpr uint128_fallback(uint64_t value = 0) : lo_(value), hi_(0) {}

  constexpr auto high() const noexcept -> uint64_t { return hi_; }
  constexpr auto low() const noexcept -> uint64_t { return lo_; }

  template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
  constexpr explicit operator T() const {
    return static_cast<T>(lo_);
  }

  friend constexpr auto operator==(const uint128_fallback& lhs,
                                   const uint128_fallback& rhs) -> bool {
    return lhs.hi_ == rhs.hi_ && lhs.lo_ == rhs.lo_;
  }
  friend constexpr auto operator!=(const uint128_fallback& lhs,
                                   const uint128_fallback& rhs) -> bool {
    return !(lhs == rhs);
  }
  friend constexpr auto operator>(const uint128_fallback& lhs,
                                  const uint128_fallback& rhs) -> bool {
    return lhs.hi_ != rhs.hi_ ? lhs.hi_ > rhs.hi_ : lhs.lo_ > rhs.lo_;
  }
  friend constexpr auto operator|(const uint128_fallback& lhs,
                                  const uint128_fallback& rhs)
      -> uint128_fallback {
    return {lhs.hi_ | rhs.hi_, lhs.lo_ | rhs.lo_};
  }
  friend constexpr auto operator&(const uint128_fallback& lhs,
                                  const uint128_fallback& rhs)
      -> uint128_fallback {
    return {lhs.hi_ & rhs.hi_, lhs.lo_ & rhs.lo_};
  }
  friend constexpr auto operator~(const uint128_fallback& n)
      -> uint128_fallback {
    return {~n.hi_, ~n.lo_};
  }
  friend FMT_CONSTEXPR auto operator+(const uint128_fallback& lhs,
                                      const uint128_fallback& rhs)
      -> uint128_fallback {
    auto result = uint128_fallback(lhs);
    result += rhs;
    return result;
  }
  friend FMT_CONSTEXPR auto operator*(const uint128_fallback& lhs, uint32_t rhs)
      -> uint128_fallback {
    FMT_ASSERT(lhs.hi_ == 0, "");
    uint64_t hi = (lhs.lo_ >> 32) * rhs;
    uint64_t lo = (lhs.lo_ & ~uint32_t()) * rhs;
    uint64_t new_lo = (hi << 32) + lo;
    return {(hi >> 32) + (new_lo < lo ? 1 : 0), new_lo};
  }
  friend constexpr auto operator-(const uint128_fallback& lhs, uint64_t rhs)
      -> uint128_fallback {
    return {lhs.hi_ - (lhs.lo_ < rhs ? 1 : 0), lhs.lo_ - rhs};
  }
  FMT_CONSTEXPR auto operator>>(int shift) const -> uint128_fallback {
    if (shift == 64) return {0, hi_};
    if (shift > 64) return uint128_fallback(0, hi_) >> (shift - 64);
    return {hi_ >> shift, (hi_ << (64 - shift)) | (lo_ >> shift)};
  }
  FMT_CONSTEXPR auto operator<<(int shift) const -> uint128_fallback {
    if (shift == 64) return {lo_, 0};
    if (shift > 64) return uint128_fallback(lo_, 0) << (shift - 64);
    return {hi_ << shift | (lo_ >> (64 - shift)), (lo_ << shift)};
  }
  FMT_CONSTEXPR auto operator>>=(int shift) -> uint128_fallback& {
    return *this = *this >> shift;
  }
  FMT_CONSTEXPR void operator+=(uint128_fallback n) {
    uint64_t new_lo = lo_ + n.lo_;
    uint64_t new_hi = hi_ + n.hi_ + (new_lo < lo_ ? 1 : 0);
    FMT_ASSERT(new_hi >= hi_, "");
    lo_ = new_lo;
    hi_ = new_hi;
  }
  FMT_CONSTEXPR void operator&=(uint128_fallback n) {
    lo_ &= n.lo_;
    hi_ &= n.hi_;
  }

  FMT_CONSTEXPR20 auto operator+=(uint64_t n) noexcept -> uint128_fallback& {
    if (is_constant_evaluated()) {
      lo_ += n;
      hi_ += (lo_ < n ? 1 : 0);
      return *this;
    }
#if FMT_HAS_BUILTIN(__builtin_addcll) && !defined(__ibmxl__)
    unsigned long long carry;
    lo_ = __builtin_addcll(lo_, n, 0, &carry);
    hi_ += carry;
#elif FMT_HAS_BUILTIN(__builtin_ia32_addcarryx_u64) && !defined(__ibmxl__)
    unsigned long long result;
    auto carry = __builtin_ia32_addcarryx_u64(0, lo_, n, &result);
    lo_ = result;
    hi_ += carry;
#elif defined(_MSC_VER) && defined(_M_X64)
    auto carry = _addcarry_u64(0, lo_, n, &lo_);
    _addcarry_u64(carry, hi_, 0, &hi_);
#else
    lo_ += n;
    hi_ += (lo_ < n ? 1 : 0);
#endif
    return *this;
  }
};

using uint128_t = conditional_t<FMT_USE_INT128, uint128_opt, uint128_fallback>;

#ifdef UINTPTR_MAX
using uintptr_t = ::uintptr_t;
#else
using uintptr_t = uint128_t;
#endif

// Returns the largest possible value for type T. Same as
// std::numeric_limits<T>::max() but shorter and not affected by the max macro.
template <typename T> constexpr auto max_value() -> T {
  return (std::numeric_limits<T>::max)();
}
template <typename T> constexpr auto num_bits() -> int {
  return std::numeric_limits<T>::digits;
}
// std::numeric_limits<T>::digits may return 0 for 128-bit ints.
template <> constexpr auto num_bits<int128_opt>() -> int { return 128; }
template <> constexpr auto num_bits<uint128_opt>() -> int { return 128; }
template <> constexpr auto num_bits<uint128_fallback>() -> int { return 128; }

// A heterogeneous bit_cast used for converting 96-bit long double to uint128_t
// and 128-bit pointers to uint128_fallback.
template <typename To, typename From, FMT_ENABLE_IF(sizeof(To) > sizeof(From))>
inline auto bit_cast(const From& from) -> To {
  constexpr auto size = static_cast<int>(sizeof(From) / sizeof(unsigned short));
  struct data_t {
    unsigned short value[static_cast<unsigned>(size)];
  } data = bit_cast<data_t>(from);
  auto result = To();
  if (const_check(is_big_endian())) {
    for (int i = 0; i < size; ++i)
      result = (result << num_bits<unsigned short>()) | data.value[i];
  } else {
    for (int i = size - 1; i >= 0; --i)
      result = (result << num_bits<unsigned short>()) | data.value[i];
  }
  return result;
}

template <typename UInt>
FMT_CONSTEXPR20 inline auto countl_zero_fallback(UInt n) -> int {
  int lz = 0;
  constexpr UInt msb_mask = static_cast<UInt>(1) << (num_bits<UInt>() - 1);
  for (; (n & msb_mask) == 0; n <<= 1) lz++;
  return lz;
}

FMT_CONSTEXPR20 inline auto countl_zero(uint32_t n) -> int {
#ifdef FMT_BUILTIN_CLZ
  if (!is_constant_evaluated()) return FMT_BUILTIN_CLZ(n);
#endif
  return countl_zero_fallback(n);
}

FMT_CONSTEXPR20 inline auto countl_zero(uint64_t n) -> int {
#ifdef FMT_BUILTIN_CLZLL
  if (!is_constant_evaluated()) return FMT_BUILTIN_CLZLL(n);
#endif
  return countl_zero_fallback(n);
}

FMT_INLINE void assume(bool condition) {
  (void)condition;
#if FMT_HAS_BUILTIN(__builtin_assume) && !FMT_ICC_VERSION
  __builtin_assume(condition);
#elif FMT_GCC_VERSION
  if (!condition) __builtin_unreachable();
#endif
}

// Attempts to reserve space for n extra characters in the output range.
// Returns a pointer to the reserved range or a reference to it.
template <typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value&&
                            is_contiguous<typename OutputIt::container>::value)>
#if FMT_CLANG_VERSION >= 307 && !FMT_ICC_VERSION
__attribute__((no_sanitize("undefined")))
#endif
FMT_CONSTEXPR20 inline auto
reserve(OutputIt it, size_t n) -> typename OutputIt::value_type* {
  auto& c = get_container(it);
  size_t size = c.size();
  c.resize(size + n);
  return &c[size];
}

template <typename T>
FMT_CONSTEXPR20 inline auto reserve(basic_appender<T> it, size_t n)
    -> basic_appender<T> {
  buffer<T>& buf = get_container(it);
  buf.try_reserve(buf.size() + n);
  return it;
}

template <typename Iterator>
constexpr auto reserve(Iterator& it, size_t) -> Iterator& {
  return it;
}

template <typename OutputIt>
using reserve_iterator =
    remove_reference_t<decltype(reserve(std::declval<OutputIt&>(), 0))>;

template <typename T, typename OutputIt>
constexpr auto to_pointer(OutputIt, size_t) -> T* {
  return nullptr;
}
template <typename T> FMT_CONSTEXPR auto to_pointer(T*& ptr, size_t n) -> T* {
  T* begin = ptr;
  ptr += n;
  return begin;
}
template <typename T>
FMT_CONSTEXPR20 auto to_pointer(basic_appender<T> it, size_t n) -> T* {
  buffer<T>& buf = get_container(it);
  buf.try_reserve(buf.size() + n);
  auto size = buf.size();
  if (buf.capacity() < size + n) return nullptr;
  buf.try_resize(size + n);
  return buf.data() + size;
}

template <typename OutputIt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value&&
                            is_contiguous<typename OutputIt::container>::value)>
inline auto base_iterator(OutputIt it,
                          typename OutputIt::container_type::value_type*)
    -> OutputIt {
  return it;
}

template <typename Iterator>
constexpr auto base_iterator(Iterator, Iterator it) -> Iterator {
  return it;
}

// <algorithm> is spectacularly slow to compile in C++20 so use a simple fill_n
// instead (#1998).
template <typename OutputIt, typename Size, typename T>
FMT_CONSTEXPR auto fill_n(OutputIt out, Size count, const T& value)
    -> OutputIt {
  for (Size i = 0; i < count; ++i) *out++ = value;
  return out;
}
template <typename T, typename Size>
FMT_CONSTEXPR20 auto fill_n(T* out, Size count, char value) -> T* {
  if (is_constant_evaluated()) return fill_n<T*, Size, T>(out, count, value);
  static_assert(sizeof(T) == 1,
                "sizeof(T) must be 1 to use char for initialization");
  std::memset(out, value, to_unsigned(count));
  return out + count;
}

template <typename OutChar, typename InputIt, typename OutputIt>
FMT_CONSTEXPR FMT_NOINLINE auto copy_noinline(InputIt begin, InputIt end,
                                              OutputIt out) -> OutputIt {
  return copy<OutChar>(begin, end, out);
}

// A public domain branchless UTF-8 decoder by Christopher Wellons:
// https://github.com/skeeto/branchless-utf8
/* Decode the next character, c, from s, reporting errors in e.
 *
 * Since this is a branchless decoder, four bytes will be read from the
 * buffer regardless of the actual length of the next character. This
 * means the buffer _must_ have at least three bytes of zero padding
 * following the end of the data stream.
 *
 * Errors are reported in e, which will be non-zero if the parsed
 * character was somehow invalid: invalid byte sequence, non-canonical
 * encoding, or a surrogate half.
 *
 * The function returns a pointer to the next character. When an error
 * occurs, this pointer will be a guess that depends on the particular
 * error, but it will always advance at least one byte.
 */
FMT_CONSTEXPR inline auto utf8_decode(const char* s, uint32_t* c, int* e)
    -> const char* {
  constexpr int masks[] = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
  constexpr uint32_t mins[] = {4194304, 0, 128, 2048, 65536};
  constexpr int shiftc[] = {0, 18, 12, 6, 0};
  constexpr int shifte[] = {0, 6, 4, 2, 0};

  int len = "\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\0\0\0\0\0\0\0\0\2\2\2\2\3\3\4"
      [static_cast<unsigned char>(*s) >> 3];
  // Compute the pointer to the next character early so that the next
  // iteration can start working on the next character. Neither Clang
  // nor GCC figure out this reordering on their own.
  const char* next = s + len + !len;

  using uchar = unsigned char;

  // Assume a four-byte character and load four bytes. Unused bits are
  // shifted out.
  *c = uint32_t(uchar(s[0]) & masks[len]) << 18;
  *c |= uint32_t(uchar(s[1]) & 0x3f) << 12;
  *c |= uint32_t(uchar(s[2]) & 0x3f) << 6;
  *c |= uint32_t(uchar(s[3]) & 0x3f) << 0;
  *c >>= shiftc[len];

  // Accumulate the various error conditions.
  *e = (*c < mins[len]) << 6;       // non-canonical encoding
  *e |= ((*c >> 11) == 0x1b) << 7;  // surrogate half?
  *e |= (*c > 0x10FFFF) << 8;       // out of range?
  *e |= (uchar(s[1]) & 0xc0) >> 2;
  *e |= (uchar(s[2]) & 0xc0) >> 4;
  *e |= uchar(s[3]) >> 6;
  *e ^= 0x2a;  // top two bits of each tail byte correct?
  *e >>= shifte[len];

  return next;
}

constexpr FMT_INLINE_VARIABLE uint32_t invalid_code_point = ~uint32_t();

// Invokes f(cp, sv) for every code point cp in s with sv being the string view
// corresponding to the code point. cp is invalid_code_point on error.
template <typename F>
FMT_CONSTEXPR void for_each_codepoint(string_view s, F f) {
  auto decode = [f](const char* buf_ptr, const char* ptr) {
    auto cp = uint32_t();
    auto error = 0;
    auto end = utf8_decode(buf_ptr, &cp, &error);
    bool result = f(error ? invalid_code_point : cp,
                    string_view(ptr, error ? 1 : to_unsigned(end - buf_ptr)));
    return result ? (error ? buf_ptr + 1 : end) : nullptr;
  };

  auto p = s.data();
  const size_t block_size = 4;  // utf8_decode always reads blocks of 4 chars.
  if (s.size() >= block_size) {
    for (auto end = p + s.size() - block_size + 1; p < end;) {
      p = decode(p, p);
      if (!p) return;
    }
  }
  auto num_chars_left = to_unsigned(s.data() + s.size() - p);
  if (num_chars_left == 0) return;

  // Suppress bogus -Wstringop-overflow.
  if (FMT_GCC_VERSION) num_chars_left &= 3;
  char buf[2 * block_size - 1] = {};
  copy<char>(p, p + num_chars_left, buf);
  const char* buf_ptr = buf;
  do {
    auto end = decode(buf_ptr, p);
    if (!end) return;
    p += end - buf_ptr;
    buf_ptr = end;
  } while (buf_ptr < buf + num_chars_left);
}

FMT_CONSTEXPR inline auto display_width_of(uint32_t cp) noexcept -> size_t {
  return to_unsigned(
      1 + (cp >= 0x1100 &&
           (cp <= 0x115f ||  // Hangul Jamo init. consonants
            cp == 0x2329 ||  // LEFT-POINTING ANGLE BRACKET
            cp == 0x232a ||  // RIGHT-POINTING ANGLE BRACKET
            // CJK ... Yi except IDEOGRAPHIC HALF FILL SPACE:
            (cp >= 0x2e80 && cp <= 0xa4cf && cp != 0x303f) ||
            (cp >= 0xac00 && cp <= 0xd7a3) ||    // Hangul Syllables
            (cp >= 0xf900 && cp <= 0xfaff) ||    // CJK Compatibility Ideographs
            (cp >= 0xfe10 && cp <= 0xfe19) ||    // Vertical Forms
            (cp >= 0xfe30 && cp <= 0xfe6f) ||    // CJK Compatibility Forms
            (cp >= 0xff00 && cp <= 0xff60) ||    // Fullwidth Forms
            (cp >= 0xffe0 && cp <= 0xffe6) ||    // Fullwidth Forms
            (cp >= 0x20000 && cp <= 0x2fffd) ||  // CJK
            (cp >= 0x30000 && cp <= 0x3fffd) ||
            // Miscellaneous Symbols and Pictographs + Emoticons:
            (cp >= 0x1f300 && cp <= 0x1f64f) ||
            // Supplemental Symbols and Pictographs:
            (cp >= 0x1f900 && cp <= 0x1f9ff))));
}

template <typename T> struct is_integral : std::is_integral<T> {};
template <> struct is_integral<int128_opt> : std::true_type {};
template <> struct is_integral<uint128_t> : std::true_type {};

template <typename T>
using is_signed =
    std::integral_constant<bool, std::numeric_limits<T>::is_signed ||
                                     std::is_same<T, int128_opt>::value>;

template <typename T>
using is_integer =
    bool_constant<is_integral<T>::value && !std::is_same<T, bool>::value &&
                  !std::is_same<T, char>::value &&
                  !std::is_same<T, wchar_t>::value>;

#if defined(FMT_USE_FLOAT128)
// Use the provided definition.
#elif FMT_CLANG_VERSION >= 309 && FMT_HAS_INCLUDE(<quadmath.h>)
#  define FMT_USE_FLOAT128 1
#elif FMT_GCC_VERSION && defined(_GLIBCXX_USE_FLOAT128) && \
    !defined(__STRICT_ANSI__)
#  define FMT_USE_FLOAT128 1
#else
#  define FMT_USE_FLOAT128 0
#endif
#if FMT_USE_FLOAT128
using float128 = __float128;
#else
struct float128 {};
#endif

template <typename T> using is_float128 = std::is_same<T, float128>;

template <typename T> struct is_floating_point : std::is_floating_point<T> {};
template <> struct is_floating_point<float128> : std::true_type {};

template <typename T, bool = is_floating_point<T>::value>
struct is_fast_float : bool_constant<std::numeric_limits<T>::is_iec559 &&
                                     sizeof(T) <= sizeof(double)> {};
template <typename T> struct is_fast_float<T, false> : std::false_type {};

template <typename T>
using fast_float_t = conditional_t<sizeof(T) == sizeof(double), double, float>;

template <typename T>
using is_double_double = bool_constant<std::numeric_limits<T>::digits == 106>;

#ifndef FMT_USE_FULL_CACHE_DRAGONBOX
#  define FMT_USE_FULL_CACHE_DRAGONBOX 0
#endif

// An allocator that uses malloc/free to allow removing dependency on the C++
// standard libary runtime. std::decay is used for back_inserter to be found by
// ADL when applied to memory_buffer.
template <typename T> struct allocator : private std::decay<void> {
  using value_type = T;

  T* allocate(size_t n) {
    FMT_ASSERT(n <= max_value<size_t>() / sizeof(T), "");
    T* p = static_cast<T*>(std::malloc(n * sizeof(T)));
    if (!p) FMT_THROW(std::bad_alloc());
    return p;
  }

  void deallocate(T* p, size_t) { std::free(p); }

  constexpr friend auto operator==(allocator, allocator) noexcept -> bool {
    return true;  // All instances of this allocator are equivalent.
  }
  constexpr friend auto operator!=(allocator, allocator) noexcept -> bool {
    return false;
  }
};

}  // namespace detail

FMT_BEGIN_EXPORT

// The number of characters to store in the basic_memory_buffer object itself
// to avoid dynamic memory allocation.
enum { inline_buffer_size = 500 };

/**
 * A dynamically growing memory buffer for trivially copyable/constructible
 * types with the first `SIZE` elements stored in the object itself. Most
 * commonly used via the `memory_buffer` alias for `char`.
 *
 * **Example**:
 *
 *     auto out = fmt::memory_buffer();
 *     fmt::format_to(std::back_inserter(out), "The answer is {}.", 42);
 *
 * This will append "The answer is 42." to `out`. The buffer content can be
 * converted to `std::string` with `to_string(out)`.
 */
template <typename T, size_t SIZE = inline_buffer_size,
          typename Allocator = detail::allocator<T>>
class basic_memory_buffer : public detail::buffer<T> {
 private:
  T store_[SIZE];

  // Don't inherit from Allocator to avoid generating type_info for it.
  FMT_NO_UNIQUE_ADDRESS Allocator alloc_;

  // Deallocate memory allocated by the buffer.
  FMT_CONSTEXPR20 void deallocate() {
    T* data = this->data();
    if (data != store_) alloc_.deallocate(data, this->capacity());
  }

  static FMT_CONSTEXPR20 void grow(detail::buffer<T>& buf, size_t size) {
    detail::abort_fuzzing_if(size > 5000);
    auto& self = static_cast<basic_memory_buffer&>(buf);
    const size_t max_size =
        std::allocator_traits<Allocator>::max_size(self.alloc_);
    size_t old_capacity = buf.capacity();
    size_t new_capacity = old_capacity + old_capacity / 2;
    if (size > new_capacity)
      new_capacity = size;
    else if (new_capacity > max_size)
      new_capacity = max_of(size, max_size);
    T* old_data = buf.data();
    T* new_data = self.alloc_.allocate(new_capacity);
    // Suppress a bogus -Wstringop-overflow in gcc 13.1 (#3481).
    detail::assume(buf.size() <= new_capacity);
    // The following code doesn't throw, so the raw pointer above doesn't leak.
    memcpy(new_data, old_data, buf.size() * sizeof(T));
    self.set(new_data, new_capacity);
    // deallocate must not throw according to the standard, but even if it does,
    // the buffer already uses the new storage and will deallocate it in
    // destructor.
    if (old_data != self.store_) self.alloc_.deallocate(old_data, old_capacity);
  }

 public:
  using value_type = T;
  using const_reference = const T&;

  FMT_CONSTEXPR explicit basic_memory_buffer(
      const Allocator& alloc = Allocator())
      : detail::buffer<T>(grow), alloc_(alloc) {
    this->set(store_, SIZE);
    if (detail::is_constant_evaluated()) detail::fill_n(store_, SIZE, T());
  }
  FMT_CONSTEXPR20 ~basic_memory_buffer() { deallocate(); }

 private:
  template <typename Alloc = Allocator,
            FMT_ENABLE_IF(std::allocator_traits<Alloc>::
                              propagate_on_container_move_assignment::value)>
  FMT_CONSTEXPR20 auto move_alloc(basic_memory_buffer& other) -> bool {
    alloc_ = std::move(other.alloc_);
    return true;
  }
  // If the allocator does not propagate then copy the data from other.
  template <typename Alloc = Allocator,
            FMT_ENABLE_IF(!std::allocator_traits<Alloc>::
                              propagate_on_container_move_assignment::value)>
  FMT_CONSTEXPR20 auto move_alloc(basic_memory_buffer& other) -> bool {
    T* data = other.data();
    if (alloc_ == other.alloc_ || data == other.store_) return true;
    size_t size = other.size();
    // Perform copy operation, allocators are different.
    this->resize(size);
    detail::copy<T>(data, data + size, this->data());
    return false;
  }

  // Move data from other to this buffer.
  FMT_CONSTEXPR20 void move(basic_memory_buffer& other) {
    T* data = other.data();
    size_t size = other.size(), capacity = other.capacity();
    if (!move_alloc(other)) return;
    if (data == other.store_) {
      this->set(store_, capacity);
      detail::copy<T>(other.store_, other.store_ + size, store_);
    } else {
      this->set(data, capacity);
      // Set pointer to the inline array so that delete is not called
      // when deallocating.
      other.set(other.store_, 0);
      other.clear();
    }
    this->resize(size);
  }

 public:
  /// Constructs a `basic_memory_buffer` object moving the content of the other
  /// object to it.
  FMT_CONSTEXPR20 basic_memory_buffer(basic_memory_buffer&& other) noexcept
      : detail::buffer<T>(grow) {
    move(other);
  }

  /// Moves the content of the other `basic_memory_buffer` object to this one.
  auto operator=(basic_memory_buffer&& other) noexcept -> basic_memory_buffer& {
    FMT_ASSERT(this != &other, "");
    deallocate();
    move(other);
    return *this;
  }

  // Returns a copy of the allocator associated with this buffer.
  auto get_allocator() const -> Allocator { return alloc_; }

  /// Resizes the buffer to contain `count` elements. If T is a POD type new
  /// elements may not be initialized.
  FMT_CONSTEXPR void resize(size_t count) { this->try_resize(count); }

  /// Increases the buffer capacity to `new_capacity`.
  void reserve(size_t new_capacity) { this->try_reserve(new_capacity); }

  using detail::buffer<T>::append;
  template <typename ContiguousRange>
  FMT_CONSTEXPR20 void append(const ContiguousRange& range) {
    append(range.data(), range.data() + range.size());
  }
};

using memory_buffer = basic_memory_buffer<char>;

template <size_t SIZE>
FMT_NODISCARD auto to_string(const basic_memory_buffer<char, SIZE>& buf)
    -> std::string {
  auto size = buf.size();
  detail::assume(size < std::string().max_size());
  return {buf.data(), size};
}

// A writer to a buffered stream. It doesn't own the underlying stream.
class writer {
 private:
  detail::buffer<char>* buf_;

  // We cannot create a file buffer in advance because any write to a FILE may
  // invalidate it.
  FILE* file_;

 public:
  inline writer(FILE* f) : buf_(nullptr), file_(f) {}
  inline writer(detail::buffer<char>& buf) : buf_(&buf) {}

  /// Formats `args` according to specifications in `fmt` and writes the
  /// output to the file.
  template <typename... T> void print(format_string<T...> fmt, T&&... args) {
    if (buf_)
      fmt::format_to(appender(*buf_), fmt, std::forward<T>(args)...);
    else
      fmt::print(file_, fmt, std::forward<T>(args)...);
  }
};

class string_buffer {
 private:
  std::string str_;
  detail::container_buffer<std::string> buf_;

 public:
  inline string_buffer() : buf_(str_) {}

  inline operator writer() { return buf_; }
  inline std::string& str() { return str_; }
};

template <typename T, size_t SIZE, typename Allocator>
struct is_contiguous<basic_memory_buffer<T, SIZE, Allocator>> : std::true_type {
};

// Suppress a misleading warning in older versions of clang.
FMT_PRAGMA_CLANG(diagnostic ignored "-Wweak-vtables")

/// An error reported from a formatting function.
class FMT_SO_VISIBILITY("default") format_error : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class loc_value;

FMT_END_EXPORT
namespace detail {
FMT_API auto write_console(int fd, string_view text) -> bool;
FMT_API void print(FILE*, string_view);
}  // namespace detail

namespace detail {
template <typename Char, size_t N> struct fixed_string {
  FMT_CONSTEXPR20 fixed_string(const Char (&s)[N]) {
    detail::copy<Char, const Char*, Char*>(static_cast<const Char*>(s), s + N,
                                           data);
  }
  Char data[N] = {};
};

// Converts a compile-time string to basic_string_view.
FMT_EXPORT template <typename Char, size_t N>
constexpr auto compile_string_to_view(const Char (&s)[N])
    -> basic_string_view<Char> {
  // Remove trailing NUL character if needed. Won't be present if this is used
  // with a raw character array (i.e. not defined as a string).
  return {s, N - (std::char_traits<Char>::to_int_type(s[N - 1]) == 0 ? 1 : 0)};
}
FMT_EXPORT template <typename Char>
constexpr auto compile_string_to_view(basic_string_view<Char> s)
    -> basic_string_view<Char> {
  return s;
}

// Returns true if value is negative, false otherwise.
// Same as `value < 0` but doesn't produce warnings if T is an unsigned type.
template <typename T, FMT_ENABLE_IF(is_signed<T>::value)>
constexpr auto is_negative(T value) -> bool {
  return value < 0;
}
template <typename T, FMT_ENABLE_IF(!is_signed<T>::value)>
constexpr auto is_negative(T) -> bool {
  return false;
}

// Smallest of uint32_t, uint64_t, uint128_t that is large enough to
// represent all values of an integral type T.
template <typename T>
using uint32_or_64_or_128_t =
    conditional_t<num_bits<T>() <= 32 && !FMT_REDUCE_INT_INSTANTIATIONS,
                  uint32_t,
                  conditional_t<num_bits<T>() <= 64, uint64_t, uint128_t>>;
template <typename T>
using uint64_or_128_t = conditional_t<num_bits<T>() <= 64, uint64_t, uint128_t>;

#define FMT_POWERS_OF_10(factor)                                  \
  factor * 10, (factor) * 100, (factor) * 1000, (factor) * 10000, \
      (factor) * 100000, (factor) * 1000000, (factor) * 10000000, \
      (factor) * 100000000, (factor) * 1000000000

// Converts value in the range [0, 100) to a string.
// GCC generates slightly better code when value is pointer-size.
inline auto digits2(size_t value) -> const char* {
  // Align data since unaligned access may be slower when crossing a
  // hardware-specific boundary.
  alignas(2) static const char data[] =
      "0001020304050607080910111213141516171819"
      "2021222324252627282930313233343536373839"
      "4041424344454647484950515253545556575859"
      "6061626364656667686970717273747576777879"
      "8081828384858687888990919293949596979899";
  return &data[value * 2];
}

template <typename Char> constexpr auto getsign(sign s) -> Char {
  return static_cast<char>(((' ' << 24) | ('+' << 16) | ('-' << 8)) >>
                           (static_cast<int>(s) * 8));
}

template <typename T> FMT_CONSTEXPR auto count_digits_fallback(T n) -> int {
  int count = 1;
  for (;;) {
    // Integer division is slow so do it for a group of four digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    if (n < 10) return count;
    if (n < 100) return count + 1;
    if (n < 1000) return count + 2;
    if (n < 10000) return count + 3;
    n /= 10000u;
    count += 4;
  }
}
#if FMT_USE_INT128
FMT_CONSTEXPR inline auto count_digits(uint128_opt n) -> int {
  return count_digits_fallback(n);
}
#endif

#ifdef FMT_BUILTIN_CLZLL
// It is a separate function rather than a part of count_digits to workaround
// the lack of static constexpr in constexpr functions.
inline auto do_count_digits(uint64_t n) -> int {
  // This has comparable performance to the version by Kendall Willets
  // (https://github.com/fmtlib/format-benchmark/blob/master/digits10)
  // but uses smaller tables.
  // Maps bsr(n) to ceil(log10(pow(2, bsr(n) + 1) - 1)).
  static constexpr uint8_t bsr2log10[] = {
      1,  1,  1,  2,  2,  2,  3,  3,  3,  4,  4,  4,  4,  5,  5,  5,
      6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9,  10, 10, 10,
      10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 15, 15,
      15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 19, 20};
  auto t = bsr2log10[FMT_BUILTIN_CLZLL(n | 1) ^ 63];
  static constexpr uint64_t zero_or_powers_of_10[] = {
      0, 0, FMT_POWERS_OF_10(1U), FMT_POWERS_OF_10(1000000000ULL),
      10000000000000000000ULL};
  return t - (n < zero_or_powers_of_10[t]);
}
#endif

// Returns the number of decimal digits in n. Leading zeros are not counted
// except for n == 0 in which case count_digits returns 1.
FMT_CONSTEXPR20 inline auto count_digits(uint64_t n) -> int {
#ifdef FMT_BUILTIN_CLZLL
  if (!is_constant_evaluated() && !FMT_OPTIMIZE_SIZE) return do_count_digits(n);
#endif
  return count_digits_fallback(n);
}

// Counts the number of digits in n. BITS = log2(radix).
template <int BITS, typename UInt>
FMT_CONSTEXPR auto count_digits(UInt n) -> int {
#ifdef FMT_BUILTIN_CLZ
  if (!is_constant_evaluated() && num_bits<UInt>() == 32)
    return (FMT_BUILTIN_CLZ(static_cast<uint32_t>(n) | 1) ^ 31) / BITS + 1;
#endif
  // Lambda avoids unreachable code warnings from NVHPC.
  return [](UInt m) {
    int num_digits = 0;
    do {
      ++num_digits;
    } while ((m >>= BITS) != 0);
    return num_digits;
  }(n);
}

#ifdef FMT_BUILTIN_CLZ
// It is a separate function rather than a part of count_digits to workaround
// the lack of static constexpr in constexpr functions.
FMT_INLINE auto do_count_digits(uint32_t n) -> int {
// An optimization by Kendall Willets from https://bit.ly/3uOIQrB.
// This increments the upper 32 bits (log10(T) - 1) when >= T is added.
#  define FMT_INC(T) (((sizeof(#T) - 1ull) << 32) - T)
  static constexpr uint64_t table[] = {
      FMT_INC(0),          FMT_INC(0),          FMT_INC(0),           // 8
      FMT_INC(10),         FMT_INC(10),         FMT_INC(10),          // 64
      FMT_INC(100),        FMT_INC(100),        FMT_INC(100),         // 512
      FMT_INC(1000),       FMT_INC(1000),       FMT_INC(1000),        // 4096
      FMT_INC(10000),      FMT_INC(10000),      FMT_INC(10000),       // 32k
      FMT_INC(100000),     FMT_INC(100000),     FMT_INC(100000),      // 256k
      FMT_INC(1000000),    FMT_INC(1000000),    FMT_INC(1000000),     // 2048k
      FMT_INC(10000000),   FMT_INC(10000000),   FMT_INC(10000000),    // 16M
      FMT_INC(100000000),  FMT_INC(100000000),  FMT_INC(100000000),   // 128M
      FMT_INC(1000000000), FMT_INC(1000000000), FMT_INC(1000000000),  // 1024M
      FMT_INC(1000000000), FMT_INC(1000000000)                        // 4B
  };
  auto inc = table[FMT_BUILTIN_CLZ(n | 1) ^ 31];
  return static_cast<int>((n + inc) >> 32);
}
#endif

// Optional version of count_digits for better performance on 32-bit platforms.
FMT_CONSTEXPR20 inline auto count_digits(uint32_t n) -> int {
#ifdef FMT_BUILTIN_CLZ
  if (!is_constant_evaluated() && !FMT_OPTIMIZE_SIZE) return do_count_digits(n);
#endif
  return count_digits_fallback(n);
}

template <typename Int> constexpr auto digits10() noexcept -> int {
  return std::numeric_limits<Int>::digits10;
}
template <> constexpr auto digits10<int128_opt>() noexcept -> int { return 38; }
template <> constexpr auto digits10<uint128_t>() noexcept -> int { return 38; }

template <typename Char> struct thousands_sep_result {
  std::string grouping;
  Char thousands_sep;
};

template <typename Char>
FMT_API auto thousands_sep_impl(locale_ref loc) -> thousands_sep_result<Char>;
template <typename Char>
inline auto thousands_sep(locale_ref loc) -> thousands_sep_result<Char> {
  auto result = thousands_sep_impl<char>(loc);
  return {result.grouping, Char(result.thousands_sep)};
}
template <>
inline auto thousands_sep(locale_ref loc) -> thousands_sep_result<wchar_t> {
  return thousands_sep_impl<wchar_t>(loc);
}

template <typename Char>
FMT_API auto decimal_point_impl(locale_ref loc) -> Char;
template <typename Char> inline auto decimal_point(locale_ref loc) -> Char {
  return Char(decimal_point_impl<char>(loc));
}
template <> inline auto decimal_point(locale_ref loc) -> wchar_t {
  return decimal_point_impl<wchar_t>(loc);
}

#ifndef FMT_HEADER_ONLY
FMT_BEGIN_EXPORT
extern template FMT_API auto thousands_sep_impl<char>(locale_ref)
    -> thousands_sep_result<char>;
extern template FMT_API auto thousands_sep_impl<wchar_t>(locale_ref)
    -> thousands_sep_result<wchar_t>;
extern template FMT_API auto decimal_point_impl(locale_ref) -> char;
extern template FMT_API auto decimal_point_impl(locale_ref) -> wchar_t;
FMT_END_EXPORT
#endif  // FMT_HEADER_ONLY

// Compares two characters for equality.
template <typename Char> auto equal2(const Char* lhs, const char* rhs) -> bool {
  return lhs[0] == Char(rhs[0]) && lhs[1] == Char(rhs[1]);
}
inline auto equal2(const char* lhs, const char* rhs) -> bool {
  return memcmp(lhs, rhs, 2) == 0;
}

// Writes a two-digit value to out.
template <typename Char>
FMT_CONSTEXPR20 FMT_INLINE void write2digits(Char* out, size_t value) {
  if (!is_constant_evaluated() && std::is_same<Char, char>::value &&
      !FMT_OPTIMIZE_SIZE) {
    memcpy(out, digits2(value), 2);
    return;
  }
  *out++ = static_cast<Char>('0' + value / 10);
  *out = static_cast<Char>('0' + value % 10);
}

// Formats a decimal unsigned integer value writing to out pointing to a buffer
// of specified size. The caller must ensure that the buffer is large enough.
template <typename Char, typename UInt>
FMT_CONSTEXPR20 auto do_format_decimal(Char* out, UInt value, int size)
    -> Char* {
  FMT_ASSERT(size >= count_digits(value), "invalid digit count");
  unsigned n = to_unsigned(size);
  while (value >= 100) {
    // Integer division is slow so do it for a group of two digits instead
    // of for every digit. The idea comes from the talk by Alexandrescu
    // "Three Optimization Tips for C++". See speed-test for a comparison.
    n -= 2;
    write2digits(out + n, static_cast<unsigned>(value % 100));
    value /= 100;
  }
  if (value >= 10) {
    n -= 2;
    write2digits(out + n, static_cast<unsigned>(value));
  } else {
    out[--n] = static_cast<Char>('0' + value);
  }
  return out + n;
}

template <typename Char, typename UInt>
FMT_CONSTEXPR FMT_INLINE auto format_decimal(Char* out, UInt value,
                                             int num_digits) -> Char* {
  do_format_decimal(out, value, num_digits);
  return out + num_digits;
}

template <typename Char, typename UInt, typename OutputIt,
          FMT_ENABLE_IF(!std::is_pointer<remove_cvref_t<OutputIt>>::value)>
FMT_CONSTEXPR auto format_decimal(OutputIt out, UInt value, int num_digits)
    -> OutputIt {
  if (auto ptr = to_pointer<Char>(out, to_unsigned(num_digits))) {
    do_format_decimal(ptr, value, num_digits);
    return out;
  }
  // Buffer is large enough to hold all digits (digits10 + 1).
  char buffer[digits10<UInt>() + 1];
  if (is_constant_evaluated()) fill_n(buffer, sizeof(buffer), '\0');
  do_format_decimal(buffer, value, num_digits);
  return copy_noinline<Char>(buffer, buffer + num_digits, out);
}

template <typename Char, typename UInt>
FMT_CONSTEXPR auto do_format_base2e(int base_bits, Char* out, UInt value,
                                    int size, bool upper = false) -> Char* {
  out += size;
  do {
    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    unsigned digit = static_cast<unsigned>(value & ((1u << base_bits) - 1));
    *--out = static_cast<Char>(base_bits < 4 ? static_cast<char>('0' + digit)
                                             : digits[digit]);
  } while ((value >>= base_bits) != 0);
  return out;
}

// Formats an unsigned integer in the power of two base (binary, octal, hex).
template <typename Char, typename UInt>
FMT_CONSTEXPR auto format_base2e(int base_bits, Char* out, UInt value,
                                 int num_digits, bool upper = false) -> Char* {
  do_format_base2e(base_bits, out, value, num_digits, upper);
  return out + num_digits;
}

template <typename Char, typename OutputIt, typename UInt,
          FMT_ENABLE_IF(is_back_insert_iterator<OutputIt>::value)>
FMT_CONSTEXPR inline auto format_base2e(int base_bits, OutputIt out, UInt value,
                                        int num_digits, bool upper = false)
    -> OutputIt {
  if (auto ptr = to_pointer<Char>(out, to_unsigned(num_digits))) {
    format_base2e(base_bits, ptr, value, num_digits, upper);
    return out;
  }
  // Make buffer large enough for any base.
  char buffer[num_bits<UInt>()];
  if (is_constant_evaluated()) fill_n(buffer, sizeof(buffer), '\0');
  format_base2e(base_bits, buffer, value, num_digits, upper);
  return detail::copy_noinline<Char>(buffer, buffer + num_digits, out);
}

// A converter from UTF-8 to UTF-16.
class utf8_to_utf16 {
 private:
  basic_memory_buffer<wchar_t> buffer_;

 public:
  FMT_API explicit utf8_to_utf16(string_view s);
  inline operator basic_string_view<wchar_t>() const {
    return {&buffer_[0], size()};
  }
  inline auto size() const -> size_t { return buffer_.size() - 1; }
  inline auto c_str() const -> const wchar_t* { return &buffer_[0]; }
  inline auto str() const -> std::wstring { return {&buffer_[0], size()}; }
};

enum class to_utf8_error_policy { abort, replace };

// A converter from UTF-16/UTF-32 (host endian) to UTF-8.
template <typename WChar, typename Buffer = memory_buffer> class to_utf8 {
 private:
  Buffer buffer_;

 public:
  to_utf8() {}
  explicit to_utf8(basic_string_view<WChar> s,
                   to_utf8_error_policy policy = to_utf8_error_policy::abort) {
    static_assert(sizeof(WChar) == 2 || sizeof(WChar) == 4,
                  "expected utf16 or utf32");
    if (!convert(s, policy)) {
      FMT_THROW(std::runtime_error(sizeof(WChar) == 2 ? "invalid utf16"
                                                      : "invalid utf32"));
    }
  }
  operator string_view() const { return string_view(&buffer_[0], size()); }
  auto size() const -> size_t { return buffer_.size() - 1; }
  auto c_str() const -> const char* { return &buffer_[0]; }
  auto str() const -> std::string { return std::string(&buffer_[0], size()); }

  // Performs conversion returning a bool instead of throwing exception on
  // conversion error. This method may still throw in case of memory allocation
  // error.
  auto convert(basic_string_view<WChar> s,
               to_utf8_error_policy policy = to_utf8_error_policy::abort)
      -> bool {
    if (!convert(buffer_, s, policy)) return false;
    buffer_.push_back(0);
    return true;
  }
  static auto convert(Buffer& buf, basic_string_view<WChar> s,
                      to_utf8_error_policy policy = to_utf8_error_policy::abort)
      -> bool {
    for (auto p = s.begin(); p != s.end(); ++p) {
      uint32_t c = static_cast<uint32_t>(*p);
      if (sizeof(WChar) == 2 && c >= 0xd800 && c <= 0xdfff) {
        // Handle a surrogate pair.
        ++p;
        if (p == s.end() || (c & 0xfc00) != 0xd800 || (*p & 0xfc00) != 0xdc00) {
          if (policy == to_utf8_error_policy::abort) return false;
          buf.append(string_view("\xEF\xBF\xBD"));
          --p;
          continue;
        }
        c = (c << 10) + static_cast<uint32_t>(*p) - 0x35fdc00;
      }
      if (c < 0x80) {
        buf.push_back(static_cast<char>(c));
      } else if (c < 0x800) {
        buf.push_back(static_cast<char>(0xc0 | (c >> 6)));
        buf.push_back(static_cast<char>(0x80 | (c & 0x3f)));
      } else if ((c >= 0x800 && c <= 0xd7ff) || (c >= 0xe000 && c <= 0xffff)) {
        buf.push_back(static_cast<char>(0xe0 | (c >> 12)));
        buf.push_back(static_cast<char>(0x80 | ((c & 0xfff) >> 6)));
        buf.push_back(static_cast<char>(0x80 | (c & 0x3f)));
      } else if (c >= 0x10000 && c <= 0x10ffff) {
        buf.push_back(static_cast<char>(0xf0 | (c >> 18)));
        buf.push_back(static_cast<char>(0x80 | ((c & 0x3ffff) >> 12)));
        buf.push_back(static_cast<char>(0x80 | ((c & 0xfff) >> 6)));
        buf.push_back(static_cast<char>(0x80 | (c & 0x3f)));
      } else {
        return false;
      }
    }
    return true;
  }
};

// Computes 128-bit result of multiplication of two 64-bit unsigned integers.
FMT_INLINE auto umul128(uint64_t x, uint64_t y) noexcept -> uint128_fallback {
#if FMT_USE_INT128
  auto p = static_cast<uint128_opt>(x) * static_cast<uint128_opt>(y);
  return {static_cast<uint64_t>(p >> 64), static_cast<uint64_t>(p)};
#elif defined(_MSC_VER) && defined(_M_X64)
  auto hi = uint64_t();
  auto lo = _umul128(x, y, &hi);
  return {hi, lo};
#else
  const uint64_t mask = static_cast<uint64_t>(max_value<uint32_t>());

  uint64_t a = x >> 32;
  uint64_t b = x & mask;
  uint64_t c = y >> 32;
  uint64_t d = y & mask;

  uint64_t ac = a * c;
  uint64_t bc = b * c;
  uint64_t ad = a * d;
  uint64_t bd = b * d;

  uint64_t intermediate = (bd >> 32) + (ad & mask) + (bc & mask);

  return {ac + (intermediate >> 32) + (ad >> 32) + (bc >> 32),
          (intermediate << 32) + (bd & mask)};
#endif
}

namespace dragonbox {
// Computes floor(log10(pow(2, e))) for e in [-2620, 2620] using the method from
// https://fmt.dev/papers/Dragonbox.pdf#page=28, section 6.1.
inline auto floor_log10_pow2(int e) noexcept -> int {
  FMT_ASSERT(e <= 2620 && e >= -2620, "too large exponent");
  static_assert((-1 >> 1) == -1, "right shift is not arithmetic");
  return (e * 315653) >> 20;
}

inline auto floor_log2_pow10(int e) noexcept -> int {
  FMT_ASSERT(e <= 1233 && e >= -1233, "too large exponent");
  return (e * 1741647) >> 19;
}

// Computes upper 64 bits of multiplication of two 64-bit unsigned integers.
inline auto umul128_upper64(uint64_t x, uint64_t y) noexcept -> uint64_t {
#if FMT_USE_INT128
  auto p = static_cast<uint128_opt>(x) * static_cast<uint128_opt>(y);
  return static_cast<uint64_t>(p >> 64);
#elif defined(_MSC_VER) && defined(_M_X64)
  return __umulh(x, y);
#else
  return umul128(x, y).high();
#endif
}

// Computes upper 128 bits of multiplication of a 64-bit unsigned integer and a
// 128-bit unsigned integer.
inline auto umul192_upper128(uint64_t x, uint128_fallback y) noexcept
    -> uint128_fallback {
  uint128_fallback r = umul128(x, y.high());
  r += umul128_upper64(x, y.low());
  return r;
}

FMT_API auto get_cached_power(int k) noexcept -> uint128_fallback;

// Type-specific information that Dragonbox uses.
template <typename T, typename Enable = void> struct float_info;

template <> struct float_info<float> {
  using carrier_uint = uint32_t;
  static const int exponent_bits = 8;
  static const int kappa = 1;
  static const int big_divisor = 100;
  static const int small_divisor = 10;
  static const int min_k = -31;
  static const int max_k = 46;
  static const int shorter_interval_tie_lower_threshold = -35;
  static const int shorter_interval_tie_upper_threshold = -35;
};

template <> struct float_info<double> {
  using carrier_uint = uint64_t;
  static const int exponent_bits = 11;
  static const int kappa = 2;
  static const int big_divisor = 1000;
  static const int small_divisor = 100;
  static const int min_k = -292;
  static const int max_k = 341;
  static const int shorter_interval_tie_lower_threshold = -77;
  static const int shorter_interval_tie_upper_threshold = -77;
};

// An 80- or 128-bit floating point number.
template <typename T>
struct float_info<T, enable_if_t<std::numeric_limits<T>::digits == 64 ||
                                 std::numeric_limits<T>::digits == 113 ||
                                 is_float128<T>::value>> {
  using carrier_uint = detail::uint128_t;
  static const int exponent_bits = 15;
};

// A double-double floating point number.
template <typename T>
struct float_info<T, enable_if_t<is_double_double<T>::value>> {
  using carrier_uint = detail::uint128_t;
};

template <typename T> struct decimal_fp {
  using significand_type = typename float_info<T>::carrier_uint;
  significand_type significand;
  int exponent;
};

template <typename T> FMT_API auto to_decimal(T x) noexcept -> decimal_fp<T>;
}  // namespace dragonbox

// Returns true iff Float has the implicit bit which is not stored.
template <typename Float> constexpr auto has_implicit_bit() -> bool {
  // An 80-bit FP number has a 64-bit significand an no implicit bit.
  return std::numeric_limits<Float>::digits != 64;
}

// Returns the number of significand bits stored in Float. The implicit bit is
// not counted since it is not stored.
template <typename Float> constexpr auto num_significand_bits() -> int {
  // std::numeric_limits may not support __float128.
  return is_float128<Float>() ? 112
                              : (std::numeric_limits<Float>::digits -
                                 (has_implicit_bit<Float>() ? 1 : 0));
}

template <typename Float>
constexpr auto exponent_mask() ->
    typename dragonbox::float_info<Float>::carrier_uint {
  using float_uint = typename dragonbox::float_info<Float>::carrier_uint;
  return ((float_uint(1) << dragonbox::float_info<Float>::exponent_bits) - 1)
         << num_significand_bits<Float>();
}
template <typename Float> constexpr auto exponent_bias() -> int {
  // std::numeric_limits may not support __float128.
  return is_float128<Float>() ? 16383
                              : std::numeric_limits<Float>::max_exponent - 1;
}

FMT_CONSTEXPR inline auto compute_exp_size(int exp) -> int {
  auto prefix_size = 2;  // sign + 'e'
  auto abs_exp = exp >= 0 ? exp : -exp;
  if (abs_exp < 100) return prefix_size + 2;
  return prefix_size + (abs_exp >= 1000 ? 4 : 3);
}

// Writes the exponent exp in the form "[+-]d{2,3}" to buffer.
template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write_exponent(int exp, OutputIt out) -> OutputIt {
  FMT_ASSERT(-10000 < exp && exp < 10000, "exponent out of range");
  if (exp < 0) {
    *out++ = static_cast<Char>('-');
    exp = -exp;
  } else {
    *out++ = static_cast<Char>('+');
  }
  auto uexp = static_cast<uint32_t>(exp);
  if (is_constant_evaluated()) {
    if (uexp < 10) *out++ = '0';
    return format_decimal<Char>(out, uexp, count_digits(uexp));
  }
  if (uexp >= 100u) {
    const char* top = digits2(uexp / 100);
    if (uexp >= 1000u) *out++ = static_cast<Char>(top[0]);
    *out++ = static_cast<Char>(top[1]);
    uexp %= 100;
  }
  const char* d = digits2(uexp);
  *out++ = static_cast<Char>(d[0]);
  *out++ = static_cast<Char>(d[1]);
  return out;
}

// A floating-point number f * pow(2, e) where F is an unsigned type.
template <typename F> struct basic_fp {
  F f;
  int e;

  static constexpr int num_significand_bits =
      static_cast<int>(sizeof(F) * num_bits<unsigned char>());

  constexpr basic_fp() : f(0), e(0) {}
  constexpr basic_fp(uint64_t f_val, int e_val) : f(f_val), e(e_val) {}

  // Constructs fp from an IEEE754 floating-point number.
  template <typename Float> FMT_CONSTEXPR basic_fp(Float n) { assign(n); }

  // Assigns n to this and return true iff predecessor is closer than successor.
  template <typename Float, FMT_ENABLE_IF(!is_double_double<Float>::value)>
  FMT_CONSTEXPR auto assign(Float n) -> bool {
    static_assert(std::numeric_limits<Float>::digits <= 113, "unsupported FP");
    // Assume Float is in the format [sign][exponent][significand].
    using carrier_uint = typename dragonbox::float_info<Float>::carrier_uint;
    const auto num_float_significand_bits =
        detail::num_significand_bits<Float>();
    const auto implicit_bit = carrier_uint(1) << num_float_significand_bits;
    const auto significand_mask = implicit_bit - 1;
    auto u = bit_cast<carrier_uint>(n);
    f = static_cast<F>(u & significand_mask);
    auto biased_e = static_cast<int>((u & exponent_mask<Float>()) >>
                                     num_float_significand_bits);
    // The predecessor is closer if n is a normalized power of 2 (f == 0)
    // other than the smallest normalized number (biased_e > 1).
    auto is_predecessor_closer = f == 0 && biased_e > 1;
    if (biased_e == 0)
      biased_e = 1;  // Subnormals use biased exponent 1 (min exponent).
    else if (has_implicit_bit<Float>())
      f += static_cast<F>(implicit_bit);
    e = biased_e - exponent_bias<Float>() - num_float_significand_bits;
    if (!has_implicit_bit<Float>()) ++e;
    return is_predecessor_closer;
  }

  template <typename Float, FMT_ENABLE_IF(is_double_double<Float>::value)>
  FMT_CONSTEXPR auto assign(Float n) -> bool {
    static_assert(std::numeric_limits<double>::is_iec559, "unsupported FP");
    return assign(static_cast<double>(n));
  }
};

using fp = basic_fp<unsigned long long>;

// Normalizes the value converted from double and multiplied by (1 << SHIFT).
template <int SHIFT = 0, typename F>
FMT_CONSTEXPR auto normalize(basic_fp<F> value) -> basic_fp<F> {
  // Handle subnormals.
  const auto implicit_bit = F(1) << num_significand_bits<double>();
  const auto shifted_implicit_bit = implicit_bit << SHIFT;
  while ((value.f & shifted_implicit_bit) == 0) {
    value.f <<= 1;
    --value.e;
  }
  // Subtract 1 to account for hidden bit.
  const auto offset = basic_fp<F>::num_significand_bits -
                      num_significand_bits<double>() - SHIFT - 1;
  value.f <<= offset;
  value.e -= offset;
  return value;
}

// Computes lhs * rhs / pow(2, 64) rounded to nearest with half-up tie breaking.
FMT_CONSTEXPR inline auto multiply(uint64_t lhs, uint64_t rhs) -> uint64_t {
#if FMT_USE_INT128
  auto product = static_cast<__uint128_t>(lhs) * rhs;
  auto f = static_cast<uint64_t>(product >> 64);
  return (static_cast<uint64_t>(product) & (1ULL << 63)) != 0 ? f + 1 : f;
#else
  // Multiply 32-bit parts of significands.
  uint64_t mask = (1ULL << 32) - 1;
  uint64_t a = lhs >> 32, b = lhs & mask;
  uint64_t c = rhs >> 32, d = rhs & mask;
  uint64_t ac = a * c, bc = b * c, ad = a * d, bd = b * d;
  // Compute mid 64-bit of result and round.
  uint64_t mid = (bd >> 32) + (ad & mask) + (bc & mask) + (1U << 31);
  return ac + (ad >> 32) + (bc >> 32) + (mid >> 32);
#endif
}

FMT_CONSTEXPR inline auto operator*(fp x, fp y) -> fp {
  return {multiply(x.f, y.f), x.e + y.e + 64};
}

template <typename T, bool doublish = num_bits<T>() == num_bits<double>()>
using convert_float_result =
    conditional_t<std::is_same<T, float>::value || doublish, double, T>;

template <typename T>
constexpr auto convert_float(T value) -> convert_float_result<T> {
  return static_cast<convert_float_result<T>>(value);
}

template <bool C, typename T, typename F, FMT_ENABLE_IF(C)>
auto select(T true_value, F) -> T {
  return true_value;
}
template <bool C, typename T, typename F, FMT_ENABLE_IF(!C)>
auto select(T, F false_value) -> F {
  return false_value;
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR FMT_NOINLINE auto fill(OutputIt it, size_t n,
                                     const basic_specs& specs) -> OutputIt {
  auto fill_size = specs.fill_size();
  if (fill_size == 1) return detail::fill_n(it, n, specs.fill_unit<Char>());
  if (const Char* data = specs.fill<Char>()) {
    for (size_t i = 0; i < n; ++i) it = copy<Char>(data, data + fill_size, it);
  }
  return it;
}

// Writes the output of f, padded according to format specifications in specs.
// size: output size in code units.
// width: output display width in (terminal) column positions.
template <typename Char, align default_align = align::left, typename OutputIt,
          typename F>
FMT_CONSTEXPR auto write_padded(OutputIt out, const format_specs& specs,
                                size_t size, size_t width, F&& f) -> OutputIt {
  static_assert(default_align == align::left || default_align == align::right,
                "");
  unsigned spec_width = to_unsigned(specs.width);
  size_t padding = spec_width > width ? spec_width - width : 0;
  // Shifts are encoded as string literals because static constexpr is not
  // supported in constexpr functions.
  auto* shifts =
      default_align == align::left ? "\x1f\x1f\x00\x01" : "\x00\x1f\x00\x01";
  size_t left_padding = padding >> shifts[static_cast<int>(specs.align())];
  size_t right_padding = padding - left_padding;
  auto it = reserve(out, size + padding * specs.fill_size());
  if (left_padding != 0) it = fill<Char>(it, left_padding, specs);
  it = f(it);
  if (right_padding != 0) it = fill<Char>(it, right_padding, specs);
  return base_iterator(out, it);
}

template <typename Char, align default_align = align::left, typename OutputIt,
          typename F>
constexpr auto write_padded(OutputIt out, const format_specs& specs,
                            size_t size, F&& f) -> OutputIt {
  return write_padded<Char, default_align>(out, specs, size, size, f);
}

template <typename Char, align default_align = align::left, typename OutputIt>
FMT_CONSTEXPR auto write_bytes(OutputIt out, string_view bytes,
                               const format_specs& specs = {}) -> OutputIt {
  return write_padded<Char, default_align>(
      out, specs, bytes.size(), [bytes](reserve_iterator<OutputIt> it) {
        const char* data = bytes.data();
        return copy<Char>(data, data + bytes.size(), it);
      });
}

template <typename Char, typename OutputIt, typename UIntPtr>
auto write_ptr(OutputIt out, UIntPtr value, const format_specs* specs)
    -> OutputIt {
  int num_digits = count_digits<4>(value);
  auto size = to_unsigned(num_digits) + size_t(2);
  auto write = [=](reserve_iterator<OutputIt> it) {
    *it++ = static_cast<Char>('0');
    *it++ = static_cast<Char>('x');
    return format_base2e<Char>(4, it, value, num_digits);
  };
  return specs ? write_padded<Char, align::right>(out, *specs, size, write)
               : base_iterator(out, write(reserve(out, size)));
}

// Returns true iff the code point cp is printable.
FMT_API auto is_printable(uint32_t cp) -> bool;

inline auto needs_escape(uint32_t cp) -> bool {
  if (cp < 0x20 || cp == 0x7f || cp == '"' || cp == '\\') return true;
  if (const_check(FMT_OPTIMIZE_SIZE > 1)) return false;
  return !is_printable(cp);
}

template <typename Char> struct find_escape_result {
  const Char* begin;
  const Char* end;
  uint32_t cp;
};

template <typename Char>
auto find_escape(const Char* begin, const Char* end)
    -> find_escape_result<Char> {
  for (; begin != end; ++begin) {
    uint32_t cp = static_cast<unsigned_char<Char>>(*begin);
    if (const_check(sizeof(Char) == 1) && cp >= 0x80) continue;
    if (needs_escape(cp)) return {begin, begin + 1, cp};
  }
  return {begin, nullptr, 0};
}

inline auto find_escape(const char* begin, const char* end)
    -> find_escape_result<char> {
  if (const_check(!use_utf8)) return find_escape<char>(begin, end);
  auto result = find_escape_result<char>{end, nullptr, 0};
  for_each_codepoint(string_view(begin, to_unsigned(end - begin)),
                     [&](uint32_t cp, string_view sv) {
                       if (needs_escape(cp)) {
                         result = {sv.begin(), sv.end(), cp};
                         return false;
                       }
                       return true;
                     });
  return result;
}

template <size_t width, typename Char, typename OutputIt>
auto write_codepoint(OutputIt out, char prefix, uint32_t cp) -> OutputIt {
  *out++ = static_cast<Char>('\\');
  *out++ = static_cast<Char>(prefix);
  Char buf[width];
  fill_n(buf, width, static_cast<Char>('0'));
  format_base2e(4, buf, cp, width);
  return copy<Char>(buf, buf + width, out);
}

template <typename OutputIt, typename Char>
auto write_escaped_cp(OutputIt out, const find_escape_result<Char>& escape)
    -> OutputIt {
  auto c = static_cast<Char>(escape.cp);
  switch (escape.cp) {
  case '\n':
    *out++ = static_cast<Char>('\\');
    c = static_cast<Char>('n');
    break;
  case '\r':
    *out++ = static_cast<Char>('\\');
    c = static_cast<Char>('r');
    break;
  case '\t':
    *out++ = static_cast<Char>('\\');
    c = static_cast<Char>('t');
    break;
  case '"':  FMT_FALLTHROUGH;
  case '\'': FMT_FALLTHROUGH;
  case '\\': *out++ = static_cast<Char>('\\'); break;
  default:
    if (escape.cp < 0x100) return write_codepoint<2, Char>(out, 'x', escape.cp);
    if (escape.cp < 0x10000)
      return write_codepoint<4, Char>(out, 'u', escape.cp);
    if (escape.cp < 0x110000)
      return write_codepoint<8, Char>(out, 'U', escape.cp);
    for (Char escape_char : basic_string_view<Char>(
             escape.begin, to_unsigned(escape.end - escape.begin))) {
      out = write_codepoint<2, Char>(out, 'x',
                                     static_cast<uint32_t>(escape_char) & 0xFF);
    }
    return out;
  }
  *out++ = c;
  return out;
}

template <typename Char, typename OutputIt>
auto write_escaped_string(OutputIt out, basic_string_view<Char> str)
    -> OutputIt {
  *out++ = static_cast<Char>('"');
  auto begin = str.begin(), end = str.end();
  do {
    auto escape = find_escape(begin, end);
    out = copy<Char>(begin, escape.begin, out);
    begin = escape.end;
    if (!begin) break;
    out = write_escaped_cp<OutputIt, Char>(out, escape);
  } while (begin != end);
  *out++ = static_cast<Char>('"');
  return out;
}

template <typename Char, typename OutputIt>
auto write_escaped_char(OutputIt out, Char v) -> OutputIt {
  Char v_array[1] = {v};
  *out++ = static_cast<Char>('\'');
  if ((needs_escape(static_cast<uint32_t>(v)) && v != static_cast<Char>('"')) ||
      v == static_cast<Char>('\'')) {
    out = write_escaped_cp(out,
                           find_escape_result<Char>{v_array, v_array + 1,
                                                    static_cast<uint32_t>(v)});
  } else {
    *out++ = v;
  }
  *out++ = static_cast<Char>('\'');
  return out;
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write_char(OutputIt out, Char value,
                              const format_specs& specs) -> OutputIt {
  bool is_debug = specs.type() == presentation_type::debug;
  return write_padded<Char>(out, specs, 1, [=](reserve_iterator<OutputIt> it) {
    if (is_debug) return write_escaped_char(it, value);
    *it++ = value;
    return it;
  });
}
template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, Char value, const format_specs& specs,
                         locale_ref loc = {}) -> OutputIt {
  // char is formatted as unsigned char for consistency across platforms.
  using unsigned_type =
      conditional_t<std::is_same<Char, char>::value, unsigned char, unsigned>;
  return check_char_specs(specs)
             ? write_char<Char>(out, value, specs)
             : write<Char>(out, static_cast<unsigned_type>(value), specs, loc);
}

template <typename Char> class digit_grouping {
 private:
  std::string grouping_;
  std::basic_string<Char> thousands_sep_;

  struct next_state {
    std::string::const_iterator group;
    int pos;
  };
  auto initial_state() const -> next_state { return {grouping_.begin(), 0}; }

  // Returns the next digit group separator position.
  auto next(next_state& state) const -> int {
    if (thousands_sep_.empty()) return max_value<int>();
    if (state.group == grouping_.end()) return state.pos += grouping_.back();
    if (*state.group <= 0 || *state.group == max_value<char>())
      return max_value<int>();
    state.pos += *state.group++;
    return state.pos;
  }

 public:
  template <typename Locale,
            FMT_ENABLE_IF(std::is_same<Locale, locale_ref>::value)>
  explicit digit_grouping(Locale loc, bool localized = true) {
    if (!localized) return;
    auto sep = thousands_sep<Char>(loc);
    grouping_ = sep.grouping;
    if (sep.thousands_sep) thousands_sep_.assign(1, sep.thousands_sep);
  }
  digit_grouping(std::string grouping, std::basic_string<Char> sep)
      : grouping_(std::move(grouping)), thousands_sep_(std::move(sep)) {}

  auto has_separator() const -> bool { return !thousands_sep_.empty(); }

  auto count_separators(int num_digits) const -> int {
    int count = 0;
    auto state = initial_state();
    while (num_digits > next(state)) ++count;
    return count;
  }

  // Applies grouping to digits and write the output to out.
  template <typename Out, typename C>
  auto apply(Out out, basic_string_view<C> digits) const -> Out {
    auto num_digits = static_cast<int>(digits.size());
    auto separators = basic_memory_buffer<int>();
    separators.push_back(0);
    auto state = initial_state();
    while (int i = next(state)) {
      if (i >= num_digits) break;
      separators.push_back(i);
    }
    for (int i = 0, sep_index = static_cast<int>(separators.size() - 1);
         i < num_digits; ++i) {
      if (num_digits - i == separators[sep_index]) {
        out = copy<Char>(thousands_sep_.data(),
                         thousands_sep_.data() + thousands_sep_.size(), out);
        --sep_index;
      }
      *out++ = static_cast<Char>(digits[to_unsigned(i)]);
    }
    return out;
  }
};

FMT_CONSTEXPR inline void prefix_append(unsigned& prefix, unsigned value) {
  prefix |= prefix != 0 ? value << 8 : value;
  prefix += (1u + (value > 0xff ? 1 : 0)) << 24;
}

// Writes a decimal integer with digit grouping.
template <typename OutputIt, typename UInt, typename Char>
auto write_int(OutputIt out, UInt value, unsigned prefix,
               const format_specs& specs, const digit_grouping<Char>& grouping)
    -> OutputIt {
  static_assert(std::is_same<uint64_or_128_t<UInt>, UInt>::value, "");
  int num_digits = 0;
  auto buffer = memory_buffer();
  switch (specs.type()) {
  default: FMT_ASSERT(false, ""); FMT_FALLTHROUGH;
  case presentation_type::none:
  case presentation_type::dec:
    num_digits = count_digits(value);
    format_decimal<char>(appender(buffer), value, num_digits);
    break;
  case presentation_type::hex:
    if (specs.alt())
      prefix_append(prefix, unsigned(specs.upper() ? 'X' : 'x') << 8 | '0');
    num_digits = count_digits<4>(value);
    format_base2e<char>(4, appender(buffer), value, num_digits, specs.upper());
    break;
  case presentation_type::oct:
    num_digits = count_digits<3>(value);
    // Octal prefix '0' is counted as a digit, so only add it if precision
    // is not greater than the number of digits.
    if (specs.alt() && specs.precision <= num_digits && value != 0)
      prefix_append(prefix, '0');
    format_base2e<char>(3, appender(buffer), value, num_digits);
    break;
  case presentation_type::bin:
    if (specs.alt())
      prefix_append(prefix, unsigned(specs.upper() ? 'B' : 'b') << 8 | '0');
    num_digits = count_digits<1>(value);
    format_base2e<char>(1, appender(buffer), value, num_digits);
    break;
  case presentation_type::chr:
    return write_char<Char>(out, static_cast<Char>(value), specs);
  }

  unsigned size = (prefix != 0 ? prefix >> 24 : 0) + to_unsigned(num_digits) +
                  to_unsigned(grouping.count_separators(num_digits));
  return write_padded<Char, align::right>(
      out, specs, size, size, [&](reserve_iterator<OutputIt> it) {
        for (unsigned p = prefix & 0xffffff; p != 0; p >>= 8)
          *it++ = static_cast<Char>(p & 0xff);
        return grouping.apply(it, string_view(buffer.data(), buffer.size()));
      });
}

#if FMT_USE_LOCALE
// Writes a localized value.
FMT_API auto write_loc(appender out, loc_value value, const format_specs& specs,
                       locale_ref loc) -> bool;
#endif
template <typename OutputIt>
inline auto write_loc(OutputIt, const loc_value&, const format_specs&,
                      locale_ref) -> bool {
  return false;
}

template <typename UInt> struct write_int_arg {
  UInt abs_value;
  unsigned prefix;
};

template <typename T>
FMT_CONSTEXPR auto make_write_int_arg(T value, sign s)
    -> write_int_arg<uint32_or_64_or_128_t<T>> {
  auto prefix = 0u;
  auto abs_value = static_cast<uint32_or_64_or_128_t<T>>(value);
  if (is_negative(value)) {
    prefix = 0x01000000 | '-';
    abs_value = 0 - abs_value;
  } else {
    constexpr unsigned prefixes[4] = {0, 0, 0x1000000u | '+', 0x1000000u | ' '};
    prefix = prefixes[static_cast<int>(s)];
  }
  return {abs_value, prefix};
}

template <typename Char = char> struct loc_writer {
  basic_appender<Char> out;
  const format_specs& specs;
  std::basic_string<Char> sep;
  std::string grouping;
  std::basic_string<Char> decimal_point;

  template <typename T, FMT_ENABLE_IF(is_integer<T>::value)>
  auto operator()(T value) -> bool {
    auto arg = make_write_int_arg(value, specs.sign());
    write_int(out, static_cast<uint64_or_128_t<T>>(arg.abs_value), arg.prefix,
              specs, digit_grouping<Char>(grouping, sep));
    return true;
  }

  template <typename T, FMT_ENABLE_IF(!is_integer<T>::value)>
  auto operator()(T) -> bool {
    return false;
  }
};

// Size and padding computation separate from write_int to avoid template bloat.
struct size_padding {
  unsigned size;
  unsigned padding;

  FMT_CONSTEXPR size_padding(int num_digits, unsigned prefix,
                             const format_specs& specs)
      : size((prefix >> 24) + to_unsigned(num_digits)), padding(0) {
    if (specs.align() == align::numeric) {
      auto width = to_unsigned(specs.width);
      if (width > size) {
        padding = width - size;
        size = width;
      }
    } else if (specs.precision > num_digits) {
      size = (prefix >> 24) + to_unsigned(specs.precision);
      padding = to_unsigned(specs.precision - num_digits);
    }
  }
};

template <typename Char, typename OutputIt, typename T>
FMT_CONSTEXPR FMT_INLINE auto write_int(OutputIt out, write_int_arg<T> arg,
                                        const format_specs& specs) -> OutputIt {
  static_assert(std::is_same<T, uint32_or_64_or_128_t<T>>::value, "");

  constexpr size_t buffer_size = num_bits<T>();
  char buffer[buffer_size];
  if (is_constant_evaluated()) fill_n(buffer, buffer_size, '\0');
  const char* begin = nullptr;
  const char* end = buffer + buffer_size;

  auto abs_value = arg.abs_value;
  auto prefix = arg.prefix;
  switch (specs.type()) {
  default: FMT_ASSERT(false, ""); FMT_FALLTHROUGH;
  case presentation_type::none:
  case presentation_type::dec:
    begin = do_format_decimal(buffer, abs_value, buffer_size);
    break;
  case presentation_type::hex:
    begin = do_format_base2e(4, buffer, abs_value, buffer_size, specs.upper());
    if (specs.alt())
      prefix_append(prefix, unsigned(specs.upper() ? 'X' : 'x') << 8 | '0');
    break;
  case presentation_type::oct: {
    begin = do_format_base2e(3, buffer, abs_value, buffer_size);
    // Octal prefix '0' is counted as a digit, so only add it if precision
    // is not greater than the number of digits.
    auto num_digits = end - begin;
    if (specs.alt() && specs.precision <= num_digits && abs_value != 0)
      prefix_append(prefix, '0');
    break;
  }
  case presentation_type::bin:
    begin = do_format_base2e(1, buffer, abs_value, buffer_size);
    if (specs.alt())
      prefix_append(prefix, unsigned(specs.upper() ? 'B' : 'b') << 8 | '0');
    break;
  case presentation_type::chr:
    return write_char<Char>(out, static_cast<Char>(abs_value), specs);
  }

  // Write an integer in the format
  //   <left-padding><prefix><numeric-padding><digits><right-padding>
  // prefix contains chars in three lower bytes and the size in the fourth byte.
  int num_digits = static_cast<int>(end - begin);
  // Slightly faster check for specs.width == 0 && specs.precision == -1.
  if ((specs.width | (specs.precision + 1)) == 0) {
    auto it = reserve(out, to_unsigned(num_digits) + (prefix >> 24));
    for (unsigned p = prefix & 0xffffff; p != 0; p >>= 8)
      *it++ = static_cast<Char>(p & 0xff);
    return base_iterator(out, copy<Char>(begin, end, it));
  }
  auto sp = size_padding(num_digits, prefix, specs);
  unsigned padding = sp.padding;
  return write_padded<Char, align::right>(
      out, specs, sp.size, [=](reserve_iterator<OutputIt> it) {
        for (unsigned p = prefix & 0xffffff; p != 0; p >>= 8)
          *it++ = static_cast<Char>(p & 0xff);
        it = detail::fill_n(it, padding, static_cast<Char>('0'));
        return copy<Char>(begin, end, it);
      });
}

template <typename Char, typename OutputIt, typename T>
FMT_CONSTEXPR FMT_NOINLINE auto write_int_noinline(OutputIt out,
                                                   write_int_arg<T> arg,
                                                   const format_specs& specs)
    -> OutputIt {
  return write_int<Char>(out, arg, specs);
}

template <typename Char, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<T, Char>::value)>
FMT_CONSTEXPR FMT_INLINE auto write(basic_appender<Char> out, T value,
                                    const format_specs& specs, locale_ref loc)
    -> basic_appender<Char> {
  if (specs.localized() && write_loc(out, value, specs, loc)) return out;
  return write_int_noinline<Char>(out, make_write_int_arg(value, specs.sign()),
                                  specs);
}

// An inlined version of write used in format string compilation.
template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<T, Char>::value &&
                        !std::is_same<OutputIt, basic_appender<Char>>::value)>
FMT_CONSTEXPR FMT_INLINE auto write(OutputIt out, T value,
                                    const format_specs& specs, locale_ref loc)
    -> OutputIt {
  if (specs.localized() && write_loc(out, value, specs, loc)) return out;
  return write_int<Char>(out, make_write_int_arg(value, specs.sign()), specs);
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(std::is_same<Char, char>::value)>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> s,
                         const format_specs& specs) -> OutputIt {
  bool is_debug = specs.type() == presentation_type::debug;
  if (specs.precision < 0 && specs.width == 0) {
    auto&& it = reserve(out, s.size());
    return is_debug ? write_escaped_string(it, s) : copy<char>(s, it);
  }

  size_t display_width_limit =
      specs.precision < 0 ? SIZE_MAX : to_unsigned(specs.precision);
  size_t display_width =
      !is_debug || specs.precision == 0 ? 0 : 1;  // Account for opening '"'.
  size_t size = !is_debug || specs.precision == 0 ? 0 : 1;
  for_each_codepoint(s, [&](uint32_t cp, string_view sv) {
    if (is_debug && needs_escape(cp)) {
      counting_buffer<char> buf;
      write_escaped_cp(basic_appender<char>(buf),
                       find_escape_result<char>{sv.begin(), sv.end(), cp});
      // We're reinterpreting bytes as display width. That's okay
      // because write_escaped_cp() only writes ASCII characters.
      size_t cp_width = buf.count();
      if (display_width + cp_width <= display_width_limit) {
        display_width += cp_width;
        size += cp_width;
        // If this is the end of the string, account for closing '"'.
        if (display_width < display_width_limit && sv.end() == s.end()) {
          ++display_width;
          ++size;
        }
        return true;
      }

      size += display_width_limit - display_width;
      display_width = display_width_limit;
      return false;
    }

    size_t cp_width = display_width_of(cp);
    if (cp_width + display_width <= display_width_limit) {
      display_width += cp_width;
      size += sv.size();
      // If this is the end of the string, account for closing '"'.
      if (is_debug && display_width < display_width_limit &&
          sv.end() == s.end()) {
        ++display_width;
        ++size;
      }
      return true;
    }

    return false;
  });

  struct bounded_output_iterator {
    reserve_iterator<OutputIt> underlying_iterator;
    size_t bound;

    FMT_CONSTEXPR auto operator*() -> bounded_output_iterator& { return *this; }
    FMT_CONSTEXPR auto operator++() -> bounded_output_iterator& {
      return *this;
    }
    FMT_CONSTEXPR auto operator++(int) -> bounded_output_iterator& {
      return *this;
    }
    FMT_CONSTEXPR auto operator=(char c) -> bounded_output_iterator& {
      if (bound > 0) {
        *underlying_iterator++ = c;
        --bound;
      }
      return *this;
    }
  };

  return write_padded<char>(
      out, specs, size, display_width, [=](reserve_iterator<OutputIt> it) {
        return is_debug
                   ? write_escaped_string(bounded_output_iterator{it, size}, s)
                         .underlying_iterator
                   : copy<char>(s.data(), s.data() + size, it);
      });
}

template <typename Char, typename OutputIt,
          FMT_ENABLE_IF(!std::is_same<Char, char>::value)>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> s,
                         const format_specs& specs) -> OutputIt {
  auto data = s.data();
  auto size = s.size();
  if (specs.precision >= 0 && to_unsigned(specs.precision) < size)
    size = to_unsigned(specs.precision);

  bool is_debug = specs.type() == presentation_type::debug;
  if (is_debug) {
    auto buf = counting_buffer<Char>();
    write_escaped_string(basic_appender<Char>(buf), s);
    size = buf.count();
  }

  return write_padded<Char>(
      out, specs, size, [=](reserve_iterator<OutputIt> it) {
        return is_debug ? write_escaped_string(it, s)
                        : copy<Char>(data, data + size, it);
      });
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> s,
                         const format_specs& specs, locale_ref) -> OutputIt {
  return write<Char>(out, s, specs);
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, const Char* s, const format_specs& specs,
                         locale_ref) -> OutputIt {
  if (specs.type() == presentation_type::pointer)
    return write_ptr<Char>(out, bit_cast<uintptr_t>(s), &specs);
  if (!s) report_error("string pointer is null");
  return write<Char>(out, basic_string_view<Char>(s), specs, {});
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_integral<T>::value &&
                        !std::is_same<T, bool>::value &&
                        !std::is_same<T, Char>::value)>
FMT_CONSTEXPR auto write(OutputIt out, T value) -> OutputIt {
  auto abs_value = static_cast<uint32_or_64_or_128_t<T>>(value);
  bool negative = is_negative(value);
  // Don't do -abs_value since it trips unsigned-integer-overflow sanitizer.
  if (negative) abs_value = ~abs_value + 1;
  int num_digits = count_digits(abs_value);
  auto size = (negative ? 1 : 0) + static_cast<size_t>(num_digits);
  if (auto ptr = to_pointer<Char>(out, size)) {
    if (negative) *ptr++ = static_cast<Char>('-');
    format_decimal<Char>(ptr, abs_value, num_digits);
    return out;
  }
  if (negative) *out++ = static_cast<Char>('-');
  return format_decimal<Char>(out, abs_value, num_digits);
}

template <typename Char>
FMT_CONSTEXPR auto parse_align(const Char* begin, const Char* end,
                               format_specs& specs) -> const Char* {
  FMT_ASSERT(begin != end, "");
  auto alignment = align::none;
  auto p = begin + code_point_length(begin);
  if (end - p <= 0) p = begin;
  for (;;) {
    switch (to_ascii(*p)) {
    case '<': alignment = align::left; break;
    case '>': alignment = align::right; break;
    case '^': alignment = align::center; break;
    }
    if (alignment != align::none) {
      if (p != begin) {
        auto c = *begin;
        if (c == '}') return begin;
        if (c == '{') {
          report_error("invalid fill character '{'");
          return begin;
        }
        specs.set_fill(basic_string_view<Char>(begin, to_unsigned(p - begin)));
        begin = p + 1;
      } else {
        ++begin;
      }
      break;
    } else if (p == begin) {
      break;
    }
    p = begin;
  }
  specs.set_align(alignment);
  return begin;
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR20 auto write_nonfinite(OutputIt out, bool isnan,
                                     format_specs specs, sign s) -> OutputIt {
  auto str =
      isnan ? (specs.upper() ? "NAN" : "nan") : (specs.upper() ? "INF" : "inf");
  constexpr size_t str_size = 3;
  auto size = str_size + (s != sign::none ? 1 : 0);
  // Replace '0'-padding with space for non-finite values.
  const bool is_zero_fill =
      specs.fill_size() == 1 && specs.fill_unit<Char>() == '0';
  if (is_zero_fill) specs.set_fill(' ');
  return write_padded<Char>(out, specs, size,
                            [=](reserve_iterator<OutputIt> it) {
                              if (s != sign::none)
                                *it++ = detail::getsign<Char>(s);
                              return copy<Char>(str, str + str_size, it);
                            });
}

// A decimal floating-point number significand * pow(10, exp).
struct big_decimal_fp {
  const char* significand;
  int significand_size;
  int exponent;
};

constexpr auto get_significand_size(const big_decimal_fp& f) -> int {
  return f.significand_size;
}
template <typename T>
inline auto get_significand_size(const dragonbox::decimal_fp<T>& f) -> int {
  return count_digits(f.significand);
}

template <typename Char, typename OutputIt>
constexpr auto write_significand(OutputIt out, const char* significand,
                                 int significand_size) -> OutputIt {
  return copy<Char>(significand, significand + significand_size, out);
}
template <typename Char, typename OutputIt, typename UInt>
inline auto write_significand(OutputIt out, UInt significand,
                              int significand_size) -> OutputIt {
  return format_decimal<Char>(out, significand, significand_size);
}
template <typename Char, typename OutputIt, typename T, typename Grouping>
FMT_CONSTEXPR20 auto write_significand(OutputIt out, T significand,
                                       int significand_size, int exponent,
                                       const Grouping& grouping) -> OutputIt {
  if (!grouping.has_separator()) {
    out = write_significand<Char>(out, significand, significand_size);
    return detail::fill_n(out, exponent, static_cast<Char>('0'));
  }
  auto buffer = memory_buffer();
  write_significand<char>(appender(buffer), significand, significand_size);
  detail::fill_n(appender(buffer), exponent, '0');
  return grouping.apply(out, string_view(buffer.data(), buffer.size()));
}

template <typename Char, typename UInt,
          FMT_ENABLE_IF(std::is_integral<UInt>::value)>
inline auto write_significand(Char* out, UInt significand, int significand_size,
                              int integral_size, Char decimal_point) -> Char* {
  if (!decimal_point) return format_decimal(out, significand, significand_size);
  out += significand_size + 1;
  Char* end = out;
  int floating_size = significand_size - integral_size;
  for (int i = floating_size / 2; i > 0; --i) {
    out -= 2;
    write2digits(out, static_cast<size_t>(significand % 100));
    significand /= 100;
  }
  if (floating_size % 2 != 0) {
    *--out = static_cast<Char>('0' + significand % 10);
    significand /= 10;
  }
  *--out = decimal_point;
  format_decimal(out - integral_size, significand, integral_size);
  return end;
}

template <typename OutputIt, typename UInt, typename Char,
          FMT_ENABLE_IF(!std::is_pointer<remove_cvref_t<OutputIt>>::value)>
inline auto write_significand(OutputIt out, UInt significand,
                              int significand_size, int integral_size,
                              Char decimal_point) -> OutputIt {
  // Buffer is large enough to hold digits (digits10 + 1) and a decimal point.
  Char buffer[digits10<UInt>() + 2];
  auto end = write_significand(buffer, significand, significand_size,
                               integral_size, decimal_point);
  return detail::copy_noinline<Char>(buffer, end, out);
}

template <typename OutputIt, typename Char>
FMT_CONSTEXPR auto write_significand(OutputIt out, const char* significand,
                                     int significand_size, int integral_size,
                                     Char decimal_point) -> OutputIt {
  out = detail::copy_noinline<Char>(significand, significand + integral_size,
                                    out);
  if (!decimal_point) return out;
  *out++ = decimal_point;
  return detail::copy_noinline<Char>(significand + integral_size,
                                     significand + significand_size, out);
}

template <typename OutputIt, typename Char, typename T, typename Grouping>
FMT_CONSTEXPR20 auto write_significand(OutputIt out, T significand,
                                       int significand_size, int integral_size,
                                       Char decimal_point,
                                       const Grouping& grouping) -> OutputIt {
  if (!grouping.has_separator()) {
    return write_significand(out, significand, significand_size, integral_size,
                             decimal_point);
  }
  auto buffer = basic_memory_buffer<Char>();
  write_significand(basic_appender<Char>(buffer), significand, significand_size,
                    integral_size, decimal_point);
  grouping.apply(
      out, basic_string_view<Char>(buffer.data(), to_unsigned(integral_size)));
  return detail::copy_noinline<Char>(buffer.data() + integral_size,
                                     buffer.end(), out);
}

// Numbers with exponents greater or equal to the returned value will use
// the exponential notation.
template <typename T> FMT_CONSTEVAL auto exp_upper() -> int {
  return std::numeric_limits<T>::digits10 != 0
             ? min_of(16, std::numeric_limits<T>::digits10 + 1)
             : 16;
}

// Use the fixed notation if the exponent is in [-4, exp_upper),
// e.g. 0.0001 instead of 1e-04. Otherwise use the exponent notation.
constexpr auto use_fixed(int exp, int exp_upper) -> bool {
  return exp >= -4 && exp < exp_upper;
}

template <typename Char> class fallback_digit_grouping {
 public:
  constexpr fallback_digit_grouping(locale_ref, bool) {}

  constexpr auto has_separator() const -> bool { return false; }

  constexpr auto count_separators(int) const -> int { return 0; }

  template <typename Out, typename C>
  constexpr auto apply(Out out, basic_string_view<C>) const -> Out {
    return out;
  }
};

template <typename Char, typename Grouping, typename OutputIt,
          typename DecimalFP>
FMT_CONSTEXPR20 auto write_fixed(OutputIt out, const DecimalFP& f,
                                 int significand_size, Char decimal_point,
                                 const format_specs& specs, sign s,
                                 locale_ref loc = {}) -> OutputIt {
  using iterator = reserve_iterator<OutputIt>;

  int exp = f.exponent + significand_size;
  long long size = significand_size + (s != sign::none ? 1 : 0);
  if (f.exponent >= 0) {
    // 1234e5 -> 123400000[.0+]
    size += f.exponent;
    int num_zeros = specs.precision - exp;
    abort_fuzzing_if(num_zeros > 5000);
    if (specs.alt()) {
      ++size;
      if (num_zeros <= 0 && specs.type() != presentation_type::fixed)
        num_zeros = 0;
      if (num_zeros > 0) size += num_zeros;
    }
    auto grouping = Grouping(loc, specs.localized());
    size += grouping.count_separators(exp);
    return write_padded<Char, align::right>(
        out, specs, to_unsigned(size), [&](iterator it) {
          if (s != sign::none) *it++ = detail::getsign<Char>(s);
          it = write_significand<Char>(it, f.significand, significand_size,
                                       f.exponent, grouping);
          if (!specs.alt()) return it;
          *it++ = decimal_point;
          return num_zeros > 0 ? detail::fill_n(it, num_zeros, Char('0')) : it;
        });
  }
  if (exp > 0) {
    // 1234e-2 -> 12.34[0+]
    int num_zeros = specs.alt() ? specs.precision - significand_size : 0;
    size += 1 + max_of(num_zeros, 0);
    auto grouping = Grouping(loc, specs.localized());
    size += grouping.count_separators(exp);
    return write_padded<Char, align::right>(
        out, specs, to_unsigned(size), [&](iterator it) {
          if (s != sign::none) *it++ = detail::getsign<Char>(s);
          it = write_significand(it, f.significand, significand_size, exp,
                                 decimal_point, grouping);
          return num_zeros > 0 ? detail::fill_n(it, num_zeros, Char('0')) : it;
        });
  }
  // 1234e-6 -> 0.001234
  int num_zeros = -exp;
  if (significand_size == 0 && specs.precision >= 0 &&
      specs.precision < num_zeros) {
    num_zeros = specs.precision;
  }
  bool pointy = num_zeros != 0 || significand_size != 0 || specs.alt();
  size += 1 + (pointy ? 1 : 0) + num_zeros;
  return write_padded<Char, align::right>(
      out, specs, to_unsigned(size), [&](iterator it) {
        if (s != sign::none) *it++ = detail::getsign<Char>(s);
        *it++ = Char('0');
        if (!pointy) return it;
        *it++ = decimal_point;
        it = detail::fill_n(it, num_zeros, Char('0'));
        return write_significand<Char>(it, f.significand, significand_size);
      });
}

template <typename Char, typename Grouping, typename OutputIt,
          typename DecimalFP>
FMT_CONSTEXPR20 auto do_write_float(OutputIt out, const DecimalFP& f,
                                    const format_specs& specs, sign s,
                                    int exp_upper, locale_ref loc) -> OutputIt {
  Char point = specs.localized() ? detail::decimal_point<Char>(loc) : Char('.');
  int significand_size = get_significand_size(f);
  int exp = f.exponent + significand_size - 1;
  if (specs.type() == presentation_type::fixed ||
      (specs.type() != presentation_type::exp &&
       use_fixed(exp, specs.precision > 0 ? specs.precision : exp_upper))) {
    return write_fixed<Char, Grouping>(out, f, significand_size, point, specs,
                                       s, loc);
  }

  // Write value in the exponential format.
  int num_zeros = 0;
  long long size = significand_size + (s != sign::none ? 1 : 0);
  if (specs.alt()) {
    num_zeros = max_of(specs.precision - significand_size, 0);
    size += num_zeros;
  } else if (significand_size == 1) {
    point = Char();
  }
  size += (point ? 1 : 0) + compute_exp_size(exp);
  char exp_char = specs.upper() ? 'E' : 'e';
  auto write = [=](reserve_iterator<OutputIt> it) {
    if (s != sign::none) *it++ = detail::getsign<Char>(s);
    // Insert a decimal point after the first digit and add an exponent.
    it = write_significand(it, f.significand, significand_size, 1, point);
    if (num_zeros > 0) it = detail::fill_n(it, num_zeros, Char('0'));
    *it++ = Char(exp_char);
    return write_exponent<Char>(exp, it);
  };
  auto usize = to_unsigned(size);
  return specs.width > 0
             ? write_padded<Char, align::right>(out, specs, usize, write)
             : base_iterator(out, write(reserve(out, usize)));
}

template <typename Char, typename OutputIt, typename DecimalFP>
FMT_CONSTEXPR20 auto write_float(OutputIt out, const DecimalFP& f,
                                 const format_specs& specs, sign s,
                                 int exp_upper, locale_ref loc) -> OutputIt {
  if (is_constant_evaluated()) {
    return do_write_float<Char, fallback_digit_grouping<Char>>(out, f, specs, s,
                                                               exp_upper, loc);
  } else {
    return do_write_float<Char, digit_grouping<Char>>(out, f, specs, s,
                                                      exp_upper, loc);
  }
}

template <typename T> constexpr auto isnan(T value) -> bool {
  return value != value;  // std::isnan doesn't support __float128.
}

template <typename T, typename Enable = void>
struct has_isfinite : std::false_type {};

template <typename T>
struct has_isfinite<T, enable_if_t<sizeof(std::isfinite(T())) != 0>>
    : std::true_type {};

template <typename T,
          FMT_ENABLE_IF(is_floating_point<T>::value&& has_isfinite<T>::value)>
FMT_CONSTEXPR20 auto isfinite(T value) -> bool {
  constexpr T inf = T(std::numeric_limits<double>::infinity());
  if (is_constant_evaluated())
    return !detail::isnan(value) && value < inf && value > -inf;
  return std::isfinite(value);
}
template <typename T, FMT_ENABLE_IF(!has_isfinite<T>::value)>
FMT_CONSTEXPR auto isfinite(T value) -> bool {
  T inf = T(std::numeric_limits<double>::infinity());
  // std::isfinite doesn't support __float128.
  return !detail::isnan(value) && value < inf && value > -inf;
}

template <typename T, FMT_ENABLE_IF(is_floating_point<T>::value)>
FMT_INLINE FMT_CONSTEXPR bool signbit(T value) {
  if (is_constant_evaluated()) {
#ifdef __cpp_if_constexpr
    if constexpr (std::numeric_limits<double>::is_iec559) {
      auto bits = detail::bit_cast<uint64_t>(static_cast<double>(value));
      return (bits >> (num_bits<uint64_t>() - 1)) != 0;
    }
#endif
  }
  return std::signbit(static_cast<double>(value));
}

inline FMT_CONSTEXPR20 void adjust_precision(int& precision, int exp10) {
  // Adjust fixed precision by exponent because it is relative to decimal
  // point.
  if (exp10 > 0 && precision > max_value<int>() - exp10)
    FMT_THROW(format_error("number is too big"));
  precision += exp10;
}

class bigint {
 private:
  // A bigint is a number in the form bigit_[N - 1] ... bigit_[0] * 32^exp_.
  using bigit = uint32_t;  // A big digit.
  using double_bigit = uint64_t;
  enum { bigit_bits = num_bits<bigit>() };
  enum { bigits_capacity = 32 };
  basic_memory_buffer<bigit, bigits_capacity> bigits_;
  int exp_;

  friend struct formatter<bigint>;

  FMT_CONSTEXPR auto get_bigit(int i) const -> bigit {
    return i >= exp_ && i < num_bigits() ? bigits_[i - exp_] : 0;
  }

  FMT_CONSTEXPR void subtract_bigits(int index, bigit other, bigit& borrow) {
    auto result = double_bigit(bigits_[index]) - other - borrow;
    bigits_[index] = static_cast<bigit>(result);
    borrow = static_cast<bigit>(result >> (bigit_bits * 2 - 1));
  }

  FMT_CONSTEXPR void remove_leading_zeros() {
    int num_bigits = static_cast<int>(bigits_.size()) - 1;
    while (num_bigits > 0 && bigits_[num_bigits] == 0) --num_bigits;
    bigits_.resize(to_unsigned(num_bigits + 1));
  }

  // Computes *this -= other assuming aligned bigints and *this >= other.
  FMT_CONSTEXPR void subtract_aligned(const bigint& other) {
    FMT_ASSERT(other.exp_ >= exp_, "unaligned bigints");
    FMT_ASSERT(compare(*this, other) >= 0, "");
    bigit borrow = 0;
    int i = other.exp_ - exp_;
    for (size_t j = 0, n = other.bigits_.size(); j != n; ++i, ++j)
      subtract_bigits(i, other.bigits_[j], borrow);
    if (borrow != 0) subtract_bigits(i, 0, borrow);
    FMT_ASSERT(borrow == 0, "");
    remove_leading_zeros();
  }

  FMT_CONSTEXPR void multiply(uint32_t value) {
    bigit carry = 0;
    const double_bigit wide_value = value;
    for (size_t i = 0, n = bigits_.size(); i < n; ++i) {
      double_bigit result = bigits_[i] * wide_value + carry;
      bigits_[i] = static_cast<bigit>(result);
      carry = static_cast<bigit>(result >> bigit_bits);
    }
    if (carry != 0) bigits_.push_back(carry);
  }

  template <typename UInt, FMT_ENABLE_IF(std::is_same<UInt, uint64_t>::value ||
                                         std::is_same<UInt, uint128_t>::value)>
  FMT_CONSTEXPR void multiply(UInt value) {
    using half_uint =
        conditional_t<std::is_same<UInt, uint128_t>::value, uint64_t, uint32_t>;
    const int shift = num_bits<half_uint>() - bigit_bits;
    const UInt lower = static_cast<half_uint>(value);
    const UInt upper = value >> num_bits<half_uint>();
    UInt carry = 0;
    for (size_t i = 0, n = bigits_.size(); i < n; ++i) {
      UInt result = lower * bigits_[i] + static_cast<bigit>(carry);
      carry = (upper * bigits_[i] << shift) + (result >> bigit_bits) +
              (carry >> bigit_bits);
      bigits_[i] = static_cast<bigit>(result);
    }
    while (carry != 0) {
      bigits_.push_back(static_cast<bigit>(carry));
      carry >>= bigit_bits;
    }
  }

  template <typename UInt, FMT_ENABLE_IF(std::is_same<UInt, uint64_t>::value ||
                                         std::is_same<UInt, uint128_t>::value)>
  FMT_CONSTEXPR void assign(UInt n) {
    size_t num_bigits = 0;
    do {
      bigits_[num_bigits++] = static_cast<bigit>(n);
      n >>= bigit_bits;
    } while (n != 0);
    bigits_.resize(num_bigits);
    exp_ = 0;
  }

 public:
  FMT_CONSTEXPR bigint() : exp_(0) {}
  explicit bigint(uint64_t n) { assign(n); }

  bigint(const bigint&) = delete;
  void operator=(const bigint&) = delete;

  FMT_CONSTEXPR void assign(const bigint& other) {
    auto size = other.bigits_.size();
    bigits_.resize(size);
    auto data = other.bigits_.data();
    copy<bigit>(data, data + size, bigits_.data());
    exp_ = other.exp_;
  }

  template <typename Int> FMT_CONSTEXPR void operator=(Int n) {
    FMT_ASSERT(n > 0, "");
    assign(uint64_or_128_t<Int>(n));
  }

  FMT_CONSTEXPR auto num_bigits() const -> int {
    return static_cast<int>(bigits_.size()) + exp_;
  }

  FMT_CONSTEXPR auto operator<<=(int shift) -> bigint& {
    FMT_ASSERT(shift >= 0, "");
    exp_ += shift / bigit_bits;
    shift %= bigit_bits;
    if (shift == 0) return *this;
    bigit carry = 0;
    for (size_t i = 0, n = bigits_.size(); i < n; ++i) {
      bigit c = bigits_[i] >> (bigit_bits - shift);
      bigits_[i] = (bigits_[i] << shift) + carry;
      carry = c;
    }
    if (carry != 0) bigits_.push_back(carry);
    return *this;
  }

  template <typename Int> FMT_CONSTEXPR auto operator*=(Int value) -> bigint& {
    FMT_ASSERT(value > 0, "");
    multiply(uint32_or_64_or_128_t<Int>(value));
    return *this;
  }

  friend FMT_CONSTEXPR auto compare(const bigint& b1, const bigint& b2) -> int {
    int num_bigits1 = b1.num_bigits(), num_bigits2 = b2.num_bigits();
    if (num_bigits1 != num_bigits2) return num_bigits1 > num_bigits2 ? 1 : -1;
    int i = static_cast<int>(b1.bigits_.size()) - 1;
    int j = static_cast<int>(b2.bigits_.size()) - 1;
    int end = i - j;
    if (end < 0) end = 0;
    for (; i >= end; --i, --j) {
      bigit b1_bigit = b1.bigits_[i], b2_bigit = b2.bigits_[j];
      if (b1_bigit != b2_bigit) return b1_bigit > b2_bigit ? 1 : -1;
    }
    if (i != j) return i > j ? 1 : -1;
    return 0;
  }

  // Returns compare(lhs1 + lhs2, rhs).
  friend FMT_CONSTEXPR auto add_compare(const bigint& lhs1, const bigint& lhs2,
                                        const bigint& rhs) -> int {
    int max_lhs_bigits = max_of(lhs1.num_bigits(), lhs2.num_bigits());
    int num_rhs_bigits = rhs.num_bigits();
    if (max_lhs_bigits + 1 < num_rhs_bigits) return -1;
    if (max_lhs_bigits > num_rhs_bigits) return 1;
    double_bigit borrow = 0;
    int min_exp = min_of(min_of(lhs1.exp_, lhs2.exp_), rhs.exp_);
    for (int i = num_rhs_bigits - 1; i >= min_exp; --i) {
      double_bigit sum = double_bigit(lhs1.get_bigit(i)) + lhs2.get_bigit(i);
      bigit rhs_bigit = rhs.get_bigit(i);
      if (sum > rhs_bigit + borrow) return 1;
      borrow = rhs_bigit + borrow - sum;
      if (borrow > 1) return -1;
      borrow <<= bigit_bits;
    }
    return borrow != 0 ? -1 : 0;
  }

  // Assigns pow(10, exp) to this bigint.
  FMT_CONSTEXPR20 void assign_pow10(int exp) {
    FMT_ASSERT(exp >= 0, "");
    if (exp == 0) return *this = 1;
    int bitmask = 1 << (num_bits<unsigned>() -
                        countl_zero(static_cast<uint32_t>(exp)) - 1);
    // pow(10, exp) = pow(5, exp) * pow(2, exp). First compute pow(5, exp) by
    // repeated squaring and multiplication.
    *this = 5;
    bitmask >>= 1;
    while (bitmask != 0) {
      square();
      if ((exp & bitmask) != 0) *this *= 5;
      bitmask >>= 1;
    }
    *this <<= exp;  // Multiply by pow(2, exp) by shifting.
  }

  FMT_CONSTEXPR20 void square() {
    int num_bigits = static_cast<int>(bigits_.size());
    int num_result_bigits = 2 * num_bigits;
    basic_memory_buffer<bigit, bigits_capacity> n(std::move(bigits_));
    bigits_.resize(to_unsigned(num_result_bigits));
    auto sum = uint128_t();
    for (int bigit_index = 0; bigit_index < num_bigits; ++bigit_index) {
      // Compute bigit at position bigit_index of the result by adding
      // cross-product terms n[i] * n[j] such that i + j == bigit_index.
      for (int i = 0, j = bigit_index; j >= 0; ++i, --j) {
        // Most terms are multiplied twice which can be optimized in the future.
        sum += double_bigit(n[i]) * n[j];
      }
      bigits_[bigit_index] = static_cast<bigit>(sum);
      sum >>= num_bits<bigit>();  // Compute the carry.
    }
    // Do the same for the top half.
    for (int bigit_index = num_bigits; bigit_index < num_result_bigits;
         ++bigit_index) {
      for (int j = num_bigits - 1, i = bigit_index - j; i < num_bigits;)
        sum += double_bigit(n[i++]) * n[j--];
      bigits_[bigit_index] = static_cast<bigit>(sum);
      sum >>= num_bits<bigit>();
    }
    remove_leading_zeros();
    exp_ *= 2;
  }

  // If this bigint has a bigger exponent than other, adds trailing zero to make
  // exponents equal. This simplifies some operations such as subtraction.
  FMT_CONSTEXPR void align(const bigint& other) {
    int exp_difference = exp_ - other.exp_;
    if (exp_difference <= 0) return;
    int num_bigits = static_cast<int>(bigits_.size());
    bigits_.resize(to_unsigned(num_bigits + exp_difference));
    for (int i = num_bigits - 1, j = i + exp_difference; i >= 0; --i, --j)
      bigits_[j] = bigits_[i];
    fill_n(bigits_.data(), to_unsigned(exp_difference), 0U);
    exp_ -= exp_difference;
  }

  // Divides this bignum by divisor, assigning the remainder to this and
  // returning the quotient.
  FMT_CONSTEXPR auto divmod_assign(const bigint& divisor) -> int {
    FMT_ASSERT(this != &divisor, "");
    if (compare(*this, divisor) < 0) return 0;
    FMT_ASSERT(divisor.bigits_[divisor.bigits_.size() - 1u] != 0, "");
    align(divisor);
    int quotient = 0;
    do {
      subtract_aligned(divisor);
      ++quotient;
    } while (compare(*this, divisor) >= 0);
    return quotient;
  }
};

// format_dragon flags.
enum dragon {
  predecessor_closer = 1,
  fixup = 2,  // Run fixup to correct exp10 which can be off by one.
  fixed = 4,
};

// Formats a floating-point number using a variation of the Fixed-Precision
// Positive Floating-Point Printout ((FPP)^2) algorithm by Steele & White:
// https://fmt.dev/papers/p372-steele.pdf.
FMT_CONSTEXPR20 inline void format_dragon(basic_fp<uint128_t> value,
                                          unsigned flags, int num_digits,
                                          buffer<char>& buf, int& exp10) {
  bigint numerator;    // 2 * R in (FPP)^2.
  bigint denominator;  // 2 * S in (FPP)^2.
  // lower and upper are differences between value and corresponding boundaries.
  bigint lower;             // (M^- in (FPP)^2).
  bigint upper_store;       // upper's value if different from lower.
  bigint* upper = nullptr;  // (M^+ in (FPP)^2).
  // Shift numerator and denominator by an extra bit or two (if lower boundary
  // is closer) to make lower and upper integers. This eliminates multiplication
  // by 2 during later computations.
  bool is_predecessor_closer = (flags & dragon::predecessor_closer) != 0;
  int shift = is_predecessor_closer ? 2 : 1;
  if (value.e >= 0) {
    numerator = value.f;
    numerator <<= value.e + shift;
    lower = 1;
    lower <<= value.e;
    if (is_predecessor_closer) {
      upper_store = 1;
      upper_store <<= value.e + 1;
      upper = &upper_store;
    }
    denominator.assign_pow10(exp10);
    denominator <<= shift;
  } else if (exp10 < 0) {
    numerator.assign_pow10(-exp10);
    lower.assign(numerator);
    if (is_predecessor_closer) {
      upper_store.assign(numerator);
      upper_store <<= 1;
      upper = &upper_store;
    }
    numerator *= value.f;
    numerator <<= shift;
    denominator = 1;
    denominator <<= shift - value.e;
  } else {
    numerator = value.f;
    numerator <<= shift;
    denominator.assign_pow10(exp10);
    denominator <<= shift - value.e;
    lower = 1;
    if (is_predecessor_closer) {
      upper_store = 1ULL << 1;
      upper = &upper_store;
    }
  }
  int even = static_cast<int>((value.f & 1) == 0);
  if (!upper) upper = &lower;
  bool shortest = num_digits < 0;
  if ((flags & dragon::fixup) != 0) {
    if (add_compare(numerator, *upper, denominator) + even <= 0) {
      --exp10;
      numerator *= 10;
      if (num_digits < 0) {
        lower *= 10;
        if (upper != &lower) *upper *= 10;
      }
    }
    if ((flags & dragon::fixed) != 0) adjust_precision(num_digits, exp10 + 1);
  }
  // Invariant: value == (numerator / denominator) * pow(10, exp10).
  if (shortest) {
    // Generate the shortest representation.
    num_digits = 0;
    char* data = buf.data();
    for (;;) {
      int digit = numerator.divmod_assign(denominator);
      bool low = compare(numerator, lower) - even < 0;  // numerator <[=] lower.
      // numerator + upper >[=] pow10:
      bool high = add_compare(numerator, *upper, denominator) + even > 0;
      data[num_digits++] = static_cast<char>('0' + digit);
      if (low || high) {
        if (!low) {
          ++data[num_digits - 1];
        } else if (high) {
          int result = add_compare(numerator, numerator, denominator);
          // Round half to even.
          if (result > 0 || (result == 0 && (digit % 2) != 0))
            ++data[num_digits - 1];
        }
        buf.try_resize(to_unsigned(num_digits));
        exp10 -= num_digits - 1;
        return;
      }
      numerator *= 10;
      lower *= 10;
      if (upper != &lower) *upper *= 10;
    }
  }
  // Generate the given number of digits.
  exp10 -= num_digits - 1;
  if (num_digits <= 0) {
    auto digit = '0';
    if (num_digits == 0) {
      denominator *= 10;
      digit = add_compare(numerator, numerator, denominator) > 0 ? '1' : '0';
    }
    buf.push_back(digit);
    return;
  }
  buf.try_resize(to_unsigned(num_digits));
  for (int i = 0; i < num_digits - 1; ++i) {
    int digit = numerator.divmod_assign(denominator);
    buf[i] = static_cast<char>('0' + digit);
    numerator *= 10;
  }
  int digit = numerator.divmod_assign(denominator);
  auto result = add_compare(numerator, numerator, denominator);
  if (result > 0 || (result == 0 && (digit % 2) != 0)) {
    if (digit == 9) {
      const auto overflow = '0' + 10;
      buf[num_digits - 1] = overflow;
      // Propagate the carry.
      for (int i = num_digits - 1; i > 0 && buf[i] == overflow; --i) {
        buf[i] = '0';
        ++buf[i - 1];
      }
      if (buf[0] == overflow) {
        buf[0] = '1';
        if ((flags & dragon::fixed) != 0)
          buf.push_back('0');
        else
          ++exp10;
      }
      return;
    }
    ++digit;
  }
  buf[num_digits - 1] = static_cast<char>('0' + digit);
}

// Formats a floating-point number using the hexfloat format.
template <typename Float, FMT_ENABLE_IF(!is_double_double<Float>::value)>
FMT_CONSTEXPR20 void format_hexfloat(Float value, format_specs specs,
                                     buffer<char>& buf) {
  // float is passed as double to reduce the number of instantiations and to
  // simplify implementation.
  static_assert(!std::is_same<Float, float>::value, "");

  using info = dragonbox::float_info<Float>;

  // Assume Float is in the format [sign][exponent][significand].
  using carrier_uint = typename info::carrier_uint;

  const auto num_float_significand_bits = detail::num_significand_bits<Float>();

  basic_fp<carrier_uint> f(value);
  f.e += num_float_significand_bits;
  if (!has_implicit_bit<Float>()) --f.e;

  const auto num_fraction_bits =
      num_float_significand_bits + (has_implicit_bit<Float>() ? 1 : 0);
  const auto num_xdigits = (num_fraction_bits + 3) / 4;

  const auto leading_shift = ((num_xdigits - 1) * 4);
  const auto leading_mask = carrier_uint(0xF) << leading_shift;
  const auto leading_xdigit =
      static_cast<uint32_t>((f.f & leading_mask) >> leading_shift);
  if (leading_xdigit > 1) f.e -= (32 - countl_zero(leading_xdigit) - 1);

  int print_xdigits = num_xdigits - 1;
  if (specs.precision >= 0 && print_xdigits > specs.precision) {
    const int shift = ((print_xdigits - specs.precision - 1) * 4);
    const auto mask = carrier_uint(0xF) << shift;
    const auto v = static_cast<uint32_t>((f.f & mask) >> shift);

    if (v >= 8) {
      const auto inc = carrier_uint(1) << (shift + 4);
      f.f += inc;
      f.f &= ~(inc - 1);
    }

    // Check long double overflow
    if (!has_implicit_bit<Float>()) {
      const auto implicit_bit = carrier_uint(1) << num_float_significand_bits;
      if ((f.f & implicit_bit) == implicit_bit) {
        f.f >>= 4;
        f.e += 4;
      }
    }

    print_xdigits = specs.precision;
  }

  char xdigits[num_bits<carrier_uint>() / 4];
  detail::fill_n(xdigits, sizeof(xdigits), '0');
  format_base2e(4, xdigits, f.f, num_xdigits, specs.upper());

  // Remove zero tail
  while (print_xdigits > 0 && xdigits[print_xdigits] == '0') --print_xdigits;

  buf.push_back('0');
  buf.push_back(specs.upper() ? 'X' : 'x');
  buf.push_back(xdigits[0]);
  if (specs.alt() || print_xdigits > 0 || print_xdigits < specs.precision)
    buf.push_back('.');
  buf.append(xdigits + 1, xdigits + 1 + print_xdigits);
  for (; print_xdigits < specs.precision; ++print_xdigits) buf.push_back('0');

  buf.push_back(specs.upper() ? 'P' : 'p');

  uint32_t abs_e;
  if (f.e < 0) {
    buf.push_back('-');
    abs_e = static_cast<uint32_t>(-f.e);
  } else {
    buf.push_back('+');
    abs_e = static_cast<uint32_t>(f.e);
  }
  format_decimal<char>(appender(buf), abs_e, detail::count_digits(abs_e));
}

template <typename Float, FMT_ENABLE_IF(is_double_double<Float>::value)>
FMT_CONSTEXPR20 void format_hexfloat(Float value, format_specs specs,
                                     buffer<char>& buf) {
  format_hexfloat(static_cast<double>(value), specs, buf);
}

constexpr auto fractional_part_rounding_thresholds(int index) -> uint32_t {
  // For checking rounding thresholds.
  // The kth entry is chosen to be the smallest integer such that the
  // upper 32-bits of 10^(k+1) times it is strictly bigger than 5 * 10^k.
  // It is equal to ceil(2^31 + 2^32/10^(k + 1)).
  // These are stored in a string literal because we cannot have static arrays
  // in constexpr functions and non-static ones are poorly optimized.
  return U"\x9999999a\x828f5c29\x80418938\x80068db9\x8000a7c6\x800010c7"
         U"\x800001ae\x8000002b"[index];
}

template <typename Float>
FMT_CONSTEXPR20 auto format_float(Float value, int precision,
                                  const format_specs& specs, bool binary32,
                                  buffer<char>& buf) -> int {
  // float is passed as double to reduce the number of instantiations.
  static_assert(!std::is_same<Float, float>::value, "");
  auto converted_value = convert_float(value);

  const bool fixed = specs.type() == presentation_type::fixed;
  if (value == 0) {
    if (precision <= 0 || !fixed) {
      buf.push_back('0');
      return 0;
    }
    buf.try_resize(to_unsigned(precision));
    fill_n(buf.data(), precision, '0');
    return -precision;
  }

  int exp = 0;
  bool use_dragon = true;
  unsigned dragon_flags = 0;
  if (!is_fast_float<Float>() || is_constant_evaluated()) {
    const auto inv_log2_10 = 0.3010299956639812;  // 1 / log2(10)
    using info = dragonbox::float_info<decltype(converted_value)>;
    const auto f = basic_fp<typename info::carrier_uint>(converted_value);
    // Compute exp, an approximate power of 10, such that
    //   10^(exp - 1) <= value < 10^exp or 10^exp <= value < 10^(exp + 1).
    // This is based on log10(value) == log2(value) / log2(10) and approximation
    // of log2(value) by e + num_fraction_bits idea from double-conversion.
    auto e = (f.e + count_digits<1>(f.f) - 1) * inv_log2_10 - 1e-10;
    exp = static_cast<int>(e);
    if (e > exp) ++exp;  // Compute ceil.
    dragon_flags = dragon::fixup;
  } else {
    // Extract significand bits and exponent bits.
    using info = dragonbox::float_info<double>;
    auto br = bit_cast<uint64_t>(static_cast<double>(value));

    const uint64_t significand_mask =
        (static_cast<uint64_t>(1) << num_significand_bits<double>()) - 1;
    uint64_t significand = (br & significand_mask);
    int exponent = static_cast<int>((br & exponent_mask<double>()) >>
                                    num_significand_bits<double>());

    if (exponent != 0) {  // Check if normal.
      exponent -= exponent_bias<double>() + num_significand_bits<double>();
      significand |=
          (static_cast<uint64_t>(1) << num_significand_bits<double>());
      significand <<= 1;
    } else {
      // Normalize subnormal inputs.
      FMT_ASSERT(significand != 0, "zeros should not appear here");
      int shift = countl_zero(significand);
      FMT_ASSERT(shift >= num_bits<uint64_t>() - num_significand_bits<double>(),
                 "");
      shift -= (num_bits<uint64_t>() - num_significand_bits<double>() - 2);
      exponent = (std::numeric_limits<double>::min_exponent -
                  num_significand_bits<double>()) -
                 shift;
      significand <<= shift;
    }

    // Compute the first several nonzero decimal significand digits.
    // We call the number we get the first segment.
    const int k = info::kappa - dragonbox::floor_log10_pow2(exponent);
    exp = -k;
    const int beta = exponent + dragonbox::floor_log2_pow10(k);
    uint64_t first_segment;
    bool has_more_segments;
    int digits_in_the_first_segment;
    {
      const auto r = dragonbox::umul192_upper128(
          significand << beta, dragonbox::get_cached_power(k));
      first_segment = r.high();
      has_more_segments = r.low() != 0;

      // The first segment can have 18 ~ 19 digits.
      if (first_segment >= 1000000000000000000ULL) {
        digits_in_the_first_segment = 19;
      } else {
        // When it is of 18-digits, we align it to 19-digits by adding a bogus
        // zero at the end.
        digits_in_the_first_segment = 18;
        first_segment *= 10;
      }
    }

    // Compute the actual number of decimal digits to print.
    if (fixed) adjust_precision(precision, exp + digits_in_the_first_segment);

    // Use Dragon4 only when there might be not enough digits in the first
    // segment.
    if (digits_in_the_first_segment > precision) {
      use_dragon = false;

      if (precision <= 0) {
        exp += digits_in_the_first_segment;

        if (precision < 0) {
          // Nothing to do, since all we have are just leading zeros.
          buf.try_resize(0);
        } else {
          // We may need to round-up.
          buf.try_resize(1);
          if ((first_segment | static_cast<uint64_t>(has_more_segments)) >
              5000000000000000000ULL) {
            buf[0] = '1';
          } else {
            buf[0] = '0';
          }
        }
      }  // precision <= 0
      else {
        exp += digits_in_the_first_segment - precision;

        // When precision > 0, we divide the first segment into three
        // subsegments, each with 9, 9, and 0 ~ 1 digits so that each fits
        // in 32-bits which usually allows faster calculation than in
        // 64-bits. Since some compiler (e.g. MSVC) doesn't know how to optimize
        // division-by-constant for large 64-bit divisors, we do it here
        // manually. The magic number 7922816251426433760 below is equal to
        // ceil(2^(64+32) / 10^10).
        const uint32_t first_subsegment = static_cast<uint32_t>(
            dragonbox::umul128_upper64(first_segment, 7922816251426433760ULL) >>
            32);
        const uint64_t second_third_subsegments =
            first_segment - first_subsegment * 10000000000ULL;

        uint64_t prod;
        uint32_t digits;
        bool should_round_up;
        int number_of_digits_to_print = min_of(precision, 9);

        // Print a 9-digits subsegment, either the first or the second.
        auto print_subsegment = [&](uint32_t subsegment, char* buffer) {
          int number_of_digits_printed = 0;

          // If we want to print an odd number of digits from the subsegment,
          if ((number_of_digits_to_print & 1) != 0) {
            // Convert to 64-bit fixed-point fractional form with 1-digit
            // integer part. The magic number 720575941 is a good enough
            // approximation of 2^(32 + 24) / 10^8; see
            // https://jk-jeon.github.io/posts/2022/12/fixed-precision-formatting/#fixed-length-case
            // for details.
            prod = ((subsegment * static_cast<uint64_t>(720575941)) >> 24) + 1;
            digits = static_cast<uint32_t>(prod >> 32);
            *buffer = static_cast<char>('0' + digits);
            number_of_digits_printed++;
          }
          // If we want to print an even number of digits from the
          // first_subsegment,
          else {
            // Convert to 64-bit fixed-point fractional form with 2-digits
            // integer part. The magic number 450359963 is a good enough
            // approximation of 2^(32 + 20) / 10^7; see
            // https://jk-jeon.github.io/posts/2022/12/fixed-precision-formatting/#fixed-length-case
            // for details.
            prod = ((subsegment * static_cast<uint64_t>(450359963)) >> 20) + 1;
            digits = static_cast<uint32_t>(prod >> 32);
            write2digits(buffer, digits);
            number_of_digits_printed += 2;
          }

          // Print all digit pairs.
          while (number_of_digits_printed < number_of_digits_to_print) {
            prod = static_cast<uint32_t>(prod) * static_cast<uint64_t>(100);
            digits = static_cast<uint32_t>(prod >> 32);
            write2digits(buffer + number_of_digits_printed, digits);
            number_of_digits_printed += 2;
          }
        };

        // Print first subsegment.
        print_subsegment(first_subsegment, buf.data());

        // Perform rounding if the first subsegment is the last subsegment to
        // print.
        if (precision <= 9) {
          // Rounding inside the subsegment.
          // We round-up if:
          //  - either the fractional part is strictly larger than 1/2, or
          //  - the fractional part is exactly 1/2 and the last digit is odd.
          // We rely on the following observations:
          //  - If fractional_part >= threshold, then the fractional part is
          //    strictly larger than 1/2.
          //  - If the MSB of fractional_part is set, then the fractional part
          //    must be at least 1/2.
          //  - When the MSB of fractional_part is set, either
          //    second_third_subsegments being nonzero or has_more_segments
          //    being true means there are further digits not printed, so the
          //    fractional part is strictly larger than 1/2.
          if (precision < 9) {
            uint32_t fractional_part = static_cast<uint32_t>(prod);
            should_round_up =
                fractional_part >= fractional_part_rounding_thresholds(
                                       8 - number_of_digits_to_print) ||
                ((fractional_part >> 31) &
                 ((digits & 1) | (second_third_subsegments != 0) |
                  has_more_segments)) != 0;
          }
          // Rounding at the subsegment boundary.
          // In this case, the fractional part is at least 1/2 if and only if
          // second_third_subsegments >= 5000000000ULL, and is strictly larger
          // than 1/2 if we further have either second_third_subsegments >
          // 5000000000ULL or has_more_segments == true.
          else {
            should_round_up = second_third_subsegments > 5000000000ULL ||
                              (second_third_subsegments == 5000000000ULL &&
                               ((digits & 1) != 0 || has_more_segments));
          }
        }
        // Otherwise, print the second subsegment.
        else {
          // Compilers are not aware of how to leverage the maximum value of
          // second_third_subsegments to find out a better magic number which
          // allows us to eliminate an additional shift. 1844674407370955162 =
          // ceil(2^64/10) < ceil(2^64*(10^9/(10^10 - 1))).
          const uint32_t second_subsegment =
              static_cast<uint32_t>(dragonbox::umul128_upper64(
                  second_third_subsegments, 1844674407370955162ULL));
          const uint32_t third_subsegment =
              static_cast<uint32_t>(second_third_subsegments) -
              second_subsegment * 10;

          number_of_digits_to_print = precision - 9;
          print_subsegment(second_subsegment, buf.data() + 9);

          // Rounding inside the subsegment.
          if (precision < 18) {
            // The condition third_subsegment != 0 implies that the segment was
            // of 19 digits, so in this case the third segment should be
            // consisting of a genuine digit from the input.
            uint32_t fractional_part = static_cast<uint32_t>(prod);
            should_round_up =
                fractional_part >= fractional_part_rounding_thresholds(
                                       8 - number_of_digits_to_print) ||
                ((fractional_part >> 31) &
                 ((digits & 1) | (third_subsegment != 0) |
                  has_more_segments)) != 0;
          }
          // Rounding at the subsegment boundary.
          else {
            // In this case, the segment must be of 19 digits, thus
            // the third subsegment should be consisting of a genuine digit from
            // the input.
            should_round_up = third_subsegment > 5 ||
                              (third_subsegment == 5 &&
                               ((digits & 1) != 0 || has_more_segments));
          }
        }

        // Round-up if necessary.
        if (should_round_up) {
          ++buf[precision - 1];
          for (int i = precision - 1; i > 0 && buf[i] > '9'; --i) {
            buf[i] = '0';
            ++buf[i - 1];
          }
          if (buf[0] > '9') {
            buf[0] = '1';
            if (fixed)
              buf[precision++] = '0';
            else
              ++exp;
          }
        }
        buf.try_resize(to_unsigned(precision));
      }
    }  // if (digits_in_the_first_segment > precision)
    else {
      // Adjust the exponent for its use in Dragon4.
      exp += digits_in_the_first_segment - 1;
    }
  }
  if (use_dragon) {
    auto f = basic_fp<uint128_t>();
    bool is_predecessor_closer = binary32 ? f.assign(static_cast<float>(value))
                                          : f.assign(converted_value);
    if (is_predecessor_closer) dragon_flags |= dragon::predecessor_closer;
    if (fixed) dragon_flags |= dragon::fixed;
    // Limit precision to the maximum possible number of significant digits in
    // an IEEE754 double because we don't need to generate zeros.
    const int max_double_digits = 767;
    if (precision > max_double_digits) precision = max_double_digits;
    format_dragon(f, dragon_flags, precision, buf, exp);
  }
  if (!fixed && !specs.alt()) {
    // Remove trailing zeros.
    auto num_digits = buf.size();
    while (num_digits > 0 && buf[num_digits - 1] == '0') {
      --num_digits;
      ++exp;
    }
    buf.try_resize(num_digits);
  }
  return exp;
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_floating_point<T>::value)>
FMT_CONSTEXPR20 auto write(OutputIt out, T value, format_specs specs,
                           locale_ref loc = {}) -> OutputIt {
  if (specs.localized() && write_loc(out, value, specs, loc)) return out;

  // Use signbit because value < 0 is false for NaN.
  sign s = detail::signbit(value) ? sign::minus : specs.sign();

  if (!detail::isfinite(value))
    return write_nonfinite<Char>(out, detail::isnan(value), specs, s);

  if (specs.align() == align::numeric && s != sign::none) {
    *out++ = detail::getsign<Char>(s);
    s = sign::none;
    if (specs.width != 0) --specs.width;
  }

  const int exp_upper = detail::exp_upper<T>();
  int precision = specs.precision;
  if (precision < 0) {
    if (specs.type() != presentation_type::none) {
      precision = 6;
    } else if (is_fast_float<T>::value && !is_constant_evaluated()) {
      // Use Dragonbox for the shortest format.
      auto dec = dragonbox::to_decimal(static_cast<fast_float_t<T>>(value));
      return write_float<Char>(out, dec, specs, s, exp_upper, loc);
    }
  }

  memory_buffer buffer;
  if (specs.type() == presentation_type::hexfloat) {
    if (s != sign::none) buffer.push_back(detail::getsign<char>(s));
    format_hexfloat(convert_float(value), specs, buffer);
    return write_bytes<Char, align::right>(out, {buffer.data(), buffer.size()},
                                           specs);
  }

  if (specs.type() == presentation_type::exp) {
    if (precision == max_value<int>())
      report_error("number is too big");
    else
      ++precision;
    if (specs.precision != 0) specs.set_alt();
  } else if (specs.type() == presentation_type::fixed) {
    if (specs.precision != 0) specs.set_alt();
  } else if (precision == 0) {
    precision = 1;
  }
  int exp = format_float(convert_float(value), precision, specs,
                         std::is_same<T, float>(), buffer);

  specs.precision = precision;
  auto f = big_decimal_fp{buffer.data(), static_cast<int>(buffer.size()), exp};
  return write_float<Char>(out, f, specs, s, exp_upper, loc);
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_fast_float<T>::value)>
FMT_CONSTEXPR20 auto write(OutputIt out, T value) -> OutputIt {
  if (is_constant_evaluated()) return write<Char>(out, value, format_specs());

  auto s = detail::signbit(value) ? sign::minus : sign::none;
  auto mask = exponent_mask<fast_float_t<T>>();
  if ((bit_cast<decltype(mask)>(value) & mask) == mask)
    return write_nonfinite<Char>(out, std::isnan(value), {}, s);

  auto dec = dragonbox::to_decimal(static_cast<fast_float_t<T>>(value));
  auto significand = dec.significand;
  int significand_size = count_digits(significand);
  int exponent = dec.exponent + significand_size - 1;
  if (use_fixed(exponent, detail::exp_upper<T>())) {
    return write_fixed<Char, fallback_digit_grouping<Char>>(
        out, dec, significand_size, Char('.'), {}, s);
  }

  // Write value in the exponential format.
  const char* prefix = "e+";
  int abs_exponent = exponent;
  if (exponent < 0) {
    abs_exponent = -exponent;
    prefix = "e-";
  }
  auto has_decimal_point = significand_size != 1;
  size_t size = std::is_pointer<OutputIt>::value
                    ? 0u
                    : to_unsigned((s != sign::none ? 1 : 0) + significand_size +
                                  (has_decimal_point ? 1 : 0) +
                                  (abs_exponent >= 100 ? 5 : 4));
  if (auto ptr = to_pointer<Char>(out, size)) {
    if (s != sign::none) *ptr++ = Char('-');
    if (has_decimal_point) {
      auto begin = ptr;
      ptr = format_decimal<Char>(ptr, significand, significand_size + 1);
      *begin = begin[1];
      begin[1] = '.';
    } else {
      *ptr++ = static_cast<Char>('0' + significand);
    }
    memcpy(ptr, prefix, 2);
    ptr += 2;
    if (abs_exponent >= 100) {
      *ptr++ = static_cast<Char>('0' + abs_exponent / 100);
      abs_exponent %= 100;
    }
    write2digits(ptr, static_cast<unsigned>(abs_exponent));
    return select<std::is_pointer<OutputIt>::value>(ptr + 2, out);
  }
  auto it = reserve(out, size);
  if (s != sign::none) *it++ = Char('-');
  // Insert a decimal point after the first digit and add an exponent.
  it = write_significand(it, significand, significand_size, 1,
                         has_decimal_point ? Char('.') : Char());
  *it++ = Char('e');
  it = write_exponent<Char>(exponent, it);
  return base_iterator(out, it);
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(is_floating_point<T>::value &&
                        !is_fast_float<T>::value)>
inline auto write(OutputIt out, T value) -> OutputIt {
  return write<Char>(out, value, {});
}

template <typename Char, typename OutputIt>
auto write(OutputIt out, monostate, format_specs = {}, locale_ref = {})
    -> OutputIt {
  FMT_ASSERT(false, "");
  return out;
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, basic_string_view<Char> value)
    -> OutputIt {
  return copy_noinline<Char>(value.begin(), value.end(), out);
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(has_to_string_view<T>::value)>
constexpr auto write(OutputIt out, const T& value) -> OutputIt {
  return write<Char>(out, to_string_view(value));
}

// FMT_ENABLE_IF() condition separated to workaround an MSVC bug.
template <
    typename Char, typename OutputIt, typename T,
    bool check = std::is_enum<T>::value && !std::is_same<T, Char>::value &&
                 mapped_type_constant<T, Char>::value != type::custom_type,
    FMT_ENABLE_IF(check)>
FMT_CONSTEXPR auto write(OutputIt out, T value) -> OutputIt {
  return write<Char>(out, static_cast<underlying_t<T>>(value));
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_same<T, bool>::value)>
FMT_CONSTEXPR auto write(OutputIt out, T value, const format_specs& specs = {},
                         locale_ref = {}) -> OutputIt {
  return specs.type() != presentation_type::none &&
                 specs.type() != presentation_type::string
             ? write<Char>(out, value ? 1 : 0, specs, {})
             : write_bytes<Char>(out, value ? "true" : "false", specs);
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR auto write(OutputIt out, Char value) -> OutputIt {
  auto it = reserve(out, 1);
  *it++ = value;
  return base_iterator(out, it);
}

template <typename Char, typename OutputIt>
FMT_CONSTEXPR20 auto write(OutputIt out, const Char* value) -> OutputIt {
  if (value) return write(out, basic_string_view<Char>(value));
  report_error("string pointer is null");
  return out;
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(std::is_same<T, void>::value)>
auto write(OutputIt out, const T* value, const format_specs& specs = {},
           locale_ref = {}) -> OutputIt {
  return write_ptr<Char>(out, bit_cast<uintptr_t>(value), &specs);
}

template <typename Char, typename OutputIt, typename T,
          FMT_ENABLE_IF(mapped_type_constant<T, Char>::value ==
                            type::custom_type &&
                        !std::is_fundamental<T>::value)>
FMT_CONSTEXPR auto write(OutputIt out, const T& value) -> OutputIt {
  auto f = formatter<T, Char>();
  auto parse_ctx = parse_context<Char>({});
  f.parse(parse_ctx);
  auto ctx = basic_format_context<OutputIt, Char>(out, {}, {});
  return f.format(value, ctx);
}

template <typename T>
using is_builtin =
    bool_constant<std::is_same<T, int>::value || FMT_BUILTIN_TYPES>;

// An argument visitor that formats the argument and writes it via the output
// iterator. It's a class and not a generic lambda for compatibility with C++11.
template <typename Char> struct default_arg_formatter {
  using context = buffered_context<Char>;

  basic_appender<Char> out;

  void operator()(monostate) { report_error("argument not found"); }

  template <typename T, FMT_ENABLE_IF(is_builtin<T>::value)>
  void operator()(T value) {
    write<Char>(out, value);
  }

  template <typename T, FMT_ENABLE_IF(!is_builtin<T>::value)>
  void operator()(T) {
    FMT_ASSERT(false, "");
  }

  void operator()(typename basic_format_arg<context>::handle h) {
    // Use a null locale since the default format must be unlocalized.
    auto parse_ctx = parse_context<Char>({});
    auto format_ctx = context(out, {}, {});
    h.format(parse_ctx, format_ctx);
  }
};

template <typename Char> struct arg_formatter {
  basic_appender<Char> out;
  const format_specs& specs;
  FMT_NO_UNIQUE_ADDRESS locale_ref locale;

  template <typename T, FMT_ENABLE_IF(is_builtin<T>::value)>
  FMT_CONSTEXPR FMT_INLINE void operator()(T value) {
    detail::write<Char>(out, value, specs, locale);
  }

  template <typename T, FMT_ENABLE_IF(!is_builtin<T>::value)>
  void operator()(T) {
    FMT_ASSERT(false, "");
  }

  void operator()(typename basic_format_arg<buffered_context<Char>>::handle) {
    // User-defined types are handled separately because they require access
    // to the parse context.
  }
};

struct dynamic_spec_getter {
  template <typename T, FMT_ENABLE_IF(is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T value) -> unsigned long long {
    return is_negative(value) ? ~0ull : static_cast<unsigned long long>(value);
  }

  template <typename T, FMT_ENABLE_IF(!is_integer<T>::value)>
  FMT_CONSTEXPR auto operator()(T) -> unsigned long long {
    report_error("width/precision is not integer");
    return 0;
  }
};

template <typename Context, typename ID>
FMT_CONSTEXPR auto get_arg(Context& ctx, ID id) -> basic_format_arg<Context> {
  auto arg = ctx.arg(id);
  if (!arg) report_error("argument not found");
  return arg;
}

template <typename Context>
FMT_CONSTEXPR int get_dynamic_spec(
    arg_id_kind kind, const arg_ref<typename Context::char_type>& ref,
    Context& ctx) {
  FMT_ASSERT(kind != arg_id_kind::none, "");
  auto arg =
      kind == arg_id_kind::index ? ctx.arg(ref.index) : ctx.arg(ref.name);
  if (!arg) report_error("argument not found");
  unsigned long long value = arg.visit(dynamic_spec_getter());
  if (value > to_unsigned(max_value<int>()))
    report_error("width/precision is out of range");
  return static_cast<int>(value);
}

template <typename Context>
FMT_CONSTEXPR void handle_dynamic_spec(
    arg_id_kind kind, int& value,
    const arg_ref<typename Context::char_type>& ref, Context& ctx) {
  if (kind != arg_id_kind::none) value = get_dynamic_spec(kind, ref, ctx);
}

#if FMT_USE_NONTYPE_TEMPLATE_ARGS
template <typename T, typename Char, size_t N,
          fmt::detail::fixed_string<Char, N> Str>
struct static_named_arg : view {
  static constexpr auto name = Str.data;

  const T& value;
  static_named_arg(const T& v) : value(v) {}
};

template <typename T, typename Char, size_t N,
          fmt::detail::fixed_string<Char, N> Str>
struct is_named_arg<static_named_arg<T, Char, N, Str>> : std::true_type {};

template <typename T, typename Char, size_t N,
          fmt::detail::fixed_string<Char, N> Str>
struct is_static_named_arg<static_named_arg<T, Char, N, Str>> : std::true_type {
};

template <typename Char, size_t N, fmt::detail::fixed_string<Char, N> Str>
struct udl_arg {
  template <typename T> auto operator=(T&& value) const {
    return static_named_arg<T, Char, N, Str>(std::forward<T>(value));
  }
};
#else
template <typename Char> struct udl_arg {
  const Char* str;

  template <typename T> auto operator=(T&& value) const -> named_arg<Char, T> {
    return {str, std::forward<T>(value)};
  }
};
#endif  // FMT_USE_NONTYPE_TEMPLATE_ARGS

template <typename Char> struct format_handler {
  parse_context<Char> parse_ctx;
  buffered_context<Char> ctx;

  void on_text(const Char* begin, const Char* end) {
    copy_noinline<Char>(begin, end, ctx.out());
  }

  FMT_CONSTEXPR auto on_arg_id() -> int { return parse_ctx.next_arg_id(); }
  FMT_CONSTEXPR auto on_arg_id(int id) -> int {
    parse_ctx.check_arg_id(id);
    return id;
  }
  FMT_CONSTEXPR auto on_arg_id(basic_string_view<Char> id) -> int {
    parse_ctx.check_arg_id(id);
    int arg_id = ctx.arg_id(id);
    if (arg_id < 0) report_error("argument not found");
    return arg_id;
  }

  FMT_INLINE void on_replacement_field(int id, const Char*) {
    ctx.arg(id).visit(default_arg_formatter<Char>{ctx.out()});
  }

  auto on_format_specs(int id, const Char* begin, const Char* end)
      -> const Char* {
    auto arg = get_arg(ctx, id);
    // Not using a visitor for custom types gives better codegen.
    if (arg.format_custom(begin, parse_ctx, ctx)) return parse_ctx.begin();

    auto specs = dynamic_format_specs<Char>();
    begin = parse_format_specs(begin, end, specs, parse_ctx, arg.type());
    if (specs.dynamic()) {
      handle_dynamic_spec(specs.dynamic_width(), specs.width, specs.width_ref,
                          ctx);
      handle_dynamic_spec(specs.dynamic_precision(), specs.precision,
                          specs.precision_ref, ctx);
    }

    arg.visit(arg_formatter<Char>{ctx.out(), specs, ctx.locale()});
    return begin;
  }

  FMT_NORETURN void on_error(const char* message) { report_error(message); }
};

using format_func = void (*)(detail::buffer<char>&, int, const char*);
FMT_API void do_report_error(format_func func, int error_code,
                             const char* message) noexcept;

FMT_API void format_error_code(buffer<char>& out, int error_code,
                               string_view message) noexcept;

template <typename T, typename Char, type TYPE>
template <typename FormatContext>
FMT_CONSTEXPR auto native_formatter<T, Char, TYPE>::format(
    const T& val, FormatContext& ctx) const -> decltype(ctx.out()) {
  if (!specs_.dynamic())
    return write<Char>(ctx.out(), val, specs_, ctx.locale());
  auto specs = format_specs(specs_);
  handle_dynamic_spec(specs.dynamic_width(), specs.width, specs_.width_ref,
                      ctx);
  handle_dynamic_spec(specs.dynamic_precision(), specs.precision,
                      specs_.precision_ref, ctx);
  return write<Char>(ctx.out(), val, specs, ctx.locale());
}

// DEPRECATED! https://github.com/fmtlib/fmt/issues/4292.
template <typename T, typename Enable = void>
struct is_locale : std::false_type {};
template <typename T>
struct is_locale<T, void_t<decltype(T::classic())>> : std::true_type {};

// DEPRECATED!
template <typename Char = char> struct vformat_args {
  using type = basic_format_args<buffered_context<Char>>;
};
template <> struct vformat_args<char> {
  using type = format_args;
};

template <typename Char>
void vformat_to(buffer<Char>& buf, basic_string_view<Char> fmt,
                typename vformat_args<Char>::type args, locale_ref loc = {}) {
  auto out = basic_appender<Char>(buf);
  parse_format_string(
      fmt, format_handler<Char>{parse_context<Char>(fmt), {out, args, loc}});
}
}  // namespace detail

FMT_BEGIN_EXPORT

// A generic formatting context with custom output iterator and character
// (code unit) support. Char is the format string code unit type which can be
// different from OutputIt::value_type.
template <typename OutputIt, typename Char> class generic_context {
 private:
  OutputIt out_;
  basic_format_args<generic_context> args_;
  detail::locale_ref loc_;

 public:
  using char_type = Char;
  using iterator = OutputIt;
  using parse_context_type FMT_DEPRECATED = parse_context<Char>;
  template <typename T>
  using formatter_type FMT_DEPRECATED = formatter<T, Char>;
  enum { builtin_types = FMT_BUILTIN_TYPES };

  constexpr generic_context(OutputIt out,
                            basic_format_args<generic_context> args,
                            detail::locale_ref loc = {})
      : out_(out), args_(args), loc_(loc) {}
  generic_context(generic_context&&) = default;
  generic_context(const generic_context&) = delete;
  void operator=(const generic_context&) = delete;

  constexpr auto arg(int id) const -> basic_format_arg<generic_context> {
    return args_.get(id);
  }
  auto arg(basic_string_view<Char> name) const
      -> basic_format_arg<generic_context> {
    return args_.get(name);
  }
  constexpr auto arg_id(basic_string_view<Char> name) const -> int {
    return args_.get_id(name);
  }

  constexpr auto out() const -> iterator { return out_; }

  void advance_to(iterator it) {
    if (!detail::is_back_insert_iterator<iterator>()) out_ = it;
  }

  constexpr auto locale() const -> detail::locale_ref { return loc_; }
};

class loc_value {
 private:
  basic_format_arg<context> value_;

 public:
  template <typename T, FMT_ENABLE_IF(!detail::is_float128<T>::value)>
  loc_value(T value) : value_(value) {}

  template <typename T, FMT_ENABLE_IF(detail::is_float128<T>::value)>
  loc_value(T) {}

  template <typename Visitor> auto visit(Visitor&& vis) -> decltype(vis(0)) {
    return value_.visit(vis);
  }
};

// A locale facet that formats values in UTF-8.
// It is parameterized on the locale to avoid the heavy <locale> include.
template <typename Locale> class format_facet : public Locale::facet {
 private:
  std::string separator_;
  std::string grouping_;
  std::string decimal_point_;

 protected:
  virtual auto do_put(appender out, loc_value val,
                      const format_specs& specs) const -> bool;

 public:
  static FMT_API typename Locale::id id;

  explicit format_facet(Locale& loc);
  explicit format_facet(string_view sep = "", std::string grouping = "\3",
                        std::string decimal_point = ".")
      : separator_(sep.data(), sep.size()),
        grouping_(grouping),
        decimal_point_(decimal_point) {}

  auto put(appender out, loc_value val, const format_specs& specs) const
      -> bool {
    return do_put(out, val, specs);
  }
};

#define FMT_FORMAT_AS(Type, Base)                                   \
  template <typename Char>                                          \
  struct formatter<Type, Char> : formatter<Base, Char> {            \
    template <typename FormatContext>                               \
    FMT_CONSTEXPR auto format(Type value, FormatContext& ctx) const \
        -> decltype(ctx.out()) {                                    \
      return formatter<Base, Char>::format(value, ctx);             \
    }                                                               \
  }

FMT_FORMAT_AS(signed char, int);
FMT_FORMAT_AS(unsigned char, unsigned);
FMT_FORMAT_AS(short, int);
FMT_FORMAT_AS(unsigned short, unsigned);
FMT_FORMAT_AS(long, detail::long_type);
FMT_FORMAT_AS(unsigned long, detail::ulong_type);
FMT_FORMAT_AS(Char*, const Char*);
FMT_FORMAT_AS(detail::std_string_view<Char>, basic_string_view<Char>);
FMT_FORMAT_AS(std::nullptr_t, const void*);
FMT_FORMAT_AS(void*, const void*);

template <typename Char, size_t N>
struct formatter<Char[N], Char> : formatter<basic_string_view<Char>, Char> {};

template <typename Char, typename Traits, typename Allocator>
class formatter<std::basic_string<Char, Traits, Allocator>, Char>
    : public formatter<basic_string_view<Char>, Char> {};

template <int N, typename Char>
struct formatter<detail::bitint<N>, Char> : formatter<long long, Char> {};
template <int N, typename Char>
struct formatter<detail::ubitint<N>, Char>
    : formatter<unsigned long long, Char> {};

template <typename Char>
struct formatter<detail::float128, Char>
    : detail::native_formatter<detail::float128, Char,
                               detail::type::float_type> {};

template <typename T, typename Char>
struct formatter<T, Char, void_t<detail::format_as_result<T>>>
    : formatter<detail::format_as_result<T>, Char> {
  template <typename FormatContext>
  FMT_CONSTEXPR auto format(const T& value, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    auto&& val = format_as(value);  // Make an lvalue reference for format.
    return formatter<detail::format_as_result<T>, Char>::format(val, ctx);
  }
};

/**
 * Converts `p` to `const void*` for pointer formatting.
 *
 * **Example**:
 *
 *     auto s = fmt::format("{}", fmt::ptr(p));
 */
template <typename T> auto ptr(T p) -> const void* {
  static_assert(std::is_pointer<T>::value, "fmt::ptr used with non-pointer");
  return detail::bit_cast<const void*>(p);
}

/**
 * Converts `e` to the underlying type.
 *
 * **Example**:
 *
 *     enum class color { red, green, blue };
 *     auto s = fmt::format("{}", fmt::underlying(color::red));  // s == "0"
 */
template <typename Enum>
constexpr auto underlying(Enum e) noexcept -> underlying_t<Enum> {
  return static_cast<underlying_t<Enum>>(e);
}

namespace enums {
template <typename Enum, FMT_ENABLE_IF(std::is_enum<Enum>::value)>
constexpr auto format_as(Enum e) noexcept -> underlying_t<Enum> {
  return static_cast<underlying_t<Enum>>(e);
}
}  // namespace enums

#ifdef __cpp_lib_byte
template <typename Char>
struct formatter<std::byte, Char> : formatter<unsigned, Char> {
  static auto format_as(std::byte b) -> unsigned char {
    return static_cast<unsigned char>(b);
  }
  template <typename Context>
  auto format(std::byte b, Context& ctx) const -> decltype(ctx.out()) {
    return formatter<unsigned, Char>::format(format_as(b), ctx);
  }
};
#endif

struct bytes {
  string_view data;

  inline explicit bytes(string_view s) : data(s) {}
};

template <> struct formatter<bytes> {
 private:
  detail::dynamic_format_specs<> specs_;

 public:
  FMT_CONSTEXPR auto parse(parse_context<>& ctx) -> const char* {
    return parse_format_specs(ctx.begin(), ctx.end(), specs_, ctx,
                              detail::type::string_type);
  }

  template <typename FormatContext>
  auto format(bytes b, FormatContext& ctx) const -> decltype(ctx.out()) {
    auto specs = specs_;
    detail::handle_dynamic_spec(specs.dynamic_width(), specs.width,
                                specs.width_ref, ctx);
    detail::handle_dynamic_spec(specs.dynamic_precision(), specs.precision,
                                specs.precision_ref, ctx);
    return detail::write_bytes<char>(ctx.out(), b.data, specs);
  }
};

// group_digits_view is not derived from view because it copies the argument.
template <typename T> struct group_digits_view {
  T value;
};

/**
 * Returns a view that formats an integer value using ',' as a
 * locale-independent thousands separator.
 *
 * **Example**:
 *
 *     fmt::print("{}", fmt::group_digits(12345));
 *     // Output: "12,345"
 */
template <typename T> auto group_digits(T value) -> group_digits_view<T> {
  return {value};
}

template <typename T> struct formatter<group_digits_view<T>> : formatter<T> {
 private:
  detail::dynamic_format_specs<> specs_;

 public:
  FMT_CONSTEXPR auto parse(parse_context<>& ctx) -> const char* {
    return parse_format_specs(ctx.begin(), ctx.end(), specs_, ctx,
                              detail::type::int_type);
  }

  template <typename FormatContext>
  auto format(group_digits_view<T> view, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    auto specs = specs_;
    detail::handle_dynamic_spec(specs.dynamic_width(), specs.width,
                                specs.width_ref, ctx);
    detail::handle_dynamic_spec(specs.dynamic_precision(), specs.precision,
                                specs.precision_ref, ctx);
    auto arg = detail::make_write_int_arg(view.value, specs.sign());
    return detail::write_int(
        ctx.out(), static_cast<detail::uint64_or_128_t<T>>(arg.abs_value),
        arg.prefix, specs, detail::digit_grouping<char>("\3", ","));
  }
};

template <typename T, typename Char> struct nested_view {
  const formatter<T, Char>* fmt;
  const T* value;
};

template <typename T, typename Char>
struct formatter<nested_view<T, Char>, Char> {
  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    return ctx.begin();
  }
  template <typename FormatContext>
  auto format(nested_view<T, Char> view, FormatContext& ctx) const
      -> decltype(ctx.out()) {
    return view.fmt->format(*view.value, ctx);
  }
};

template <typename T, typename Char = char> struct nested_formatter {
 private:
  basic_specs specs_;
  int width_;
  formatter<T, Char> formatter_;

 public:
  constexpr nested_formatter() : width_(0) {}

  FMT_CONSTEXPR auto parse(parse_context<Char>& ctx) -> const Char* {
    auto it = ctx.begin(), end = ctx.end();
    if (it == end) return it;
    auto specs = format_specs();
    it = detail::parse_align(it, end, specs);
    specs_ = specs;
    Char c = *it;
    auto width_ref = detail::arg_ref<Char>();
    if ((c >= '0' && c <= '9') || c == '{') {
      it = detail::parse_width(it, end, specs, width_ref, ctx);
      width_ = specs.width;
    }
    ctx.advance_to(it);
    return formatter_.parse(ctx);
  }

  template <typename FormatContext, typename F>
  auto write_padded(FormatContext& ctx, F write) const -> decltype(ctx.out()) {
    if (width_ == 0) return write(ctx.out());
    auto buf = basic_memory_buffer<Char>();
    write(basic_appender<Char>(buf));
    auto specs = format_specs();
    specs.width = width_;
    specs.copy_fill_from(specs_);
    specs.set_align(specs_.align());
    return detail::write<Char>(
        ctx.out(), basic_string_view<Char>(buf.data(), buf.size()), specs);
  }

  auto nested(const T& value) const -> nested_view<T, Char> {
    return nested_view<T, Char>{&formatter_, &value};
  }
};

inline namespace literals {
#if FMT_USE_NONTYPE_TEMPLATE_ARGS
template <detail::fixed_string S> constexpr auto operator""_a() {
  using char_t = remove_cvref_t<decltype(*S.data)>;
  return detail::udl_arg<char_t, sizeof(S.data) / sizeof(char_t), S>();
}
#else
/**
 * User-defined literal equivalent of `fmt::arg`.
 *
 * **Example**:
 *
 *     using namespace fmt::literals;
 *     fmt::print("The answer is {answer}.", "answer"_a=42);
 */
constexpr auto operator""_a(const char* s, size_t) -> detail::udl_arg<char> {
  return {s};
}
#endif  // FMT_USE_NONTYPE_TEMPLATE_ARGS
}  // namespace literals

/// A fast integer formatter.
class format_int {
 private:
  // Buffer should be large enough to hold all digits (digits10 + 1),
  // a sign and a null character.
  enum { buffer_size = std::numeric_limits<unsigned long long>::digits10 + 3 };
  mutable char buffer_[buffer_size];
  char* str_;

  template <typename UInt>
  FMT_CONSTEXPR20 auto format_unsigned(UInt value) -> char* {
    auto n = static_cast<detail::uint32_or_64_or_128_t<UInt>>(value);
    return detail::do_format_decimal(buffer_, n, buffer_size - 1);
  }

  template <typename Int>
  FMT_CONSTEXPR20 auto format_signed(Int value) -> char* {
    auto abs_value = static_cast<detail::uint32_or_64_or_128_t<Int>>(value);
    bool negative = value < 0;
    if (negative) abs_value = 0 - abs_value;
    auto begin = format_unsigned(abs_value);
    if (negative) *--begin = '-';
    return begin;
  }

 public:
  FMT_CONSTEXPR20 explicit format_int(int value) : str_(format_signed(value)) {}
  FMT_CONSTEXPR20 explicit format_int(long value)
      : str_(format_signed(value)) {}
  FMT_CONSTEXPR20 explicit format_int(long long value)
      : str_(format_signed(value)) {}
  FMT_CONSTEXPR20 explicit format_int(unsigned value)
      : str_(format_unsigned(value)) {}
  FMT_CONSTEXPR20 explicit format_int(unsigned long value)
      : str_(format_unsigned(value)) {}
  FMT_CONSTEXPR20 explicit format_int(unsigned long long value)
      : str_(format_unsigned(value)) {}

  /// Returns the number of characters written to the output buffer.
  FMT_CONSTEXPR20 auto size() const -> size_t {
    return detail::to_unsigned(buffer_ - str_ + buffer_size - 1);
  }

  /// Returns a pointer to the output buffer content. No terminating null
  /// character is appended.
  FMT_CONSTEXPR20 auto data() const -> const char* { return str_; }

  /// Returns a pointer to the output buffer content with terminating null
  /// character appended.
  FMT_CONSTEXPR20 auto c_str() const -> const char* {
    buffer_[buffer_size - 1] = '\0';
    return str_;
  }

  /// Returns the content of the output buffer as an `std::string`.
  inline auto str() const -> std::string { return {str_, size()}; }
};

#define FMT_STRING_IMPL(s, base)                                              \
  [] {                                                                        \
    /* Use the hidden visibility as a workaround for a GCC bug (#1973). */    \
    /* Use a macro-like name to avoid shadowing warnings. */                  \
    struct FMT_VISIBILITY("hidden") FMT_COMPILE_STRING : base {               \
      using char_type = fmt::remove_cvref_t<decltype(s[0])>;                  \
      constexpr explicit operator fmt::basic_string_view<char_type>() const { \
        return fmt::detail::compile_string_to_view<char_type>(s);             \
      }                                                                       \
    };                                                                        \
    using FMT_STRING_VIEW =                                                   \
        fmt::basic_string_view<typename FMT_COMPILE_STRING::char_type>;       \
    fmt::detail::ignore_unused(FMT_STRING_VIEW(FMT_COMPILE_STRING()));        \
    return FMT_COMPILE_STRING();                                              \
  }()

/**
 * Constructs a legacy compile-time format string from a string literal `s`.
 *
 * **Example**:
 *
 *     // A compile-time error because 'd' is an invalid specifier for strings.
 *     std::string s = fmt::format(FMT_STRING("{:d}"), "foo");
 */
#define FMT_STRING(s) FMT_STRING_IMPL(s, fmt::detail::compile_string)

FMT_API auto vsystem_error(int error_code, string_view fmt, format_args args)
    -> std::system_error;

/**
 * Constructs `std::system_error` with a message formatted with
 * `fmt::format(fmt, args...)`.
 * `error_code` is a system error code as given by `errno`.
 *
 * **Example**:
 *
 *     // This throws std::system_error with the description
 *     //   cannot open file 'madeup': No such file or directory
 *     // or similar (system message may vary).
 *     const char* filename = "madeup";
 *     FILE* file = fopen(filename, "r");
 *     if (!file)
 *       throw fmt::system_error(errno, "cannot open file '{}'", filename);
 */
template <typename... T>
auto system_error(int error_code, format_string<T...> fmt, T&&... args)
    -> std::system_error {
  return vsystem_error(error_code, fmt.str, vargs<T...>{{args...}});
}

/**
 * Formats an error message for an error returned by an operating system or a
 * language runtime, for example a file opening error, and writes it to `out`.
 * The format is the same as the one used by `std::system_error(ec, message)`
 * where `ec` is `std::error_code(error_code, std::generic_category())`.
 * It is implementation-defined but normally looks like:
 *
 *     <message>: <system-message>
 *
 * where `<message>` is the passed message and `<system-message>` is the system
 * message corresponding to the error code.
 * `error_code` is a system error code as given by `errno`.
 */
FMT_API void format_system_error(detail::buffer<char>& out, int error_code,
                                 const char* message) noexcept;

// Reports a system error without throwing an exception.
// Can be used to report errors from destructors.
FMT_API void report_system_error(int error_code, const char* message) noexcept;

template <typename Locale, FMT_ENABLE_IF(detail::is_locale<Locale>::value)>
inline auto vformat(const Locale& loc, string_view fmt, format_args args)
    -> std::string {
  auto buf = memory_buffer();
  detail::vformat_to(buf, fmt, args, detail::locale_ref(loc));
  return {buf.data(), buf.size()};
}

template <typename Locale, typename... T,
          FMT_ENABLE_IF(detail::is_locale<Locale>::value)>
FMT_INLINE auto format(const Locale& loc, format_string<T...> fmt, T&&... args)
    -> std::string {
  return vformat(loc, fmt.str, vargs<T...>{{args...}});
}

template <typename OutputIt, typename Locale,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value)>
auto vformat_to(OutputIt out, const Locale& loc, string_view fmt,
                format_args args) -> OutputIt {
  auto&& buf = detail::get_buffer<char>(out);
  detail::vformat_to(buf, fmt, args, detail::locale_ref(loc));
  return detail::get_iterator(buf, out);
}

template <typename OutputIt, typename Locale, typename... T,
          FMT_ENABLE_IF(detail::is_output_iterator<OutputIt, char>::value&&
                            detail::is_locale<Locale>::value)>
FMT_INLINE auto format_to(OutputIt out, const Locale& loc,
                          format_string<T...> fmt, T&&... args) -> OutputIt {
  return fmt::vformat_to(out, loc, fmt.str, vargs<T...>{{args...}});
}

template <typename Locale, typename... T,
          FMT_ENABLE_IF(detail::is_locale<Locale>::value)>
FMT_NODISCARD FMT_INLINE auto formatted_size(const Locale& loc,
                                             format_string<T...> fmt,
                                             T&&... args) -> size_t {
  auto buf = detail::counting_buffer<>();
  detail::vformat_to(buf, fmt.str, vargs<T...>{{args...}},
                     detail::locale_ref(loc));
  return buf.count();
}

FMT_API auto vformat(string_view fmt, format_args args) -> std::string;

/**
 * Formats `args` according to specifications in `fmt` and returns the result
 * as a string.
 *
 * **Example**:
 *
 *     #include <fmt/format.h>
 *     std::string message = fmt::format("The answer is {}.", 42);
 */
template <typename... T>
FMT_NODISCARD FMT_INLINE auto format(format_string<T...> fmt, T&&... args)
    -> std::string {
  return vformat(fmt.str, vargs<T...>{{args...}});
}

/**
 * Converts `value` to `std::string` using the default format for type `T`.
 *
 * **Example**:
 *
 *     std::string answer = fmt::to_string(42);
 */
template <typename T, FMT_ENABLE_IF(std::is_integral<T>::value)>
FMT_NODISCARD FMT_CONSTEXPR_STRING auto to_string(T value) -> std::string {
  // The buffer should be large enough to store the number including the sign
  // or "false" for bool.
  char buffer[max_of(detail::digits10<T>() + 2, 5)];
  return {buffer, detail::write<char>(buffer, value)};
}

template <typename T, FMT_ENABLE_IF(detail::use_format_as<T>::value)>
FMT_NODISCARD FMT_CONSTEXPR_STRING auto to_string(const T& value)
    -> std::string {
  return to_string(format_as(value));
}

template <typename T, FMT_ENABLE_IF(!std::is_integral<T>::value &&
                                    !detail::use_format_as<T>::value)>
FMT_NODISCARD FMT_CONSTEXPR_STRING auto to_string(const T& value)
    -> std::string {
  auto buffer = memory_buffer();
  detail::write<char>(appender(buffer), value);
  return {buffer.data(), buffer.size()};
}

FMT_END_EXPORT
FMT_END_NAMESPACE

#ifdef FMT_HEADER_ONLY
#  define FMT_FUNC inline
// Formatting library for C++ - implementation
//
// Copyright (c) 2012 - 2016, Victor Zverovich
// All rights reserved.
//
// For the license information refer to format.h.

#ifndef FMT_FORMAT_INL_H_
#define FMT_FORMAT_INL_H_

#ifndef FMT_MODULE
#  include <algorithm>
#  include <cerrno>  // errno
#  include <climits>
#  include <cmath>
#  include <exception>
#endif

#if defined(_WIN32) && !defined(FMT_USE_WRITE_CONSOLE)
#  include <io.h>  // _isatty
#endif

// NOTE disabled
// #include "format.h"

#if FMT_USE_LOCALE && !defined(FMT_MODULE)
#  include <locale>
#endif

#ifndef FMT_FUNC
#  define FMT_FUNC
#endif

FMT_BEGIN_NAMESPACE
namespace detail {

FMT_FUNC void assert_fail(const char* file, int line, const char* message) {
  // Use unchecked std::fprintf to avoid triggering another assertion when
  // writing to stderr fails.
  fprintf(stderr, "%s:%d: assertion failed: %s", file, line, message);
  abort();
}

FMT_FUNC void format_error_code(detail::buffer<char>& out, int error_code,
                                string_view message) noexcept {
  // Report error code making sure that the output fits into
  // inline_buffer_size to avoid dynamic memory allocation and potential
  // bad_alloc.
  out.try_resize(0);
  static const char SEP[] = ": ";
  static const char ERROR_STR[] = "error ";
  // Subtract 2 to account for terminating null characters in SEP and ERROR_STR.
  size_t error_code_size = sizeof(SEP) + sizeof(ERROR_STR) - 2;
  auto abs_value = static_cast<uint32_or_64_or_128_t<int>>(error_code);
  if (detail::is_negative(error_code)) {
    abs_value = 0 - abs_value;
    ++error_code_size;
  }
  error_code_size += detail::to_unsigned(detail::count_digits(abs_value));
  auto it = appender(out);
  if (message.size() <= inline_buffer_size - error_code_size)
    fmt::format_to(it, FMT_STRING("{}{}"), message, SEP);
  fmt::format_to(it, FMT_STRING("{}{}"), ERROR_STR, error_code);
  FMT_ASSERT(out.size() <= inline_buffer_size, "");
}

FMT_FUNC void do_report_error(format_func func, int error_code,
                              const char* message) noexcept {
  memory_buffer full_message;
  func(full_message, error_code, message);
  // Don't use fwrite_all because the latter may throw.
  if (std::fwrite(full_message.data(), full_message.size(), 1, stderr) > 0)
    std::fputc('\n', stderr);
}

// A wrapper around fwrite that throws on error.
inline void fwrite_all(const void* ptr, size_t count, FILE* stream) {
  size_t written = std::fwrite(ptr, 1, count, stream);
  if (written < count)
    FMT_THROW(system_error(errno, FMT_STRING("cannot write to file")));
}

#if FMT_USE_LOCALE
using std::locale;
using std::numpunct;
using std::use_facet;

template <typename Locale>
locale_ref::locale_ref(const Locale& loc) : locale_(&loc) {
  static_assert(std::is_same<Locale, locale>::value, "");
}
#else
struct locale {};
template <typename Char> struct numpunct {
  auto grouping() const -> std::string { return "\03"; }
  auto thousands_sep() const -> Char { return ','; }
  auto decimal_point() const -> Char { return '.'; }
};
template <typename Facet> Facet use_facet(locale) { return {}; }
#endif  // FMT_USE_LOCALE

template <typename Locale> auto locale_ref::get() const -> Locale {
  static_assert(std::is_same<Locale, locale>::value, "");
#if FMT_USE_LOCALE
  if (locale_) return *static_cast<const locale*>(locale_);
#endif
  return locale();
}

template <typename Char>
FMT_FUNC auto thousands_sep_impl(locale_ref loc) -> thousands_sep_result<Char> {
  auto&& facet = use_facet<numpunct<Char>>(loc.get<locale>());
  auto grouping = facet.grouping();
  auto thousands_sep = grouping.empty() ? Char() : facet.thousands_sep();
  return {std::move(grouping), thousands_sep};
}
template <typename Char>
FMT_FUNC auto decimal_point_impl(locale_ref loc) -> Char {
  return use_facet<numpunct<Char>>(loc.get<locale>()).decimal_point();
}

#if FMT_USE_LOCALE
FMT_FUNC auto write_loc(appender out, loc_value value,
                        const format_specs& specs, locale_ref loc) -> bool {
  auto locale = loc.get<std::locale>();
  // We cannot use the num_put<char> facet because it may produce output in
  // a wrong encoding.
  using facet = format_facet<std::locale>;
  if (std::has_facet<facet>(locale))
    return use_facet<facet>(locale).put(out, value, specs);
  return facet(locale).put(out, value, specs);
}
#endif
}  // namespace detail

FMT_FUNC void report_error(const char* message) {
#if FMT_USE_EXCEPTIONS
  // Use FMT_THROW instead of throw to avoid bogus unreachable code warnings
  // from MSVC.
  FMT_THROW(format_error(message));
#else
  // Silence unreachable code warnings in MSVC.
  volatile bool b = true;
  if (b) {
    fputs(message, stderr);
    abort();
  }
#endif
}

template <typename Locale> typename Locale::id format_facet<Locale>::id;

template <typename Locale> format_facet<Locale>::format_facet(Locale& loc) {
  auto& np = detail::use_facet<detail::numpunct<char>>(loc);
  grouping_ = np.grouping();
  if (!grouping_.empty()) separator_ = std::string(1, np.thousands_sep());
}

#if FMT_USE_LOCALE
template <>
FMT_API FMT_FUNC auto format_facet<std::locale>::do_put(
    appender out, loc_value val, const format_specs& specs) const -> bool {
  return val.visit(
      detail::loc_writer<>{out, specs, separator_, grouping_, decimal_point_});
}
#endif

FMT_FUNC auto vsystem_error(int error_code, string_view fmt, format_args args)
    -> std::system_error {
  auto ec = std::error_code(error_code, std::generic_category());
  return std::system_error(ec, vformat(fmt, args));
}

namespace detail {

template <typename F>
inline auto operator==(basic_fp<F> x, basic_fp<F> y) -> bool {
  return x.f == y.f && x.e == y.e;
}

// Compilers should be able to optimize this into the ror instruction.
FMT_INLINE auto rotr(uint32_t n, uint32_t r) noexcept -> uint32_t {
  r &= 31;
  return (n >> r) | (n << (32 - r));
}
FMT_INLINE auto rotr(uint64_t n, uint32_t r) noexcept -> uint64_t {
  r &= 63;
  return (n >> r) | (n << (64 - r));
}

// Implementation of Dragonbox algorithm: https://github.com/jk-jeon/dragonbox.
namespace dragonbox {
// Computes upper 64 bits of multiplication of a 32-bit unsigned integer and a
// 64-bit unsigned integer.
inline auto umul96_upper64(uint32_t x, uint64_t y) noexcept -> uint64_t {
  return umul128_upper64(static_cast<uint64_t>(x) << 32, y);
}

// Computes lower 128 bits of multiplication of a 64-bit unsigned integer and a
// 128-bit unsigned integer.
inline auto umul192_lower128(uint64_t x, uint128_fallback y) noexcept
    -> uint128_fallback {
  uint64_t high = x * y.high();
  uint128_fallback high_low = umul128(x, y.low());
  return {high + high_low.high(), high_low.low()};
}

// Computes lower 64 bits of multiplication of a 32-bit unsigned integer and a
// 64-bit unsigned integer.
inline auto umul96_lower64(uint32_t x, uint64_t y) noexcept -> uint64_t {
  return x * y;
}

// Various fast log computations.
inline auto floor_log10_pow2_minus_log10_4_over_3(int e) noexcept -> int {
  FMT_ASSERT(e <= 2936 && e >= -2985, "too large exponent");
  return (e * 631305 - 261663) >> 21;
}

FMT_INLINE_VARIABLE constexpr struct div_small_pow10_infos_struct {
  uint32_t divisor;
  int shift_amount;
} div_small_pow10_infos[] = {{10, 16}, {100, 16}};

// Replaces n by floor(n / pow(10, N)) returning true if and only if n is
// divisible by pow(10, N).
// Precondition: n <= pow(10, N + 1).
template <int N>
auto check_divisibility_and_divide_by_pow10(uint32_t& n) noexcept -> bool {
  // The numbers below are chosen such that:
  //   1. floor(n/d) = floor(nm / 2^k) where d=10 or d=100,
  //   2. nm mod 2^k < m if and only if n is divisible by d,
  // where m is magic_number, k is shift_amount
  // and d is divisor.
  //
  // Item 1 is a common technique of replacing division by a constant with
  // multiplication, see e.g. "Division by Invariant Integers Using
  // Multiplication" by Granlund and Montgomery (1994). magic_number (m) is set
  // to ceil(2^k/d) for large enough k.
  // The idea for item 2 originates from Schubfach.
  constexpr auto info = div_small_pow10_infos[N - 1];
  FMT_ASSERT(n <= info.divisor * 10, "n is too large");
  constexpr uint32_t magic_number =
      (1u << info.shift_amount) / info.divisor + 1;
  n *= magic_number;
  const uint32_t comparison_mask = (1u << info.shift_amount) - 1;
  bool result = (n & comparison_mask) < magic_number;
  n >>= info.shift_amount;
  return result;
}

// Computes floor(n / pow(10, N)) for small n and N.
// Precondition: n <= pow(10, N + 1).
template <int N> auto small_division_by_pow10(uint32_t n) noexcept -> uint32_t {
  constexpr auto info = div_small_pow10_infos[N - 1];
  FMT_ASSERT(n <= info.divisor * 10, "n is too large");
  constexpr uint32_t magic_number =
      (1u << info.shift_amount) / info.divisor + 1;
  return (n * magic_number) >> info.shift_amount;
}

// Computes floor(n / 10^(kappa + 1)) (float)
inline auto divide_by_10_to_kappa_plus_1(uint32_t n) noexcept -> uint32_t {
  // 1374389535 = ceil(2^37/100)
  return static_cast<uint32_t>((static_cast<uint64_t>(n) * 1374389535) >> 37);
}
// Computes floor(n / 10^(kappa + 1)) (double)
inline auto divide_by_10_to_kappa_plus_1(uint64_t n) noexcept -> uint64_t {
  // 2361183241434822607 = ceil(2^(64+7)/1000)
  return umul128_upper64(n, 2361183241434822607ull) >> 7;
}

// Various subroutines using pow10 cache
template <typename T> struct cache_accessor;

template <> struct cache_accessor<float> {
  using carrier_uint = float_info<float>::carrier_uint;
  using cache_entry_type = uint64_t;

  static auto get_cached_power(int k) noexcept -> uint64_t {
    FMT_ASSERT(k >= float_info<float>::min_k && k <= float_info<float>::max_k,
               "k is out of range");
    static constexpr uint64_t pow10_significands[] = {
        0x81ceb32c4b43fcf5, 0xa2425ff75e14fc32, 0xcad2f7f5359a3b3f,
        0xfd87b5f28300ca0e, 0x9e74d1b791e07e49, 0xc612062576589ddb,
        0xf79687aed3eec552, 0x9abe14cd44753b53, 0xc16d9a0095928a28,
        0xf1c90080baf72cb2, 0x971da05074da7bef, 0xbce5086492111aeb,
        0xec1e4a7db69561a6, 0x9392ee8e921d5d08, 0xb877aa3236a4b44a,
        0xe69594bec44de15c, 0x901d7cf73ab0acda, 0xb424dc35095cd810,
        0xe12e13424bb40e14, 0x8cbccc096f5088cc, 0xafebff0bcb24aaff,
        0xdbe6fecebdedd5bf, 0x89705f4136b4a598, 0xabcc77118461cefd,
        0xd6bf94d5e57a42bd, 0x8637bd05af6c69b6, 0xa7c5ac471b478424,
        0xd1b71758e219652c, 0x83126e978d4fdf3c, 0xa3d70a3d70a3d70b,
        0xcccccccccccccccd, 0x8000000000000000, 0xa000000000000000,
        0xc800000000000000, 0xfa00000000000000, 0x9c40000000000000,
        0xc350000000000000, 0xf424000000000000, 0x9896800000000000,
        0xbebc200000000000, 0xee6b280000000000, 0x9502f90000000000,
        0xba43b74000000000, 0xe8d4a51000000000, 0x9184e72a00000000,
        0xb5e620f480000000, 0xe35fa931a0000000, 0x8e1bc9bf04000000,
        0xb1a2bc2ec5000000, 0xde0b6b3a76400000, 0x8ac7230489e80000,
        0xad78ebc5ac620000, 0xd8d726b7177a8000, 0x878678326eac9000,
        0xa968163f0a57b400, 0xd3c21bcecceda100, 0x84595161401484a0,
        0xa56fa5b99019a5c8, 0xcecb8f27f4200f3a, 0x813f3978f8940985,
        0xa18f07d736b90be6, 0xc9f2c9cd04674edf, 0xfc6f7c4045812297,
        0x9dc5ada82b70b59e, 0xc5371912364ce306, 0xf684df56c3e01bc7,
        0x9a130b963a6c115d, 0xc097ce7bc90715b4, 0xf0bdc21abb48db21,
        0x96769950b50d88f5, 0xbc143fa4e250eb32, 0xeb194f8e1ae525fe,
        0x92efd1b8d0cf37bf, 0xb7abc627050305ae, 0xe596b7b0c643c71a,
        0x8f7e32ce7bea5c70, 0xb35dbf821ae4f38c, 0xe0352f62a19e306f};
    return pow10_significands[k - float_info<float>::min_k];
  }

  struct compute_mul_result {
    carrier_uint result;
    bool is_integer;
  };
  struct compute_mul_parity_result {
    bool parity;
    bool is_integer;
  };

  static auto compute_mul(carrier_uint u,
                          const cache_entry_type& cache) noexcept
      -> compute_mul_result {
    auto r = umul96_upper64(u, cache);
    return {static_cast<carrier_uint>(r >> 32),
            static_cast<carrier_uint>(r) == 0};
  }

  static auto compute_delta(const cache_entry_type& cache, int beta) noexcept
      -> uint32_t {
    return static_cast<uint32_t>(cache >> (64 - 1 - beta));
  }

  static auto compute_mul_parity(carrier_uint two_f,
                                 const cache_entry_type& cache,
                                 int beta) noexcept
      -> compute_mul_parity_result {
    FMT_ASSERT(beta >= 1, "");
    FMT_ASSERT(beta < 64, "");

    auto r = umul96_lower64(two_f, cache);
    return {((r >> (64 - beta)) & 1) != 0,
            static_cast<uint32_t>(r >> (32 - beta)) == 0};
  }

  static auto compute_left_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta) noexcept -> carrier_uint {
    return static_cast<carrier_uint>(
        (cache - (cache >> (num_significand_bits<float>() + 2))) >>
        (64 - num_significand_bits<float>() - 1 - beta));
  }

  static auto compute_right_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta) noexcept -> carrier_uint {
    return static_cast<carrier_uint>(
        (cache + (cache >> (num_significand_bits<float>() + 1))) >>
        (64 - num_significand_bits<float>() - 1 - beta));
  }

  static auto compute_round_up_for_shorter_interval_case(
      const cache_entry_type& cache, int beta) noexcept -> carrier_uint {
    return (static_cast<carrier_uint>(
                cache >> (64 - num_significand_bits<float>() - 2 - beta)) +
            1) /
           2;
  }
};

template <> struct cache_accessor<double> {
  using carrier_uint = float_info<double>::carrier_uint;
  using cache_entry_type = uint128_fallback;

  static auto get_cached_power(int k) noexcept -> uint128_fallback {
    FMT_ASSERT(k >= float_info<double>::min_k && k <= float_info<double>::max_k,
               "k is out of range");

    static constexpr uint128_fallback pow10_significands[] = {
#if FMT_USE_FULL_CACHE_DRAGONBOX
      {0xff77b1fcbebcdc4f, 0x25e8e89c13bb0f7b},
      {0x9faacf3df73609b1, 0x77b191618c54e9ad},
      {0xc795830d75038c1d, 0xd59df5b9ef6a2418},
      {0xf97ae3d0d2446f25, 0x4b0573286b44ad1e},
      {0x9becce62836ac577, 0x4ee367f9430aec33},
      {0xc2e801fb244576d5, 0x229c41f793cda740},
      {0xf3a20279ed56d48a, 0x6b43527578c11110},
      {0x9845418c345644d6, 0x830a13896b78aaaa},
      {0xbe5691ef416bd60c, 0x23cc986bc656d554},
      {0xedec366b11c6cb8f, 0x2cbfbe86b7ec8aa9},
      {0x94b3a202eb1c3f39, 0x7bf7d71432f3d6aa},
      {0xb9e08a83a5e34f07, 0xdaf5ccd93fb0cc54},
      {0xe858ad248f5c22c9, 0xd1b3400f8f9cff69},
      {0x91376c36d99995be, 0x23100809b9c21fa2},
      {0xb58547448ffffb2d, 0xabd40a0c2832a78b},
      {0xe2e69915b3fff9f9, 0x16c90c8f323f516d},
      {0x8dd01fad907ffc3b, 0xae3da7d97f6792e4},
      {0xb1442798f49ffb4a, 0x99cd11cfdf41779d},
      {0xdd95317f31c7fa1d, 0x40405643d711d584},
      {0x8a7d3eef7f1cfc52, 0x482835ea666b2573},
      {0xad1c8eab5ee43b66, 0xda3243650005eed0},
      {0xd863b256369d4a40, 0x90bed43e40076a83},
      {0x873e4f75e2224e68, 0x5a7744a6e804a292},
      {0xa90de3535aaae202, 0x711515d0a205cb37},
      {0xd3515c2831559a83, 0x0d5a5b44ca873e04},
      {0x8412d9991ed58091, 0xe858790afe9486c3},
      {0xa5178fff668ae0b6, 0x626e974dbe39a873},
      {0xce5d73ff402d98e3, 0xfb0a3d212dc81290},
      {0x80fa687f881c7f8e, 0x7ce66634bc9d0b9a},
      {0xa139029f6a239f72, 0x1c1fffc1ebc44e81},
      {0xc987434744ac874e, 0xa327ffb266b56221},
      {0xfbe9141915d7a922, 0x4bf1ff9f0062baa9},
      {0x9d71ac8fada6c9b5, 0x6f773fc3603db4aa},
      {0xc4ce17b399107c22, 0xcb550fb4384d21d4},
      {0xf6019da07f549b2b, 0x7e2a53a146606a49},
      {0x99c102844f94e0fb, 0x2eda7444cbfc426e},
      {0xc0314325637a1939, 0xfa911155fefb5309},
      {0xf03d93eebc589f88, 0x793555ab7eba27cb},
      {0x96267c7535b763b5, 0x4bc1558b2f3458df},
      {0xbbb01b9283253ca2, 0x9eb1aaedfb016f17},
      {0xea9c227723ee8bcb, 0x465e15a979c1cadd},
      {0x92a1958a7675175f, 0x0bfacd89ec191eca},
      {0xb749faed14125d36, 0xcef980ec671f667c},
      {0xe51c79a85916f484, 0x82b7e12780e7401b},
      {0x8f31cc0937ae58d2, 0xd1b2ecb8b0908811},
      {0xb2fe3f0b8599ef07, 0x861fa7e6dcb4aa16},
      {0xdfbdcece67006ac9, 0x67a791e093e1d49b},
      {0x8bd6a141006042bd, 0xe0c8bb2c5c6d24e1},
      {0xaecc49914078536d, 0x58fae9f773886e19},
      {0xda7f5bf590966848, 0xaf39a475506a899f},
      {0x888f99797a5e012d, 0x6d8406c952429604},
      {0xaab37fd7d8f58178, 0xc8e5087ba6d33b84},
      {0xd5605fcdcf32e1d6, 0xfb1e4a9a90880a65},
      {0x855c3be0a17fcd26, 0x5cf2eea09a550680},
      {0xa6b34ad8c9dfc06f, 0xf42faa48c0ea481f},
      {0xd0601d8efc57b08b, 0xf13b94daf124da27},
      {0x823c12795db6ce57, 0x76c53d08d6b70859},
      {0xa2cb1717b52481ed, 0x54768c4b0c64ca6f},
      {0xcb7ddcdda26da268, 0xa9942f5dcf7dfd0a},
      {0xfe5d54150b090b02, 0xd3f93b35435d7c4d},
      {0x9efa548d26e5a6e1, 0xc47bc5014a1a6db0},
      {0xc6b8e9b0709f109a, 0x359ab6419ca1091c},
      {0xf867241c8cc6d4c0, 0xc30163d203c94b63},
      {0x9b407691d7fc44f8, 0x79e0de63425dcf1e},
      {0xc21094364dfb5636, 0x985915fc12f542e5},
      {0xf294b943e17a2bc4, 0x3e6f5b7b17b2939e},
      {0x979cf3ca6cec5b5a, 0xa705992ceecf9c43},
      {0xbd8430bd08277231, 0x50c6ff782a838354},
      {0xece53cec4a314ebd, 0xa4f8bf5635246429},
      {0x940f4613ae5ed136, 0x871b7795e136be9a},
      {0xb913179899f68584, 0x28e2557b59846e40},
      {0xe757dd7ec07426e5, 0x331aeada2fe589d0},
      {0x9096ea6f3848984f, 0x3ff0d2c85def7622},
      {0xb4bca50b065abe63, 0x0fed077a756b53aa},
      {0xe1ebce4dc7f16dfb, 0xd3e8495912c62895},
      {0x8d3360f09cf6e4bd, 0x64712dd7abbbd95d},
      {0xb080392cc4349dec, 0xbd8d794d96aacfb4},
      {0xdca04777f541c567, 0xecf0d7a0fc5583a1},
      {0x89e42caaf9491b60, 0xf41686c49db57245},
      {0xac5d37d5b79b6239, 0x311c2875c522ced6},
      {0xd77485cb25823ac7, 0x7d633293366b828c},
      {0x86a8d39ef77164bc, 0xae5dff9c02033198},
      {0xa8530886b54dbdeb, 0xd9f57f830283fdfd},
      {0xd267caa862a12d66, 0xd072df63c324fd7c},
      {0x8380dea93da4bc60, 0x4247cb9e59f71e6e},
      {0xa46116538d0deb78, 0x52d9be85f074e609},
      {0xcd795be870516656, 0x67902e276c921f8c},
      {0x806bd9714632dff6, 0x00ba1cd8a3db53b7},
      {0xa086cfcd97bf97f3, 0x80e8a40eccd228a5},
      {0xc8a883c0fdaf7df0, 0x6122cd128006b2ce},
      {0xfad2a4b13d1b5d6c, 0x796b805720085f82},
      {0x9cc3a6eec6311a63, 0xcbe3303674053bb1},
      {0xc3f490aa77bd60fc, 0xbedbfc4411068a9d},
      {0xf4f1b4d515acb93b, 0xee92fb5515482d45},
      {0x991711052d8bf3c5, 0x751bdd152d4d1c4b},
      {0xbf5cd54678eef0b6, 0xd262d45a78a0635e},
      {0xef340a98172aace4, 0x86fb897116c87c35},
      {0x9580869f0e7aac0e, 0xd45d35e6ae3d4da1},
      {0xbae0a846d2195712, 0x8974836059cca10a},
      {0xe998d258869facd7, 0x2bd1a438703fc94c},
      {0x91ff83775423cc06, 0x7b6306a34627ddd0},
      {0xb67f6455292cbf08, 0x1a3bc84c17b1d543},
      {0xe41f3d6a7377eeca, 0x20caba5f1d9e4a94},
      {0x8e938662882af53e, 0x547eb47b7282ee9d},
      {0xb23867fb2a35b28d, 0xe99e619a4f23aa44},
      {0xdec681f9f4c31f31, 0x6405fa00e2ec94d5},
      {0x8b3c113c38f9f37e, 0xde83bc408dd3dd05},
      {0xae0b158b4738705e, 0x9624ab50b148d446},
      {0xd98ddaee19068c76, 0x3badd624dd9b0958},
      {0x87f8a8d4cfa417c9, 0xe54ca5d70a80e5d7},
      {0xa9f6d30a038d1dbc, 0x5e9fcf4ccd211f4d},
      {0xd47487cc8470652b, 0x7647c32000696720},
      {0x84c8d4dfd2c63f3b, 0x29ecd9f40041e074},
      {0xa5fb0a17c777cf09, 0xf468107100525891},
      {0xcf79cc9db955c2cc, 0x7182148d4066eeb5},
      {0x81ac1fe293d599bf, 0xc6f14cd848405531},
      {0xa21727db38cb002f, 0xb8ada00e5a506a7d},
      {0xca9cf1d206fdc03b, 0xa6d90811f0e4851d},
      {0xfd442e4688bd304a, 0x908f4a166d1da664},
      {0x9e4a9cec15763e2e, 0x9a598e4e043287ff},
      {0xc5dd44271ad3cdba, 0x40eff1e1853f29fe},
      {0xf7549530e188c128, 0xd12bee59e68ef47d},
      {0x9a94dd3e8cf578b9, 0x82bb74f8301958cf},
      {0xc13a148e3032d6e7, 0xe36a52363c1faf02},
      {0xf18899b1bc3f8ca1, 0xdc44e6c3cb279ac2},
      {0x96f5600f15a7b7e5, 0x29ab103a5ef8c0ba},
      {0xbcb2b812db11a5de, 0x7415d448f6b6f0e8},
      {0xebdf661791d60f56, 0x111b495b3464ad22},
      {0x936b9fcebb25c995, 0xcab10dd900beec35},
      {0xb84687c269ef3bfb, 0x3d5d514f40eea743},
      {0xe65829b3046b0afa, 0x0cb4a5a3112a5113},
      {0x8ff71a0fe2c2e6dc, 0x47f0e785eaba72ac},
      {0xb3f4e093db73a093, 0x59ed216765690f57},
      {0xe0f218b8d25088b8, 0x306869c13ec3532d},
      {0x8c974f7383725573, 0x1e414218c73a13fc},
      {0xafbd2350644eeacf, 0xe5d1929ef90898fb},
      {0xdbac6c247d62a583, 0xdf45f746b74abf3a},
      {0x894bc396ce5da772, 0x6b8bba8c328eb784},
      {0xab9eb47c81f5114f, 0x066ea92f3f326565},
      {0xd686619ba27255a2, 0xc80a537b0efefebe},
      {0x8613fd0145877585, 0xbd06742ce95f5f37},
      {0xa798fc4196e952e7, 0x2c48113823b73705},
      {0xd17f3b51fca3a7a0, 0xf75a15862ca504c6},
      {0x82ef85133de648c4, 0x9a984d73dbe722fc},
      {0xa3ab66580d5fdaf5, 0xc13e60d0d2e0ebbb},
      {0xcc963fee10b7d1b3, 0x318df905079926a9},
      {0xffbbcfe994e5c61f, 0xfdf17746497f7053},
      {0x9fd561f1fd0f9bd3, 0xfeb6ea8bedefa634},
      {0xc7caba6e7c5382c8, 0xfe64a52ee96b8fc1},
      {0xf9bd690a1b68637b, 0x3dfdce7aa3c673b1},
      {0x9c1661a651213e2d, 0x06bea10ca65c084f},
      {0xc31bfa0fe5698db8, 0x486e494fcff30a63},
      {0xf3e2f893dec3f126, 0x5a89dba3c3efccfb},
      {0x986ddb5c6b3a76b7, 0xf89629465a75e01d},
      {0xbe89523386091465, 0xf6bbb397f1135824},
      {0xee2ba6c0678b597f, 0x746aa07ded582e2d},
      {0x94db483840b717ef, 0xa8c2a44eb4571cdd},
      {0xba121a4650e4ddeb, 0x92f34d62616ce414},
      {0xe896a0d7e51e1566, 0x77b020baf9c81d18},
      {0x915e2486ef32cd60, 0x0ace1474dc1d122f},
      {0xb5b5ada8aaff80b8, 0x0d819992132456bb},
      {0xe3231912d5bf60e6, 0x10e1fff697ed6c6a},
      {0x8df5efabc5979c8f, 0xca8d3ffa1ef463c2},
      {0xb1736b96b6fd83b3, 0xbd308ff8a6b17cb3},
      {0xddd0467c64bce4a0, 0xac7cb3f6d05ddbdf},
      {0x8aa22c0dbef60ee4, 0x6bcdf07a423aa96c},
      {0xad4ab7112eb3929d, 0x86c16c98d2c953c7},
      {0xd89d64d57a607744, 0xe871c7bf077ba8b8},
      {0x87625f056c7c4a8b, 0x11471cd764ad4973},
      {0xa93af6c6c79b5d2d, 0xd598e40d3dd89bd0},
      {0xd389b47879823479, 0x4aff1d108d4ec2c4},
      {0x843610cb4bf160cb, 0xcedf722a585139bb},
      {0xa54394fe1eedb8fe, 0xc2974eb4ee658829},
      {0xce947a3da6a9273e, 0x733d226229feea33},
      {0x811ccc668829b887, 0x0806357d5a3f5260},
      {0xa163ff802a3426a8, 0xca07c2dcb0cf26f8},
      {0xc9bcff6034c13052, 0xfc89b393dd02f0b6},
      {0xfc2c3f3841f17c67, 0xbbac2078d443ace3},
      {0x9d9ba7832936edc0, 0xd54b944b84aa4c0e},
      {0xc5029163f384a931, 0x0a9e795e65d4df12},
      {0xf64335bcf065d37d, 0x4d4617b5ff4a16d6},
      {0x99ea0196163fa42e, 0x504bced1bf8e4e46},
      {0xc06481fb9bcf8d39, 0xe45ec2862f71e1d7},
      {0xf07da27a82c37088, 0x5d767327bb4e5a4d},
      {0x964e858c91ba2655, 0x3a6a07f8d510f870},
      {0xbbe226efb628afea, 0x890489f70a55368c},
      {0xeadab0aba3b2dbe5, 0x2b45ac74ccea842f},
      {0x92c8ae6b464fc96f, 0x3b0b8bc90012929e},
      {0xb77ada0617e3bbcb, 0x09ce6ebb40173745},
      {0xe55990879ddcaabd, 0xcc420a6a101d0516},
      {0x8f57fa54c2a9eab6, 0x9fa946824a12232e},
      {0xb32df8e9f3546564, 0x47939822dc96abfa},
      {0xdff9772470297ebd, 0x59787e2b93bc56f8},
      {0x8bfbea76c619ef36, 0x57eb4edb3c55b65b},
      {0xaefae51477a06b03, 0xede622920b6b23f2},
      {0xdab99e59958885c4, 0xe95fab368e45ecee},
      {0x88b402f7fd75539b, 0x11dbcb0218ebb415},
      {0xaae103b5fcd2a881, 0xd652bdc29f26a11a},
      {0xd59944a37c0752a2, 0x4be76d3346f04960},
      {0x857fcae62d8493a5, 0x6f70a4400c562ddc},
      {0xa6dfbd9fb8e5b88e, 0xcb4ccd500f6bb953},
      {0xd097ad07a71f26b2, 0x7e2000a41346a7a8},
      {0x825ecc24c873782f, 0x8ed400668c0c28c9},
      {0xa2f67f2dfa90563b, 0x728900802f0f32fb},
      {0xcbb41ef979346bca, 0x4f2b40a03ad2ffba},
      {0xfea126b7d78186bc, 0xe2f610c84987bfa9},
      {0x9f24b832e6b0f436, 0x0dd9ca7d2df4d7ca},
      {0xc6ede63fa05d3143, 0x91503d1c79720dbc},
      {0xf8a95fcf88747d94, 0x75a44c6397ce912b},
      {0x9b69dbe1b548ce7c, 0xc986afbe3ee11abb},
      {0xc24452da229b021b, 0xfbe85badce996169},
      {0xf2d56790ab41c2a2, 0xfae27299423fb9c4},
      {0x97c560ba6b0919a5, 0xdccd879fc967d41b},
      {0xbdb6b8e905cb600f, 0x5400e987bbc1c921},
      {0xed246723473e3813, 0x290123e9aab23b69},
      {0x9436c0760c86e30b, 0xf9a0b6720aaf6522},
      {0xb94470938fa89bce, 0xf808e40e8d5b3e6a},
      {0xe7958cb87392c2c2, 0xb60b1d1230b20e05},
      {0x90bd77f3483bb9b9, 0xb1c6f22b5e6f48c3},
      {0xb4ecd5f01a4aa828, 0x1e38aeb6360b1af4},
      {0xe2280b6c20dd5232, 0x25c6da63c38de1b1},
      {0x8d590723948a535f, 0x579c487e5a38ad0f},
      {0xb0af48ec79ace837, 0x2d835a9df0c6d852},
      {0xdcdb1b2798182244, 0xf8e431456cf88e66},
      {0x8a08f0f8bf0f156b, 0x1b8e9ecb641b5900},
      {0xac8b2d36eed2dac5, 0xe272467e3d222f40},
      {0xd7adf884aa879177, 0x5b0ed81dcc6abb10},
      {0x86ccbb52ea94baea, 0x98e947129fc2b4ea},
      {0xa87fea27a539e9a5, 0x3f2398d747b36225},
      {0xd29fe4b18e88640e, 0x8eec7f0d19a03aae},
      {0x83a3eeeef9153e89, 0x1953cf68300424ad},
      {0xa48ceaaab75a8e2b, 0x5fa8c3423c052dd8},
      {0xcdb02555653131b6, 0x3792f412cb06794e},
      {0x808e17555f3ebf11, 0xe2bbd88bbee40bd1},
      {0xa0b19d2ab70e6ed6, 0x5b6aceaeae9d0ec5},
      {0xc8de047564d20a8b, 0xf245825a5a445276},
      {0xfb158592be068d2e, 0xeed6e2f0f0d56713},
      {0x9ced737bb6c4183d, 0x55464dd69685606c},
      {0xc428d05aa4751e4c, 0xaa97e14c3c26b887},
      {0xf53304714d9265df, 0xd53dd99f4b3066a9},
      {0x993fe2c6d07b7fab, 0xe546a8038efe402a},
      {0xbf8fdb78849a5f96, 0xde98520472bdd034},
      {0xef73d256a5c0f77c, 0x963e66858f6d4441},
      {0x95a8637627989aad, 0xdde7001379a44aa9},
      {0xbb127c53b17ec159, 0x5560c018580d5d53},
      {0xe9d71b689dde71af, 0xaab8f01e6e10b4a7},
      {0x9226712162ab070d, 0xcab3961304ca70e9},
      {0xb6b00d69bb55c8d1, 0x3d607b97c5fd0d23},
      {0xe45c10c42a2b3b05, 0x8cb89a7db77c506b},
      {0x8eb98a7a9a5b04e3, 0x77f3608e92adb243},
      {0xb267ed1940f1c61c, 0x55f038b237591ed4},
      {0xdf01e85f912e37a3, 0x6b6c46dec52f6689},
      {0x8b61313bbabce2c6, 0x2323ac4b3b3da016},
      {0xae397d8aa96c1b77, 0xabec975e0a0d081b},
      {0xd9c7dced53c72255, 0x96e7bd358c904a22},
      {0x881cea14545c7575, 0x7e50d64177da2e55},
      {0xaa242499697392d2, 0xdde50bd1d5d0b9ea},
      {0xd4ad2dbfc3d07787, 0x955e4ec64b44e865},
      {0x84ec3c97da624ab4, 0xbd5af13bef0b113f},
      {0xa6274bbdd0fadd61, 0xecb1ad8aeacdd58f},
      {0xcfb11ead453994ba, 0x67de18eda5814af3},
      {0x81ceb32c4b43fcf4, 0x80eacf948770ced8},
      {0xa2425ff75e14fc31, 0xa1258379a94d028e},
      {0xcad2f7f5359a3b3e, 0x096ee45813a04331},
      {0xfd87b5f28300ca0d, 0x8bca9d6e188853fd},
      {0x9e74d1b791e07e48, 0x775ea264cf55347e},
      {0xc612062576589dda, 0x95364afe032a819e},
      {0xf79687aed3eec551, 0x3a83ddbd83f52205},
      {0x9abe14cd44753b52, 0xc4926a9672793543},
      {0xc16d9a0095928a27, 0x75b7053c0f178294},
      {0xf1c90080baf72cb1, 0x5324c68b12dd6339},
      {0x971da05074da7bee, 0xd3f6fc16ebca5e04},
      {0xbce5086492111aea, 0x88f4bb1ca6bcf585},
      {0xec1e4a7db69561a5, 0x2b31e9e3d06c32e6},
      {0x9392ee8e921d5d07, 0x3aff322e62439fd0},
      {0xb877aa3236a4b449, 0x09befeb9fad487c3},
      {0xe69594bec44de15b, 0x4c2ebe687989a9b4},
      {0x901d7cf73ab0acd9, 0x0f9d37014bf60a11},
      {0xb424dc35095cd80f, 0x538484c19ef38c95},
      {0xe12e13424bb40e13, 0x2865a5f206b06fba},
      {0x8cbccc096f5088cb, 0xf93f87b7442e45d4},
      {0xafebff0bcb24aafe, 0xf78f69a51539d749},
      {0xdbe6fecebdedd5be, 0xb573440e5a884d1c},
      {0x89705f4136b4a597, 0x31680a88f8953031},
      {0xabcc77118461cefc, 0xfdc20d2b36ba7c3e},
      {0xd6bf94d5e57a42bc, 0x3d32907604691b4d},
      {0x8637bd05af6c69b5, 0xa63f9a49c2c1b110},
      {0xa7c5ac471b478423, 0x0fcf80dc33721d54},
      {0xd1b71758e219652b, 0xd3c36113404ea4a9},
      {0x83126e978d4fdf3b, 0x645a1cac083126ea},
      {0xa3d70a3d70a3d70a, 0x3d70a3d70a3d70a4},
      {0xcccccccccccccccc, 0xcccccccccccccccd},
      {0x8000000000000000, 0x0000000000000000},
      {0xa000000000000000, 0x0000000000000000},
      {0xc800000000000000, 0x0000000000000000},
      {0xfa00000000000000, 0x0000000000000000},
      {0x9c40000000000000, 0x0000000000000000},
      {0xc350000000000000, 0x0000000000000000},
      {0xf424000000000000, 0x0000000000000000},
      {0x9896800000000000, 0x0000000000000000},
      {0xbebc200000000000, 0x0000000000000000},
      {0xee6b280000000000, 0x0000000000000000},
      {0x9502f90000000000, 0x0000000000000000},
      {0xba43b74000000000, 0x0000000000000000},
      {0xe8d4a51000000000, 0x0000000000000000},
      {0x9184e72a00000000, 0x0000000000000000},
      {0xb5e620f480000000, 0x0000000000000000},
      {0xe35fa931a0000000, 0x0000000000000000},
      {0x8e1bc9bf04000000, 0x0000000000000000},
      {0xb1a2bc2ec5000000, 0x0000000000000000},
      {0xde0b6b3a76400000, 0x0000000000000000},
      {0x8ac7230489e80000, 0x0000000000000000},
      {0xad78ebc5ac620000, 0x0000000000000000},
      {0xd8d726b7177a8000, 0x0000000000000000},
      {0x878678326eac9000, 0x0000000000000000},
      {0xa968163f0a57b400, 0x0000000000000000},
      {0xd3c21bcecceda100, 0x0000000000000000},
      {0x84595161401484a0, 0x0000000000000000},
      {0xa56fa5b99019a5c8, 0x0000000000000000},
      {0xcecb8f27f4200f3a, 0x0000000000000000},
      {0x813f3978f8940984, 0x4000000000000000},
      {0xa18f07d736b90be5, 0x5000000000000000},
      {0xc9f2c9cd04674ede, 0xa400000000000000},
      {0xfc6f7c4045812296, 0x4d00000000000000},
      {0x9dc5ada82b70b59d, 0xf020000000000000},
      {0xc5371912364ce305, 0x6c28000000000000},
      {0xf684df56c3e01bc6, 0xc732000000000000},
      {0x9a130b963a6c115c, 0x3c7f400000000000},
      {0xc097ce7bc90715b3, 0x4b9f100000000000},
      {0xf0bdc21abb48db20, 0x1e86d40000000000},
      {0x96769950b50d88f4, 0x1314448000000000},
      {0xbc143fa4e250eb31, 0x17d955a000000000},
      {0xeb194f8e1ae525fd, 0x5dcfab0800000000},
      {0x92efd1b8d0cf37be, 0x5aa1cae500000000},
      {0xb7abc627050305ad, 0xf14a3d9e40000000},
      {0xe596b7b0c643c719, 0x6d9ccd05d0000000},
      {0x8f7e32ce7bea5c6f, 0xe4820023a2000000},
      {0xb35dbf821ae4f38b, 0xdda2802c8a800000},
      {0xe0352f62a19e306e, 0xd50b2037ad200000},
      {0x8c213d9da502de45, 0x4526f422cc340000},
      {0xaf298d050e4395d6, 0x9670b12b7f410000},
      {0xdaf3f04651d47b4c, 0x3c0cdd765f114000},
      {0x88d8762bf324cd0f, 0xa5880a69fb6ac800},
      {0xab0e93b6efee0053, 0x8eea0d047a457a00},
      {0xd5d238a4abe98068, 0x72a4904598d6d880},
      {0x85a36366eb71f041, 0x47a6da2b7f864750},
      {0xa70c3c40a64e6c51, 0x999090b65f67d924},
      {0xd0cf4b50cfe20765, 0xfff4b4e3f741cf6d},
      {0x82818f1281ed449f, 0xbff8f10e7a8921a5},
      {0xa321f2d7226895c7, 0xaff72d52192b6a0e},
      {0xcbea6f8ceb02bb39, 0x9bf4f8a69f764491},
      {0xfee50b7025c36a08, 0x02f236d04753d5b5},
      {0x9f4f2726179a2245, 0x01d762422c946591},
      {0xc722f0ef9d80aad6, 0x424d3ad2b7b97ef6},
      {0xf8ebad2b84e0d58b, 0xd2e0898765a7deb3},
      {0x9b934c3b330c8577, 0x63cc55f49f88eb30},
      {0xc2781f49ffcfa6d5, 0x3cbf6b71c76b25fc},
      {0xf316271c7fc3908a, 0x8bef464e3945ef7b},
      {0x97edd871cfda3a56, 0x97758bf0e3cbb5ad},
      {0xbde94e8e43d0c8ec, 0x3d52eeed1cbea318},
      {0xed63a231d4c4fb27, 0x4ca7aaa863ee4bde},
      {0x945e455f24fb1cf8, 0x8fe8caa93e74ef6b},
      {0xb975d6b6ee39e436, 0xb3e2fd538e122b45},
      {0xe7d34c64a9c85d44, 0x60dbbca87196b617},
      {0x90e40fbeea1d3a4a, 0xbc8955e946fe31ce},
      {0xb51d13aea4a488dd, 0x6babab6398bdbe42},
      {0xe264589a4dcdab14, 0xc696963c7eed2dd2},
      {0x8d7eb76070a08aec, 0xfc1e1de5cf543ca3},
      {0xb0de65388cc8ada8, 0x3b25a55f43294bcc},
      {0xdd15fe86affad912, 0x49ef0eb713f39ebf},
      {0x8a2dbf142dfcc7ab, 0x6e3569326c784338},
      {0xacb92ed9397bf996, 0x49c2c37f07965405},
      {0xd7e77a8f87daf7fb, 0xdc33745ec97be907},
      {0x86f0ac99b4e8dafd, 0x69a028bb3ded71a4},
      {0xa8acd7c0222311bc, 0xc40832ea0d68ce0d},
      {0xd2d80db02aabd62b, 0xf50a3fa490c30191},
      {0x83c7088e1aab65db, 0x792667c6da79e0fb},
      {0xa4b8cab1a1563f52, 0x577001b891185939},
      {0xcde6fd5e09abcf26, 0xed4c0226b55e6f87},
      {0x80b05e5ac60b6178, 0x544f8158315b05b5},
      {0xa0dc75f1778e39d6, 0x696361ae3db1c722},
      {0xc913936dd571c84c, 0x03bc3a19cd1e38ea},
      {0xfb5878494ace3a5f, 0x04ab48a04065c724},
      {0x9d174b2dcec0e47b, 0x62eb0d64283f9c77},
      {0xc45d1df942711d9a, 0x3ba5d0bd324f8395},
      {0xf5746577930d6500, 0xca8f44ec7ee3647a},
      {0x9968bf6abbe85f20, 0x7e998b13cf4e1ecc},
      {0xbfc2ef456ae276e8, 0x9e3fedd8c321a67f},
      {0xefb3ab16c59b14a2, 0xc5cfe94ef3ea101f},
      {0x95d04aee3b80ece5, 0xbba1f1d158724a13},
      {0xbb445da9ca61281f, 0x2a8a6e45ae8edc98},
      {0xea1575143cf97226, 0xf52d09d71a3293be},
      {0x924d692ca61be758, 0x593c2626705f9c57},
      {0xb6e0c377cfa2e12e, 0x6f8b2fb00c77836d},
      {0xe498f455c38b997a, 0x0b6dfb9c0f956448},
      {0x8edf98b59a373fec, 0x4724bd4189bd5ead},
      {0xb2977ee300c50fe7, 0x58edec91ec2cb658},
      {0xdf3d5e9bc0f653e1, 0x2f2967b66737e3ee},
      {0x8b865b215899f46c, 0xbd79e0d20082ee75},
      {0xae67f1e9aec07187, 0xecd8590680a3aa12},
      {0xda01ee641a708de9, 0xe80e6f4820cc9496},
      {0x884134fe908658b2, 0x3109058d147fdcde},
      {0xaa51823e34a7eede, 0xbd4b46f0599fd416},
      {0xd4e5e2cdc1d1ea96, 0x6c9e18ac7007c91b},
      {0x850fadc09923329e, 0x03e2cf6bc604ddb1},
      {0xa6539930bf6bff45, 0x84db8346b786151d},
      {0xcfe87f7cef46ff16, 0xe612641865679a64},
      {0x81f14fae158c5f6e, 0x4fcb7e8f3f60c07f},
      {0xa26da3999aef7749, 0xe3be5e330f38f09e},
      {0xcb090c8001ab551c, 0x5cadf5bfd3072cc6},
      {0xfdcb4fa002162a63, 0x73d9732fc7c8f7f7},
      {0x9e9f11c4014dda7e, 0x2867e7fddcdd9afb},
      {0xc646d63501a1511d, 0xb281e1fd541501b9},
      {0xf7d88bc24209a565, 0x1f225a7ca91a4227},
      {0x9ae757596946075f, 0x3375788de9b06959},
      {0xc1a12d2fc3978937, 0x0052d6b1641c83af},
      {0xf209787bb47d6b84, 0xc0678c5dbd23a49b},
      {0x9745eb4d50ce6332, 0xf840b7ba963646e1},
      {0xbd176620a501fbff, 0xb650e5a93bc3d899},
      {0xec5d3fa8ce427aff, 0xa3e51f138ab4cebf},
      {0x93ba47c980e98cdf, 0xc66f336c36b10138},
      {0xb8a8d9bbe123f017, 0xb80b0047445d4185},
      {0xe6d3102ad96cec1d, 0xa60dc059157491e6},
      {0x9043ea1ac7e41392, 0x87c89837ad68db30},
      {0xb454e4a179dd1877, 0x29babe4598c311fc},
      {0xe16a1dc9d8545e94, 0xf4296dd6fef3d67b},
      {0x8ce2529e2734bb1d, 0x1899e4a65f58660d},
      {0xb01ae745b101e9e4, 0x5ec05dcff72e7f90},
      {0xdc21a1171d42645d, 0x76707543f4fa1f74},
      {0x899504ae72497eba, 0x6a06494a791c53a9},
      {0xabfa45da0edbde69, 0x0487db9d17636893},
      {0xd6f8d7509292d603, 0x45a9d2845d3c42b7},
      {0x865b86925b9bc5c2, 0x0b8a2392ba45a9b3},
      {0xa7f26836f282b732, 0x8e6cac7768d7141f},
      {0xd1ef0244af2364ff, 0x3207d795430cd927},
      {0x8335616aed761f1f, 0x7f44e6bd49e807b9},
      {0xa402b9c5a8d3a6e7, 0x5f16206c9c6209a7},
      {0xcd036837130890a1, 0x36dba887c37a8c10},
      {0x802221226be55a64, 0xc2494954da2c978a},
      {0xa02aa96b06deb0fd, 0xf2db9baa10b7bd6d},
      {0xc83553c5c8965d3d, 0x6f92829494e5acc8},
      {0xfa42a8b73abbf48c, 0xcb772339ba1f17fa},
      {0x9c69a97284b578d7, 0xff2a760414536efc},
      {0xc38413cf25e2d70d, 0xfef5138519684abb},
      {0xf46518c2ef5b8cd1, 0x7eb258665fc25d6a},
      {0x98bf2f79d5993802, 0xef2f773ffbd97a62},
      {0xbeeefb584aff8603, 0xaafb550ffacfd8fb},
      {0xeeaaba2e5dbf6784, 0x95ba2a53f983cf39},
      {0x952ab45cfa97a0b2, 0xdd945a747bf26184},
      {0xba756174393d88df, 0x94f971119aeef9e5},
      {0xe912b9d1478ceb17, 0x7a37cd5601aab85e},
      {0x91abb422ccb812ee, 0xac62e055c10ab33b},
      {0xb616a12b7fe617aa, 0x577b986b314d600a},
      {0xe39c49765fdf9d94, 0xed5a7e85fda0b80c},
      {0x8e41ade9fbebc27d, 0x14588f13be847308},
      {0xb1d219647ae6b31c, 0x596eb2d8ae258fc9},
      {0xde469fbd99a05fe3, 0x6fca5f8ed9aef3bc},
      {0x8aec23d680043bee, 0x25de7bb9480d5855},
      {0xada72ccc20054ae9, 0xaf561aa79a10ae6b},
      {0xd910f7ff28069da4, 0x1b2ba1518094da05},
      {0x87aa9aff79042286, 0x90fb44d2f05d0843},
      {0xa99541bf57452b28, 0x353a1607ac744a54},
      {0xd3fa922f2d1675f2, 0x42889b8997915ce9},
      {0x847c9b5d7c2e09b7, 0x69956135febada12},
      {0xa59bc234db398c25, 0x43fab9837e699096},
      {0xcf02b2c21207ef2e, 0x94f967e45e03f4bc},
      {0x8161afb94b44f57d, 0x1d1be0eebac278f6},
      {0xa1ba1ba79e1632dc, 0x6462d92a69731733},
      {0xca28a291859bbf93, 0x7d7b8f7503cfdcff},
      {0xfcb2cb35e702af78, 0x5cda735244c3d43f},
      {0x9defbf01b061adab, 0x3a0888136afa64a8},
      {0xc56baec21c7a1916, 0x088aaa1845b8fdd1},
      {0xf6c69a72a3989f5b, 0x8aad549e57273d46},
      {0x9a3c2087a63f6399, 0x36ac54e2f678864c},
      {0xc0cb28a98fcf3c7f, 0x84576a1bb416a7de},
      {0xf0fdf2d3f3c30b9f, 0x656d44a2a11c51d6},
      {0x969eb7c47859e743, 0x9f644ae5a4b1b326},
      {0xbc4665b596706114, 0x873d5d9f0dde1fef},
      {0xeb57ff22fc0c7959, 0xa90cb506d155a7eb},
      {0x9316ff75dd87cbd8, 0x09a7f12442d588f3},
      {0xb7dcbf5354e9bece, 0x0c11ed6d538aeb30},
      {0xe5d3ef282a242e81, 0x8f1668c8a86da5fb},
      {0x8fa475791a569d10, 0xf96e017d694487bd},
      {0xb38d92d760ec4455, 0x37c981dcc395a9ad},
      {0xe070f78d3927556a, 0x85bbe253f47b1418},
      {0x8c469ab843b89562, 0x93956d7478ccec8f},
      {0xaf58416654a6babb, 0x387ac8d1970027b3},
      {0xdb2e51bfe9d0696a, 0x06997b05fcc0319f},
      {0x88fcf317f22241e2, 0x441fece3bdf81f04},
      {0xab3c2fddeeaad25a, 0xd527e81cad7626c4},
      {0xd60b3bd56a5586f1, 0x8a71e223d8d3b075},
      {0x85c7056562757456, 0xf6872d5667844e4a},
      {0xa738c6bebb12d16c, 0xb428f8ac016561dc},
      {0xd106f86e69d785c7, 0xe13336d701beba53},
      {0x82a45b450226b39c, 0xecc0024661173474},
      {0xa34d721642b06084, 0x27f002d7f95d0191},
      {0xcc20ce9bd35c78a5, 0x31ec038df7b441f5},
      {0xff290242c83396ce, 0x7e67047175a15272},
      {0x9f79a169bd203e41, 0x0f0062c6e984d387},
      {0xc75809c42c684dd1, 0x52c07b78a3e60869},
      {0xf92e0c3537826145, 0xa7709a56ccdf8a83},
      {0x9bbcc7a142b17ccb, 0x88a66076400bb692},
      {0xc2abf989935ddbfe, 0x6acff893d00ea436},
      {0xf356f7ebf83552fe, 0x0583f6b8c4124d44},
      {0x98165af37b2153de, 0xc3727a337a8b704b},
      {0xbe1bf1b059e9a8d6, 0x744f18c0592e4c5d},
      {0xeda2ee1c7064130c, 0x1162def06f79df74},
      {0x9485d4d1c63e8be7, 0x8addcb5645ac2ba9},
      {0xb9a74a0637ce2ee1, 0x6d953e2bd7173693},
      {0xe8111c87c5c1ba99, 0xc8fa8db6ccdd0438},
      {0x910ab1d4db9914a0, 0x1d9c9892400a22a3},
      {0xb54d5e4a127f59c8, 0x2503beb6d00cab4c},
      {0xe2a0b5dc971f303a, 0x2e44ae64840fd61e},
      {0x8da471a9de737e24, 0x5ceaecfed289e5d3},
      {0xb10d8e1456105dad, 0x7425a83e872c5f48},
      {0xdd50f1996b947518, 0xd12f124e28f7771a},
      {0x8a5296ffe33cc92f, 0x82bd6b70d99aaa70},
      {0xace73cbfdc0bfb7b, 0x636cc64d1001550c},
      {0xd8210befd30efa5a, 0x3c47f7e05401aa4f},
      {0x8714a775e3e95c78, 0x65acfaec34810a72},
      {0xa8d9d1535ce3b396, 0x7f1839a741a14d0e},
      {0xd31045a8341ca07c, 0x1ede48111209a051},
      {0x83ea2b892091e44d, 0x934aed0aab460433},
      {0xa4e4b66b68b65d60, 0xf81da84d56178540},
      {0xce1de40642e3f4b9, 0x36251260ab9d668f},
      {0x80d2ae83e9ce78f3, 0xc1d72b7c6b42601a},
      {0xa1075a24e4421730, 0xb24cf65b8612f820},
      {0xc94930ae1d529cfc, 0xdee033f26797b628},
      {0xfb9b7cd9a4a7443c, 0x169840ef017da3b2},
      {0x9d412e0806e88aa5, 0x8e1f289560ee864f},
      {0xc491798a08a2ad4e, 0xf1a6f2bab92a27e3},
      {0xf5b5d7ec8acb58a2, 0xae10af696774b1dc},
      {0x9991a6f3d6bf1765, 0xacca6da1e0a8ef2a},
      {0xbff610b0cc6edd3f, 0x17fd090a58d32af4},
      {0xeff394dcff8a948e, 0xddfc4b4cef07f5b1},
      {0x95f83d0a1fb69cd9, 0x4abdaf101564f98f},
      {0xbb764c4ca7a4440f, 0x9d6d1ad41abe37f2},
      {0xea53df5fd18d5513, 0x84c86189216dc5ee},
      {0x92746b9be2f8552c, 0x32fd3cf5b4e49bb5},
      {0xb7118682dbb66a77, 0x3fbc8c33221dc2a2},
      {0xe4d5e82392a40515, 0x0fabaf3feaa5334b},
      {0x8f05b1163ba6832d, 0x29cb4d87f2a7400f},
      {0xb2c71d5bca9023f8, 0x743e20e9ef511013},
      {0xdf78e4b2bd342cf6, 0x914da9246b255417},
      {0x8bab8eefb6409c1a, 0x1ad089b6c2f7548f},
      {0xae9672aba3d0c320, 0xa184ac2473b529b2},
      {0xda3c0f568cc4f3e8, 0xc9e5d72d90a2741f},
      {0x8865899617fb1871, 0x7e2fa67c7a658893},
      {0xaa7eebfb9df9de8d, 0xddbb901b98feeab8},
      {0xd51ea6fa85785631, 0x552a74227f3ea566},
      {0x8533285c936b35de, 0xd53a88958f872760},
      {0xa67ff273b8460356, 0x8a892abaf368f138},
      {0xd01fef10a657842c, 0x2d2b7569b0432d86},
      {0x8213f56a67f6b29b, 0x9c3b29620e29fc74},
      {0xa298f2c501f45f42, 0x8349f3ba91b47b90},
      {0xcb3f2f7642717713, 0x241c70a936219a74},
      {0xfe0efb53d30dd4d7, 0xed238cd383aa0111},
      {0x9ec95d1463e8a506, 0xf4363804324a40ab},
      {0xc67bb4597ce2ce48, 0xb143c6053edcd0d6},
      {0xf81aa16fdc1b81da, 0xdd94b7868e94050b},
      {0x9b10a4e5e9913128, 0xca7cf2b4191c8327},
      {0xc1d4ce1f63f57d72, 0xfd1c2f611f63a3f1},
      {0xf24a01a73cf2dccf, 0xbc633b39673c8ced},
      {0x976e41088617ca01, 0xd5be0503e085d814},
      {0xbd49d14aa79dbc82, 0x4b2d8644d8a74e19},
      {0xec9c459d51852ba2, 0xddf8e7d60ed1219f},
      {0x93e1ab8252f33b45, 0xcabb90e5c942b504},
      {0xb8da1662e7b00a17, 0x3d6a751f3b936244},
      {0xe7109bfba19c0c9d, 0x0cc512670a783ad5},
      {0x906a617d450187e2, 0x27fb2b80668b24c6},
      {0xb484f9dc9641e9da, 0xb1f9f660802dedf7},
      {0xe1a63853bbd26451, 0x5e7873f8a0396974},
      {0x8d07e33455637eb2, 0xdb0b487b6423e1e9},
      {0xb049dc016abc5e5f, 0x91ce1a9a3d2cda63},
      {0xdc5c5301c56b75f7, 0x7641a140cc7810fc},
      {0x89b9b3e11b6329ba, 0xa9e904c87fcb0a9e},
      {0xac2820d9623bf429, 0x546345fa9fbdcd45},
      {0xd732290fbacaf133, 0xa97c177947ad4096},
      {0x867f59a9d4bed6c0, 0x49ed8eabcccc485e},
      {0xa81f301449ee8c70, 0x5c68f256bfff5a75},
      {0xd226fc195c6a2f8c, 0x73832eec6fff3112},
      {0x83585d8fd9c25db7, 0xc831fd53c5ff7eac},
      {0xa42e74f3d032f525, 0xba3e7ca8b77f5e56},
      {0xcd3a1230c43fb26f, 0x28ce1bd2e55f35ec},
      {0x80444b5e7aa7cf85, 0x7980d163cf5b81b4},
      {0xa0555e361951c366, 0xd7e105bcc3326220},
      {0xc86ab5c39fa63440, 0x8dd9472bf3fefaa8},
      {0xfa856334878fc150, 0xb14f98f6f0feb952},
      {0x9c935e00d4b9d8d2, 0x6ed1bf9a569f33d4},
      {0xc3b8358109e84f07, 0x0a862f80ec4700c9},
      {0xf4a642e14c6262c8, 0xcd27bb612758c0fb},
      {0x98e7e9cccfbd7dbd, 0x8038d51cb897789d},
      {0xbf21e44003acdd2c, 0xe0470a63e6bd56c4},
      {0xeeea5d5004981478, 0x1858ccfce06cac75},
      {0x95527a5202df0ccb, 0x0f37801e0c43ebc9},
      {0xbaa718e68396cffd, 0xd30560258f54e6bb},
      {0xe950df20247c83fd, 0x47c6b82ef32a206a},
      {0x91d28b7416cdd27e, 0x4cdc331d57fa5442},
      {0xb6472e511c81471d, 0xe0133fe4adf8e953},
      {0xe3d8f9e563a198e5, 0x58180fddd97723a7},
      {0x8e679c2f5e44ff8f, 0x570f09eaa7ea7649},
      {0xb201833b35d63f73, 0x2cd2cc6551e513db},
      {0xde81e40a034bcf4f, 0xf8077f7ea65e58d2},
      {0x8b112e86420f6191, 0xfb04afaf27faf783},
      {0xadd57a27d29339f6, 0x79c5db9af1f9b564},
      {0xd94ad8b1c7380874, 0x18375281ae7822bd},
      {0x87cec76f1c830548, 0x8f2293910d0b15b6},
      {0xa9c2794ae3a3c69a, 0xb2eb3875504ddb23},
      {0xd433179d9c8cb841, 0x5fa60692a46151ec},
      {0x849feec281d7f328, 0xdbc7c41ba6bcd334},
      {0xa5c7ea73224deff3, 0x12b9b522906c0801},
      {0xcf39e50feae16bef, 0xd768226b34870a01},
      {0x81842f29f2cce375, 0xe6a1158300d46641},
      {0xa1e53af46f801c53, 0x60495ae3c1097fd1},
      {0xca5e89b18b602368, 0x385bb19cb14bdfc5},
      {0xfcf62c1dee382c42, 0x46729e03dd9ed7b6},
      {0x9e19db92b4e31ba9, 0x6c07a2c26a8346d2},
      {0xc5a05277621be293, 0xc7098b7305241886},
      {0xf70867153aa2db38, 0xb8cbee4fc66d1ea8},
      {0x9a65406d44a5c903, 0x737f74f1dc043329},
      {0xc0fe908895cf3b44, 0x505f522e53053ff3},
      {0xf13e34aabb430a15, 0x647726b9e7c68ff0},
      {0x96c6e0eab509e64d, 0x5eca783430dc19f6},
      {0xbc789925624c5fe0, 0xb67d16413d132073},
      {0xeb96bf6ebadf77d8, 0xe41c5bd18c57e890},
      {0x933e37a534cbaae7, 0x8e91b962f7b6f15a},
      {0xb80dc58e81fe95a1, 0x723627bbb5a4adb1},
      {0xe61136f2227e3b09, 0xcec3b1aaa30dd91d},
      {0x8fcac257558ee4e6, 0x213a4f0aa5e8a7b2},
      {0xb3bd72ed2af29e1f, 0xa988e2cd4f62d19e},
      {0xe0accfa875af45a7, 0x93eb1b80a33b8606},
      {0x8c6c01c9498d8b88, 0xbc72f130660533c4},
      {0xaf87023b9bf0ee6a, 0xeb8fad7c7f8680b5},
      {0xdb68c2ca82ed2a05, 0xa67398db9f6820e2},
#else
      {0xff77b1fcbebcdc4f, 0x25e8e89c13bb0f7b},
      {0xce5d73ff402d98e3, 0xfb0a3d212dc81290},
      {0xa6b34ad8c9dfc06f, 0xf42faa48c0ea481f},
      {0x86a8d39ef77164bc, 0xae5dff9c02033198},
      {0xd98ddaee19068c76, 0x3badd624dd9b0958},
      {0xafbd2350644eeacf, 0xe5d1929ef90898fb},
      {0x8df5efabc5979c8f, 0xca8d3ffa1ef463c2},
      {0xe55990879ddcaabd, 0xcc420a6a101d0516},
      {0xb94470938fa89bce, 0xf808e40e8d5b3e6a},
      {0x95a8637627989aad, 0xdde7001379a44aa9},
      {0xf1c90080baf72cb1, 0x5324c68b12dd6339},
      {0xc350000000000000, 0x0000000000000000},
      {0x9dc5ada82b70b59d, 0xf020000000000000},
      {0xfee50b7025c36a08, 0x02f236d04753d5b5},
      {0xcde6fd5e09abcf26, 0xed4c0226b55e6f87},
      {0xa6539930bf6bff45, 0x84db8346b786151d},
      {0x865b86925b9bc5c2, 0x0b8a2392ba45a9b3},
      {0xd910f7ff28069da4, 0x1b2ba1518094da05},
      {0xaf58416654a6babb, 0x387ac8d1970027b3},
      {0x8da471a9de737e24, 0x5ceaecfed289e5d3},
      {0xe4d5e82392a40515, 0x0fabaf3feaa5334b},
      {0xb8da1662e7b00a17, 0x3d6a751f3b936244},
      {0x95527a5202df0ccb, 0x0f37801e0c43ebc9},
      {0xf13e34aabb430a15, 0x647726b9e7c68ff0}
#endif
    };

#if FMT_USE_FULL_CACHE_DRAGONBOX
    return pow10_significands[k - float_info<double>::min_k];
#else
    static constexpr uint64_t powers_of_5_64[] = {
        0x0000000000000001, 0x0000000000000005, 0x0000000000000019,
        0x000000000000007d, 0x0000000000000271, 0x0000000000000c35,
        0x0000000000003d09, 0x000000000001312d, 0x000000000005f5e1,
        0x00000000001dcd65, 0x00000000009502f9, 0x0000000002e90edd,
        0x000000000e8d4a51, 0x0000000048c27395, 0x000000016bcc41e9,
        0x000000071afd498d, 0x0000002386f26fc1, 0x000000b1a2bc2ec5,
        0x000003782dace9d9, 0x00001158e460913d, 0x000056bc75e2d631,
        0x0001b1ae4d6e2ef5, 0x000878678326eac9, 0x002a5a058fc295ed,
        0x00d3c21bcecceda1, 0x0422ca8b0a00a425, 0x14adf4b7320334b9};

    static const int compression_ratio = 27;

    // Compute base index.
    int cache_index = (k - float_info<double>::min_k) / compression_ratio;
    int kb = cache_index * compression_ratio + float_info<double>::min_k;
    int offset = k - kb;

    // Get base cache.
    uint128_fallback base_cache = pow10_significands[cache_index];
    if (offset == 0) return base_cache;

    // Compute the required amount of bit-shift.
    int alpha = floor_log2_pow10(kb + offset) - floor_log2_pow10(kb) - offset;
    FMT_ASSERT(alpha > 0 && alpha < 64, "shifting error detected");

    // Try to recover the real cache.
    uint64_t pow5 = powers_of_5_64[offset];
    uint128_fallback recovered_cache = umul128(base_cache.high(), pow5);
    uint128_fallback middle_low = umul128(base_cache.low(), pow5);

    recovered_cache += middle_low.high();

    uint64_t high_to_middle = recovered_cache.high() << (64 - alpha);
    uint64_t middle_to_low = recovered_cache.low() << (64 - alpha);

    recovered_cache =
        uint128_fallback{(recovered_cache.low() >> alpha) | high_to_middle,
                         ((middle_low.low() >> alpha) | middle_to_low)};
    FMT_ASSERT(recovered_cache.low() + 1 != 0, "");
    return {recovered_cache.high(), recovered_cache.low() + 1};
#endif
  }

  struct compute_mul_result {
    carrier_uint result;
    bool is_integer;
  };
  struct compute_mul_parity_result {
    bool parity;
    bool is_integer;
  };

  static auto compute_mul(carrier_uint u,
                          const cache_entry_type& cache) noexcept
      -> compute_mul_result {
    auto r = umul192_upper128(u, cache);
    return {r.high(), r.low() == 0};
  }

  static auto compute_delta(const cache_entry_type& cache, int beta) noexcept
      -> uint32_t {
    return static_cast<uint32_t>(cache.high() >> (64 - 1 - beta));
  }

  static auto compute_mul_parity(carrier_uint two_f,
                                 const cache_entry_type& cache,
                                 int beta) noexcept
      -> compute_mul_parity_result {
    FMT_ASSERT(beta >= 1, "");
    FMT_ASSERT(beta < 64, "");

    auto r = umul192_lower128(two_f, cache);
    return {((r.high() >> (64 - beta)) & 1) != 0,
            ((r.high() << beta) | (r.low() >> (64 - beta))) == 0};
  }

  static auto compute_left_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta) noexcept -> carrier_uint {
    return (cache.high() -
            (cache.high() >> (num_significand_bits<double>() + 2))) >>
           (64 - num_significand_bits<double>() - 1 - beta);
  }

  static auto compute_right_endpoint_for_shorter_interval_case(
      const cache_entry_type& cache, int beta) noexcept -> carrier_uint {
    return (cache.high() +
            (cache.high() >> (num_significand_bits<double>() + 1))) >>
           (64 - num_significand_bits<double>() - 1 - beta);
  }

  static auto compute_round_up_for_shorter_interval_case(
      const cache_entry_type& cache, int beta) noexcept -> carrier_uint {
    return ((cache.high() >> (64 - num_significand_bits<double>() - 2 - beta)) +
            1) /
           2;
  }
};

FMT_FUNC auto get_cached_power(int k) noexcept -> uint128_fallback {
  return cache_accessor<double>::get_cached_power(k);
}

// Various integer checks
template <typename T>
auto is_left_endpoint_integer_shorter_interval(int exponent) noexcept -> bool {
  const int case_shorter_interval_left_endpoint_lower_threshold = 2;
  const int case_shorter_interval_left_endpoint_upper_threshold = 3;
  return exponent >= case_shorter_interval_left_endpoint_lower_threshold &&
         exponent <= case_shorter_interval_left_endpoint_upper_threshold;
}

// Remove trailing zeros from n and return the number of zeros removed (float).
FMT_INLINE int remove_trailing_zeros(uint32_t& n, int s = 0) noexcept {
  FMT_ASSERT(n != 0, "");
  // Modular inverse of 5 (mod 2^32): (mod_inv_5 * 5) mod 2^32 = 1.
  constexpr uint32_t mod_inv_5 = 0xcccccccd;
  constexpr uint32_t mod_inv_25 = 0xc28f5c29;  // = mod_inv_5 * mod_inv_5

  while (true) {
    auto q = rotr(n * mod_inv_25, 2);
    if (q > max_value<uint32_t>() / 100) break;
    n = q;
    s += 2;
  }
  auto q = rotr(n * mod_inv_5, 1);
  if (q <= max_value<uint32_t>() / 10) {
    n = q;
    s |= 1;
  }
  return s;
}

// Removes trailing zeros and returns the number of zeros removed (double).
FMT_INLINE int remove_trailing_zeros(uint64_t& n) noexcept {
  FMT_ASSERT(n != 0, "");

  // Is n is divisible by 10^8?
  constexpr uint32_t ten_pow_8 = 100000000u;
  if ((n % ten_pow_8) == 0) {
    // If yes, work with the quotient...
    auto n32 = static_cast<uint32_t>(n / ten_pow_8);
    // ... and use the 32 bit variant of the function
    int num_zeros = remove_trailing_zeros(n32, 8);
    n = n32;
    return num_zeros;
  }

  // If n is not divisible by 10^8, work with n itself.
  constexpr uint64_t mod_inv_5 = 0xcccccccccccccccd;
  constexpr uint64_t mod_inv_25 = 0x8f5c28f5c28f5c29;  // mod_inv_5 * mod_inv_5

  int s = 0;
  while (true) {
    auto q = rotr(n * mod_inv_25, 2);
    if (q > max_value<uint64_t>() / 100) break;
    n = q;
    s += 2;
  }
  auto q = rotr(n * mod_inv_5, 1);
  if (q <= max_value<uint64_t>() / 10) {
    n = q;
    s |= 1;
  }

  return s;
}

// The main algorithm for shorter interval case
template <typename T>
FMT_INLINE decimal_fp<T> shorter_interval_case(int exponent) noexcept {
  decimal_fp<T> ret_value;
  // Compute k and beta
  const int minus_k = floor_log10_pow2_minus_log10_4_over_3(exponent);
  const int beta = exponent + floor_log2_pow10(-minus_k);

  // Compute xi and zi
  using cache_entry_type = typename cache_accessor<T>::cache_entry_type;
  const cache_entry_type cache = cache_accessor<T>::get_cached_power(-minus_k);

  auto xi = cache_accessor<T>::compute_left_endpoint_for_shorter_interval_case(
      cache, beta);
  auto zi = cache_accessor<T>::compute_right_endpoint_for_shorter_interval_case(
      cache, beta);

  // If the left endpoint is not an integer, increase it
  if (!is_left_endpoint_integer_shorter_interval<T>(exponent)) ++xi;

  // Try bigger divisor
  ret_value.significand = zi / 10;

  // If succeed, remove trailing zeros if necessary and return
  if (ret_value.significand * 10 >= xi) {
    ret_value.exponent = minus_k + 1;
    ret_value.exponent += remove_trailing_zeros(ret_value.significand);
    return ret_value;
  }

  // Otherwise, compute the round-up of y
  ret_value.significand =
      cache_accessor<T>::compute_round_up_for_shorter_interval_case(cache,
                                                                    beta);
  ret_value.exponent = minus_k;

  // When tie occurs, choose one of them according to the rule
  if (exponent >= float_info<T>::shorter_interval_tie_lower_threshold &&
      exponent <= float_info<T>::shorter_interval_tie_upper_threshold) {
    ret_value.significand = ret_value.significand % 2 == 0
                                ? ret_value.significand
                                : ret_value.significand - 1;
  } else if (ret_value.significand < xi) {
    ++ret_value.significand;
  }
  return ret_value;
}

template <typename T> auto to_decimal(T x) noexcept -> decimal_fp<T> {
  // Step 1: integer promotion & Schubfach multiplier calculation.

  using carrier_uint = typename float_info<T>::carrier_uint;
  using cache_entry_type = typename cache_accessor<T>::cache_entry_type;
  auto br = bit_cast<carrier_uint>(x);

  // Extract significand bits and exponent bits.
  const carrier_uint significand_mask =
      (static_cast<carrier_uint>(1) << num_significand_bits<T>()) - 1;
  carrier_uint significand = (br & significand_mask);
  int exponent =
      static_cast<int>((br & exponent_mask<T>()) >> num_significand_bits<T>());

  if (exponent != 0) {  // Check if normal.
    exponent -= exponent_bias<T>() + num_significand_bits<T>();

    // Shorter interval case; proceed like Schubfach.
    // In fact, when exponent == 1 and significand == 0, the interval is
    // regular. However, it can be shown that the end-results are anyway same.
    if (significand == 0) return shorter_interval_case<T>(exponent);

    significand |= (static_cast<carrier_uint>(1) << num_significand_bits<T>());
  } else {
    // Subnormal case; the interval is always regular.
    if (significand == 0) return {0, 0};
    exponent =
        std::numeric_limits<T>::min_exponent - num_significand_bits<T>() - 1;
  }

  const bool include_left_endpoint = (significand % 2 == 0);
  const bool include_right_endpoint = include_left_endpoint;

  // Compute k and beta.
  const int minus_k = floor_log10_pow2(exponent) - float_info<T>::kappa;
  const cache_entry_type cache = cache_accessor<T>::get_cached_power(-minus_k);
  const int beta = exponent + floor_log2_pow10(-minus_k);

  // Compute zi and deltai.
  // 10^kappa <= deltai < 10^(kappa + 1)
  const uint32_t deltai = cache_accessor<T>::compute_delta(cache, beta);
  const carrier_uint two_fc = significand << 1;

  // For the case of binary32, the result of integer check is not correct for
  // 29711844 * 2^-82
  // = 6.1442653300000000008655037797566933477355632930994033813476... * 10^-18
  // and 29711844 * 2^-81
  // = 1.2288530660000000001731007559513386695471126586198806762695... * 10^-17,
  // and they are the unique counterexamples. However, since 29711844 is even,
  // this does not cause any problem for the endpoints calculations; it can only
  // cause a problem when we need to perform integer check for the center.
  // Fortunately, with these inputs, that branch is never executed, so we are
  // fine.
  const typename cache_accessor<T>::compute_mul_result z_mul =
      cache_accessor<T>::compute_mul((two_fc | 1) << beta, cache);

  // Step 2: Try larger divisor; remove trailing zeros if necessary.

  // Using an upper bound on zi, we might be able to optimize the division
  // better than the compiler; we are computing zi / big_divisor here.
  decimal_fp<T> ret_value;
  ret_value.significand = divide_by_10_to_kappa_plus_1(z_mul.result);
  uint32_t r = static_cast<uint32_t>(z_mul.result - float_info<T>::big_divisor *
                                                        ret_value.significand);

  if (r < deltai) {
    // Exclude the right endpoint if necessary.
    if (r == 0 && (z_mul.is_integer & !include_right_endpoint)) {
      --ret_value.significand;
      r = float_info<T>::big_divisor;
      goto small_divisor_case_label;
    }
  } else if (r > deltai) {
    goto small_divisor_case_label;
  } else {
    // r == deltai; compare fractional parts.
    const typename cache_accessor<T>::compute_mul_parity_result x_mul =
        cache_accessor<T>::compute_mul_parity(two_fc - 1, cache, beta);

    if (!(x_mul.parity | (x_mul.is_integer & include_left_endpoint)))
      goto small_divisor_case_label;
  }
  ret_value.exponent = minus_k + float_info<T>::kappa + 1;

  // We may need to remove trailing zeros.
  ret_value.exponent += remove_trailing_zeros(ret_value.significand);
  return ret_value;

  // Step 3: Find the significand with the smaller divisor.

small_divisor_case_label:
  ret_value.significand *= 10;
  ret_value.exponent = minus_k + float_info<T>::kappa;

  uint32_t dist = r - (deltai / 2) + (float_info<T>::small_divisor / 2);
  const bool approx_y_parity =
      ((dist ^ (float_info<T>::small_divisor / 2)) & 1) != 0;

  // Is dist divisible by 10^kappa?
  const bool divisible_by_small_divisor =
      check_divisibility_and_divide_by_pow10<float_info<T>::kappa>(dist);

  // Add dist / 10^kappa to the significand.
  ret_value.significand += dist;

  if (!divisible_by_small_divisor) return ret_value;

  // Check z^(f) >= epsilon^(f).
  // We have either yi == zi - epsiloni or yi == (zi - epsiloni) - 1,
  // where yi == zi - epsiloni if and only if z^(f) >= epsilon^(f).
  // Since there are only 2 possibilities, we only need to care about the
  // parity. Also, zi and r should have the same parity since the divisor
  // is an even number.
  const auto y_mul = cache_accessor<T>::compute_mul_parity(two_fc, cache, beta);

  // If z^(f) >= epsilon^(f), we might have a tie when z^(f) == epsilon^(f),
  // or equivalently, when y is an integer.
  if (y_mul.parity != approx_y_parity)
    --ret_value.significand;
  else if (y_mul.is_integer & (ret_value.significand % 2 != 0))
    --ret_value.significand;
  return ret_value;
}
}  // namespace dragonbox
}  // namespace detail

template <> struct formatter<detail::bigint> {
  FMT_CONSTEXPR auto parse(format_parse_context& ctx)
      -> format_parse_context::iterator {
    return ctx.begin();
  }

  auto format(const detail::bigint& n, format_context& ctx) const
      -> format_context::iterator {
    auto out = ctx.out();
    bool first = true;
    for (auto i = n.bigits_.size(); i > 0; --i) {
      auto value = n.bigits_[i - 1u];
      if (first) {
        out = fmt::format_to(out, FMT_STRING("{:x}"), value);
        first = false;
        continue;
      }
      out = fmt::format_to(out, FMT_STRING("{:08x}"), value);
    }
    if (n.exp_ > 0)
      out = fmt::format_to(out, FMT_STRING("p{}"),
                           n.exp_ * detail::bigint::bigit_bits);
    return out;
  }
};

FMT_FUNC detail::utf8_to_utf16::utf8_to_utf16(string_view s) {
  for_each_codepoint(s, [this](uint32_t cp, string_view) {
    if (cp == invalid_code_point) FMT_THROW(std::runtime_error("invalid utf8"));
    if (cp <= 0xFFFF) {
      buffer_.push_back(static_cast<wchar_t>(cp));
    } else {
      cp -= 0x10000;
      buffer_.push_back(static_cast<wchar_t>(0xD800 + (cp >> 10)));
      buffer_.push_back(static_cast<wchar_t>(0xDC00 + (cp & 0x3FF)));
    }
    return true;
  });
  buffer_.push_back(0);
}

FMT_FUNC void format_system_error(detail::buffer<char>& out, int error_code,
                                  const char* message) noexcept {
  FMT_TRY {
    auto ec = std::error_code(error_code, std::generic_category());
    detail::write(appender(out), std::system_error(ec, message).what());
    return;
  }
  FMT_CATCH(...) {}
  format_error_code(out, error_code, message);
}

FMT_FUNC void report_system_error(int error_code,
                                  const char* message) noexcept {
  do_report_error(format_system_error, error_code, message);
}

FMT_FUNC auto vformat(string_view fmt, format_args args) -> std::string {
  // Don't optimize the "{}" case to keep the binary size small and because it
  // can be better optimized in fmt::format anyway.
  auto buffer = memory_buffer();
  detail::vformat_to(buffer, fmt, args);
  return to_string(buffer);
}

namespace detail {

FMT_FUNC void vformat_to(buffer<char>& buf, string_view fmt, format_args args,
                         locale_ref loc) {
  auto out = appender(buf);
  if (fmt.size() == 2 && equal2(fmt.data(), "{}"))
    return args.get(0).visit(default_arg_formatter<char>{out});
  parse_format_string(
      fmt, format_handler<char>{parse_context<char>(fmt), {out, args, loc}});
}

template <typename T> struct span {
  T* data;
  size_t size;
};

template <typename F> auto flockfile(F* f) -> decltype(_lock_file(f)) {
  _lock_file(f);
}
template <typename F> auto funlockfile(F* f) -> decltype(_unlock_file(f)) {
  _unlock_file(f);
}

#ifndef getc_unlocked
template <typename F> auto getc_unlocked(F* f) -> decltype(_fgetc_nolock(f)) {
  return _fgetc_nolock(f);
}
#endif

template <typename F = FILE, typename Enable = void>
struct has_flockfile : std::false_type {};

template <typename F>
struct has_flockfile<F, void_t<decltype(flockfile(&std::declval<F&>()))>>
    : std::true_type {};

// A FILE wrapper. F is FILE defined as a template parameter to make system API
// detection work.
template <typename F> class file_base {
 public:
  F* file_;

 public:
  file_base(F* file) : file_(file) {}
  operator F*() const { return file_; }

  // Reads a code unit from the stream.
  auto get() -> int {
    int result = getc_unlocked(file_);
    if (result == EOF && ferror(file_) != 0)
      FMT_THROW(system_error(errno, FMT_STRING("getc failed")));
    return result;
  }

  // Puts the code unit back into the stream buffer.
  void unget(char c) {
    if (ungetc(c, file_) == EOF)
      FMT_THROW(system_error(errno, FMT_STRING("ungetc failed")));
  }

  void flush() { fflush(this->file_); }
};

// A FILE wrapper for glibc.
template <typename F> class glibc_file : public file_base<F> {
 private:
  enum {
    line_buffered = 0x200,  // _IO_LINE_BUF
    unbuffered = 2          // _IO_UNBUFFERED
  };

 public:
  using file_base<F>::file_base;

  auto is_buffered() const -> bool {
    return (this->file_->_flags & unbuffered) == 0;
  }

  void init_buffer() {
    if (this->file_->_IO_write_ptr < this->file_->_IO_write_end) return;
    // Force buffer initialization by placing and removing a char in a buffer.
    putc_unlocked(0, this->file_);
    --this->file_->_IO_write_ptr;
  }

  // Returns the file's read buffer.
  auto get_read_buffer() const -> span<const char> {
    auto ptr = this->file_->_IO_read_ptr;
    return {ptr, to_unsigned(this->file_->_IO_read_end - ptr)};
  }

  // Returns the file's write buffer.
  auto get_write_buffer() const -> span<char> {
    auto ptr = this->file_->_IO_write_ptr;
    return {ptr, to_unsigned(this->file_->_IO_buf_end - ptr)};
  }

  void advance_write_buffer(size_t size) { this->file_->_IO_write_ptr += size; }

  bool needs_flush() const {
    if ((this->file_->_flags & line_buffered) == 0) return false;
    char* end = this->file_->_IO_write_end;
    auto size = max_of<ptrdiff_t>(this->file_->_IO_write_ptr - end, 0);
    return memchr(end, '\n', static_cast<size_t>(size));
  }

  void flush() { fflush_unlocked(this->file_); }
};

// A FILE wrapper for Apple's libc.
template <typename F> class apple_file : public file_base<F> {
 private:
  enum {
    line_buffered = 1,  // __SNBF
    unbuffered = 2      // __SLBF
  };

 public:
  using file_base<F>::file_base;

  auto is_buffered() const -> bool {
    return (this->file_->_flags & unbuffered) == 0;
  }

  void init_buffer() {
    if (this->file_->_p) return;
    // Force buffer initialization by placing and removing a char in a buffer.
    putc_unlocked(0, this->file_);
    --this->file_->_p;
    ++this->file_->_w;
  }

  auto get_read_buffer() const -> span<const char> {
    return {reinterpret_cast<char*>(this->file_->_p),
            to_unsigned(this->file_->_r)};
  }

  auto get_write_buffer() const -> span<char> {
    return {reinterpret_cast<char*>(this->file_->_p),
            to_unsigned(this->file_->_bf._base + this->file_->_bf._size -
                        this->file_->_p)};
  }

  void advance_write_buffer(size_t size) {
    this->file_->_p += size;
    this->file_->_w -= size;
  }

  bool needs_flush() const {
    if ((this->file_->_flags & line_buffered) == 0) return false;
    return memchr(this->file_->_p + this->file_->_w, '\n',
                  to_unsigned(-this->file_->_w));
  }
};

// A fallback FILE wrapper.
template <typename F> class fallback_file : public file_base<F> {
 private:
  char next_;  // The next unconsumed character in the buffer.
  bool has_next_ = false;

 public:
  using file_base<F>::file_base;

  auto is_buffered() const -> bool { return false; }
  auto needs_flush() const -> bool { return false; }
  void init_buffer() {}

  auto get_read_buffer() const -> span<const char> {
    return {&next_, has_next_ ? 1u : 0u};
  }

  auto get_write_buffer() const -> span<char> { return {nullptr, 0}; }

  void advance_write_buffer(size_t) {}

  auto get() -> int {
    has_next_ = false;
    return file_base<F>::get();
  }

  void unget(char c) {
    file_base<F>::unget(c);
    next_ = c;
    has_next_ = true;
  }
};

#ifndef FMT_USE_FALLBACK_FILE
#  define FMT_USE_FALLBACK_FILE 0
#endif

template <typename F,
          FMT_ENABLE_IF(sizeof(F::_p) != 0 && !FMT_USE_FALLBACK_FILE)>
auto get_file(F* f, int) -> apple_file<F> {
  return f;
}
template <typename F,
          FMT_ENABLE_IF(sizeof(F::_IO_read_ptr) != 0 && !FMT_USE_FALLBACK_FILE)>
inline auto get_file(F* f, int) -> glibc_file<F> {
  return f;
}

inline auto get_file(FILE* f, ...) -> fallback_file<FILE> { return f; }

using file_ref = decltype(get_file(static_cast<FILE*>(nullptr), 0));

template <typename F = FILE, typename Enable = void>
class file_print_buffer : public buffer<char> {
 public:
  explicit file_print_buffer(F*) : buffer(nullptr, size_t()) {}
};

template <typename F>
class file_print_buffer<F, enable_if_t<has_flockfile<F>::value>>
    : public buffer<char> {
 private:
  file_ref file_;

  static void grow(buffer<char>& base, size_t) {
    auto& self = static_cast<file_print_buffer&>(base);
    self.file_.advance_write_buffer(self.size());
    if (self.file_.get_write_buffer().size == 0) self.file_.flush();
    auto buf = self.file_.get_write_buffer();
    FMT_ASSERT(buf.size > 0, "");
    self.set(buf.data, buf.size);
    self.clear();
  }

 public:
  explicit file_print_buffer(F* f) : buffer(grow, size_t()), file_(f) {
    flockfile(f);
    file_.init_buffer();
    auto buf = file_.get_write_buffer();
    set(buf.data, buf.size);
  }
  ~file_print_buffer() {
    file_.advance_write_buffer(size());
    bool flush = file_.needs_flush();
    F* f = file_;    // Make funlockfile depend on the template parameter F
    funlockfile(f);  // for the system API detection to work.
    if (flush) fflush(file_);
  }
};

#if !defined(_WIN32) || defined(FMT_USE_WRITE_CONSOLE)
FMT_FUNC auto write_console(int, string_view) -> bool { return false; }
#else
using dword = conditional_t<sizeof(long) == 4, unsigned long, unsigned>;
extern "C" __declspec(dllimport) int __stdcall WriteConsoleW(  //
    void*, const void*, dword, dword*, void*);

FMT_FUNC bool write_console(int fd, string_view text) {
  auto u16 = utf8_to_utf16(text);
  return WriteConsoleW(reinterpret_cast<void*>(_get_osfhandle(fd)), u16.c_str(),
                       static_cast<dword>(u16.size()), nullptr, nullptr) != 0;
}
#endif

#ifdef _WIN32
// Print assuming legacy (non-Unicode) encoding.
FMT_FUNC void vprint_mojibake(std::FILE* f, string_view fmt, format_args args,
                              bool newline) {
  auto buffer = memory_buffer();
  detail::vformat_to(buffer, fmt, args);
  if (newline) buffer.push_back('\n');
  fwrite_all(buffer.data(), buffer.size(), f);
}
#endif

FMT_FUNC void print(std::FILE* f, string_view text) {
#if defined(_WIN32) && !defined(FMT_USE_WRITE_CONSOLE)
  int fd = _fileno(f);
  if (_isatty(fd)) {
    std::fflush(f);
    if (write_console(fd, text)) return;
  }
#endif
  fwrite_all(text.data(), text.size(), f);
}
}  // namespace detail

FMT_FUNC void vprint_buffered(std::FILE* f, string_view fmt, format_args args) {
  auto buffer = memory_buffer();
  detail::vformat_to(buffer, fmt, args);
  detail::print(f, {buffer.data(), buffer.size()});
}

FMT_FUNC void vprint(std::FILE* f, string_view fmt, format_args args) {
  if (!detail::file_ref(f).is_buffered() || !detail::has_flockfile<>())
    return vprint_buffered(f, fmt, args);
  auto&& buffer = detail::file_print_buffer<>(f);
  return detail::vformat_to(buffer, fmt, args);
}

FMT_FUNC void vprintln(std::FILE* f, string_view fmt, format_args args) {
  auto buffer = memory_buffer();
  detail::vformat_to(buffer, fmt, args);
  buffer.push_back('\n');
  detail::print(f, {buffer.data(), buffer.size()});
}

FMT_FUNC void vprint(string_view fmt, format_args args) {
  vprint(stdout, fmt, args);
}

namespace detail {

struct singleton {
  unsigned char upper;
  unsigned char lower_count;
};

inline auto is_printable(uint16_t x, const singleton* singletons,
                         size_t singletons_size,
                         const unsigned char* singleton_lowers,
                         const unsigned char* normal, size_t normal_size)
    -> bool {
  auto upper = x >> 8;
  auto lower_start = 0;
  for (size_t i = 0; i < singletons_size; ++i) {
    auto s = singletons[i];
    auto lower_end = lower_start + s.lower_count;
    if (upper < s.upper) break;
    if (upper == s.upper) {
      for (auto j = lower_start; j < lower_end; ++j) {
        if (singleton_lowers[j] == (x & 0xff)) return false;
      }
    }
    lower_start = lower_end;
  }

  auto xsigned = static_cast<int>(x);
  auto current = true;
  for (size_t i = 0; i < normal_size; ++i) {
    auto v = static_cast<int>(normal[i]);
    auto len = (v & 0x80) != 0 ? (v & 0x7f) << 8 | normal[++i] : v;
    xsigned -= len;
    if (xsigned < 0) break;
    current = !current;
  }
  return current;
}

// This code is generated by support/printable.py.
FMT_FUNC auto is_printable(uint32_t cp) -> bool {
  static constexpr singleton singletons0[] = {
      {0x00, 1},  {0x03, 5},  {0x05, 6},  {0x06, 3},  {0x07, 6},  {0x08, 8},
      {0x09, 17}, {0x0a, 28}, {0x0b, 25}, {0x0c, 20}, {0x0d, 16}, {0x0e, 13},
      {0x0f, 4},  {0x10, 3},  {0x12, 18}, {0x13, 9},  {0x16, 1},  {0x17, 5},
      {0x18, 2},  {0x19, 3},  {0x1a, 7},  {0x1c, 2},  {0x1d, 1},  {0x1f, 22},
      {0x20, 3},  {0x2b, 3},  {0x2c, 2},  {0x2d, 11}, {0x2e, 1},  {0x30, 3},
      {0x31, 2},  {0x32, 1},  {0xa7, 2},  {0xa9, 2},  {0xaa, 4},  {0xab, 8},
      {0xfa, 2},  {0xfb, 5},  {0xfd, 4},  {0xfe, 3},  {0xff, 9},
  };
  static constexpr unsigned char singletons0_lower[] = {
      0xad, 0x78, 0x79, 0x8b, 0x8d, 0xa2, 0x30, 0x57, 0x58, 0x8b, 0x8c, 0x90,
      0x1c, 0x1d, 0xdd, 0x0e, 0x0f, 0x4b, 0x4c, 0xfb, 0xfc, 0x2e, 0x2f, 0x3f,
      0x5c, 0x5d, 0x5f, 0xb5, 0xe2, 0x84, 0x8d, 0x8e, 0x91, 0x92, 0xa9, 0xb1,
      0xba, 0xbb, 0xc5, 0xc6, 0xc9, 0xca, 0xde, 0xe4, 0xe5, 0xff, 0x00, 0x04,
      0x11, 0x12, 0x29, 0x31, 0x34, 0x37, 0x3a, 0x3b, 0x3d, 0x49, 0x4a, 0x5d,
      0x84, 0x8e, 0x92, 0xa9, 0xb1, 0xb4, 0xba, 0xbb, 0xc6, 0xca, 0xce, 0xcf,
      0xe4, 0xe5, 0x00, 0x04, 0x0d, 0x0e, 0x11, 0x12, 0x29, 0x31, 0x34, 0x3a,
      0x3b, 0x45, 0x46, 0x49, 0x4a, 0x5e, 0x64, 0x65, 0x84, 0x91, 0x9b, 0x9d,
      0xc9, 0xce, 0xcf, 0x0d, 0x11, 0x29, 0x45, 0x49, 0x57, 0x64, 0x65, 0x8d,
      0x91, 0xa9, 0xb4, 0xba, 0xbb, 0xc5, 0xc9, 0xdf, 0xe4, 0xe5, 0xf0, 0x0d,
      0x11, 0x45, 0x49, 0x64, 0x65, 0x80, 0x84, 0xb2, 0xbc, 0xbe, 0xbf, 0xd5,
      0xd7, 0xf0, 0xf1, 0x83, 0x85, 0x8b, 0xa4, 0xa6, 0xbe, 0xbf, 0xc5, 0xc7,
      0xce, 0xcf, 0xda, 0xdb, 0x48, 0x98, 0xbd, 0xcd, 0xc6, 0xce, 0xcf, 0x49,
      0x4e, 0x4f, 0x57, 0x59, 0x5e, 0x5f, 0x89, 0x8e, 0x8f, 0xb1, 0xb6, 0xb7,
      0xbf, 0xc1, 0xc6, 0xc7, 0xd7, 0x11, 0x16, 0x17, 0x5b, 0x5c, 0xf6, 0xf7,
      0xfe, 0xff, 0x80, 0x0d, 0x6d, 0x71, 0xde, 0xdf, 0x0e, 0x0f, 0x1f, 0x6e,
      0x6f, 0x1c, 0x1d, 0x5f, 0x7d, 0x7e, 0xae, 0xaf, 0xbb, 0xbc, 0xfa, 0x16,
      0x17, 0x1e, 0x1f, 0x46, 0x47, 0x4e, 0x4f, 0x58, 0x5a, 0x5c, 0x5e, 0x7e,
      0x7f, 0xb5, 0xc5, 0xd4, 0xd5, 0xdc, 0xf0, 0xf1, 0xf5, 0x72, 0x73, 0x8f,
      0x74, 0x75, 0x96, 0x2f, 0x5f, 0x26, 0x2e, 0x2f, 0xa7, 0xaf, 0xb7, 0xbf,
      0xc7, 0xcf, 0xd7, 0xdf, 0x9a, 0x40, 0x97, 0x98, 0x30, 0x8f, 0x1f, 0xc0,
      0xc1, 0xce, 0xff, 0x4e, 0x4f, 0x5a, 0x5b, 0x07, 0x08, 0x0f, 0x10, 0x27,
      0x2f, 0xee, 0xef, 0x6e, 0x6f, 0x37, 0x3d, 0x3f, 0x42, 0x45, 0x90, 0x91,
      0xfe, 0xff, 0x53, 0x67, 0x75, 0xc8, 0xc9, 0xd0, 0xd1, 0xd8, 0xd9, 0xe7,
      0xfe, 0xff,
  };
  static constexpr singleton singletons1[] = {
      {0x00, 6},  {0x01, 1}, {0x03, 1},  {0x04, 2}, {0x08, 8},  {0x09, 2},
      {0x0a, 5},  {0x0b, 2}, {0x0e, 4},  {0x10, 1}, {0x11, 2},  {0x12, 5},
      {0x13, 17}, {0x14, 1}, {0x15, 2},  {0x17, 2}, {0x19, 13}, {0x1c, 5},
      {0x1d, 8},  {0x24, 1}, {0x6a, 3},  {0x6b, 2}, {0xbc, 2},  {0xd1, 2},
      {0xd4, 12}, {0xd5, 9}, {0xd6, 2},  {0xd7, 2}, {0xda, 1},  {0xe0, 5},
      {0xe1, 2},  {0xe8, 2}, {0xee, 32}, {0xf0, 4}, {0xf8, 2},  {0xf9, 2},
      {0xfa, 2},  {0xfb, 1},
  };
  static constexpr unsigned char singletons1_lower[] = {
      0x0c, 0x27, 0x3b, 0x3e, 0x4e, 0x4f, 0x8f, 0x9e, 0x9e, 0x9f, 0x06, 0x07,
      0x09, 0x36, 0x3d, 0x3e, 0x56, 0xf3, 0xd0, 0xd1, 0x04, 0x14, 0x18, 0x36,
      0x37, 0x56, 0x57, 0x7f, 0xaa, 0xae, 0xaf, 0xbd, 0x35, 0xe0, 0x12, 0x87,
      0x89, 0x8e, 0x9e, 0x04, 0x0d, 0x0e, 0x11, 0x12, 0x29, 0x31, 0x34, 0x3a,
      0x45, 0x46, 0x49, 0x4a, 0x4e, 0x4f, 0x64, 0x65, 0x5c, 0xb6, 0xb7, 0x1b,
      0x1c, 0x07, 0x08, 0x0a, 0x0b, 0x14, 0x17, 0x36, 0x39, 0x3a, 0xa8, 0xa9,
      0xd8, 0xd9, 0x09, 0x37, 0x90, 0x91, 0xa8, 0x07, 0x0a, 0x3b, 0x3e, 0x66,
      0x69, 0x8f, 0x92, 0x6f, 0x5f, 0xee, 0xef, 0x5a, 0x62, 0x9a, 0x9b, 0x27,
      0x28, 0x55, 0x9d, 0xa0, 0xa1, 0xa3, 0xa4, 0xa7, 0xa8, 0xad, 0xba, 0xbc,
      0xc4, 0x06, 0x0b, 0x0c, 0x15, 0x1d, 0x3a, 0x3f, 0x45, 0x51, 0xa6, 0xa7,
      0xcc, 0xcd, 0xa0, 0x07, 0x19, 0x1a, 0x22, 0x25, 0x3e, 0x3f, 0xc5, 0xc6,
      0x04, 0x20, 0x23, 0x25, 0x26, 0x28, 0x33, 0x38, 0x3a, 0x48, 0x4a, 0x4c,
      0x50, 0x53, 0x55, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 0x60, 0x63, 0x65, 0x66,
      0x6b, 0x73, 0x78, 0x7d, 0x7f, 0x8a, 0xa4, 0xaa, 0xaf, 0xb0, 0xc0, 0xd0,
      0xae, 0xaf, 0x79, 0xcc, 0x6e, 0x6f, 0x93,
  };
  static constexpr unsigned char normal0[] = {
      0x00, 0x20, 0x5f, 0x22, 0x82, 0xdf, 0x04, 0x82, 0x44, 0x08, 0x1b, 0x04,
      0x06, 0x11, 0x81, 0xac, 0x0e, 0x80, 0xab, 0x35, 0x28, 0x0b, 0x80, 0xe0,
      0x03, 0x19, 0x08, 0x01, 0x04, 0x2f, 0x04, 0x34, 0x04, 0x07, 0x03, 0x01,
      0x07, 0x06, 0x07, 0x11, 0x0a, 0x50, 0x0f, 0x12, 0x07, 0x55, 0x07, 0x03,
      0x04, 0x1c, 0x0a, 0x09, 0x03, 0x08, 0x03, 0x07, 0x03, 0x02, 0x03, 0x03,
      0x03, 0x0c, 0x04, 0x05, 0x03, 0x0b, 0x06, 0x01, 0x0e, 0x15, 0x05, 0x3a,
      0x03, 0x11, 0x07, 0x06, 0x05, 0x10, 0x07, 0x57, 0x07, 0x02, 0x07, 0x15,
      0x0d, 0x50, 0x04, 0x43, 0x03, 0x2d, 0x03, 0x01, 0x04, 0x11, 0x06, 0x0f,
      0x0c, 0x3a, 0x04, 0x1d, 0x25, 0x5f, 0x20, 0x6d, 0x04, 0x6a, 0x25, 0x80,
      0xc8, 0x05, 0x82, 0xb0, 0x03, 0x1a, 0x06, 0x82, 0xfd, 0x03, 0x59, 0x07,
      0x15, 0x0b, 0x17, 0x09, 0x14, 0x0c, 0x14, 0x0c, 0x6a, 0x06, 0x0a, 0x06,
      0x1a, 0x06, 0x59, 0x07, 0x2b, 0x05, 0x46, 0x0a, 0x2c, 0x04, 0x0c, 0x04,
      0x01, 0x03, 0x31, 0x0b, 0x2c, 0x04, 0x1a, 0x06, 0x0b, 0x03, 0x80, 0xac,
      0x06, 0x0a, 0x06, 0x21, 0x3f, 0x4c, 0x04, 0x2d, 0x03, 0x74, 0x08, 0x3c,
      0x03, 0x0f, 0x03, 0x3c, 0x07, 0x38, 0x08, 0x2b, 0x05, 0x82, 0xff, 0x11,
      0x18, 0x08, 0x2f, 0x11, 0x2d, 0x03, 0x20, 0x10, 0x21, 0x0f, 0x80, 0x8c,
      0x04, 0x82, 0x97, 0x19, 0x0b, 0x15, 0x88, 0x94, 0x05, 0x2f, 0x05, 0x3b,
      0x07, 0x02, 0x0e, 0x18, 0x09, 0x80, 0xb3, 0x2d, 0x74, 0x0c, 0x80, 0xd6,
      0x1a, 0x0c, 0x05, 0x80, 0xff, 0x05, 0x80, 0xdf, 0x0c, 0xee, 0x0d, 0x03,
      0x84, 0x8d, 0x03, 0x37, 0x09, 0x81, 0x5c, 0x14, 0x80, 0xb8, 0x08, 0x80,
      0xcb, 0x2a, 0x38, 0x03, 0x0a, 0x06, 0x38, 0x08, 0x46, 0x08, 0x0c, 0x06,
      0x74, 0x0b, 0x1e, 0x03, 0x5a, 0x04, 0x59, 0x09, 0x80, 0x83, 0x18, 0x1c,
      0x0a, 0x16, 0x09, 0x4c, 0x04, 0x80, 0x8a, 0x06, 0xab, 0xa4, 0x0c, 0x17,
      0x04, 0x31, 0xa1, 0x04, 0x81, 0xda, 0x26, 0x07, 0x0c, 0x05, 0x05, 0x80,
      0xa5, 0x11, 0x81, 0x6d, 0x10, 0x78, 0x28, 0x2a, 0x06, 0x4c, 0x04, 0x80,
      0x8d, 0x04, 0x80, 0xbe, 0x03, 0x1b, 0x03, 0x0f, 0x0d,
  };
  static constexpr unsigned char normal1[] = {
      0x5e, 0x22, 0x7b, 0x05, 0x03, 0x04, 0x2d, 0x03, 0x66, 0x03, 0x01, 0x2f,
      0x2e, 0x80, 0x82, 0x1d, 0x03, 0x31, 0x0f, 0x1c, 0x04, 0x24, 0x09, 0x1e,
      0x05, 0x2b, 0x05, 0x44, 0x04, 0x0e, 0x2a, 0x80, 0xaa, 0x06, 0x24, 0x04,
      0x24, 0x04, 0x28, 0x08, 0x34, 0x0b, 0x01, 0x80, 0x90, 0x81, 0x37, 0x09,
      0x16, 0x0a, 0x08, 0x80, 0x98, 0x39, 0x03, 0x63, 0x08, 0x09, 0x30, 0x16,
      0x05, 0x21, 0x03, 0x1b, 0x05, 0x01, 0x40, 0x38, 0x04, 0x4b, 0x05, 0x2f,
      0x04, 0x0a, 0x07, 0x09, 0x07, 0x40, 0x20, 0x27, 0x04, 0x0c, 0x09, 0x36,
      0x03, 0x3a, 0x05, 0x1a, 0x07, 0x04, 0x0c, 0x07, 0x50, 0x49, 0x37, 0x33,
      0x0d, 0x33, 0x07, 0x2e, 0x08, 0x0a, 0x81, 0x26, 0x52, 0x4e, 0x28, 0x08,
      0x2a, 0x56, 0x1c, 0x14, 0x17, 0x09, 0x4e, 0x04, 0x1e, 0x0f, 0x43, 0x0e,
      0x19, 0x07, 0x0a, 0x06, 0x48, 0x08, 0x27, 0x09, 0x75, 0x0b, 0x3f, 0x41,
      0x2a, 0x06, 0x3b, 0x05, 0x0a, 0x06, 0x51, 0x06, 0x01, 0x05, 0x10, 0x03,
      0x05, 0x80, 0x8b, 0x62, 0x1e, 0x48, 0x08, 0x0a, 0x80, 0xa6, 0x5e, 0x22,
      0x45, 0x0b, 0x0a, 0x06, 0x0d, 0x13, 0x39, 0x07, 0x0a, 0x36, 0x2c, 0x04,
      0x10, 0x80, 0xc0, 0x3c, 0x64, 0x53, 0x0c, 0x48, 0x09, 0x0a, 0x46, 0x45,
      0x1b, 0x48, 0x08, 0x53, 0x1d, 0x39, 0x81, 0x07, 0x46, 0x0a, 0x1d, 0x03,
      0x47, 0x49, 0x37, 0x03, 0x0e, 0x08, 0x0a, 0x06, 0x39, 0x07, 0x0a, 0x81,
      0x36, 0x19, 0x80, 0xb7, 0x01, 0x0f, 0x32, 0x0d, 0x83, 0x9b, 0x66, 0x75,
      0x0b, 0x80, 0xc4, 0x8a, 0xbc, 0x84, 0x2f, 0x8f, 0xd1, 0x82, 0x47, 0xa1,
      0xb9, 0x82, 0x39, 0x07, 0x2a, 0x04, 0x02, 0x60, 0x26, 0x0a, 0x46, 0x0a,
      0x28, 0x05, 0x13, 0x82, 0xb0, 0x5b, 0x65, 0x4b, 0x04, 0x39, 0x07, 0x11,
      0x40, 0x05, 0x0b, 0x02, 0x0e, 0x97, 0xf8, 0x08, 0x84, 0xd6, 0x2a, 0x09,
      0xa2, 0xf7, 0x81, 0x1f, 0x31, 0x03, 0x11, 0x04, 0x08, 0x81, 0x8c, 0x89,
      0x04, 0x6b, 0x05, 0x0d, 0x03, 0x09, 0x07, 0x10, 0x93, 0x60, 0x80, 0xf6,
      0x0a, 0x73, 0x08, 0x6e, 0x17, 0x46, 0x80, 0x9a, 0x14, 0x0c, 0x57, 0x09,
      0x19, 0x80, 0x87, 0x81, 0x47, 0x03, 0x85, 0x42, 0x0f, 0x15, 0x85, 0x50,
      0x2b, 0x80, 0xd5, 0x2d, 0x03, 0x1a, 0x04, 0x02, 0x81, 0x70, 0x3a, 0x05,
      0x01, 0x85, 0x00, 0x80, 0xd7, 0x29, 0x4c, 0x04, 0x0a, 0x04, 0x02, 0x83,
      0x11, 0x44, 0x4c, 0x3d, 0x80, 0xc2, 0x3c, 0x06, 0x01, 0x04, 0x55, 0x05,
      0x1b, 0x34, 0x02, 0x81, 0x0e, 0x2c, 0x04, 0x64, 0x0c, 0x56, 0x0a, 0x80,
      0xae, 0x38, 0x1d, 0x0d, 0x2c, 0x04, 0x09, 0x07, 0x02, 0x0e, 0x06, 0x80,
      0x9a, 0x83, 0xd8, 0x08, 0x0d, 0x03, 0x0d, 0x03, 0x74, 0x0c, 0x59, 0x07,
      0x0c, 0x14, 0x0c, 0x04, 0x38, 0x08, 0x0a, 0x06, 0x28, 0x08, 0x22, 0x4e,
      0x81, 0x54, 0x0c, 0x15, 0x03, 0x03, 0x05, 0x07, 0x09, 0x19, 0x07, 0x07,
      0x09, 0x03, 0x0d, 0x07, 0x29, 0x80, 0xcb, 0x25, 0x0a, 0x84, 0x06,
  };
  auto lower = static_cast<uint16_t>(cp);
  if (cp < 0x10000) {
    return is_printable(lower, singletons0,
                        sizeof(singletons0) / sizeof(*singletons0),
                        singletons0_lower, normal0, sizeof(normal0));
  }
  if (cp < 0x20000) {
    return is_printable(lower, singletons1,
                        sizeof(singletons1) / sizeof(*singletons1),
                        singletons1_lower, normal1, sizeof(normal1));
  }
  if (0x2a6de <= cp && cp < 0x2a700) return false;
  if (0x2b735 <= cp && cp < 0x2b740) return false;
  if (0x2b81e <= cp && cp < 0x2b820) return false;
  if (0x2cea2 <= cp && cp < 0x2ceb0) return false;
  if (0x2ebe1 <= cp && cp < 0x2f800) return false;
  if (0x2fa1e <= cp && cp < 0x30000) return false;
  if (0x3134b <= cp && cp < 0xe0100) return false;
  if (0xe01f0 <= cp && cp < 0x110000) return false;
  return cp < 0x110000;
}

}  // namespace detail

FMT_END_NAMESPACE

#endif  // FMT_FORMAT_INL_H_
#endif

// Restore _LIBCPP_REMOVE_TRANSITIVE_INCLUDES.
#ifdef FMT_REMOVE_TRANSITIVE_INCLUDES
#  undef _LIBCPP_REMOVE_TRANSITIVE_INCLUDES
#endif

#endif  // FMT_FORMAT_H_
