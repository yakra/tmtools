#ifndef tmtools_tmsystem_cpp
#define tmtools_tmsystem_cpp
#include <deque>
#include <iostream>
#include <string>
#include <vector>

class tmsystem
{	public:
	std::string filename;
	std::string System, CountryCode, Name, Color;	// CSV data
	  std::string UnColor, ClColor;			// hex codes for canvas
	unsigned char Tier;				// CSV data
	unsigned char SubTier;				// color hierarchy for canvas
	std::string Level;				// CSV data
	  char LevNum;					// numeric level representation
		// 0:	boundaries
		// 1:	unrecognized system code
		// 2:	devel
		// 3:	preview
		// 4:	active
		// Negative numbers are reserved for representing commented out systems.
		// Boundaries are not included in systems.csv & thus can't be commented out. Thus no need to distinguish +/- 0.

	tmsystem(std::string S, std::string CC, std::string N, std::string Co, unsigned char T, std::string L, std::string FN)
	{	System = S;
		CountryCode = CC;
		Name = N;
		Color = Co;
		Tier = T;
		SetSubTier();
		Level = L;
		SetLevNum();
		filename = FN;
	}

	tmsystem(std::string FN)
	{	Color = "default";
		Tier = 255;
		SubTier = 9;
		LevNum = 5; // Yes, five. Hail Eris!
		filename = FN;
	}

	tmsystem(){}

	bool operator < (tmsystem &other)
	{	if (Tier < other.Tier) return 1;
		if (Tier > other.Tier) return 0;
		if (SubTier < other.SubTier) return 1;
		return 0;
	}

	bool operator > (tmsystem &other)
	{	if (Tier > other.Tier) return 1;
		if (Tier < other.Tier) return 0;
		if (SubTier > other.SubTier) return 1;
		return 0;
	}

	std::string CSVline()
	{	return System+";"+CountryCode+";"+Name+";"+Color+";"+std::to_string(Tier)+";"+Level;
	}

	void SetColors(std::vector<std::string> &N_Colors, std::vector<std::string> &UnColors, std::vector<std::string> &ClColors)
	{	unsigned int i;
		for (i = 0; i < N_Colors.size(); i++)
		  if	(Color == N_Colors[i])
		  {	UnColor = UnColors[i];
			ClColor = ClColors[i];
			break;
		  }
		if (i == N_Colors.size())
		{	if (Color == "b_water")
			{	std::cout << "b_water color code unspecified; defaulting to 0000a0\n";
				UnColor = "0000a0";	ClColor = "0000a0";	return;
			}
			if (Color == "b_subdiv")
			{	std::cout << "b_subdiv color code unspecified; defaulting to a0a0a0\n";
				UnColor = "a0a0a0";	ClColor = "a0a0a0";	return;
			}
			if (Color == "b_country")
			{	std::cout << "b_country color code unspecified; defaulting to 3c3c3c\n";
				UnColor = "3c3c3c";	ClColor = "3c3c3c";	return;
			}
			if (Color == "yellow")
			{	std::cout << "yellow color code unspecified; defaulting to FFD864 & E8B000\n";
				UnColor = "FFD864";	ClColor = "E8B000";	return;
			}
			if (Color == "brown")
			{	std::cout << "brown color code unspecified; defaulting to 999866 & 996600\n";
				UnColor = "999866";	ClColor = "996600";	return;
			}
			if (Color == "lightsalmon")
			{	std::cout << "lightsalmon color code unspecified; defaulting to E0A2A2 & F09673\n";
				UnColor = "E0A2A2";	ClColor = "F09673";	return;
			}
			if (Color == "magenta")
			{	std::cout << "magenta color code unspecified; defaulting to FF64FF & D000D0\n";
				UnColor = "FF64FF";	ClColor = "D000D0";	return;
			}
			if (Color == "red")
			{	std::cout << "red color code unspecified; defaulting to FF6464 & E00000\n";
				UnColor = "FF6464";	ClColor = "E00000";	return;
			}
			if (Color == "green")
			{	std::cout << "green color code unspecified; defaulting to 64C864 & 00E000\n";
				UnColor = "64C864";	ClColor = "00E000";	return;
			}
			if (Color == "teal")
			{	std::cout << "teal color code unspecified; defaulting to 64C8C8 & 008CA0\n";
				UnColor = "64C8C8";	ClColor = "008CA0";	return;
			}
			if (Color == "blue")
			{	std::cout << "blue color code unspecified; defaulting to 6464FF & 0000DC\n";
				UnColor = "6464FF";	ClColor = "0000DC";	return;
			}
			std::cout << "Unrecognized Color code \"" << Color << "\" will be colored gray. (System = ";
			if (System.empty()) std::cout << filename << ")\n";
			else std::cout << System << ")\n";
				UnColor = "aaaaaa";	ClColor = "555555";
		}
	}

	void SetSubTier()
	{	if (Color == "b_water")		{ SubTier = 10; return; }
		if (Color == "b_subdiv")	{ SubTier = 12; return; }
		if (Color == "b_country")	{ SubTier = 11; return; }
		if (Color == "yellow")		{ SubTier = 8; return; }
		if (Color == "brown")		{ SubTier = 7; return; }
		if (Color == "lightsalmon")	{ SubTier = 6; return; }
		if (Color == "magenta")		{ SubTier = 5; return; }
		if (Color == "red")		{ SubTier = 4; return; }
		if (Color == "green")		{ SubTier = 3; return; }
		if (Color == "teal")		{ SubTier = 2; return; }
		if (Color == "blue")		{ SubTier = 1; return; }
		/* default/unrecognized */	  SubTier = 9;
	}

	void SetLevNum()
	{	if (Level == "active") LevNum = 4;
	   else if (Level == "preview") LevNum = 3;
	   else if (Level == "devel") LevNum = 2;
	   else if (Level == "boundaries" || Level == "_boundaries") LevNum = 0;
	   else LevNum = 1;
	}

};

tmsystem* tmSysPtr(tmsystem &sys, std::deque<tmsystem> &SysDeq, std::string &SysCode)
{	if (sys.System.empty()) // IE, custom system CSV
	  for (unsigned int i = 0; i < SysDeq.size(); i++)
	    if (SysDeq[i].System == SysCode)
	      return &SysDeq[i];
	    // unrecognized codes fall thru...
	return &sys;
}
#endif
