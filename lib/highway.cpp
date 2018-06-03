#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include "caps.cpp"	// includes string
#include "dbltext.cpp"	// includes string
#include "waypoint.h"	// includes deque, string

class highway
{	public:
	std::string System, Region, Route, Banner, Abbrev, City, Root;
	std::vector<std::string> AltRouteNames;
	std::list<waypoint> pt;

	highway(std::string SysIn, std::string RegIn, std::string RteIn, std::string BanIn, std::string AbbIn, std::string CityIn, std::string RootIn, std::string AltsIn)
	{	System = SysIn;
		Region = RegIn;
		Route = RteIn;
		Banner = BanIn;
		Abbrev = AbbIn;
		City = CityIn;
		Root = RootIn;

		char *Alts = new char[AltsIn.size()+1];
		strcpy(Alts, AltsIn.data());
		for (char *ARN = strtok(Alts, ","); ARN; ARN = strtok(0, ",")) AltRouteNames.push_back(ARN);
	}

	~highway()
	{	AltRouteNames.clear();
		pt.clear();
	}

	std::string CSVline()
	{	std::string CSVline = System + ";" + Region + ";" + Route + ";" + Banner + ";" + Abbrev + ";" + City + ";" + Root + ";";
		for (unsigned int i = 0; i+1 < AltRouteNames.size(); i++) CSVline += AltRouteNames[i] + ",";
		if (AltRouteNames.size()) CSVline += AltRouteNames.back();
		return CSVline;
	}

	unsigned int GetIndByLabel(std::string lbl) // returning a value >= pt.size() == fail.
	{	unsigned int p = 0;
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

	bool NameMatch(std::string ListName)
	{	if (ListName == caps(Route+Banner+Abbrev)) return 1;
		for (unsigned short i = 0; i < AltRouteNames.size(); i++)
			if (ListName == caps(AltRouteNames[i])) return 1;
		return 0;
	}

	void write(std::string OrigName, unsigned char OrgBy)	{ write(OrigName, 1, OrgBy, 0); }		// general purpose usage
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

highway* BuildRte(std::string filename, std::string Sys, std::string Reg, std::string Rte, std::string Ban, std::string Abb, std::string City, std::string Root, std::string Alts)
{	std::ifstream WPT (filename);
	if (!WPT)
	{	std::cout << filename << " file not found\n";
		return 0;
	}
	highway *hwy = new highway(Sys, Reg, Rte, Ban, Abb, City, Root, Alts);

	std::string WPTline;
	while (getline(WPT, WPTline))
	{	while (WPTline.back() == 0x0D) WPTline.erase(WPTline.end()-1);	// trim DOS newlines
		waypoint point(hwy, WPTline);
		if (!point.label.empty()) hwy->pt.push_back(point);
	}
	return hwy;
}

bool StrInVec(std::string &needle, std::vector<std::string> haystack)
{	for (unsigned int i = 0; i < haystack.size(); i++)
		if (needle == haystack[i]) return 1;
	return 0;
}

bool ChoppedRtesCSV(std::vector<highway*> &HwyVec, std::vector<std::string> &IncludeRg, std::string input, std::string path, bool RepoDirs)
{	std::ifstream CSV(input.data());
	if (!CSV)
	{	std::cout << "InputFile \"" << input << "\" not found!" << '\n';
		return 0;
	}
	std::string CSVline;
	getline(CSV, CSVline); //skip header row

	while (getline(CSV, CSVline)) // build hwy vector
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
			highway *hwy = BuildRte(wptFile, System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames);
			if (hwy) HwyVec.push_back(hwy);
		     }
	} // end while (build hwy vector)
	return 1;
}

bool ChoppedRtesCSV(std::vector<highway*> &HwyVec, std::string input, std::string path, bool RepoDirs)
{	std::vector<std::string> dummy;
	return ChoppedRtesCSV(HwyVec, dummy, input, path, RepoDirs);
}
