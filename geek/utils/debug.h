#pragma once
#include <geek/utils/strutil.h>
#include <geek/global.h>

namespace geek {
void DebugOutput(std::string_view str) noexcept;
void WDebugOutput(std::wstring_view str) noexcept;

namespace internal {
void AssertionFailed(std::string_view file, std::string_view func, int line, std::string_view condition, std::string_view message = "") noexcept;
void WAssertionFailed(std::wstring_view file, std::wstring_view func, int line, std::wstring_view condition, std::wstring_view message = L"") noexcept;
}
}

#if defined(GEEK_BUILD_DEBUG) || defined(GEEK_ALWAYS_ASSERT)

#define _GEEK_ASSERT(cond, msg)								\
	do {													\
		if (!!bool(cond)) break;								\
		::geek::internal::AssertionFailed(__FILE__, __FUNCTION__, __LINE__, #cond, msg); \
	} while (0)

#define _GEEK_WASSERT(cond, msg)							\
	do {													\
		if (!!bool(cond)) break;								\
		::geek::internal::WAssertionFailed(_GEEK_WIDE_STR(__FILE__), _GEEK_WIDE_STR(__FUNCTION__), _GEEK_WIDE_STR(__LINE__), #cond, msg); \
	} while (0)

#define _GEEK_WIDE_STR_(x)		L ## x
#define _GEEK_WIDE_STR(x)		_GEEK_WIDE_STR_(x)

#define GEEK_ASSERT(cond, ...)	_GEEK_ASSERT(cond, ::geek::StrUtil::Combine(__VA_ARGS__))
#define GEEK_WASSERT(cond, ...)	_GEEK_WASSERT(cond, ::geek::StrUtil::WCombine(__VA_ARGS__))

#else

#define GEEK_ASSERT(cond, ...)
#define GEEK_WASSERT(cond, ...)

#endif

#define GEEK_THROW(...)			throw std::exception(geek::internal::MsgOfThrow(__FILE__, __FUNCTION__, __LINE__, geek::StrUtil::Combine(__VA_ARGS__)).c_str())
#define GEEK_WTHROW(...)		throw std::exception(geek::internal::WMsgOfThrow(_GEEK_WIDE_STR(__FILE__), _GEEK_WIDE_STR(__FUNCTION__), _GEEK_WIDE_STR(__LINE__), geek::StrUtil::WCombine(__VA_ARGS__)).c_str())
