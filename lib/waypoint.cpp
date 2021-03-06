#include <cmath>	// sin, cos, pow
#include <cstdlib>	// strtod
#include "highway.cpp"	// includes deque, string

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
	return pow(x2-x1,2)+pow(y2-y1,2)+pow(z2-z1,2);
}

waypoint::waypoint(highway *rte, std::string &WPTline)
{	hwy = rte;
	char *wlArr = new char[WPTline.size()+1];
	strcpy(wlArr, WPTline.data());
	for (char *token = strtok(wlArr, " "); token; token = strtok(0, " "))
		label.push_back(token);	// get all tokens & put into label deque
	if (!label.empty())
	{	URL = label.back();	// last token is actually the URL...
		label.pop_back();	// ...and not a label.
		if (label.empty()) label.push_back("NULL");
		InitCoords();
	}
}

waypoint::waypoint(highway *rte, std::string PriLabel, double lat, double lon)
{	hwy = rte;
	label.push_front(PriLabel);
	Lat = lat; Lon = lon;
	CoordCheck();
	URL = "http://www.openstreetmap.org/?lat="+std::to_string(Lat)+"&lon="+std::to_string(Lon);
}

waypoint::~waypoint()
{	label.clear();
	URL.clear();
}

void waypoint::CoordCheck()
{	// check for out-of-bounds coords
	if (abs(Lat) > 90)
	{	std::cout << "Warning: out-of-bounds latitude in " << Root() << " @ " << label[0] << "\n";
		std::cout << "         " << std::to_string(Lat) << " converted to ";
		Lat = remainder(Lat, 360);
		if (Lat < -90)		Lat = -180-Lat;
		if (Lat > 90)		Lat = 180-Lat;
		std::cout << std::to_string(Lat) << '\n';
	}
	if (Lon > 180)
	{	std::cout << "Warning: longitude > 180 in " << Root() << " @ " << label[0] << "\n";
		std::cout << "         " << std::to_string(Lon) << " converted to ";
		Lon = remainder(Lon, 360);
		std::cout << std::to_string(Lon) << '\n';
	}
	if (Lon < -180)
	{	std::cout << "Warning: longitude < -180 in " << Root() << " @ " << label[0] << "\n";
		std::cout << "         " << std::to_string(Lon) << " converted to ";
		while (Lon < -180)	// using while loop instead of remainder here
			Lon += 360;	// to avoid -180 being "corrected" to 180
		std::cout << std::to_string(Lon) << '\n';
	}//*/
}

void waypoint::demote(std::string NewPriLbl)
{	if (label.size()) label.at(0) = "+" + label.at(0);
	label.push_front(NewPriLbl);
}

void waypoint::InitCoords()
{	// parse URL
	size_t latBeg = URL.find("lat=")+4;	if (latBeg == 3) latBeg = URL.size();
	size_t lonBeg = URL.find("lon=")+4;	if (lonBeg == 3) lonBeg = URL.size();
	Lat = strtod(&URL[latBeg], 0);
	Lon = strtod(&URL[lonBeg], 0);

	//CoordCheck(); // commented out because Fiji. Possibly re-implement as an option in the future.

	// "Worst First" for gisplunge coords, so everything after will be an improvement
	OnLat = Lat * -1; OffLat = OnLat;
	OnLon = Lon + 180; OffLon = OnLon;
	OnDist = measure(OnLat, OnLon, Lat, Lon); OffDist = OnDist;
}

inline char* waypoint::NakedLabel (unsigned int l)
{	unsigned int i = 0;
	while (label[l][i] == '+' || label[l][i] == '*') i++;
	return &label[l][i];
}

bool waypoint::nearby(waypoint &other, double tolerance)
/* return if this waypoint's coordinates are within the given
tolerance (in degrees) of the other */
{	return abs(Lat - other.Lat) < tolerance && abs(Lon - other.Lon) < tolerance;
}

std::string waypoint::Root()
{	if (hwy) return hwy->Root;
	return "unknown highway (null pointer in waypoint::hwy); URL = " + URL;
}
