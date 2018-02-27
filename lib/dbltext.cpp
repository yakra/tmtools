#ifndef tmtools_dbltext_cpp
#define tmtools_dbltext_cpp

#include <cmath>
#include <string>

std::string DblText(double number, int DecDig, bool DoubleBug)
{	if (!DoubleBug) return std::to_string(number); // requires -std=c++11 on compile.

	// Old, buggy rounding is retained for comparison against output from earlier gisplunge versions.
	// Least significant digit will sometimes be 1 too low, due to floating point imprecision + truncation.
	// That, and it's still used for adding integers to SegDump filenames, without the ".000000". Hoping integers work out OK. :P
	std::string result = "\0";
	// sign
	if (number < 0)
	{	result = "-";
		number *= -1;
	}
	// round least significant digit
	number *= pow(10,DecDig);
	if (number-int(number) >= 0.5) number++;
	number /= pow(10,DecDig);
	// left of decimal
	if (!int(number)) result.push_back('0');
	else for (int pl = log10(number); pl >= 0; pl--)
	{	unsigned int sPlace = pow(10, pl);
		unsigned char digit = number/sPlace;
		result.push_back(digit+48);
		number -= digit*sPlace;
	}
	// decimal
	if (DecDig)
	{	result += ".";
		while (DecDig)
		{	number *= 10;
			result.push_back(int(number)+48);
			number -= int(number);
			DecDig--;
		}
	}
	return result;
}
#endif
