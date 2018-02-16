#include <list>
#include "../lib/dbf.cpp"	// includes cstring, fstream, iostream
using namespace std;

inline void ProgBar(unsigned int numerator, unsigned int denominator)
{	cout << numerator << '/' << denominator << char(0x0D);
}

void mine(DBF& rDBF)
{	cout << "Mining...\n";

	char *KFieldVal;
	list<char*> ValList(1, (char*)"\0");
	list<char*>::iterator it;

	ofstream file("output.txt", ios::app);
	ifstream fDBF(rDBF.name);
	fDBF.seekg(rDBF.HeaLen+rDBF.KeyOffset+1); // seek to first record's Key Field
	cout << "Seeking to " << rDBF.HeaLen+rDBF.KeyOffset+1 << endl;

	for (unsigned int RecNum = 0; RecNum < rDBF.NumRec && fDBF.tellg() < rDBF.size; RecNum++)
	{	// get KFieldVal
		KFieldVal = new char[rDBF.KeyLen+1];
		KFieldVal[rDBF.KeyLen] = 0;
		fDBF.read(KFieldVal, rDBF.KeyLen);
		fDBF.seekg(rDBF.RecLen-rDBF.KeyLen, ios::cur);
		// scan thru asciibetical list
		for (it = ValList.begin(); it != ValList.end(); it++) if (strcmp(KFieldVal, *it) <= 0) break;
		if (it == ValList.end()) ValList.insert(it, KFieldVal);
		else	if (strcmp(KFieldVal, *it) < 0)
				ValList.insert(it, KFieldVal);
			else delete KFieldVal;			// strcmp == 0; value already on list

		ProgBar(RecNum+1, rDBF.NumRec); //zeroth vs first
	}
	fDBF.close();
	it = ValList.begin();
	for (it++; it != ValList.end(); it++) file << *it << endl;
	cout << endl;
}

int main(int argc, char *argv[])
{	if (argc < 3) {cout << "usage: ./DBFmine DBFname KeyField" << endl; return 0;}
	bool OK;
	DBF rDBF(argv[1], OK);	if (!OK) return 0;	
	if (!rDBF.SetKey(argv[2])) return 0;

	ofstream file("output.txt"); // write commandline
	  for (int i = 0; i < argc; i++) file << argv[i] << ' ';
	  file << endl;
	  file.close();

	mine(rDBF);
	return 0;
}
