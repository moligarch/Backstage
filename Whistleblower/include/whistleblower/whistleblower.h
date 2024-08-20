#pragma once

#include <string>
#include <Windows.h>


class Whistleblower
{
public:
	Whistleblower(const std::string& identifier);
	~Whistleblower();

	void ReadAndProcessLogs() const; // Function to read and process logs
private:
	HANDLE m_hMapFile;
	HANDLE m_lpMapAddress;
	HANDLE m_hMutex;
	PLARGE_INTEGER m_bufferSize;
};
