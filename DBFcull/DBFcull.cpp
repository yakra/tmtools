#include "../lib/dbf.cpp"	// includes cstring, fstream, iostream
using namespace std;

inline void ProgBar(unsigned int numerator, unsigned int denominator)
{	cout << numerator << '/' << denominator << char(0x0D);
}

bool match(int argc, char *argv[], DBF& DBFh, bool *keep, unsigned int index)
{	for (int a = 3; a < argc; a++)
	  if (!strcmp(DBFh.fArr[index].name, argv[a]))
	  {	DBFh.HeaLen -= 0x20;
		DBFh.RecLen -= DBFh.fArr[index].len;
		keep[index] = 0;
		return 0;
	  }
	keep[index] = 1;
	return 1;
}

int main(int argc, char *argv[])
{	cout << endl;
	if (argc < 4)	{ cout << "usage: ./DBFcull InputFile OutputFile Field1 (Field2) (...)\n\n"; return 0; }
	ifstream filetest(argv[2]);
	if (filetest)
	{	cout << "OutputFile " << argv[2] << " already exists; overwriting not permitted. Aborting.\n\n";
		return 0;
	} filetest.close();

	bool OK;
	DBF DBFh(argv[1], OK);	if (!OK) return 0;
	bool *keep = new bool[DBFh.NumFields]; // whether to cull or keep each field

	//field info display
	cout << "FieldName\tType\tLength\tKeep?\n";
	for (unsigned int i = 0; i < DBFh.NumFields; i++)
	{	cout << DBFh.fArr[i].name;	if (strlen(DBFh.fArr[i].name) < 8) cout << '\t'; // tab stop
		cout << '\t' << DBFh.fArr[i].type;
		cout << '\t' << int(DBFh.fArr[i].len);
		if (match(argc, argv, DBFh, keep, i))
			cout << "\tkeep\n";
		else	cout << "\tcull\n";
	}
	cout << "New Header Length:\t0x" << hex << DBFh.HeaLen << '\t' << dec << DBFh.HeaLen << endl;
	cout << "New Record Length:\t0x" << hex << DBFh.RecLen << '\t' << dec << DBFh.RecLen << endl;

	// write output file
	ifstream inDBF(argv[1]);
	inDBF.seekg(0x20);
	ofstream outDBF(argv[2]);
	// write header
	cout << "Writing header...\n";
	outDBF.write((char*)&DBFh, 32);
	for (unsigned int n = 0; n < DBFh.NumFields; n++)				// field descriptor array
		if (keep[n]) for (char c = 0; c < 0x20; c++) outDBF.put(inDBF.get());	// keep & copy field descriptor
		else inDBF.seekg(0x20, ios::cur);					// cull & skip field descriptor
	outDBF.put(inDBF.get());							// 0Dh stored as the field terminator
	// the actual records
	cout << "Writing records...\n";
	for (unsigned int r = 0; r < DBFh.NumRec; r++)
	{	outDBF.put(inDBF.get()); // either ' ' or '*'
		for (unsigned int n = 0; n < DBFh.NumFields; n++)
			if (keep[n])
				for (unsigned char c = 0; c < DBFh.fArr[n].len; c++) outDBF.put(inDBF.get());	// keep & copy field
			else	inDBF.seekg(DBFh.fArr[n].len, ios::cur);					// cull & skip field
		ProgBar(r+1, DBFh.NumRec);
	}
	// EOF marker
	if (!DBFh.borderline) outDBF.put(char(0x1A));

	// one more newline for the road
	cout << endl;
}
