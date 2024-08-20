#include "whistleblower/whistleblower.h"
#include <string>
#include  <iostream>

Whistleblower::Whistleblower(const std::string& identifier)
{
    // Open shared memory
    m_hMapFile = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,   // Read/write access
        FALSE,                 // Do not inherit the name
        identifier.c_str());     // Name of mapping object 

    if (m_hMapFile == NULL) {
        // Opening failed, attempt to create a new file mapping
        m_hMapFile = CreateFileMappingA(
            INVALID_HANDLE_VALUE,   // Not associated with a file
            NULL,                   // Default security attributes
            PAGE_READWRITE,         // Read/write access
            0,               // Maximum object size (high-order DWORD)
            1024,            // Maximum object size (low-order DWORD, 1MB)
            identifier.c_str());    // Name of mapping object

        if (m_hMapFile == NULL) 
        {
            throw std::runtime_error("Unable to open file mapping");
        }
    }

    m_lpMapAddress = MapViewOfFile(
        m_hMapFile,             // Handle to map object
        FILE_MAP_ALL_ACCESS,  // Read/write permission
        0,
        0,
        0);                     // Map entire file

    if (m_lpMapAddress == NULL) {
        // Handle error mapping view of file
        CloseHandle(m_hMapFile);
        m_hMapFile = NULL;
        std::cout << "Failed to init m_lpMapAddress";
        return;
    }

    // Open mutex
    m_hMutex = OpenMutexA(
        SYNCHRONIZE,           // Request full access
        FALSE,                 // Do not inherit the handle
        (identifier + "Mutex").c_str());

    if (m_hMutex == NULL) {
        // Initialize mutex
        m_hMutex = CreateMutexA(
            NULL,              // Default security attributes
            FALSE,             // Initially not owned
            (identifier + "Mutex").c_str());
        if (m_hMutex == NULL)
        {
            // Handle error opening mutex
            UnmapViewOfFile(m_lpMapAddress);
            CloseHandle(m_hMapFile);
            m_hMapFile = NULL;
            return;
        }
    }
}

Whistleblower::~Whistleblower()
{
    if (m_lpMapAddress != NULL)
        UnmapViewOfFile(m_lpMapAddress);

    if (m_hMapFile != NULL)
        CloseHandle(m_hMapFile);

    if (m_hMutex != NULL)
        CloseHandle(m_hMutex);
}

void Whistleblower::ReadAndProcessLogs() const
{
    while (true) { // Continuously read and process logs (you might want to add a termination condition)

        // Wait for mutex
        WaitForSingleObject(m_hMutex, INFINITE);

        // Read from shared memory
        BOOL res = GetFileSizeEx(m_hMapFile, m_bufferSize);
        if (!res) {
            continue;
        }

        std::string logMessage;
        if (!m_bufferSize) {
            char* buffer = static_cast<char*>(m_lpMapAddress);
            logMessage = std::string(buffer, size_t(m_bufferSize));
        }

        // Release mutex
        ReleaseMutex(m_hMutex);

        // Process the log message (replace with your actual logic)
        if (!logMessage.empty()) {
            std::cout << logMessage << std::endl;
        }
    }
}