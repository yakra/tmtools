#include <cstdlib> // for functions rand and srand
#include <ctime> // for function time, and for data type time_t
#include "../lib/waypoint.cpp"	// includes cmath, cstring, fstream, iostream, list, string, vector
using namespace std;

double merc(double lat) { return log(tan(0.785398163+lat*3.1415926535898/360))*180/3.1415926535898; }
double amerc(double y)	{ return (atan(pow(2.718281828459, y/180*3.1415926535898))-0.785398163)/3.1415926535898*360; }

void AddPoint(highway &hwy, list<waypoint>::iterator post, double Lat, double Lon)
{	list<waypoint>::iterator NewPt = hwy.pt.emplace(post, &hwy, "+x4u70-"+to_string(rand()%1000000), Lat, Lon);
	cout << "      " << NewPt->label[0] << ' ' << NewPt->URL << '\n';
}

double EdgeSegDist(double m1, double b1, double PtX, double PtY)
// find nearest point on edge segment; return distance from test point
{	double m2 = 1/m1;
	double b2 = PtY-m2*PtX;
	double x = (b1-b2)/(m2-m1);
	double y = m1*x+b1;
	return hypot(PtX-x, PtY-y);
}

double six(double val)
{	return round(val*1000000)/1000000;
}

int main(int argc, char *argv[])
{	time_t rseed; time(&rseed);
	srand((unsigned int)rseed);
	if (argc < 3)	{ cout << "usage: ./autoshape <HwyDataDir> <IniFile>\n"; return 0; }
	ifstream ini(argv[2]);
	if (!ini)	{ cout << argv[2] << " not found.\n"; return 0; }

	string line;
	while (getline(ini, line))
	{	char *iniLine = new char[line.size()+1];
		strcpy(iniLine, line.data());
		string Region = strtok(iniLine, " ");
		string System = strtok(0, " ");
		string Root = strtok(0, " ");
		string fname = argv[1]+Region+"/"+System+"/"+Root+".wpt";
		cout << fname << '\n';

		vector<string> AftPt;
		for (char *token = strtok(0, " "); token; token = strtok(0, " "))
			AftPt.push_back(token);	// get all remaining tokens & put into label vector*/

		highway hwy(fname, new tmsystem, "", "", "", "", "", "", Root, "");
		for (unsigned int i = 0; i < AftPt.size(); i++) // process each point in ini file line
		{	list<waypoint>::iterator pre = hwy.label2iter(AftPt[i]);
			if (pre != hwy.pt.end()) // if point label was found
			{	list<waypoint>::iterator post = pre; post++;
				if (post == hwy.pt.end())
					cout << "  " << pre->label[0] << " is final point in file; skipping.\n";
				else {	cout << "  " << pre->label[0] << " @ " << to_string(pre->Lat) << ',' << to_string(pre->Lon) << \
					    " <-> " << post->label[0] << " @ " << to_string(post->Lat) << ',' << to_string(post->Lon) << '\n';
					double south = fmin(pre->Lat, post->Lat);
					double north = fmax(pre->Lat, post->Lat);
					double west = fmin(pre->Lon, post->Lon);
					double east = fmax(pre->Lon, post->Lon);
					double vdiff = north - south;
					double hdiff = east - west;
					cout << "  vdiff = " << to_string(vdiff) << '\n';
					cout << "  hdiff = " << to_string(hdiff) << '\n';
					if (vdiff == 0) AddPoint(hwy, post, pre->Lat, (west+east)/2);
					if (hdiff == 0) AddPoint(hwy, post, amerc((merc(south)+merc(north))/2), pre->Lon);
					double m1 = (merc(post->Lat)-merc(pre->Lat))/(post->Lon-pre->Lon);
					double b1 = merc(pre->Lat)-m1*pre->Lon;
					double BestDist = hypot(hdiff, merc(north)-merc(south));
					double BestLat, BestLon;
					double thresh = 0.000001;
					if (argc >= 4) if (!strcmp(argv[3], "-n") || !strcmp(argv[3], "--NoNMPs")) thresh = 0.000500;

					if (hdiff >= vdiff && vdiff)
					{	for (double TestX = west+thresh; TestX <= east-thresh; TestX += 0.000001)
						{	double TestY = amerc(m1*TestX+b1);
							double TestDist = EdgeSegDist(m1, b1, TestX, merc(six(TestY)));
							cout.precision(10);
							if (TestDist < BestDist)
							{	BestDist = TestDist;
								BestLat = TestY;
								BestLon = TestX;
								cout << fixed << "    " << TestY << " --> " << to_string(six(TestY)) << ", ";
								cout << to_string(TestX) << ":\t" << scientific << TestDist << '\n';
							}
						}
						AddPoint(hwy, post, BestLat, BestLon);
					}
					if (vdiff > hdiff && hdiff)
					{	for (double TestY = south+thresh; TestY <= north-thresh; TestY += 0.000001)
						{	double TestX = (merc(TestY)-b1)/m1;
							double TestDist = EdgeSegDist(m1, b1, TestX, merc(six(TestY)));
							cout.precision(10);
							if (TestDist < BestDist)
							{	BestDist = TestDist;
								BestLat = TestY;
								BestLon = TestX;
								cout << fixed << "    " << to_string(TestY) << ", " << to_string(six(TestX)) << " <-- ";
								cout << TestX << ":\t" << scientific << TestDist << '\n';
							}
						}
						AddPoint(hwy, post, BestLat, BestLon);
					}

				     } // end else (specified point is not final point in file)
			} // end if (point label was found)
		} // end for (process each point in ini file line)
		hwy.write(argv[1]+Region+"/"+System+"/", 0);
	} // end while (getline(ini, line))
}
