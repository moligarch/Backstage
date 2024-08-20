#define SHOW_LOG
#include "Backstage/Backstage.h"
#include <string>
#include <iostream>
using namespace std::string_literals;

int main()
{
	const std::string id{ "Hidden" };
	std::string log_msg;
	while (std::cin >> log_msg)
	{
		Backstage<level::info>(id) << log_msg;
	}
	return 0;
}