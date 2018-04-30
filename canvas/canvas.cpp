using namespace std;
#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>
#include <string>

class waypoint
{	public:
	unsigned int NumLabels;
	char **label;
	double OrigLat, OrigLon, OffLat, OffLon, OnLat, OnLon, OffDist, OnDist;
	waypoint *prev, *next;
	
	waypoint(waypoint *ref)
	{	prev = ref;
		NumLabels = 0;
		label = 0;
		next = 0;
	}

	~waypoint()
	{	if (prev) prev->next = next;
		if (next) next->prev = prev;
		for (unsigned int l = 0; l < NumLabels; l++)
			delete[] label[l];
		if (NumLabels) delete[] label;
	}

	void InitLabels()
	{	if (!NumLabels) return;
		label = new char*[NumLabels];
		for (unsigned int l = 0; l < NumLabels; l++)
			label[l] = new char[256];
	}
};

class highway
{	public:
	char System[256], Region[256], Route[256], Banner[256], Abbrev[256], City[256], Root[256];
	unsigned int NumAltRtes; char **AltRouteNames;
	char Name[768], KeyValue[256];
	bool KeyFound;
	unsigned int NumPts;
	waypoint *pt1;
	highway *next;

	highway
	(	char SysIn[256],
		char RegIn[256],
		char RteIn[256],
		char BanIn[256],
		char AbbIn[256],
		char CityIn[256],
		char RootIn[256],
		unsigned int NAR,
		char **ARN,
		char KeyIn[256]
	)

	{	NumPts = 0;
		strcpy(System, SysIn);
		strcpy(Region, RegIn);
		strcpy(Route, RteIn);
		strcpy(Banner, BanIn);
		strcpy(Abbrev, AbbIn);
		strcpy(City, CityIn);
		strcpy(Root, RootIn);
		strcpy(KeyValue, KeyIn);
		NumAltRtes = NAR;
		AltRouteNames = ARN;
		KeyFound = 0;
		next = 0;
		strcpy(Name, Route); strcat(Name, Banner); strcat(Name, Abbrev);
	}

	~highway()
	{	if (pt1)
		{	while (pt1->next) delete pt1->next;
			delete pt1;
		}
	}

	unsigned int LblInd(char *lbl) // returning a value >= NumLabels = failure.
	{	unsigned int p = 0;
		for (waypoint *point = pt1; p < NumPts; p++)
		{	for (unsigned int l = 0; l < point->NumLabels; l++)
				if (!strcmp(point->label[l], lbl))
				{	if (l) cout << Region << ' ' << Name << ' ' << point->label[l] << ": deprecated in favor of " << point->label[0] << endl;
					return p;
				}
			point = point->next;
		}
		cout << Region << ' ' << Name << ' ' << lbl << ": point label not recognized\n";
		return p;
	}

	void CSVline()
	{	cout << System << ';' << Region << ';' << Route << ';' << Banner << ';' << Abbrev << ';' << City << ';' << Root << ';';
		if (NumAltRtes)
		{	cout << AltRouteNames[0];
			for (unsigned short i = 1; i < NumAltRtes; i++) cout << ',' << AltRouteNames[i];
		}
		cout << endl;
	}

	bool NameMatch(const char *string)
	{	if (!strcmp(string, Name)) return 1;
		for (unsigned short i = 0; i < NumAltRtes; i++)
			if (!strcmp(string, AltRouteNames[i])) return 1;
		return 0;
	}
}; //end highway class

// replace this idiotic function with calls to strstr, or something
bool parseURL(char *URL, double *wptLat, double *wptLon)
{	int index = 0;
	int sign = 0;
	char tempURL[256];
	strcpy(tempURL, URL);
	*wptLat=0;
	*wptLon=0;

	//search for lat= parameter
	for (index=0; index < strlen(URL)-4; index++)
	{	tempURL[index+4] = 0;
		if (!strcmp(&tempURL[index], "lat=")) break;
		else strcpy(tempURL, URL);
	}
	strcpy(tempURL, URL);
	if (index >= strlen(URL)-4)
	{	cout << "lat= parameter not found in URL!" << endl;
		cout << "Skipping this point." << endl;
		return 0;
	}
	index+=4; //seek past lat= parameter
	//parse latitude
	if (URL[index] == '-')
	{	sign = -1;
		index++;
	}
	else sign = 1;
	while (URL[index] == '0' && index < strlen(URL)) index++;
	if (URL[index] >= '1' && URL[index] <= '9')
	{	*wptLat = (URL[index]-0x30);
		index++;
	}
	while (URL[index] >= '0' && URL[index] <= '9' && index < strlen(URL))
	{	*wptLat *= 10;
		*wptLat += (URL[index]-0x30);
		index++;
	}
	if (URL[index] == '.')
	{	index++;
		double DecDig=0;
		while (URL[index] >= '0' && URL[index] <= '9' && index < strlen(URL))
		{	DecDig--;
			*wptLat += ((URL[index]-0x30)*pow(10,DecDig));
			index++;
		}
	}
	*wptLat *= sign;

	//search for lon= parameter
	for (index=0; index < strlen(URL)-4; index++)
	{	tempURL[index+4] = 0;
		if (!strcmp(&tempURL[index], "lon=")) break;
		else strcpy(tempURL, URL);
	}
	strcpy(tempURL, URL);
	if (index >= strlen(URL)-4)
	{	cout << "lon= parameter not found in URL!" << endl;
		cout << "Skipping this point." << endl;
		return 0;
	}
	index+=4; //seek past lon= parameter
	//parse longitude
	if (URL[index] == '-')
	{	sign = -1;
		index++;
	}
	else sign = 1;
	while (URL[index] == '0' && index < strlen(URL)) index++;
	if (URL[index] >= '1' && URL[index] <= '9')
	{	*wptLon = (URL[index]-0x30);
		index++;
	}
	while (URL[index] >= '0' && URL[index] <= '9' && index < strlen(URL))
	{	*wptLon *= 10;
		*wptLon += (URL[index]-0x30);
		index++;
	}
	if (URL[index] == '.')
	{	index++;
		double DecDig=0;
		while (URL[index] >= '0' && URL[index] <= '9' && index < strlen(URL))
		{	DecDig--;
			*wptLon += ((URL[index]-0x30)*pow(10,DecDig));
			index++;
		}
	}
	*wptLon *= sign;

	return 1;
} //end parseURL

highway* BuildRte
(	char *filename,
	char System[256],
	char Region[256],
	char Route[256],
	char Banner[256],
	char Abbrev[256],
	char City[256],
	char Root[256],
	unsigned int NumAltRtes,
	char **AltRouteNames,
	char KeyValue[256]
)

{	// The WPT actually exists, right?
	ifstream WPT1 (filename, ifstream::in);
	if (!WPT1.is_open())
	{	cout << filename << " file not found!\nSkipping this file." << endl;
		return 0;
	}
	WPT1.seekg(0, ios::end);
	if (!WPT1.tellg())
	{	cout << filename << " filesize = 0!\nSkipping this file." << endl;
		return 0;
	}
	WPT1.seekg(0);
	ifstream WPT2 (filename, ifstream::in);

	char str[4096];
	int bookmark = 0;
	highway *hwy = new highway(System, Region, Route, Banner, Abbrev, City, Root, NumAltRtes, AltRouteNames, KeyValue);
	hwy->pt1 = new waypoint(0);
	waypoint *point = hwy->pt1;

	while (!WPT1.eof())
	{	// read until URL found
		while (WPT1 >> str && !strstr(str, "http://")) point->NumLabels += 1;
		//cout << point->NumLabels << ' ';
		point->InitLabels(); // located *here* to make ~waypoint() work.
		//cout << WPT1.tellg() << ", ";

		// exception for extra junk "labels" at end sans URL
		if (WPT1.tellg() == -1) // reaches EOF without finding another URL
			//FIXME some compilers will not like this conditional. Cause undefined behavior?
		{	point = point->prev; //cout << "point = point->prev OK\n";
			delete point->next;
			point->next = 0;
			//cout << "<BALEETED!> (break)\n";
			break;
		}
		hwy->NumPts += 1;

		// read in and store labels
		WPT2.seekg(bookmark);
		//cout << "seek " << bookmark << ", ";
		for (unsigned int l = 0; l < point->NumLabels; l++)
		{	WPT2 >> point->label[l]; // read in & store labels in their slots
			// strip leading '+' from hidden & deprecated labels; leading '*' from closed point labels
			while (point->label[l][0] == '+' || point->label[l][0] == '*')
				strcpy (point->label[l], &point->label[l][1]);
		}
		WPT2 >> str; // read in the URL
		bookmark = WPT1.tellg(); // bkmk the WPT file after the URL
		//cout << "bkmk " << bookmark << ", \n";
		// Mon 06 May 2013 01:32:31 PM EDT : Using WPT2 in addition to WPT1 FINALLY got around the missing CRLF bug!  w0ot w0ot

		if (!parseURL(str, &point->OrigLat, &point->OrigLon))
			if (point->prev) // if any point but the first failed
			{	point = point->prev;
				delete point->next;
				if (!WPT1.eof())
				{	point->next = new waypoint(point);
					point = point->next;
				}
				else point->next = 0;
			} // if the first point failed:
			else { delete point; hwy->pt1 = new waypoint(0); point = hwy->pt1; }
		else if (!WPT1.eof())
		{	point->next = new waypoint(point);
			point = point->next;
		}
	} //cout << "while (!WPT1.eof()) finished\n";
	//cout << "\n\nbookmark = " << bookmark << " | tellg() = " << WPT1.tellg() << "\n\n";
	WPT1.close(); WPT2.close(); //cout << "input files closed\n";

	//cout << "BuildRte: " << filename << " | " << Root << " OK\n";
	return hwy;
} //end BuildRte

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
		if (CSVline[CSVline.size()-1] == '\n') CSVline[CSVline.size()-1] = 0; //account for missing terminal '\n'
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
{	char Region[256], Name[256], pl1[256], pl2[256];
	unsigned int pi1, pi2;
	ifstream list("list.list");
	bool comma = 0;

	while (list >> Region >> Name >> pl1 >> pl2) //FIXME: assumes perfectly formatted .list file with exactly four strings on every line
		if (!strcmp(Region, hwy->Region) && hwy->NameMatch(Name))
		{	while (pl1[0] == '+' || pl1[0] == '*') strcpy (pl1, &pl1[1]);
			while (pl2[0] == '+' || pl2[0] == '*') strcpy (pl2, &pl2[1]);
			pi1 = hwy->LblInd(pl1);
			pi2 = hwy->LblInd(pl2);
			if (pi1 < hwy->NumPts && pi2 < hwy->NumPts)
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

void HTML(highway *hwy, char *SysCSV)
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
	for (unsigned int HwyNum = 0; hwy; hwy = hwy->next)
	{	// not all of this may be necessary but leaving it in nonetheless.
		html << "rte[" << HwyNum << "] = {\n";
		html << "System:\"" << hwy->System << "\", ";
		html << "Region:\"" << hwy->Region << "\", ";
		html << "Route:\"" << hwy->Route << "\", ";
		html << "Banner:\"" << hwy->Banner << "\", ";
		html << "Abbrev:\"" << hwy->Abbrev << "\", ";
		html << "City:\"" << hwy->City << "\",\n";

		GetColors2(hwy->System, SysCSV, UnColor, ClColor);
		html << "UnColor:'#" << UnColor << "', ClColor:'#" << ClColor << "',\n";

		html << "//EDB - point latitudes\n";
		html << "lat:[";
		for (waypoint *point = hwy->pt1; point; point = point->next)
		{	html << to_string(point->OrigLat);
			if (point->next) html << ", ";
		}
		html << "],\n";

		html << "//EDB - point longitudes\n";
		html << "lon:[";
		for (waypoint *point = hwy->pt1; point; point = point->next)
		{	html << to_string(point->OrigLon);
			if (point->next) html << ", ";
		}
		html << "],\n";

		/*html << "//vdeane - labels from highway browser\n";
		html << "labels:[";
		for (waypoint *point = hwy->pt1; point; point = point->next)
		{	html << '"' << point->label[0] << '"';
			if (point->next) html << ", ";
		}
		html << "],\n";//*/

		html << "//vdeane & EDB - indices to .listfile endpoints\ncliSegments:[";
		readlist(&html, hwy);
		html << "]\n} //end object definition\n\n";

		HwyNum++;
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

bool CSVmode(char** CharVars, unsigned int CharSkip, char *SysCSV)
{	char System[256], Region[256], Route[256], Banner[256], Abbrev[256], City[256], Root[256], KeyValue[256];
	char *InputFile = CharVars[3];
	char *SourceDir = CharVars[6];
	char CSVname[1024];
	strcpy(CSVname, InputFile);
	highway *FirstRte = 0;
	highway *hwy = 0;
	unsigned int i, bookmark, NumAltRtes;
	char **AltRouteNames;

	ifstream CSV(CSVname, ios::in);
	if (!CSV.is_open())
	{	cout << CSVname << " file not found!" << endl;
		return 0;
	}

	CSV.seekg(0);
	while ((CSV.get() != '\n') && !CSV.eof()); //skip header row
	while (!CSV.eof())
	{	System[0] = CSV.get(); for (i = 0; i < 256 && System[i] != ';' && !CSV.eof(); System[i] = CSV.get()) i++; System[i] = 0;
		Region[0] = CSV.get(); for (i = 0; i < 256 && Region[i] != ';' && !CSV.eof(); Region[i] = CSV.get()) i++; Region[i] = 0;
		Route[0] = CSV.get(); for (i = 0; i < 256 && Route[i] != ';' && !CSV.eof(); Route[i] = CSV.get()) i++; Route[i] = 0; strcpy(KeyValue, &Route[CharSkip]);
		Banner[0] = CSV.get(); for (i = 0; i < 256 && Banner[i] != ';' && !CSV.eof(); Banner[i] = CSV.get()) i++; Banner[i] = 0;
		Abbrev[0] = CSV.get(); for (i = 0; i < 256 && Abbrev[i] != ';' && !CSV.eof(); Abbrev[i] = CSV.get()) i++; Abbrev[i] = 0;
		City[0] = CSV.get(); for (i = 0; i < 256 && City[i] != ';' && !CSV.eof(); City[i] = CSV.get()) i++; City[i] = 0;
		Root[0] = CSV.get(); for (i = 0; i < 256 && Root[i] != ';' && Root[i] != '\n' && !CSV.eof(); Root[i] = CSV.get()) i++;
		char charlie = Root[i]; Root[i] = 0; NumAltRtes = 0;
		if (charlie == ';') // AltRouteNames column exists
		{	bookmark = CSV.tellg();
			CSV.get(charlie);
			if (charlie != '\n') NumAltRtes = 1;
			while ((charlie != '\n') && !CSV.eof()) //FIXME tellg() filesize yadda yadda
			{	if (charlie == ',') NumAltRtes++;
				CSV.get(charlie);
			}
			AltRouteNames = new char*[NumAltRtes];
			CSV.seekg(bookmark);
			if (NumAltRtes)
				for (unsigned int a = 0; a < NumAltRtes; a++)
				{	AltRouteNames[a] = new char[256];
					AltRouteNames[a][0] = CSV.get();
					for (i = 0; i < 256 && AltRouteNames[a][i] != ',' && AltRouteNames[a][i] != '\n' && !CSV.eof(); AltRouteNames[a][i] = CSV.get())
						i++;
					AltRouteNames[a][i] = 0;
				}
			else CSV.get();
		}
		//construct input filename
		strcpy(InputFile, SourceDir);
		strcat(InputFile, System);
		strcat(InputFile, "/");
		strcat(InputFile, Root);
		strcat(InputFile, ".wpt");

		if (!CSV.eof())
			if (!FirstRte)
			{	FirstRte = BuildRte(InputFile, System, Region, Route, Banner, Abbrev, City, Root, NumAltRtes, AltRouteNames, KeyValue);
				hwy = FirstRte;
			} else
			{	hwy->next = BuildRte(InputFile, System, Region, Route, Banner, Abbrev, City, Root, NumAltRtes, AltRouteNames, KeyValue);
				if (hwy->next) hwy = hwy->next;
			}
	} //end while (step thru each line of CSV*/
	cout << endl;
	if (FirstRte)
	{	HTML(FirstRte, SysCSV);
		/*for (hwy = FirstRte; hwy; hwy = hwy->next)
			hwy->CSVline();//*/
	}
} //end CSVmode

int main(int argc, char *argv[])
{	char InputFile[1024] = "routes.csv";
	char SourceDir[1024] = "data/";
	char KeyValue[2] = "\0";
	char* CharVars[7] = {0,0,0, InputFile, 0, KeyValue, SourceDir};

	if (argc != 2)
	{	cout << "Usage: ./canvas SystemsCSVFile\n";
		cout << "For example,\n";
		cout << "./canvas /home/yakra/TravelMapping/HighwayData-master/systems.csv\n";
		return 0;
	}

	CSVmode(CharVars, 0, argv[1]);
}
