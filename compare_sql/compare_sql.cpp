#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <thread>

struct DB
{	std::ifstream file;
	std::list<std::string> lines;
	std::string line;
	int beg;
	int erasures;

	DB(const char* path): beg(1), erasures(0), file(path) {}

	void read_table()
	{	beg += lines.size() + erasures;
		erasures = 0;
		lines.clear();
		if (line.size()) lines.emplace_back(std::move(line));
		while (getline(file, line) && strncmp(line.data(), "CREATE TABLE", 12))
		  lines.emplace_back(std::move(line));
	}

	std::string table_name()
	{	if (!strncmp(lines.front().data(), "DROP TABLE IF EXISTS", 20)) return "DROP TABLE IF EXISTS";
		if (!strncmp(lines.front().data(), "CREATE TABLE ", 13))
		{	size_t space = lines.front().find(' ', 13);
			if (space != -1) return lines.front().substr(13, space-13);
		}
		return "\x1B[31m" + lines.front() + "\x1B[0m";
	}

	int len()
	{	return lines.size();
	}

	void sort()
	{	lines.sort();
	}

	void xcommas()
	{	for (auto& l : lines)
		  if (l[0] == ',')
		    l.erase(0,1);
	}

	void round(int p)
	{	char fstr[39];
		char pstr[12];
		strcpy(pstr, "%.");
		int e = sprintf(pstr+2, "%i", p);
		strcpy(pstr+2+e, "g");
		char* endptr;
		for (auto& l : lines)
		  for (size_t i = 0; i < l.size();)
		  {	while (!isdigit(l[i]) && i < l.size()) ++i;
			if (i < l.size())
			{	double n = strtod(l.data()+i, &endptr);
				e = sprintf(fstr, pstr, n);
				l.replace(i, endptr-l.data()-i, fstr);
				i += e;
			}
		  }
	}

	void xinsert()
	{	std::string insert("INSERT INTO "+table_name()+" VALUES");
		for (auto it = lines.begin(); it != lines.end();)
		  if (*it == ";" || *it == insert)
		  {	++erasures;
			it = lines.erase(it);
		  }
		  else	it++;
	}
};

void compare(DB& db1, DB& db2)
{	if (db1.lines.size() != db2.lines.size())
	{	std::cout << "\x1B[31mLine counts mismatch\x1B[0m " << db1.lines.size() << " <-> " << db2.lines.size() << std::endl;
		return;
	}
	auto it1 = db1.lines.begin();
	auto it2 = db2.lines.begin();
	while (it1 != db1.lines.end() && it2 != db2.lines.end())
	{	if (*it1 != *it2)
		{	std::cout << "\x1B[31m" << *it1 << "\x1B[0m <-> \x1B[31m"  << *it2 << "\x1B[0m\n";
			break;
		}
		++it1;
		++it2;
	}
	if (it1 == db1.lines.end() && it2 == db2.lines.end())
		std::cout << "\x1B[32mOK.\x1B[0m\n";
}
void sort   (DB& db1, DB& db2)
{	std::thread thr1(&DB::sort, &db1);
	std::thread thr2(&DB::sort, &db2);
	thr1.join();	thr2.join();
}
void xcommas(DB& db1, DB& db2)
{	std::thread thr1(&DB::xcommas, &db1);
	std::thread thr2(&DB::xcommas, &db2);
	thr1.join();	thr2.join();
}
void round  (DB& db1, DB& db2, int p)
{	std::thread thr1(&DB::round, &db1, p);
	std::thread thr2(&DB::round, &db2, p);
	thr1.join();	thr2.join();
}
void xinsert(DB& db1, DB& db2)
{	std::thread thr1(&DB::xinsert, &db1);
	std::thread thr2(&DB::xinsert, &db2);
	thr1.join();	thr2.join();
}

int main(int argc, char *argv[])
{	if (argc != 3)
	{	std::cout << "Please specify two versions to be compared.\n";
		return 1;
	}
	DB db1(argv[1]); if (!db1.file.is_open()) { std::cout << "can't open " << argv[1] << std::endl; return 1; }
	DB db2(argv[2]); if (!db2.file.is_open()) { std::cout << "can't open " << argv[2] << std::endl; return 1; }
	std::cout << "    BEG1     LEN1                          TABLE     BEG2     LEN2\n";
	std::cout << "-------- -------- ------------------------------ -------- --------\n";
	while (!db1.file.eof() && !db2.file.eof())
	{	std::thread thr1(&DB::read_table, &db1);
		std::thread thr2(&DB::read_table, &db2);
		thr1.join();
		thr2.join();
		std::string tablename(db1.table_name());
		printf("%8i %8i %30s %8i %8i ", db1.beg, db1.len(), tablename.data(), db2.beg, db2.len()); fflush(stdout);

		if (tablename == "clinched"				// 7332387
		 || tablename == "regions"				// 512
		   ) {	xcommas(db1, db2);
			sort   (db1, db2);
			compare(db1, db2);
		     }	else
		if (tablename == "waypoints"				// 1333538
		 || tablename == "segments"				// 1246418
		 || tablename == "datacheckErrors"			// 15588
		 || tablename == "updates"				// 6153
		 || tablename == "connectedRouteRoots"			// 3103
		 || tablename == "systemUpdates"			// 716
		 || tablename == "systems"				// 494
		 || tablename == "countries"				// 211
		 || tablename == "DROP TABLE IF EXISTS"			// 19
		 || tablename == "continents"				// 10
		 || tablename == "graphArchiveSets"
		   ) {	compare(db1, db2);
		     }	else
		if (tablename == "clinchedRoutes"			// 425822
		 || tablename == "clinchedConnectedRoutes"		// 352890
		   ) {	xinsert(db1, db2);
			xcommas(db1, db2);
			round  (db1, db2, 17);
			sort   (db1, db2);
			compare(db1, db2);
		     }	else
		if (tablename == "routes"				// 87121
		 || tablename == "connectedRoutes"			// 84021
		   ) {	round  (db1, db2, 17);
			compare(db1, db2);
		     }	else
		if (tablename == "clinchedSystemMileageByRegion"	// 45013
		   ) {	xcommas(db1, db2);
			round  (db1, db2, 9);
			sort   (db1, db2);
			compare(db1, db2);
		     }	else
		if (tablename == "clinchedOverallMileageByRegion"	// 11459
		   ) {	xcommas(db1, db2);
			round  (db1, db2, 9);
			sort   (db1, db2);
			compare(db1, db2);
		     }	else
		if (tablename == "systemMileageByRegion"		// 1285
		   ) {	round  (db1, db2, 12);	// Python
			sort   (db1, db2);	// Python
			compare(db1, db2);
		     }	else
		if (tablename == "graphs"				// 1227
		 || tablename == "graphArchives"
		 || tablename == "graphTypes"				// 11
		   ) {	sort   (db1, db2);	// Python & C++
			compare(db1, db2);	// in diff order
		     }	else
		if (tablename == "overallMileageByRegion"		// 344
		   ) {	xcommas(db1, db2);	// Python
			round  (db1, db2, 12);	// Python
			sort   (db1, db2);	// Python
			//round(db1, db2, 13);	// C++
			compare(db1, db2);
		     }
		else std::cout << "\x1B[31m" << tablename << " not handled\x1B[0m" << std::endl;
	}
}
