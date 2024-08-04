#define SHOW_LOG
#include "Backstage/Backstage.h"
#include <string>
using namespace std::string_literals;

int main()
{
	Backstage<level::info>() << "This is Test."s;
	Backstage<level::fatal>() << 12345 << "   "s << 12378123ul << "    "s << 0.023;
	Backstage<level::fatal>() << L"This is Test as well"s;
	return 0;
}