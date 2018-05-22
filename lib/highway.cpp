#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include "waypoint.cpp"	// includes deque, string
#include "dbltext.cpp"	// includes string

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
				if (lbl == point->NakedLabel(l))
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
	{	if (ListName == Route+Banner+Abbrev) return 1;
		for (unsigned short i = 0; i < AltRouteNames.size(); i++)
			if (ListName == AltRouteNames[i]) return 1;
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
	WPT.seekg(0, std::ios::end); unsigned int EoF = WPT.tellg(); WPT.seekg(0);
	highway *hwy = new highway(Sys, Reg, Rte, Ban, Abb, City, Root, Alts);

	while (WPT.tellg() < EoF) // step thru each WPT line
	{	std::string WPTline; // read individual line
		for (char charlie = 0; charlie != '\n' && WPT.tellg() < EoF; WPTline.push_back(charlie)) WPT.get(charlie);
		while (WPTline.back() == 0x0A || WPTline.back() == 0x0D)	// either DOS or UNIX...
			WPTline.erase(WPTline.end()-1);				// strip out terminal '\n'
		// parse WPT line
		waypoint point;
		char *wlArr = new char[WPTline.size()+1];
		strcpy(wlArr, WPTline.data());
		for (char *label = strtok(wlArr, " "); label; label = strtok(0, " "))
			point.label.push_back(label);		// get all tokens & put into label deque
		if (!point.label.empty())
		{	point.URL = point.label.back();		// last token is actually the URL...
			point.label.pop_back();			// ...and not a label.
			if (point.label.empty()) point.label.push_back("NULL");
			point.InitCoords();
			hwy->pt.push_back(point);		// add completed line to waypoint list
		}
	} //end while (step thru each WPT line)
	return hwy;
}

bool ChoppedRtesCSV(std::vector<highway*> &HwyVec, std::string input, std::string path, bool RepoDirs)
{	std::ifstream CSV(input.data());
	if (!CSV)
	{	std::cout << "InputFile \"" << input << "\" not found!" << '\n';
		return 0;
	}
	CSV.seekg(0, std::ios::end); unsigned int EoF = CSV.tellg(); CSV.seekg(0);
	while (CSV.get() != '\n' && CSV.tellg() < EoF); //skip header row

	while (CSV.tellg() < EoF) // build hwy list
	{	std::string System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames;
		std::string CSVline; // read individual line
		for (char charlie = 0; charlie != '\n' && CSV.tellg() < EoF; CSVline.push_back(charlie)) CSV.get(charlie);
		while (CSVline.back() == 0x0A || CSVline.back() == 0x0D)	// either DOS or UNIX...
			CSVline.erase(CSVline.end()-1);				// strip out terminal '\n'
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
		else {	std::string wptFile = path;
			if (RepoDirs) wptFile += Region+"/"+System+"/";
			wptFile += Root+".wpt";
			highway *hwy = BuildRte(wptFile, System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames);
			if (hwy) HwyVec.push_back(hwy);
		     }
	} // end while (build hwy list)
	return 1;
}
