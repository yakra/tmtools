#include <list>
#include <unordered_set>
#include "../lib/dbf.cpp"	// includes cstring, fstream, iostream
using namespace std;

inline void ProgBar(unsigned int numerator, unsigned int denominator)
{	cout << numerator << '/' << denominator << char(0x0D);
}

void mine(DBF& rDBF, const char *filename)
{	cout << "Mining...\n";

	char *KFieldVal;
	unordered_set<string> values;

	ofstream file(filename, ios::app);
	ifstream fDBF(rDBF.name);
	fDBF.seekg(rDBF.HeaLen+rDBF.KeyOffset+1); // seek to first record's Key Field
	cout << "Seeking to " << rDBF.HeaLen+rDBF.KeyOffset+1 << endl;

	for (unsigned int RecNum = 0; RecNum < rDBF.NumRec && fDBF.tellg() < rDBF.size; RecNum++)
	{	// get KFieldVal
		KFieldVal = new char[rDBF.KeyLen+1];
		KFieldVal[rDBF.KeyLen] = 0;
		fDBF.read(KFieldVal, rDBF.KeyLen);
		fDBF.seekg(rDBF.RecLen-rDBF.KeyLen, ios::cur);
		values.emplace(KFieldVal);
		delete KFieldVal;
		ProgBar(RecNum+1, rDBF.NumRec); //zeroth vs first
	}
	fDBF.close();
	list<string> ValList(values.begin(), values.end());
	ValList.sort();
	for (string &entry : ValList) file << entry << endl;
	cout << endl;
}

int main(int argc, char *argv[])
{	if (argc < 3) {cout << "usage: ./DBFmine DBFname KeyField" << endl; return 0;}
	bool OK;
	DBF rDBF(argv[1], OK);	if (!OK) return 0;	
	if (!rDBF.SetKey(argv[2])) return 0;

	// prep filename
	string filename(argv[1]);
	filename = filename.substr(filename.find_last_of("/\\")+1);
	if (!filename.compare(filename.size()-4, 4, ".dbf")) filename.erase(filename.size()-4, 4);
	filename += "--";
	filename += argv[2];
	filename += ".txt";

	// write commandline
	ofstream file(filename.data());
	  for (int i = 0; i < argc; i++) file << argv[i] << ' ';
	  file << endl;
	  file.close();

	mine(rDBF, filename.data());
	cout << filename << endl;
	return 0;
}
