using namespace std;
#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>

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

void DblWrite(char *ofname, double number, double DecDig)
{	ofstream outfile(ofname, ios::app);
	int sign = 1;
	if (number < 0) sign = -1;

	number *= pow(10,DecDig);
	if (abs(number-int(number)) >= 0.5) number +=sign;
	number /= pow(10,DecDig);

	outfile << int(number) << '.';
	if (number < 0) number *= -1;

	while (DecDig)
	{	number -= int(number);
		number *= 10;
		outfile << int(number);
		DecDig--;
	}
	outfile.close();
}

double measure(double lat1, double lon1, double lat2, double lon2)
{	lat1 *= (3.1415926535898/180); // deg -> rad
	lon1 *= (3.1415926535898/180); // deg -> rad
	lat2 *= (3.1415926535898/180); // deg -> rad
	lon2 *= (3.1415926535898/180); // deg -> rad
	double x1=cos(lon1)*cos(lat1);
	double y1=sin(lon1)*cos(lat1);
	double z1=sin(lat1);
	double x2=cos(lon2)*cos(lat2);
	double y2=sin(lon2)*cos(lat2);
	double z2=sin(lat2);
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2)+pow(z2-z1,2));
}

class highway
{	public:
	char System[256], Region[256], Route[256], Banner[256], Abbrev[256], City[256], Root[256]; /*AltRouteNames*/
	char Name[768], KeyValue[256];
	bool KeyFound;
	unsigned int NumPts;
	waypoint *pt1;
	highway *next;

	highway(char SysIn[256], char RegIn[256], char RteIn[256], char BanIn[256], char AbbIn[256], char CityIn[256], char RootIn[256], char KeyIn[256])
	{	NumPts = 0;
		strcpy(System, SysIn);
		strcpy(Region, RegIn);
		strcpy(Route, RteIn);
		strcpy(Banner, BanIn);
		strcpy(Abbrev, AbbIn);
		strcpy(City, CityIn);
		strcpy(Root, RootIn);
		strcpy(KeyValue, KeyIn);
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

	void ListPts(bool AllLabels)
	{	cout.precision(8);
		for (waypoint *point = pt1; point; point = point->next)
		{	if (AllLabels)
				for (unsigned int l = 0; l < point->NumLabels; l++) cout << point->label[l] << '\t'; // print labels
			else {	cout << point->label[0] << '\t';
				if (strlen(point->label[0]) < 8) cout << '\t';
			} // print coords
			cout << point->OrigLat << '\t' << point->OrigLon << '\t';
			cout << point->OffLat << '\t' << point->OffLon << '\t';
			cout << point->OnLat << '\t' << point->OnLon << endl;
		}
	}

	void write(bool DoOff, bool DoOn)
	{	if (!KeyFound)
		{	cout << "KeyValue " << KeyValue << " not found!" << endl;
			cout << "Skipping " << Root << endl << endl;
			return;
		}
		char OffName[1024] = "output/off/"; strcat(OffName, Root);
		if (strcmp(&OffName[strlen(OffName)-4], ".wpt")) strcat(OffName, ".wpt");
		char OnName[1024] = "output/on/"; strcat(OnName, Root);
		if (strcmp(&OnName[strlen(OnName)-4], ".wpt")) strcat(OnName, ".wpt");
		for (waypoint *point = pt1; point; point = point->next) // process each point
		{	for (unsigned int l = 0; l < point->NumLabels; l++) // write labels
			{	if (DoOff)
				{	ofstream OffFile(OffName, ios::app);
					OffFile << point->label[l] << ' ';
					OffFile.close();
				}
				if (DoOn)
				{	ofstream OnFile(OnName, ios::app);
					OnFile << point->label[l] << ' ';
					OnFile.close();
				}
			} //end for (write labels)
			if (DoOff)
			{	ofstream OffFile(OffName, ios::app);
				OffFile << "http://www.openstreetmap.org/?lat=";
				 OffFile.close();
				DblWrite(OffName, point->OffLat, 6);
				 OffFile.open(OffName, ios::app);
				  OffFile << "&lon=";
				   OffFile.close();
				DblWrite(OffName, point->OffLon, 6);
				 OffFile.open(OffName, ios::app);
				  OffFile << endl;
				   OffFile.close();
			}
			if (DoOn)
			{	ofstream OnFile(OnName, ios::app);
				OnFile << "http://www.openstreetmap.org/?lat=";
				 OnFile.close();
				DblWrite(OnName, point->OnLat, 6);
				 OnFile.open(OnName, ios::app);
				  OnFile << "&lon=";
				   OnFile.close();
				DblWrite(OnName, point->OnLon, 6);
				 OnFile.open(OnName, ios::app);
				  OnFile << endl;
				   OnFile.close();
			}
		} //end for (process each point)
	} //end write()

	void TakePt(double shpLat, double shpLon, bool firstPt, bool lastPt) // Compare shapefile point coords against coords of each WPT point
	{	double shpDist;
		KeyFound = 1;
		for (waypoint *point = pt1; point; point = point->next)
		{	shpDist = measure(shpLat, shpLon, point->OrigLat, point->OrigLon);
			if (shpDist < point->OffDist)
			{	point->OffDist = shpDist;
				point->OffLat = shpLat;
				point->OffLon = shpLon;
			}
			if (shpDist < point->OnDist)
				if (firstPt || lastPt || point->label[0][0] == '+')
				{	point->OnDist = shpDist;
					point->OnLat = shpLat;
					point->OnLon = shpLon;
				}
		}
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

highway* BuildRte(char *filename, char System[256], char Region[256], char Route[256], char Banner[256], char Abbrev[256], char City[256], char Root[256], char KeyValue[256])
{	// The WPT actually exists, right?
	ifstream WPT1 (filename, ifstream::in);
	if (!WPT1.is_open())
	{	cout << filename << " file not found!" << endl;
		cout << "Skipping this file." << endl;
		return 0;
	}
	ifstream WPT2 (filename, ifstream::in);

	char str[4096];
	int bookmark = 0;
	highway *hwy = new highway(System, Region, Route, Banner, Abbrev, City, Root, KeyValue);
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

	for (point = hwy->pt1; point; point = point->next)
	{	point->OffLat = point->OrigLat * -1;
		point->OffLon = point->OrigLon + 180;
		point->OffDist = measure(point->OffLat, point->OffLon, point->OrigLat, point->OrigLon);
		point->OnLat = point->OrigLat * -1;
		point->OnLon = point->OrigLon + 180;
		point->OnDist = measure(point->OnLat, point->OnLon, point->OrigLat, point->OrigLon);
	}

	//cout << "BuildRte: " << filename << " | " << Root << " OK\n";
	return hwy;
} //end BuildRte

void GetColors(char *System, char *UnColor, char *ClColor)
{	if (!strcmp(System, "USAI") || !strcmp(System, "usai"))
	{	strcpy (UnColor, "32ffff");
		strcpy (ClColor, "0000dc");
		return;
	}
	if (!strcmp(System, "USAME") || !strcmp(System, "usame"))
	{	strcpy (UnColor, "ffdcbe");
		strcpy (ClColor, "643200");
		return;
	}
	if (!strcmp(System, "USAUS") || !strcmp(System, "usaus"))
	{	strcpy (UnColor, "ffb4b4");
		strcpy (ClColor, "c80000");
		return;
	}
	if (!strcmp(System, "USAUSB") || !strcmp(System, "usausb"))
	{	strcpy (UnColor, "ff96ff");
		strcpy (ClColor, "b400b4");
		return;
	}
	strcpy (UnColor, "aaaaaa");
	strcpy (ClColor, "555555");
}

/*void readlist(char *htmlFile, highway *hwy)
{	char Region[256], Name[256], pl1[256], pl2[256];
	ofstream html(htmlFile, ios::app);
	ifstream list("list.list");

	struct ptPairT
	{	char pl1[256], pl2[256];
		ptPairT *next;
	};

	ptPairT *ptPair1 = 0;
	ptPairT *ptPair = 0;

	while (!list.eof())
	{	list >> Region >> Name >> pl1 >> pl2; //FIXME: assumes perfectly formatted .list file with exactly four strings on every line
		if (!strcmp(Region, hwy->Region) && !strcmp(Name, hwy->Name))
		{	if (!ptPair1)
			{	ptPair1 = new ptPairT;
				ptPair = ptPair1;
				ptPair->next = 0;
				strcpy(ptPair->pl1, pl1);
				strcpy(ptPair->pl2, pl2);
			} else
			{	ptPair->next = new ptPairT;
				ptPair = ptPair->next;
				ptPair->next = 0;
				strcpy(ptPair->pl1, pl1);
				strcpy(ptPair->pl2, pl2);
			}
			while (ptPair->pl1[0] == '+' || ptPair->pl1[0] == '*')
				strcpy (ptPair->pl1, &ptPair->pl1[1]);
			while (ptPair->pl2[0] == '+' || ptPair->pl2[0] == '*')
				strcpy (ptPair->pl2, &ptPair->pl2[1]);
		}
	}
	if (ptPair1)
	{	for (ptPair = ptPair1; ptPair->next; ptPair = ptPair->next)
			html << "[\"" << ptPair->pl1 << "\", \"" << ptPair->pl2 << "\"], ";
		html << "[\"" << ptPair->pl1 << "\", \"" << ptPair->pl2 << "\"]"; // write the last one
	}
	html.close();
	list.close();
}//*/

void readlist(char *htmlFile, highway *hwy)
{	char Region[256], Name[256], pl1[256], pl2[256];
	ofstream html(htmlFile, ios::app);
	ifstream list("list.list");
	bool comma = 0;

	while (!list.eof())
	{	list >> Region >> Name >> pl1 >> pl2; //FIXME: assumes perfectly formatted .list file with exactly four strings on every line
		if (!strcmp(Region, hwy->Region) && !strcmp(Name, hwy->Name))
		{	if (comma)	html << ", [\"" << pl1 << "\", \"" << pl2 << "\"]";
			else		html << "[\"" << pl1 << "\", \"" << pl2 << "\"]"; // write the first one
			comma = 1;
		}
	}
	html.close();
	list.close();
}

void HTML(highway *hwy)
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

	html << "(To see Canvas, upgrade browser.)\n\n";

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

		GetColors(hwy->System, UnColor, ClColor);
		html << "UnColor:'#" << UnColor << "', ClColor:'#" << ClColor << "',\n";

		html << "//EDB - point latitudes\n";
		html << "lat:[";
		for (waypoint *point = hwy->pt1; point; point = point->next)
		{	html.close();
			DblWrite(filename, point->OrigLat, 6);
			html.open(filename, ios::app);
			if (point->next) html << ", ";
		}
		html << "],\n";

		html << "//EDB - point longitudes\n";
		html << "lon:[";
		for (waypoint *point = hwy->pt1; point; point = point->next)
		{	html.close();
			DblWrite(filename, point->OrigLon, 6);
			html.open(filename, ios::app);
			if (point->next) html << ", ";
		}
		html << "],\n";

		// FIXME: deprecated; will disappear soon
		html << "//vdeane - labels from highway browser\n";
		html << "labels:[";
		for (waypoint *point = hwy->pt1; point; point = point->next)
		{	html << '"' << point->label[0] << '"';
			if (point->next) html << ", ";
		}
		html << "],\n";

		html << "//vdeane - data from .list file\ncliSegments:[";
		html.close();
		readlist(filename, hwy);
		html.open(filename, ios::app);
		html << "]\n} //end object definition\n\n";

		HwyNum++;
	} //end for (route objects)

	html << "var MinLat = rte[0].lat[0];\n";
	html << "var MinLon = rte[0].lon[0];\n";
	html << "var MaxLat = rte[0].lat[0];\n";
	html << "var MaxLon = rte[0].lon[0];\n";
	html << "var i, j, k;\n\n";

	html << "//EDB - get maximum and minimum latitude and longitude for a quick-n-dirty scale of the route trace to fill the canvas\n";
	html << "for (j = 0; j < rte.length; j++)\n";
	html << "{	for (i = 0; i < rte[j].lat.length; i++)\n";
	html << "	{	if (rte[j].lat[i] < MinLat) MinLat = rte[j].lat[i];\n";
	html << "		if (rte[j].lon[i] < MinLon) MinLon = rte[j].lon[i];\n";
	html << "		if (rte[j].lat[i] > MaxLat) MaxLat = rte[j].lat[i];\n";
	html << "		if (rte[j].lon[i] > MaxLon) MaxLon = rte[j].lon[i];\n";
	html << "	}\n";
	html << "}\n";
	html << "var width = MaxLon - MinLon;\n";
	html << "var height = MaxLat - MinLat;\n\n";

	html << "//John Pound - initialize canvas\n";
	html << "var canvas = document.getElementsByTagName(\"canvas\")[0];\n\n";

	html << "//vdeane & EDB - base route line traces\n";
	html << "for (k = 0; k < rte.length; k++)\n";
	html << "{	c = canvas.getContext(\"2d\");\n";
	html << "	c.save();\n";
	html << "	c.beginPath();\n";
	html << "	c.strokeStyle = rte[k].UnColor;\n";
	html << "	c.moveTo((rte[k].lon[0] - MinLon) * 699 / width, 699 - (rte[k].lat[0] - MinLat) * 699 / height);\n";
	html << "	for (i = 1; i < rte[k].lat.length; i++) c.lineTo((rte[k].lon[i] - MinLon) * 699 / width, 699 - (rte[k].lat[i] - MinLat) * 699 / height);\n";
	html << "	c.stroke();\n";
	html << "	c.restore();\n";
	html << "}\n\n";

	html << "//vdeane & EDB - begin drawing segments\n";
	html << "for (k = 0; k < rte.length; k++)\n";
	html << "{	for (i = 0; i < rte[k].cliSegments.length; i++)\n";
	html << "	{	var CliPt = false; //vdeane - track if start or end of segment\n";
	html << "		c = canvas.getContext(\"2d\");\n";
	html << "		c.save();\n";
	html << "		c.beginPath();\n";
	html << "		c.strokeStyle = rte[k].ClColor;\n\n";

	html << "		for (j = 0; j < rte[k].lat.length; j++)\n";
	html << "		{	if (rte[k].cliSegments[i][0] === rte[k].labels[j])\n";
	html << "			{	c.moveTo((rte[k].lon[j] - MinLon) * 699 / width, 699 - (rte[k].lat[j] - MinLat) * 699 / height);\n";
	html << "				CliPt = true;\n";
	html << "			}\n";
	html << "			if (CliPt === true && j !== 0)\n";
	html << "				c.lineTo((rte[k].lon[j] - MinLon) * 699 / width, 699 - (rte[k].lat[j] - MinLat) * 699 / height);\n";
	html << "			if (rte[k].cliSegments[i][1] === rte[k].labels[j])\n";
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

bool CSVmode(char** CharVars, unsigned int CharSkip)
{	char System[256], Region[256], Route[256], Banner[256], Abbrev[256], City[256], Root[256], KeyValue[256];
	char *InputFile = CharVars[3];
	char *SourceDir = CharVars[6];
	char CSVname[1024];
	strcpy(CSVname, InputFile);
	highway *FirstRte = 0;
	highway *hwy = 0;
	unsigned int i;

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
		char breaker = Root[i]; Root[i] = 0;
		if (breaker == ';') while ((CSV.get() != '\n') && !CSV.eof()); //skip to row end
		strcpy(InputFile, SourceDir); strcat(InputFile, Root); strcat(InputFile, ".wpt"); //construct input filename

		if (!CSV.eof())
			if (!FirstRte)
			{	FirstRte = BuildRte(InputFile, System, Region, Route, Banner, Abbrev, City, Root, KeyValue);
				hwy = FirstRte;
			} else
			{	hwy->next = BuildRte(InputFile, System, Region, Route, Banner, Abbrev, City, Root, KeyValue);
				if (hwy->next) hwy = hwy->next;
			}
	} //end while (step thru each line of CSV*/
	cout << endl;
	if (FirstRte) HTML(FirstRte);
} //end CSVmode2

int main()
{	char InputFile[1024] = "routes.csv";
	char SourceDir[1024] = "data/";
	char KeyValue[2] = "\0";
	char* CharVars[7] = {0,0,0, InputFile, 0, KeyValue, SourceDir};
	CSVmode(CharVars, 0);
}
