#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>
#include "waypoint.cpp"	// includes string
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

	void showCSVline()
	{	std::cout << System << ';' << Region << ';' << Route << ';' << Banner << ';' << Abbrev << ';' << City << ';' << Root << ';';
		for (unsigned int i = 0; i+1 < AltRouteNames.size(); i++) std::cout << AltRouteNames[i] << ',';
		if (AltRouteNames.size()) std::cout << AltRouteNames.back();
		std::cout << '\n';
	}

	void ListPts(bool AllLabels)
	{	std::cout.precision(9);
		for (std::list<waypoint>::iterator ptI = pt.begin(); ptI != pt.end(); ptI++)
		{	if (AllLabels)
				for (unsigned int i = 0; i < ptI->label.size(); i++) std::cout << ptI->label[i] << '\t'; // print labels
			else {	std::cout << ptI->label.front() << '\t';
				if (ptI->label.front().size() < 8) std::cout << '\t';
			     }
			// print coords
			std::cout << ptI->OrigLat << '\t' << ptI->OrigLon << '\t';
			std::cout << ptI->OffLat << '\t' << ptI->OffLon << '\t';
			std::cout << ptI->OnLat << '\t' << ptI->OnLon << '\n';
		}
	}//*/

	void write(unsigned char WriteMe, bool B)
	{	std::string OrigName = "output/orig/" + Root + ".wpt";
		std::string OffName = "output/off/" + Root + ".wpt";
		std::string OnName = "output/on/" + Root + ".wpt";
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
			if (WriteMe & 1) OrigFile << "http://www.openstreetmap.org/?lat=" << DblText(ptI->OrigLat,6,B) << "&lon=" << DblText(ptI->OrigLon,6,B) << '\n';
			if (WriteMe & 2) OffFile << "http://www.openstreetmap.org/?lat=" << DblText(ptI->OffLat,6,B) << "&lon=" << DblText(ptI->OffLon,6,B) << '\n';
			if (WriteMe & 4) OnFile << "http://www.openstreetmap.org/?lat=" << DblText(ptI->OnLat,6,B) << "&lon=" << DblText(ptI->OnLon,6,B) << '\n';
		}
		if (WriteMe & 1) OrigFile.close();
		if (WriteMe & 2) OffFile.close();
		if (WriteMe & 4) OnFile.close();
	}
};

highway* BuildRte(const char *filename, std::string Sys, std::string Reg, std::string Rte, std::string Ban, std::string Abb, std::string City, std::string Root, std::string Alts)
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
			point.label.push_back(label);	// get all tokens & put into label array
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
