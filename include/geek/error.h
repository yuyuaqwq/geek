#pragma once
#include <cstdint>
#include <string>

namespace geek {
enum class ErrorType
{
	Win,
	Nt,
	Geek
};

//TODO 应用错误码
class LastError {
public:
	static ErrorType TypeOfError();

	static uint32_t Code();
	static std::string Message();
	static std::string_view Where();
};
}
