#pragma once
#include <string>
#include <iostream>
#include <type_traits>
#include <sstream>

#include <Windows.h>

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
	Backstage(const std::string& identifier) : m_identifier(identifier)
	{
		init();

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
		// Wait for mutex
		WaitForSingleObject(m_hMutex, INFINITE);
		// Copy to shared memory (ensure it fits)
		size_t msgSize = m_message.str().size() + 1; // +1 for null terminator
		if (msgSize <= SHARED_MEM_SIZE) {
			memcpy(m_lpMapAddress, m_message.str().c_str(), msgSize);
		}
		else {
			// Handle buffer overflow (e.g., truncate message, log error)
			memcpy(m_lpMapAddress, "Log was overflowed", msgSize);
		}

		// Release mutex
		ReleaseMutex(m_hMutex);

		if (m_lpMapAddress != NULL)
			UnmapViewOfFile(m_lpMapAddress);

		if (m_hMapFile != NULL)
			CloseHandle(m_hMapFile);

		if (m_hMutex != NULL)
			CloseHandle(m_hMutex);
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

	void init()
	{
		// Create shared memory
		m_hMapFile = CreateFileMappingA(
			INVALID_HANDLE_VALUE,    // Use paging file
			NULL,                    // Default security 
			PAGE_READWRITE,          // Read/write access
			0,
			// Maximum object size (high-order DWORD) 
			SHARED_MEM_SIZE,         // Maximum object size (low-order DWORD)   

			m_identifier.c_str());       // Name of mapping object 

		if (m_hMapFile == NULL) {
			// Handle error creating file mapping
			// Open shared memory
			m_hMapFile = OpenFileMappingA(
				FILE_MAP_ALL_ACCESS,   // Read/write access
				FALSE,                 // Do not inherit the name
				m_identifier.c_str());     // Name of mapping object 
			if (m_hMapFile == NULL)
			{
				return;
			}
		}

		m_lpMapAddress = MapViewOfFile(
			m_hMapFile,             // Handle to map object
			FILE_MAP_ALL_ACCESS,     // Read/write permission
			0,
			0,
			SHARED_MEM_SIZE);

		if (m_lpMapAddress == NULL) {
			// Handle error mapping view of file
			CloseHandle(m_hMapFile);
			m_hMapFile = NULL;
			return;
		}

		// Initialize mutex
		m_hMutex = CreateMutexA(
			NULL,              // Default security attributes
			FALSE,             // Initially not owned
			(m_identifier + "Mutex").c_str());

		if (m_hMutex == NULL) {
			// Open mutex
			m_hMutex = OpenMutexA(
				SYNCHRONIZE,           // Request full access
				FALSE,                 // Do not inherit the handle
				(m_identifier + "Mutex").c_str());
			if (m_hMutex == NULL)
			{
				// Handle error creating mutex
				UnmapViewOfFile(m_lpMapAddress);
				CloseHandle(m_hMapFile);
				m_hMapFile = NULL;
				return;
			}
		}
	}

	std::string m_identifier;
	std::wstringstream m_message;
	HANDLE m_hMapFile;
	HANDLE m_lpMapAddress;
	HANDLE m_hMutex;
	static const int SHARED_MEM_SIZE = 1024; // Adjust as needed
};
