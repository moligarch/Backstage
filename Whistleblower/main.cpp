// Whistleblower.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <whistleblower/whistleblower.h>
#include <iostream>

int main()
{
    const std::string id{ "Hidden" };
    auto debugger{ Whistleblower(id) };
    debugger.ReadAndProcessLogs();
}
