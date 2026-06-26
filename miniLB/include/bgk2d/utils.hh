#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <string_view>
#include <vector>


namespace bgk::utils {
template<typename T>
static constexpr auto type_name() {
    std::string_view name, prefix, suffix;
#ifdef __clang__
    name = __PRETTY_FUNCTION__;
    prefix = "auto type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name = __PRETTY_FUNCTION__;
    prefix = "constexpr auto type_name() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name = __FUNCSIG__;
    prefix = "auto __cdecl type_name<";
    suffix = ">(void)";
#endif
    name.remove_prefix(sizeof("bgk::utils::") + prefix.size() -1 );
    name.remove_suffix(suffix.size());
    return name;
}


static inline void trim(std::string &s) {
    std::erase_if(s, [](char c) { return std::isspace(c); });
}

static constexpr inline size_t flp2 (size_t x)
	{
		    x = x | (x >> 1);
		    x = x | (x >> 2);
		    x = x | (x >> 4);
		    x = x | (x >> 8);
		    x = x | (x >> 16);
		    x = x | (x >> 32);
		    return x - (x >> 1);
	}

} // namespace bgk::utils

#endif // __UTILS_H__