using namespace std;
#include <cmath>
#include "../lib/highway.cpp"	// includes cstring, fstream, iostream, list, string, vector

inline void ColorCodes(string Color, char *UnColor, char *ClColor)
//TODO input from INI file
{	if (Color == "blue")		{ strcpy (UnColor, "6464FF");	strcpy (ClColor, "0000DC");	return; }
	if (Color == "teal")		{ strcpy (UnColor, "64C8C8");	strcpy (ClColor, "008CA0");	return; }
	if (Color == "green")		{ strcpy (UnColor, "64C864");	strcpy (ClColor, "00E000");	return; }
	if (Color == "red")		{ strcpy (UnColor, "FF6464");	strcpy (ClColor, "E00000");	return; }
	if (Color == "magenta")		{ strcpy (UnColor, "FF64FF");	strcpy (ClColor, "D000D0");	return; }
	if (Color == "lightsalmon")	{ strcpy (UnColor, "E0A2A2");	strcpy (ClColor, "F09673");	return; }
	if (Color == "brown")		{ strcpy (UnColor, "999866");	strcpy (ClColor, "996600");	return; }
	if (Color == "yellow")		{ strcpy (UnColor, "FFD864");	strcpy (ClColor, "E8B000");	return; }
	/* default/unrecognized */	  strcpy (UnColor, "aaaaaa");	strcpy (ClColor, "555555");
	cout << "Warning: unrecognized Color code " << Color << " will be colored gray.\n";
}

void GetColors2(string SysCode, char *SysCSV, char *UnColor, char *ClColor)
// Yes, this function is a bit overbuilt. I may do something with it some day.
{	//border colors
	if (SysCode == "B_COUNTRY" || SysCode == "b_country")
					{ strcpy (UnColor, "3c3c3c");	strcpy (ClColor, "3c3c3c");	return; }
	if (SysCode == "B_SUBDIV" || SysCode == "b_subdiv")
					{ strcpy (UnColor, "a0a0a0");	strcpy (ClColor, "a0a0a0");	return; }
	if (SysCode == "B_WATER" || SysCode == "b_water")
					{ strcpy (UnColor, "0000a0");	strcpy (ClColor, "0000a0");	return; }

	ifstream CSV(SysCSV); //TODO input from INI file
	if (!CSV)
	{	cout << SysCSV << " file not found!" << endl;
		strcpy (UnColor, "aaaaaa");	strcpy (ClColor, "555555");
		return;
	}
	CSV.seekg(0, ios::end); unsigned int EoF = CSV.tellg(); CSV.seekg(0);
	while (CSV.get() != '\n' && CSV.tellg() < EoF); //skip header row

	while (CSV.tellg() < EoF)
	{	string System, CountryCode, Name, Color, Tier, Level;
		string CSVline; // read individual line
		for (char charlie = 0; charlie != '\n' && CSV.tellg() < EoF; CSVline.push_back(charlie)) CSV.get(charlie);
		while (CSVline.back() == 0x0A || CSVline.back() == 0x0D)	// either DOS or UNIX...
			CSVline.erase(CSVline.end()-1);				// strip out terminal '\n'
		// parse CSV line
		unsigned int i = 0;
		while (CSVline[i] != ';') { System.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { CountryCode.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { Name.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { Color.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { Tier.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';' && i < CSVline.size()) { Level.push_back(CSVline[i]); i++; } i++;

		if (System == SysCode)
		{	ColorCodes(Color, UnColor, ClColor);
			return;
		}
	}

	// default/unrecognized
	strcpy (UnColor, "aaaaaa");	strcpy (ClColor, "555555");
	cout << "Warning: unrecognized System code " << SysCode << " will be colored gray.\n";
}

void readlist(ofstream *html, highway *hwy)
{	string Region, Name, pl1, pl2;
	unsigned int pi1, pi2;
	ifstream list("list.list");
	bool comma = 0;

	while (list >> Region >> Name >> pl1 >> pl2) //FIXME: assumes perfectly formatted .list file with exactly four strings on every line
		if (Region == hwy->Region && hwy->NameMatch(Name))
		{	while (pl1[0] == '+' || pl1[0] == '*') pl1 = &pl1[1];
			while (pl2[0] == '+' || pl2[0] == '*') pl2 = &pl2[1];
			pi1 = hwy->GetIndByLabel(pl1);
			pi2 = hwy->GetIndByLabel(pl2);
			if (pi1 < hwy->pt.size() && pi2 < hwy->pt.size())
			{	if (comma)
					if (pi1 < pi2)	*html << ", " << pi1 << ", " << pi2;
					else		*html << ", " << pi2 << ", " << pi1;
				else	// write the first one
					if (pi1 < pi2)	*html << pi1 << ", " << pi2;
					else		*html << pi2 << ", " << pi1;
				comma = 1;
			}
		}
	list.close();
}

void HTML(vector<highway*> hwy, char *SysCSV)
{	char UnColor[6], ClColor[6];
	char filename[] = "map.htm";
	ofstream html(filename);

	html << "<!doctype html>\n";
	html << "<html>\n";
	html << "<head>\n";
	html << "	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	html << "	<title>Clinched Highway Map</title>\n";
	html << "</head>\n\n";

	html << "<body>\n";
	html << "<canvas width=700 height=700>\n\n";

	html << "(To see Map, upgrade browser.)\n\n";

	html << "</canvas>\n\n";

	html << "<script> // JavaScript starts here\n";
	html << "//EDB - route objects\n";
	html << "var rte = [];\n\n";

	// route objects
	for (unsigned int num = 0; num < hwy.size(); num++)
	{	// not all of this may be necessary but leaving it in nonetheless.
		html << "rte[" << num << "] = {\n";
		html << "System:\"" << hwy[num]->System << "\", ";
		html << "Region:\"" << hwy[num]->Region << "\", ";
		html << "Route:\"" << hwy[num]->Route << "\", ";
		html << "Banner:\"" << hwy[num]->Banner << "\", ";
		html << "Abbrev:\"" << hwy[num]->Abbrev << "\", ";
		html << "City:\"" << hwy[num]->City << "\",\n";

		GetColors2(hwy[num]->System, SysCSV, UnColor, ClColor);
		html << "UnColor:'#" << UnColor << "', ClColor:'#" << ClColor << "',\n";

		html << "//EDB - point latitudes\n";
		html << "lat:[";
		list<waypoint>::iterator point = hwy[num]->pt.begin();
			html << to_string(point->OrigLat);
		for (point++; point != hwy[num]->pt.end(); point++)
			html << ", " << to_string(point->OrigLat);
		html << "],\n";

		html << "//EDB - point longitudes\n";
		html << "lon:[";
		point = hwy[num]->pt.begin();
			html << to_string(point->OrigLon);
		for (point++; point != hwy[num]->pt.end(); point++)
			html << ", " << to_string(point->OrigLon);
		html << "],\n";

		html << "//vdeane & EDB - indices to .listfile endpoints\ncliSegments:[";
		readlist(&html, hwy[num]);
		html << "]\n} //end object definition\n\n";
	} //end for (route objects)

	html << "var MinLat = rte[rte.length-1].lat[0];\n";
	html << "var MinLon = rte[rte.length-1].lon[0];\n";
	html << "var MaxLat = rte[rte.length-1].lat[0];\n";
	html << "var MaxLon = rte[rte.length-1].lon[0];\n";
	html << "var i, j, k;\n\n";

	html << "function merc(lat)\n";
	html << "{	return Math.log(Math.tan(0.785398163+lat*3.1415926535898/360))*180/3.1415926535898;\n";
	html << "}\n\n";

	html << "//John Pound - initialize canvas\n";
	html << "var canvas = document.getElementsByTagName(\"canvas\")[0];\n\n";

	html << "//EDB - get maximum and minimum latitude and longitude for a quick-n-dirty scale of the route trace to fill the canvas\n";
	html << "for (j = 0; j < rte.length; j++)\n";
	html << "{	for (i = 0; i < rte[j].lat.length; i++)\n";
	html << "	{	if (rte[j].lat[i] < MinLat && rte[j].Region != \"B\" && rte[j].Region != \"b\") MinLat = rte[j].lat[i];\n";
	html << "		if (rte[j].lon[i] < MinLon && rte[j].Region != \"B\" && rte[j].Region != \"b\") MinLon = rte[j].lon[i];\n";
	html << "		if (rte[j].lat[i] > MaxLat && rte[j].Region != \"B\" && rte[j].Region != \"b\") MaxLat = rte[j].lat[i];\n";
	html << "		if (rte[j].lon[i] > MaxLon && rte[j].Region != \"B\" && rte[j].Region != \"b\") MaxLon = rte[j].lon[i];\n";
	html << "	}\n";
	html << "}\n";
	html << "var ScaleFac = Math.min((canvas.width-1)/(MaxLon-MinLon), (canvas.height-1)/(merc(MaxLat)-merc(MinLat)));\n";
	html << "var MinMerc = merc(MinLat);\n\n";

	html << "//vdeane & EDB - base route line traces\n";
	html << "for (k = 0; k < rte.length; k++)\n";
	html << "{	c = canvas.getContext(\"2d\");\n";
	html << "	c.save();\n";
	html << "	c.beginPath();\n";
	html << "	c.strokeStyle = rte[k].UnColor;\n";
	html << "	c.moveTo((rte[k].lon[0]-MinLon)*ScaleFac, canvas.height-1-(merc(rte[k].lat[0])-MinMerc)*ScaleFac);\n";
	html << "	for (i = 1; i < rte[k].lat.length; i++) c.lineTo((rte[k].lon[i]-MinLon)*ScaleFac, canvas.height-1-(merc(rte[k].lat[i])-MinMerc)*ScaleFac);\n";
	html << "	c.stroke();\n";
	html << "	c.restore();\n";
	html << "}\n\n";

	html << "//vdeane & EDB - begin drawing segments\n";
	html << "for (k = 0; k < rte.length; k++)\n";
	html << "{	for (i = 0; i < rte[k].cliSegments.length; i+=2)\n";
	html << "	{	var CliPt = false; //vdeane - track if start or end of segment\n";
	html << "		c = canvas.getContext(\"2d\");\n";
	html << "		c.save();\n";
	html << "		c.beginPath();\n";
	html << "		c.strokeStyle = rte[k].ClColor;\n\n";

	html << "		for (j = 0; j < rte[k].lat.length; j++)\n";
	html << "		{	if (rte[k].cliSegments[i] === j)\n";
	html << "			{	c.moveTo((rte[k].lon[j]-MinLon)*ScaleFac, canvas.height-1-(merc(rte[k].lat[j])-MinMerc)*ScaleFac);\n";
	html << "				CliPt = true;\n";
	html << "			}\n";
	html << "			if (CliPt === true && j !== 0)\n";
	html << "				c.lineTo((rte[k].lon[j]-MinLon)*ScaleFac, canvas.height-1-(merc(rte[k].lat[j])-MinMerc)*ScaleFac);\n";
	html << "			if (rte[k].cliSegments[i+1] === j)\n";
	html << "			{	c.stroke();\n";
	html << "				c.restore();\n";
	html << "				CliPt = false;\n";
	html << "			} //end if (Does label end cliSegment?)\n";
	html << "		} //end for (step thru point coords in route)\n";
	html << "	} // end for (step thru clinched segments)\n";
	html << "} //end for (step thru routes)//*/\n\n";

	html << "// JavaScript ends here\n";
	html << "</script>\n";
	html << "</body>\n";
	html << "</html>\n";
}

bool CSVmode(string InputFile, string SourceDir, char* SysCSV)
{	vector<highway*> HwyList;

	ifstream CSV(InputFile.data());
	if (!CSV)
	{	cout << InputFile << " file not found!" << endl;
		return 0;
	}
	CSV.seekg(0, ios::end); unsigned int EoF = CSV.tellg(); CSV.seekg(0);
	while (CSV.get() != '\n' && CSV.tellg() < EoF); //skip header row

	while (CSV.tellg() < EoF) // build hwy list
	{	string System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames;
		string CSVline; // read individual line
		for (char charlie = 0; charlie != '\n' && CSV.tellg() < EoF; CSVline.push_back(charlie)) CSV.get(charlie);
		while (CSVline.back() == 0x0A || CSVline.back() == 0x0D)	// either DOS or UNIX...
			CSVline.erase(CSVline.end()-1);				// strip out terminal '\n'
		// parse CSV line
		unsigned int i = 0;
		while (CSVline[i] != ';') { System.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { Region.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { Route.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { Banner.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { Abbrev.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';') { City.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';' && i < CSVline.size()) { Root.push_back(CSVline[i]); i++; } i++;
		while (CSVline[i] != ';' && i < CSVline.size()) { AltRouteNames.push_back(CSVline[i]); i++; } i++;

		string wptFile = SourceDir+System+"/"+Root+".wpt";
		highway *hwy = BuildRte(wptFile.data(), System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames);
		if (hwy) HwyList.push_back(hwy);
	} // end while (build hwy list)

	HTML(HwyList, SysCSV);
}

int main(int argc, char *argv[])
{	string InputFile = "routes.csv";
	string SourceDir = "data/";

	if (argc != 2)
	{	cout << "Usage: ./canvas SystemsCSVFile\n";
		cout << "For example,\n";
		cout << "./canvas /home/yakra/TravelMapping/HighwayData-master/systems.csv\n";
		return 0;
	}

	CSVmode(InputFile, SourceDir, argv[1]);
}
