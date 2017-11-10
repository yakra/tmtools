using namespace std;
#include <fstream>
#include <iostream>
#include <cstring>

class DBF;
#include "../lib/field.h"
#include "../lib/dbf.cpp"
#include "../lib/field.cpp"

inline void ProgBar(unsigned int numerator, unsigned int denominator)
{	cout << numerator << '/' << denominator << char(0x0D);
}

int main(int argc, char *argv[])
{	//ofstream timestamp("timestamp"); //TEST
	cout << endl;
	if (argc != 3)	{ cout << "usage: DBFtrim InputFile OutputFile\n\n"; return 0; }
	ifstream filetest(argv[2]);
	if (filetest)
	{	cout << "OutputFile " << argv[2] << " already exists; overwriting not permitted. Aborting.\n\n";
		return 0;
	} filetest.close();

	bool OK;
	DBF oDBF(argv[1], OK);	if (!OK) return 0;	// o is for original
	DBF tDBF(oDBF);		tDBF.InitCopy(oDBF);	// t is for trimmed

	// gather field info
	ifstream inDBF(argv[1]);
	inDBF.seekg(oDBF.HeaLen);
	cout << "Scanning DBF file...\n";
	for (unsigned int rNum = 0; rNum < oDBF.NumRec && inDBF.tellg() < oDBF.size; rNum++)
	{	inDBF.get(); // seek past leading ' ' or '*'
		for (unsigned int fNum = 0; fNum < oDBF.NumFields; fNum++)
		{	char *fVal = new char[oDBF.fArr[fNum].len+1]; fVal[oDBF.fArr[fNum].len] = 0;
			inDBF.read(fVal, oDBF.fArr[fNum].len); // read in value from file
			oDBF.fArr[fNum].GetMax(tDBF, fNum, fVal);
		}
		ProgBar(rNum+1, oDBF.NumRec);
	}//*/

	//field info display
	cout << "\nFieldName\tType\tLength\tMax\tData\n";
	for (unsigned int i = 0; i < oDBF.NumFields; i++)
	{	cout << oDBF.fArr[i].name;	if (strlen(oDBF.fArr[i].name) < 8) cout << '\t'; // tab stop
		cout << '\t' << oDBF.fArr[i].type;
		cout << '\t' << int(oDBF.fArr[i].len);
		cout << '\t' << int(tDBF.fArr[i].len);
		cout << '\t' << oDBF.fArr[i].MaxVal;
		    if (oDBF.fArr[i].MinEx0)
		    {	if (strchr(oDBF.fArr[i].MaxVal, '.')) oDBF.fArr[i].MaxVal[tDBF.fArr[i].len] = '0';
			else oDBF.fArr[i].MaxVal[tDBF.fArr[i].len] = '.';
			cout << " <- " << oDBF.fArr[i].MaxVal;
		    }
		cout << endl;
	}

	// write output file
	tDBF.SetRecLen();
	ofstream outDBF(argv[2]);
	inDBF.seekg(oDBF.HeaLen-1); // minus 1 in order to get 0Dh stored as the field terminator.
	cout << "Saving trimmed file...\n";
	// write header
	outDBF.write((char*)&tDBF, 32);
	outDBF.write((char*)tDBF.fArr, 32*tDBF.NumFields);	// write field descriptor array
	outDBF.put(inDBF.get());				// 0Dh stored as the field terminator.
	// write records
	for (unsigned int rNum = 0; rNum < oDBF.NumRec && inDBF.tellg() < oDBF.size; rNum++)
	{	outDBF.put(inDBF.get()); // ' ' or '*' precedes record contents
		for (unsigned int fNum = 0; fNum < oDBF.NumFields; fNum++)
			if (oDBF.fArr[fNum].type != 'C')
			{	inDBF.seekg(oDBF.fArr[fNum].len-tDBF.fArr[fNum].len-oDBF.fArr[fNum].MinEx0, ios::cur);
				for (unsigned char c = 0; c < tDBF.fArr[fNum].len; c++) outDBF.put(inDBF.get());
				inDBF.seekg(oDBF.fArr[fNum].MinEx0, ios::cur);
			}
			else {	for (unsigned char c = 0; c < tDBF.fArr[fNum].len; c++) outDBF.put(inDBF.get());
				inDBF.seekg(oDBF.fArr[fNum].len-tDBF.fArr[fNum].len, ios::cur);
			     }
		ProgBar(rNum+1, oDBF.NumRec);
	}
	if (!tDBF.borderline) outDBF.put(0x1A); // EOF marker */
	cout << endl;
}
