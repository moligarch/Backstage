#pragma once
#include <string>
#include <iostream>
#include <type_traits>
#include <sstream>

enum class level
{
	debug = 0,
	info,
	warning,
	error,
	fatal
};

template <level _Level>
class Backstage final
{
public:
	Backstage()
	{
		switch (_Level)
		{
		case level::debug:
			m_message << "[DEBUG] ";
			break;
		case level::info:
			m_message << "[INFO] ";
			break;
		case level::warning:
			m_message << "[WARN] ";
			break;
		case level::error:
			m_message << "[ERR] ";
			break;
		case level::fatal:
			m_message << "[FATAL] ";
			break;
		default:
			break;
		}
	}
	~Backstage()
	{
		std::wcout << m_message.str() << std::endl;
	}

	template<typename U>
	Backstage& operator<<(const U& value) {
		log<U>(value);
		return *this;
	}


private:
	template <typename T>
	void log(const T& msg)
	{
		if constexpr (
			std::is_same_v<T, std::wstring>/*		||
			std::is_same_v < T, const wchar_t*> ||
			std::is_same_v < T, wchar_t*>		||
			std::is_same_v < T, const wchar_t>	||
			std::is_same_v < T, wchar_t>*/)
		{
			m_message << msg;
		}
		else
		{
			std::stringstream ss;
			ss << msg;
			size_t cSize = ss.str().size() + 1;
			wchar_t* wc = new(std::nothrow) wchar_t[cSize];
			if (!wc)
				return;
			size_t convertedChars = 0;
			mbstowcs_s(&convertedChars, wc, cSize, ss.str().c_str(), _TRUNCATE);
			std::wstring wStr(wc);
			delete[] wc;
			wc = nullptr;
			m_message << wStr;
		}
	}

	std::wstringstream m_message;
};
