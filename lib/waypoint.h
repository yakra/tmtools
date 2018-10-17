#include <deque>
#include <string>

class highway;

class waypoint
{	highway *hwy;
	std::string Root();
	public:
	std::deque<std::string> label;
	std::string URL;
	double Lat, Lon, OffLat, OffLon, OnLat, OnLon, OffDist, OnDist;

	waypoint(highway *, std::string &);
	waypoint(highway *, std::string, double, double);
	~waypoint();
	void CoordCheck();
	void demote(std::string);
	void InitCoords();
	inline char* NakedLabel (unsigned int);
	bool nearby(waypoint &, double);
};
