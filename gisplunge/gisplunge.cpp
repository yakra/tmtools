#include <thread>		// requires -std=c++11 -pthread on compile.
#include "../lib/dbf.cpp"	// includes cstring, fstream, iostream
#include "../lib/highway.cpp"	// includes cstring, fstream, iostream, list, string, vector
using namespace std;

class envV
{	public:
	string InputFile, ShapeRoot, KeyField, KeyValue, SourceDir;
	unsigned int CharSkip, Threads;
	unsigned char WriteMe;
	bool SegDump, CSVflag, DoubleBug;
	string SHPname, SHXname, DBFname;

	bool set()
	{	string iniField;
		ifstream INI("splunge.ini");
		if (!INI)
		{	cout << "splunge.ini file not found!\n";
			return 0;
		}

		// set InputFile & CSVflag
		while (iniField != "InputFile" && !INI.eof())	INI >> iniField;
		if (iniField != "InputFile")
		{	cout << "InputFile field not found in .INI file!\n";
			return 0;
		}
		INI >> InputFile;
		if (InputFile.size() > 3) if (!InputFile.compare(InputFile.size()-4, 4, ".csv"))
			CSVflag = 1;
		else	CSVflag = 0;

		// set ShapeRoot
		INI.seekg(0); iniField.clear();
		while (iniField != "ShapeRoot" && !INI.eof())	INI >> iniField;
		if (iniField != "ShapeRoot")
		{	cout << "ShapeRoot field not found in .INI file!\n";
			return 0;
		}
		INI >> ShapeRoot;
		DBFname = ShapeRoot + ".dbf";
		SHPname = ShapeRoot + ".shp";	ifstream SHP(SHPname.data());
		SHXname = ShapeRoot + ".shx";	ifstream SHX(SHXname.data());
		if (!SHP || !SHX)
		{	cout << SHPname << " or" << SHXname << " file(s) not found" << endl;
			return 0;
		}
		SHP.close();	SHX.close();

		// set KeyField
		INI.seekg(0); iniField.clear();
		while (iniField != "KeyField" && !INI.eof())	INI >> iniField;
		if (iniField != "KeyField")
		{	cout << "KeyField field not found in .INI file!\n";
			return 0;
		}
		INI >> KeyField;

		// set KeyValue
		INI.seekg(0); iniField.clear();
		while (iniField != "KeyValue" && !INI.eof())	INI >> iniField;
		if (iniField != "KeyValue")
		{	cout << "KeyValue field not found in .INI file!\n";
			return 0;
		}
		char charlie = INI.get(); // read rest of line
		for (INI.get(charlie); charlie != '\n' && !INI.eof(); INI.get(charlie)) KeyValue.push_back(charlie);

		// set SourceDir
		if (CSVflag)
		{	INI.seekg(0); iniField.clear();
			while (iniField != "SourceDir" && !INI.eof())	INI >> iniField;
			if (iniField != "SourceDir")
			{	cout << "CSV mode: SourceDir field not found in .INI file!\n";
				return 0;
			}
			INI >> SourceDir;

		// set CharSkip
			INI.seekg(0); iniField.clear();
			while (iniField != "CharSkip" && !INI.eof())	INI >> iniField;
			if (iniField != "CharSkip")
			{	cout << "CSVmode: CharSkip field not found in .INI file!\n";
				return 0;
			}
			INI >> CharSkip;

		// set Threads
			INI.seekg(0); iniField.clear(); Threads = 1;
			while (iniField != "Threads" && !INI.eof())	INI >> iniField;
			if (iniField == "Threads") INI >> Threads;
		}

		// set JctSnap
		INI.seekg(0); iniField.clear(); WriteMe = 0;
		while (iniField != "JctSnap" && !INI.eof())	INI >> iniField;
		if (iniField == "JctSnap")
		{	INI >> iniField;
			if (iniField == "OFF") WriteMe |= 2;
			else if (iniField == "ON") WriteMe |= 4;
			else if (iniField == "BOTH") WriteMe |= 6;
			else if (iniField != "NONE")
			{	cout << "JctSnap must be OFF, ON, BOTH, or NONE.\n";
				return 0;
			}
		}

		// set SegDump
		INI.seekg(0); iniField.clear(); SegDump = 0;
		while (iniField != "SegDump" && !INI.eof())	INI >> iniField;
		if (iniField == "SegDump")
		{	INI >> iniField;
			if (iniField == "ON") SegDump = 1;
		}

		// set WriteOrig
		INI.seekg(0); iniField.clear();
		while (iniField != "WriteOrig" && !INI.eof())	INI >> iniField;
		if (iniField == "WriteOrig")
		{	INI >> iniField;
			if (iniField == "ON") WriteMe |= 1;
		}

		// set DoubleBug
		INI.seekg(0); iniField.clear(); DoubleBug = 0;
		while (iniField != "DoubleBug" && !INI.eof())	INI >> iniField;
		if (iniField == "DoubleBug")
		{	INI >> iniField;
			if (iniField == "ON") DoubleBug = 1;
		}

		cout << InputFile << endl << ShapeRoot << endl << KeyField << " = " << KeyValue << endl;
		return 1;
	}
};

bool KeyMatch(DBF &dbf, const char *KeyValue, unsigned int offset)
// compares desired KeyValue against value in record's descriptor field
{	unsigned char lpad = 0;
	ifstream file (dbf.name);
	for (file.seekg(offset); file.get() == 0x20 && lpad < dbf.KeyLen-strlen(KeyValue); lpad++);
	file.seekg(-1, ios::cur);
	for (int index = 0; index < strlen(KeyValue); index++)
	{	if (file.get() != KeyValue[index])
		{	file.close();
			return 0;
		}
	}
	if (file.get() > 0x20 && strlen(KeyValue)+lpad < dbf.KeyLen) return 0;
	file.close();
	return 1;
}

bool ShpTypeSup(unsigned int ShpType)
{	switch (ShpType)
	{ case 3: return 1;
	  case 5: return 1;
	  case 13: return 1;
	  case 15: return 1;
	  case 23: return 1;
	  case 25: return 1;
	  case 0: cout << "Unsupported Shape type: 0 = Null shape\nSkipping this file.\n"; return 0;
	  case 1: cout << "Unsupported Shape type: 1 = Point\nSkipping this file.\n"; return 0;
	  case 8: cout << "Unsupported Shape type: 8 = MultiPoint\nSkipping this file.\n"; return 0;
	  case 11: cout << "Unsupported Shape type: 11 = PointZ\nSkipping this file.\n"; return 0;
	  case 18: cout << "Unsupported Shape type: 18 = MultiPointZ\nSkipping this file.\n"; return 0;
	  case 21: cout << "Unsupported Shape type: 21 = PointM\nSkipping this file.\n"; return 0;
	  case 28: cout << "Unsupported Shape type: 28 = MultiPointM\nSkipping this file.\n"; return 0;
	  case 31: cout << "Unsupported Shape type: 31 = MultiPatch\nSkipping this file.\n"; return 0;
	  default: cout << "Unsupported Shape type: " << ShpType << " = (unknown)\nSkipping this file.\n"; return 0;
	}
}

int ProcRte(envV &env, DBF &dbf, highway *hwy)
// returns 0: fatal error; program terminates // 1: success // 2: error; skips to next route
{	char zero = 0;
	bool KeyFound = 0;
	double shpLat,shpLon,shpDist;
	string segfn, KeyValue;
	if (env.CSVflag) KeyValue.assign(&hwy->Route[env.CharSkip]);
	else KeyValue = env.KeyValue;
	ifstream DBFf(env.DBFname.data());
	ifstream SHP(env.SHPname.data());
	ifstream SHX(env.SHXname.data());

	cout << hwy->Root << ".wpt\n";

	// search for each record of KeyValue
	for (DBFf.seekg(dbf.HeaLen + dbf.KeyOffset + 1); DBFf.tellg() < dbf.size; DBFf.seekg(dbf.RecLen, ios::cur))
	{	if (KeyMatch(dbf, KeyValue.data(), DBFf.tellg()))
		{	unsigned int rec = ((unsigned int)DBFf.tellg()-dbf.HeaLen)/dbf.RecLen+1;
			//cout << "Record " << rec << ": ";
			KeyFound = 1;
			SHX.seekg(8*(rec-1)+100); //seek to record in SHX file
			// get offset, in order to seek to it in SHP
			int offset = SHX.get()*0x1000000;
			    offset += SHX.get()*0x10000;
			    offset += SHX.get()*0x100;
			    offset += SHX.get();
			    offset*=2;

			SHP.seekg(offset);
			if (rec != SHP.get()*0x1000000 + SHP.get()*0x10000 + SHP.get()*0x100 + SHP.get())
			{	cout << "It's raining cheese! Record number mismatch." << endl;
				cout << "Skipping this file." << endl;
				return 2;
			}
			SHP.seekg(4, ios::cur);		 // skip over Content Length
			unsigned int ShpType;		 SHP.read((char*)&ShpType, 4);
			if (!ShpTypeSup(ShpType))	 return 2;
			SHP.seekg(32, ios::cur);	 // skip over bounding box
			unsigned int NumParts;		 SHP.read((char*)&NumParts, 4);
			unsigned int NumPts;		 SHP.read((char*)&NumPts, 4);
			SHP.seekg(4*NumParts, ios::cur); // skip over Parts' indices
			//cout << NumPts << " Points" << endl;

			ofstream SegFile;
			segfn = "output/segdump/" + KeyValue + "-" + DblText(rec, 0, 1) + ".wpt"; //prep filename for WPT dump
			if (env.SegDump) SegFile.open(segfn.data());

			for (bool firstPt = 1; NumPts; NumPts--) //collect each point
			{	SHP.read((char*)&shpLon, 8);
				SHP.read((char*)&shpLat, 8);

				if (env.SegDump)
				{	SegFile << rec << '-' << NumPts << " http://www.openstreetmap.org/?lat=";
					SegFile << DblText(shpLat, 6, env.DoubleBug) << "&lon=" << DblText(shpLon, 6, env.DoubleBug) << endl;
				}

				// Compare shapefile point coords against coords of each WPT point
				for (list<waypoint>::iterator ptI = hwy->pt.begin(); ptI != hwy->pt.end(); ptI++)
				{	shpDist = measure(shpLat, shpLon, ptI->Lat, ptI->Lon);
					if (shpDist < ptI->OffDist)
					{	ptI->OffDist = shpDist;
						ptI->OffLat = shpLat;
						ptI->OffLon = shpLon;
					}
					if (shpDist < ptI->OnDist)
					   if (	firstPt || (NumPts == 1) ||
						(ptI->label[0][0] == '+' && ptI->label[0][ptI->label.size()-1] != 'x' && ptI->label[0][ptI->label.size()-1] != 'X')
					      )
					   {	ptI->OnDist = shpDist;
						ptI->OnLat = shpLat;
						ptI->OnLon = shpLon;
					   }
				}
				firstPt = 0;
			} // end for (collect each point)
			//cout << endl;
		} // end if (KeyMatch)
	} // end for (Crawl DBF for each record)
	if (!KeyFound)
	{	cout << KeyValue << " not found!" << endl;
		cout << "Skipping this file." << endl << endl;
		return 2;
	}
	hwy->write(env.WriteMe, env.DoubleBug);
	delete hwy;
	return 1;
}

void ProcList(envV env, DBF dbf, vector<highway*> HwyVec, unsigned int ThreadNum)
{	for (unsigned int n = ThreadNum; n < HwyVec.size(); n += env.Threads) ProcRte(env, dbf, HwyVec[n]);
}

bool CSVmode(envV &env, DBF &dbf)
{	vector<highway*> HwyVec;
	ChoppedRtesCSV(HwyVec, env.InputFile, env.SourceDir, 0);

	if (env.Threads > HwyVec.size() || !env.Threads) env.Threads = HwyVec.size();
	thread **thr = new thread*[env.Threads];
	for (unsigned int t = 0; t < env.Threads; t++)
	{	thr[t] = new thread(ProcList, env, dbf, HwyVec, t);
	}
	for (unsigned int t = 0; t < env.Threads; t++)
	{	thr[t]->join();
	}
}

int main()
{	envV env; if (!env.set()) return 0;
	cout << endl;
	bool OK;
	DBF dbf(env.DBFname.data(), OK); if (!OK) return 0;	
	if (!dbf.SetKey(env.KeyField.data())) return 0;

	if (env.CSVflag) CSVmode(env, dbf);
	else	// initialize root with filename from InputFile
	{	string Root = &env.InputFile[env.InputFile.find_last_of("/\\")+1];
		if (!strcmp(&Root[Root.size()-4], ".wpt")) Root.erase(Root.size()-4, 4);

		highway *hwy = BuildRte(env.InputFile.data(), "\0", "\0", "\0", "\0", "\0", "\0", Root.data(), "\0");
		if (hwy) ProcRte(env, dbf, hwy);
	}
	return 0;
}
