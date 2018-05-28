#ifndef tmtools_caps_cpp
#define tmtools_caps_cpp
#include <string>

std::string caps(std::string str)
{	for (unsigned int i = 0; i < str.size(); i++)
		if (str[i] >= 97 && str[i] <= 122) str[i] -= 0x20;
	return str;
}
#endif
