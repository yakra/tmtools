#include <cmath>
#include <cstdlib>
#include <deque>
#include <string>

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

class waypoint
{	public:
	std::deque<std::string> label;
	std::string URL;
	double OrigLat, OrigLon, OffLat, OffLon, OnLat, OnLon, OffDist, OnDist;

	~waypoint()
	{	label.clear();
		URL.clear();
	}
	
	void demote(std::string NewPriLbl)
	{	if (label.size()) label.at(0) = "+" + label.at(0);
		label.push_front(NewPriLbl);
	}

	void InitCoords()
	{	// parse URL
		size_t latBeg = URL.find("lat=")+4;	if (latBeg == 3) latBeg = URL.size();
		size_t lonBeg = URL.find("lon=")+4;	if (lonBeg == 3) lonBeg = URL.size();
		OrigLat = strtod(&URL[latBeg], 0);
		OrigLon = strtod(&URL[lonBeg], 0);
		// "Worst First" for gisplunge coords, so everything after will be an improvement
		OnLat = OrigLat * -1; OffLat = OnLat;
		OnLon = OrigLon + 180; OffLon = OnLon;
		OnDist = measure(OnLat, OnLon, OrigLat, OrigLon); OffDist = OnDist;
	}

	inline char* NakedLabel (unsigned int l)
	{	unsigned int i = 0;
		while (label[l][i] == '+' || label[l][i] == '*') i++;
		return &label[l][i];
	}
};
