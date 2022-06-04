// ----------------------------------------------------------------------------
// @file    usf_main.hpp
// @brief   Main process functions and public interface.
// @date    14 January 2019
// ----------------------------------------------------------------------------

#ifndef USF_MAIN_HPP
#define USF_MAIN_HPP

namespace usf {
  namespace internal {

    template <typename CharT>
    constexpr void parse_format_string(std::span<CharT> &str, std::basic_string_view<CharT> &fmt) {
      CharT *str_it = str.begin().base();
      const CharT *fmt_it = fmt.cbegin();
      while (fmt_it < fmt.cend() && str_it < str.end().base()) {
        if (*fmt_it == '{') {
          if (*(fmt_it + 1) == '{') {
            // Found '{{' escape character, skip the first and copy the second '{'.
            ++fmt_it;
            *str_it++ = *fmt_it++;
          } else {
            // A type format should follow...
            break;
          }
        } else if (*fmt_it == '}') {
          USF_ENFORCE(*(fmt_it + 1) == '}', std::runtime_error);

          // Found '}}' escape character, skip the first and copy the second '}'.
          ++fmt_it;
          *str_it++ = *fmt_it++;
        } else {
          // Copy literal text
          *str_it++ = *fmt_it++;
        }
      }

      //USF_ENFORCE(str_it < str.end(), std::runtime_error);

      str = str.subspan(static_cast<uint32_t>(str_it - str.data()));    // TODO: Sign conversion error
                                                                        //    str.remove_prefix();
      fmt.remove_prefix(static_cast<uint32_t>(fmt_it - fmt.cbegin()));  // TODO: Sign conversion error
    }

    template <typename CharT>
    constexpr void process(std::span<CharT> &str, std::basic_string_view<CharT> &fmt,
                           const Argument<CharT> *const args, const int arg_count) {
      // Argument's sequential index
      int arg_seq_index = 0;

      parse_format_string(str, fmt);

      while (!fmt.empty()) {
        ArgFormat<CharT> format(fmt, arg_count);  // Parse the format specifier and generate a format object

        // Determine which argument index to use, sequential or positional.
        int arg_index = format.index();

        if (arg_index < 0) {  // If it is sequential (arg_index == -1)
          USF_ENFORCE(arg_seq_index < arg_count, std::runtime_error);
          arg_index = arg_seq_index++;  // Assign it the next index
        }

        args[arg_index].format(str, format);

        parse_format_string(str, fmt);
      }
    }

  }  // namespace internal

  template <typename CharT, typename... Args>
  constexpr std::span<CharT> basic_format_to(std::span<CharT> str, std::basic_string_view<CharT> fmt) {
    auto str_begin = str.begin();

    internal::parse_format_string(str, fmt);

    USF_ENFORCE(fmt.empty(), std::runtime_error);

#if !defined(USF_DISABLE_STRING_TERMINATION)
    // If not disabled in configuration, null terminate the resulting string.
    str[0] = CharT{};
#endif

    // Return a string span to the resulting string
    return std::span<CharT>(str_begin, str.begin());
  }

  template <typename CharT, typename... Args>
  constexpr std::span<CharT> basic_format_to(std::span<CharT> str, std::basic_string_view<CharT> fmt, Args &&...args) {
    // Nobody should be that crazy, still... it costs nothing to be sure!
    static_assert(sizeof...(Args) < 128, "usf::basic_format_to(): crazy number of arguments supplied!");

    auto str_begin = str.begin();  // This keeps the start of the string since the str pointer will be incremented throughout the following methods

    const internal::Argument<CharT> arguments[sizeof...(Args)]{internal::make_argument<CharT>(args)...};

    internal::process(str, fmt, arguments, static_cast<int>(sizeof...(Args)));

#if !defined(USF_DISABLE_STRING_TERMINATION)
    // If not disabled in configuration, null terminate the resulting string.
    str[0] = CharT{};  // Since str has been incremented through the above methods, it now resides at the end of the formatted string so the termination can be written directly at it
#endif

    // Return a string span to the resulting string
    return std::span<CharT>(str_begin, str.begin());  // The complete string is now residing between str_begin and str, so return that
  }

  template <typename CharT, typename... Args>
  constexpr CharT *
  basic_format_to(CharT *str, const std::ptrdiff_t str_count, std::basic_string_view<CharT> fmt, Args &&...args) {
    return basic_format_to(std::span<CharT>(str, str_count), fmt, args...).end().base();
  }

  // ----------------------------------------------------------------------------
  // Formats a char string
  // ---------------------------------------------------------------------------
  template <typename... Args>
  constexpr std::span<char> format_to(std::span<char> str, std::string_view fmt, Args &&...args) {
    return basic_format_to(str, fmt, args...);
  }

  //  template<typename... Args>
  //  constexpr
  //  char *format_to(char *str, const std::ptrdiff_t str_count, std::string_view fmt, Args &&... args) {
  //    return basic_format_to(str, str_count, fmt, args...);
  //  }

  //// ----------------------------------------------------------------------------
  //// Formats a wchar_t string
  //// ---------------------------------------------------------------------------
  //template <typename... Args> constexpr
  //WStringSpan format_to(WStringSpan str, WStringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, fmt, args...);
  //}
  //
  //template <typename... Args> constexpr
  //wchar_t* format_to(wchar_t* str, const std::ptrdiff_t str_count, WStringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, str_count, fmt, args...);
  //}
  //
  //// ----------------------------------------------------------------------------
  //// Formats a char8_t string
  //// ---------------------------------------------------------------------------
  //#if defined(USF_CPP20_CHAR8_T_SUPPORT)
  //template <typename... Args> constexpr
  //U8StringSpan format_to(U8StringSpan str, U8StringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, fmt, args...);
  //}
  //
  //template <typename... Args> constexpr
  //char8_t* format_to(char8_t* str, const std::ptrdiff_t str_count, U8StringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, str_count, fmt, args...);
  //}
  //#endif // defined(USF_CPP20_CHAR8_T_SUPPORT)
  //
  //// ----------------------------------------------------------------------------
  //// Formats a char16_t string
  //// ---------------------------------------------------------------------------
  //template <typename... Args> constexpr
  //U16StringSpan format_to(U16StringSpan str, U16StringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, fmt, args...);
  //}
  //
  //template <typename... Args> constexpr
  //char16_t* format_to(char16_t* str, const std::ptrdiff_t str_count, U16StringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, str_count, fmt, args...);
  //}
  //
  //// ----------------------------------------------------------------------------
  //// Formats a char32_t string
  //// ---------------------------------------------------------------------------
  //template <typename... Args> constexpr
  //U32StringSpan format_to(U32StringSpan str, U32StringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, fmt, args...);
  //}
  //
  //template <typename... Args> constexpr
  //char32_t* format_to(char32_t* str, const std::ptrdiff_t str_count, U32StringView fmt, Args&&... args)
  //{
  //    return basic_format_to(str, str_count, fmt, args...);
  //}
  //
  //// ----------------------------------------------------------------------------
  //// Formats a byte string as char string
  //// ----------------------------------------------------------------------------
  //template <typename... Args> constexpr
  //ByteStringSpan format_to(ByteStringSpan str, StringView fmt, Args&&... args)
  //{
  //    static_assert(CHAR_BIT == 8, "usf::format_to(): invalid char size.");
  //    char *end = basic_format_to(reinterpret_cast<char*>(str.data()), str.size(), fmt, args...);
  //
  //    return ByteStringSpan(str.begin(), reinterpret_cast<uint8_t*>(end));
  //}
  //
  //template <typename... Args> constexpr
  //uint8_t* format_to(uint8_t* str, const std::ptrdiff_t str_count, StringView fmt, Args&&... args)
  //{
  //    static_assert(CHAR_BIT == 8, "usf::format_to(): invalid char size.");
  //    return reinterpret_cast<uint8_t*>(basic_format_to(reinterpret_cast<char*>(str), str_count, fmt, args...));
  //}

}  // namespace usf

#endif  // USF_MAIN_HPP
