#include <cmath>
#include "../lib/highway.cpp"	// includes cstring, fstream, iostream, list, string, vector
using namespace std;

class envV
{	public:
	string Input, List, Colors, Output, Repo, Width, Height, UnStroke, ClStroke;
	vector<string> N_Colors, UnColors, ClColors;
	bool MsgSeen;

	bool set(int argc, char *argv[])
	{	MsgSeen = 0;
		unsigned short envInfo = 0;

		// INI file options:
		string iniField;
		ifstream INI("canvas.ini");
		if (!INI) cout << "canvas.ini file not found. All required options must be specified by commandline\n";
		else {	INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Output" && !INI.eof())	INI >> iniField;
			  INI >> Output;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Repo" && !INI.eof())	INI >> iniField;
			  INI >> Repo;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Input" && !INI.eof())	INI >> iniField;
			  INI >> Input;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "List" && !INI.eof())	INI >> iniField;
			  INI >> List;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Colors" && !INI.eof())	INI >> iniField;
			  INI >> Colors;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Width" && !INI.eof())	INI >> iniField;
			  INI >> Width;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Height" && !INI.eof())	INI >> iniField;
			  INI >> Height;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Stroke" && !INI.eof())	INI >> iniField;
			  INI >> UnStroke >> ClStroke;
		     }

		// commandline options:
		for (int a = 1; a < argc; a++)
		{	if (!strcmp(argv[a], "-C") || !strcmp(argv[a], "--Colors"))
			{ if (a+1 < argc)
			  {	Colors = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-c") || !strcmp(argv[a], "--Color"))
			{ if (a+3 < argc)
			{	N_Colors.push_back(argv[a+1]);
				UnColors.push_back(argv[a+2]);
				ClColors.push_back(argv[a+3]);
				a += 3;
			} }
			else if (!strcmp(argv[a], "-h") || !strcmp(argv[a], "--Height"))
			{ if (a+1 < argc)
			  {	Height = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-i") || !strcmp(argv[a], "--Input"))
			{ if (a+1 < argc)
			  {	Input = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-l") || !strcmp(argv[a], "--List"))
			{ if (a+1 < argc)
			  {	List = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-o") || !strcmp(argv[a], "--Output"))
			{ if (a+1 < argc)
			  {	Output = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-r") || !strcmp(argv[a], "--Repo"))
			{ if (a+1 < argc)
			  {	Repo = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-s") || !strcmp(argv[a], "--Stroke"))
			{ if (a+2 < argc)
			  {	UnStroke = argv[a+1];
				ClStroke = argv[a+2];
				a += 2;
			} }
			else if (!strcmp(argv[a], "-w") || !strcmp(argv[a], "--Width"))
			{ if (a+1 < argc)
			  {	Width = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-?") || !strcmp(argv[a], "--help") || !strcmp(argv[a], "--Help"))
			{	cout << "Options:\n";
				cout << "Mandatory arguments to long options are mandatory for short options too.\n";
				cout << "  -C, --Colors <ColorFile>         Color definitions file.\n";
				cout << "  -c, --Color <Name> <Base> <Clin> Color definitions file.\n";
				cout << "  -h, --Height <Height>            Canvas height.\n";
				cout << "  -i, --Input <InputFile>          CSV file listing routes to be plotted.\n";
				cout << "  -l, --List <ListFile>            Filename of .list file to process.\n";
				cout << "  -o, --Output <OutputFile>        Filename of output HTML file.\n";
				cout << "  -r, --Repo <Repo>                Path of HighwayData repository.\n";
				cout << "                                   Trailing slash required.\n";
				cout << "  -s, --Stroke <Base> <Clinched>   Stroke width of base & clinched segments.\n";
				cout << "                                   Both arguments are required.\n";
				cout << "  -w, --Width <Width>              Canvas width.\n";
				cout << "  -?, --help, --Help               Display this help and exit.\n\n";

				cout << "INI file:\n";
				cout << "Default options may also be specified in canvas.ini, and will be overridden by\n";
				cout << "commandline switches. Format is one option per line, followed by arguments as\n";
				cout << "listed above. Options are the same as the long options listed above, without the\n";
				cout << "leading dashes, and are case sensitive.\n";
				cout << "Only single color definitions (--Color) are unsupported in canvas.ini.\n";
				cout << "These should be defined in their own separate colors INI file instead..\n";
				return 0;
			}
		}

		if (UnStroke.empty())	{ envInfo = 1; cout << "Base stroke thickness unspecified; defaulting to 1.\n"; UnStroke = "1"; }
		if (ClStroke.empty())	{ envInfo = 1; cout << "Clinched stroke thickness unspecified; defaulting to 2.5.\n"; ClStroke = "2.5"; }
		if (Width.empty())	{ envInfo = 1; cout << "Width unspecified; defaulting to 700.\n"; Width = "700"; }
		if (Height.empty())	{ envInfo = 1; cout << "Height unspecified; defaulting to 700.\n"; Height = "700"; }
		if (Input.empty())	{ envInfo = 2; cout << "Input CSV unspecified.\n"; }
		if (List.empty())	{ envInfo = 2; cout << ".list file unspecified.\n"; }
		if (Repo.empty())	{ envInfo = 2; cout << "Repo location unspecified.\n"; }

		if (envInfo)
		{	cout << "For more info, use --help commandline option.\n";
			if (envInfo > 1) return 0;
		}

		// colors:
		ifstream colINI(Colors);
		if (!colINI) cout << Colors << " file not found. All colors not specified by commandline will be colored gray.\n";
		string N_Color, UnColor, ClColor;
		while (colINI >> N_Color >> UnColor >> ClColor)
		{	N_Colors.push_back(N_Color);
			UnColors.push_back(UnColor);
			ClColors.push_back(ClColor);
		}
		if (N_Colors.size() != UnColors.size())
		   { cout << "Oh dear. N_Colors.size() != UnColors.size() in envV::set(). Terminating.\n"; return 0; }
		if (UnColors.size() != ClColors.size())
		   { cout << "Oh dear. UnColors.size() != ClColors.size() in envV::set(). Terminating.\n"; return 0; }
		if (ClColors.size() != N_Colors.size())
		   { cout << "Oh dear. ClColors.size() != N_Colors.size() in envV::set(). Terminating.\n"; return 0; }
		return 1;
	}
};

void GetColors2(string &SysCode, envV &env, string &UnColor, string &ClColor)
// Yes, this function is a bit overbuilt. I may do something with it some day.
{	string SysCSV = env.Repo+"systems.csv";

	//border colors
	if (SysCode == "B_COUNTRY" || SysCode == "b_country")
		{ UnColor = "3c3c3c";	ClColor = "3c3c3c";	return; }
	if (SysCode == "B_SUBDIV" || SysCode == "b_subdiv")
		{ UnColor = "a0a0a0";	ClColor = "a0a0a0";	return; }
	if (SysCode == "B_WATER" || SysCode == "b_water")
		{ UnColor = "0000a0";	ClColor = "0000a0";	return; }

	ifstream CSV(SysCSV);
	if (!CSV)
	{	cout << SysCSV << " file not found!" << endl;
		UnColor = "aaaaaa";	ClColor = "555555";
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
		{	for (unsigned int i = 0; i < env.N_Colors.size(); i++)
			  if	(Color == env.N_Colors[i])
			  {	UnColor = env.UnColors[i];
				ClColor = env.ClColors[i];
				return;
			  }
			cout << "Warning: unrecognized Color code \"" << Color << "\" will be colored gray. (System = " << SysCode << ")\n";
			UnColor = "aaaaaa";	ClColor = "555555";
		}
	}
	cout << "Warning: unrecognized System code \"" << SysCode << "\" will be colored gray.\n";
	UnColor = "aaaaaa";	ClColor = "555555";
}

void readlist(envV &env, ofstream &html, highway *hwy)
{	string Region, Name, pl1, pl2;
	unsigned int pi1, pi2;
	ifstream list(env.List.data());
	if (!env.MsgSeen && !list) { cout << env.List << " not found. Plotting base route traces only.\n"; env.MsgSeen = 1; }
	bool comma = 0;

	while (list >> Region >> Name >> pl1 >> pl2) //FIXME: assumes perfectly formatted .list file with exactly four strings on every line
		if (Region == hwy->Region && hwy->NameMatch(Name))
		{	while (pl1[0] == '+' || pl1[0] == '*') pl1 = &pl1[1];
			while (pl2[0] == '+' || pl2[0] == '*') pl2 = &pl2[1];
			pi1 = hwy->GetIndByLabel(pl1);
			pi2 = hwy->GetIndByLabel(pl2);
			if (pi1 < hwy->pt.size() && pi2 < hwy->pt.size())
			{	if (comma)
					if (pi1 < pi2)	html << ", " << pi1 << ", " << pi2;
					else		html << ", " << pi2 << ", " << pi1;
				else	// write the first one
					if (pi1 < pi2)	html << pi1 << ", " << pi2;
					else		html << pi2 << ", " << pi1;
				comma = 1;
			}
		}
	list.close();
}

void HTML(vector<highway*> &hwy, envV &env)
{	string UnColor, ClColor;
	ofstream html(env.Output.data());

	html << "<!doctype html>\n";
	html << "<html>\n";
	html << "<head>\n";
	html << "	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	html << "	<title>Clinched Highway Map</title>\n";
	html << "</head>\n\n";

	html << "<body>\n";
	html << "<canvas width=" << env.Width << " height=" << env.Height << ">\n\n";

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

		GetColors2(hwy[num]->System, env, UnColor, ClColor);
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
		readlist(env, html, hwy[num]);
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
	html << "	{	if (!(rte[j].Region == \"B\" || rte[j].Region == \"b\" || rte[j].Region == \"_boundaries\"))\n";
	html << "		{	if (rte[j].lat[i] < MinLat) MinLat = rte[j].lat[i];\n";
	html << "			if (rte[j].lon[i] < MinLon) MinLon = rte[j].lon[i];\n";
	html << "			if (rte[j].lat[i] > MaxLat) MaxLat = rte[j].lat[i];\n";
	html << "			if (rte[j].lon[i] > MaxLon) MaxLon = rte[j].lon[i];\n";
	html << "		}\n";
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
	html << "	c.lineWidth=" << env.UnStroke << ";\n";
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
	html << "		c.strokeStyle = rte[k].ClColor;\n";
	html << "		c.lineWidth=" << env.ClStroke << ";\n\n";

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

bool CSVmode(envV &env)
{	vector<highway*> HwyList;

	ifstream CSV(env.Input.data());
	if (!CSV)
	{	cout << "InputFile \"" << env.Input << "\" not found!" << endl;
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

		string wptFile = env.Repo+"hwy_data/"+Region+"/"+System+"/"+Root+".wpt";
		highway *hwy = BuildRte(wptFile.data(), System, Region, Route, Banner, Abbrev, City, Root, AltRouteNames);
		if (hwy) HwyList.push_back(hwy);
	} // end while (build hwy list)

	HTML(HwyList, env);
}

int main(int argc, char *argv[])
{	envV env; if (!env.set(argc, argv)) return 0;
	CSVmode(env);
}
