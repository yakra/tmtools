#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include "caps.cpp"	// includes string
#include "dbltext.cpp"	// includes string
#include "waypoint.h"	// includes deque, string

#ifndef tmtools_tmsystem_cpp
class tmsystem
{
};
#endif

class highway
{	public:
	tmsystem *HwySys;
	std::string System, Region, Route, Banner, Abbrev, City, Root;
	std::vector<std::string> AltRouteNames;
	std::list<waypoint> pt;
	bool error;

	highway(std::string filename, tmsystem *SysPtr,
		std::string SysIn, std::string RegIn, std::string RteIn, std::string BanIn,
		std::string AbbIn, std::string CityIn, std::string RootIn, std::string AltsIn)
	{	HwySys = SysPtr;
		System = SysIn;
		Region = RegIn;
		Route = RteIn;
		Banner = BanIn;
		Abbrev = AbbIn;
		City = CityIn;
		Root = RootIn;

		char *Alts = new char[AltsIn.size()+1];
		strcpy(Alts, AltsIn.data());
		for (char *ARN = strtok(Alts, ","); ARN; ARN = strtok(0, ",")) AltRouteNames.push_back(ARN);

		// Build route from .wpt file
		std::ifstream WPT (filename);
		if (!WPT)
		{	std::cout << filename << " file not found\n";
			error = 1; return;
		}
		error = 0;
		std::string WPTline;
		while (getline(WPT, WPTline))
		{	while (WPTline.back() == 0x0D) WPTline.erase(WPTline.end()-1);	// trim DOS newlines
			waypoint point(this, WPTline);
			if (!point.label.empty()) pt.push_back(point);
		}
	}

	~highway()
	{	AltRouteNames.clear();
		pt.clear();
	}

    #ifdef tmtools_tmsystem_cpp
	bool operator < (highway &other)
	{	if (*HwySys < *other.HwySys) return 1;
		if (*HwySys > *other.HwySys) return 0;
		if (Root < other.Root) return 1;
		return 0;
	}

	bool operator > (highway &other)
	{	if (*HwySys > *other.HwySys) return 1;
		if (*HwySys < *other.HwySys) return 0;
		if (Root > other.Root) return 1;
		return 0;
	}
    #endif

	std::string CSVline()
	{	std::string CSVline = System + ";" + Region + ";" + Route + ";" + Banner + ";" + Abbrev + ";" + City + ";" + Root + ";";
		for (unsigned int i = 0; i+1 < AltRouteNames.size(); i++) CSVline += AltRouteNames[i] + ",";
		if (AltRouteNames.size()) CSVline += AltRouteNames.back();
		return CSVline;
	}

	unsigned int label2index(std::string lbl) // returning a value >= pt.size() == fail.
	{	unsigned int p = 0;
		while (lbl[0] == '+' || lbl[0] == '*') lbl.erase(0, 1);
		for (std::list<waypoint>::iterator point = pt.begin(); p < pt.size(); p++)
		{	for (unsigned int l = 0; l < point->label.size(); l++)
				if (caps(lbl) == caps(point->NakedLabel(l)))
				{	if (l)	std::cout << Region << ' ' << Route+Banner+Abbrev << ' ' << lbl \
						<< ": deprecated in favor of " << point->label[0] << '\n';
					return p;
				}
			point++;
		}
		std::cout << Region << ' ' << Route+Banner+Abbrev << ' ' << lbl << ": point label not recognized\n";
		return p;
	}

	std::list<waypoint>::iterator label2iter(std::string lbl) // returning pt.end() == fail.
	{	while (lbl[0] == '+' || lbl[0] == '*') lbl.erase(0, 1);
		for (std::list<waypoint>::iterator point = pt.begin(); point != pt.end(); point++)
		{	for (unsigned int l = 0; l < point->label.size(); l++)
				if (caps(lbl) == caps(point->NakedLabel(l)))
				{	if (l)	std::cout << Region << ' ' << Route+Banner+Abbrev << ' ' << lbl \
						<< ": deprecated in favor of " << point->label[0] << '\n';
					return point;
				}
		}
		std::cout << Region << ' ' << Route+Banner+Abbrev << ' ' << lbl << ": point label not recognized\n";
		return pt.end();
	}

	bool NameMatch(std::string ListName)
	{	if (ListName == caps(Route+Banner+Abbrev)) return 1;
		for (unsigned short i = 0; i < AltRouteNames.size(); i++)
			if (ListName == caps(AltRouteNames[i])) return 1;
		return 0;
	}

	void write(std::string path, unsigned char OrgBy)	{ write(path, 1, OrgBy, 0); }			// general purpose usage
	void write(unsigned char WriteMe, bool B)		{ write("output/orig/", WriteMe, 0, B); }	// gisplunge usage
	void write(std::string OrigName, unsigned char WriteMe, unsigned char OrgBy, bool B)
	{	std::string OffName = "output/off/";
		std::string OnName = "output/on/";
		if (OrgBy > 1) // organize by Region/System
		{	OrigName += Region + "/";
			OffName += Region + "/";
			OnName += Region + "/";
		}
		if (OrgBy > 0) // organize by System
		{	OrigName += System + "/";
			OffName += System + "/";
			OnName += System + "/";
		}
		OrigName += Root + ".wpt";
		OffName += Root + ".wpt";
		OnName += Root + ".wpt";
		std::ofstream OrigFile, OffFile, OnFile;
		if (WriteMe & 1) OrigFile.open(OrigName.data());
		if (WriteMe & 2) OffFile.open(OffName.data());
		if (WriteMe & 4) OnFile.open(OnName.data());
		for (std::list<waypoint>::iterator ptI = pt.begin(); ptI != pt.end(); ptI++) // process each point
		{	for (unsigned int i = 0; i < ptI->label.size(); i++) // write labels
			{	if (WriteMe & 1) OrigFile << ptI->label[i] << ' ';
				if (WriteMe & 2) OffFile << ptI->label[i] << ' ';
				if (WriteMe & 4) OnFile << ptI->label[i] << ' ';
			}
			if (WriteMe & 1) OrigFile << "http://www.openstreetmap.org/?lat=" << DblText(ptI->Lat,6,B) << "&lon=" << DblText(ptI->Lon,6,B) << '\n';
			if (WriteMe & 2) OffFile << "http://www.openstreetmap.org/?lat=" << DblText(ptI->OffLat,6,B) << "&lon=" << DblText(ptI->OffLon,6,B) << '\n';
			if (WriteMe & 4) OnFile << "http://www.openstreetmap.org/?lat=" << DblText(ptI->OnLat,6,B) << "&lon=" << DblText(ptI->OnLon,6,B) << '\n';
		}
		if (WriteMe & 1) OrigFile.close();
		if (WriteMe & 2) OffFile.close();
		if (WriteMe & 4) OnFile.close();
	}
};

bool StrInVec(std::string &needle, std::vector<std::string> haystack)
{	for (unsigned int i = 0; i < haystack.size(); i++)
		if (needle == haystack[i]) return 1;
	return 0;
}

tmsystem *NullSysPtr(tmsystem &sys, std::deque<tmsystem> &SysDeq, std::string &SysCode)
{	return 0;
}

void ChoppedRtesCSV(std::list<highway> &HwyList, std::vector<std::string> &IncludeRg, std::string input, std::string path, bool RepoDirs,
		    tmsystem &sys, std::deque<tmsystem> &SysDeq,
		    tmsystem *(*GetSysPtr)(tmsystem&, std::deque<tmsystem>&, std::string&))
{	std::ifstream CSV(input.data());
	if (!CSV)
	{	std::cout << "InputFile \"" << input << "\" not found!" << '\n';
		return;
	}
	std::string CSVline;
	getline(CSV, CSVline); //skip header row

	while (getline(CSV, CSVline)) // build hwy list
	{	std::string System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames;
		while (CSVline.back() == 0x0D) CSVline.erase(CSVline.end()-1);	// trim DOS newlines
		// parse CSV line
		unsigned int i = 0;
		while (i < CSVline.size() && CSVline[i] != ';') { System.push_back(CSVline[i]); i++; } i++;
		while (i < CSVline.size() && CSVline[i] != ';') { Region.push_back(CSVline[i]); i++; } i++;
		while (i < CSVline.size() && CSVline[i] != ';') { Route.push_back(CSVline[i]); i++; } i++;
		while (i < CSVline.size() && CSVline[i] != ';') { Banner.push_back(CSVline[i]); i++; } i++;
		while (i < CSVline.size() && CSVline[i] != ';') { Abbrev.push_back(CSVline[i]); i++; } i++;
		while (i < CSVline.size() && CSVline[i] != ';') { City.push_back(CSVline[i]); i++; } i++;
		while (i < CSVline.size() && CSVline[i] != ';') { Root.push_back(CSVline[i]); i++; } i++;
		while (i < CSVline.size() && CSVline[i] != ';') { AltRouteNames.push_back(CSVline[i]); i++; } i++;

		if (Root.empty()) std::cout << "Bad CSV line in " << input << ": \"" << CSVline << "\"\n";
		else if (IncludeRg.empty() || StrInVec(Region, IncludeRg))
		     {	std::string wptFile = path;
			if (RepoDirs) wptFile += Region+"/"+System+"/";
			wptFile += Root+".wpt";
			HwyList.emplace_back(wptFile, GetSysPtr(sys, SysDeq, System), System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames);
			if (HwyList.back().error) HwyList.pop_back();
		     }
	} // end while (build hwy list)
}

void ChoppedRtesCSV(std::list<highway> &HwyList, std::string input, std::string path, bool RepoDirs)
{	std::vector<std::string> IncludeRg;	// dummy
	std::deque<tmsystem> SysDeq;		// dummy
	tmsystem DummySys;			// dummy
	ChoppedRtesCSV(HwyList, IncludeRg, input, path, RepoDirs, DummySys, SysDeq, NullSysPtr);
}//*/
