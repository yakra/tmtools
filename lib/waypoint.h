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
	~waypoint();
	void demote(std::string);
	void InitCoords();
	inline char* NakedLabel (unsigned int);
	bool nearby(waypoint &, double);
};
