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

waypoint::waypoint(highway *rte)
{	hwy = rte;
}

waypoint::~waypoint()
{	label.clear();
	URL.clear();
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
	// check for out-of-bounds coords
	if (Lat > 90)
	{	std::cout << "Warning: latitude > 90 in " << hwy->Root << " @ " << label[0] << "\n";
		std::cout << "         " << std::to_string(Lat) << " converted to ";
		while (Lat > 360)	Lat -= 360;
		if (Lat > 90)		Lat = 180-Lat;
		if (Lat < -90)		Lat = -180-Lat;
		std::cout << std::to_string(Lat) << '\n';
	}
	if (Lat < -90)
	{	std::cout << "Warning: latitude < -90 in " << hwy->Root << " @ " << label[0] << "\n";
		std::cout << "         " << std::to_string(Lat) << " converted to ";
		while (Lat < -360)	Lat += 360;
		if (Lat < -90)		Lat = -180-Lat;
		if (Lat > 90)		Lat = 180-Lat;
		std::cout << std::to_string(Lat) << '\n';
	}
	if (Lon > 180)
	{	std::cout << "Warning: longitude > 180 in " << hwy->Root << " @ " << label[0] << "\n";
		std::cout << "         " << std::to_string(Lon) << " converted to ";
		while (Lon > 180)	Lon -= 360;
		std::cout << std::to_string(Lon) << '\n';
	}
	if (Lon < -180)
	{	std::cout << "Warning: longitude < -180 in " << hwy->Root << " @ " << label[0] << "\n";
		std::cout << "         " << std::to_string(Lon) << " converted to ";
		while (Lon < -180)	Lon += 360;
		std::cout << std::to_string(Lon) << '\n';
	}//*/
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
