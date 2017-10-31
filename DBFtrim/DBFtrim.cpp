using namespace std;
#include <fstream>
#include <iostream>
#include <cstring>

inline void ProgBar5(unsigned int numerator, unsigned int denominator)
{	cout << numerator << '/' << denominator << char(0x0D);
}

class DBF;

class field
{	public:
	char name[11];	// 11 B on disk; 10 B practical storage space. Final element is null terminator.
	char type;	// Field type in ASCII (C, D, L, M, or N).
	unsigned int DataAddx;
	char len;
	char DecCount;	// may be useful for numeric fields?
	char padding[6];
	char *MaxVal; // only works with 64-bit memory addressing, E.G. x86-64
	// No more variables! Must be able to write/read a whole array to/from disk.
	void GetMax(DBF&, unsigned int, char*);
}; //WARNING: functionality is predicated on sizeof(field) being 32 bytes. Meaning, 64-bit memory addressing; see comment for *MaxVal above

class DBF
{	public:
	// first 32 bytes of header, as contained in file	
	char ValidFile;
	char LastUpdate[3];
	unsigned int NumRec;	// NumRecords
	unsigned short HeaLen;	// HeaderLength
	unsigned short RecLen;	// RecordLength
	char reserved[20];
	// in-program use only; not to be read from / written to disk
	field *fArr;		// field descriptor array
	unsigned int size, NumFields;
	char FirstChar, FinalChar;
	bool borderline;	// flags cases where actual input filesize = 1 less than calculated filesize
	char *name;

	DBF(char *filename, bool &OK) // read in header from file
	{	ifstream inDBF(filename, ios::in);
		if (!inDBF.is_open()) { OK = 0; cout << filename << " file not found!\n"; }
		else {	OK = 1; cout << filename << " opened.\n";
			name = filename;
			inDBF.get(FirstChar);
			inDBF.seekg(0, ios::end);	size = inDBF.tellg();
			inDBF.seekg(-1, ios::cur);	inDBF.get(FinalChar);
			inDBF.seekg(0);
			inDBF.read((char*)this, 0x20);
			NumFields = (HeaLen-0x21)/0x20;
			fArr = new field[NumFields];
			inDBF.read((char*)fArr, 0x20*NumFields);

			cout << "DBF Filesize:\t" << size << " (sanity check ";
			if	(size == NumRec*RecLen+HeaLen+1) { borderline = 0; cout << "pass)\n"; }
			else if	(size == NumRec*RecLen+HeaLen)	 { borderline = 1; cout << "borderline; may be missing terminal 0x1A)\n"; }
			else	{ OK = 0; cout << "fail: " << NumRec*RecLen+HeaLen+1 << " expected)\n"; }
			cout << "Number Records:\t0x" << hex << NumRec << '\t' << dec << NumRec << endl;
			cout << "Header Length:\t0x" << hex << HeaLen << '\t' << dec << HeaLen << endl;
			cout << "Record Length:\t0x" << hex << RecLen << '\t' << dec << RecLen << endl;
			cout << "First char:\t0x" << hex << int(FirstChar) << '\t' << dec << int(FirstChar) << endl;
			cout << "Final char:\t0x" << hex << int(FinalChar) << '\t' << dec << int(FinalChar) << endl;
			cout << NumFields << " fields:\n";
		     }
	}

	void CopyFields(DBF& oDBF)
	{	fArr = new field[NumFields];
		for (unsigned int byte = 0; byte < 32*NumFields; byte++)
			*((char*)fArr+byte) = *((char*)oDBF.fArr+byte);
		for (unsigned int i = 0; i < NumFields; i++)
		  if (fArr[i].MaxVal)
		  {	fArr[i].MaxVal = 0;
			cout << "Warning: overwriting reserved nonzero bytes in field #" << i << ", " << fArr[i].name << endl;
		  }
	}

	void SetRecLen()
	{	RecLen = 1; // start at ' ' or '*'
		for (unsigned int fNum = 0; fNum < NumFields; fNum++)
			RecLen += fArr[fNum].len;
	}
};

void field::GetMax(DBF& tDBF, unsigned int fNum, char* value)
{	switch (type)
	{   case 'C':
		while (value[strlen(value)-1] == ' ') value[strlen(value)-1] = 0; // trim whitespace
		if (!MaxVal)
		{	tDBF.fArr[fNum].len = 0;
			MaxVal = new char[1]; MaxVal[0] = '\0';
		}
		if (strlen(value) > tDBF.fArr[fNum].len)
		{	tDBF.fArr[fNum].len = strlen(value);
			delete[] MaxVal; // delete[]ing a null pointer appears to be OK
			MaxVal = value;
		}
		return;
//	    case 'N':
		//TODO: add numeric field support
	    default:
		if (!MaxVal) // ELSE case should only ever be "  <Type ? fields unsupported>", where '?' is field type
		{	MaxVal = new char[30];
			strcpy(MaxVal, "  <Type ? fields unsupported>");
			MaxVal[8] = type;
		}
	}
}

int main(int argc, char *argv[])
{	//ofstream timestamp("timestamp"); //TEST
	cout << endl;
	if (argc != 3)	{ cout << "usage: ./DBFtrim InputFile OutputFile\n\n"; return 0; }
	if (!strcmp(argv[1], argv[2]))
	{	cout << "FATAL ERROR: input and output file paths match!\n";
		cout << "If you wanna be clever, try /home/dave/foo.dbf & /home/dave/./foo.dbf or something\n\n";
		return 0;
	}

	bool OK;
	DBF oDBF(argv[1], OK);	if (!OK) return 0;	// o is for original
	DBF tDBF(oDBF);		tDBF.CopyFields(oDBF);	// t is for trimmed

	// gather field info
	ifstream inDBF(argv[1], ios::in);
	inDBF.seekg(oDBF.HeaLen);
	for (unsigned int rNum = 0; rNum < oDBF.NumRec && inDBF.tellg() < oDBF.size; rNum++)
	{	inDBF.get(); // seek past leading ' ' or '*'
		for (unsigned int fNum = 0; fNum < oDBF.NumFields; fNum++)
		{	char *value = new char[oDBF.fArr[fNum].len+1]; value[oDBF.fArr[fNum].len] = 0;
			inDBF.read(value, oDBF.fArr[fNum].len);					// read in value from file
			oDBF.fArr[fNum].GetMax(tDBF, fNum, value);
		}
	}//*/

	//field info display
	cout << "FieldName\tType\tLength\tMax\tData\n";
	for (unsigned int i = 0; i < oDBF.NumFields; i++)
	{	cout << oDBF.fArr[i].name;	if (strlen(oDBF.fArr[i].name) < 8) cout << '\t'; // tab stop
		cout << '\t' << oDBF.fArr[i].type;
		cout << '\t' << int(oDBF.fArr[i].len);
		cout << '\t' << int(tDBF.fArr[i].len);
		cout << '\t' << oDBF.fArr[i].MaxVal << endl;
	}

	// write output file
	tDBF.SetRecLen();
	ofstream outDBF(argv[2]);
	inDBF.seekg(oDBF.HeaLen-1); // minus 1 in order to get 0Dh stored as the field terminator.
	// write header
	outDBF.write((char*)&tDBF, 32);
	outDBF.write((char*)tDBF.fArr, 32*tDBF.NumFields);	// write field descriptor array
	outDBF.put(inDBF.get());				// 0Dh stored as the field terminator.
	// write records
	for (unsigned int rNum = 0; rNum < oDBF.NumRec && inDBF.tellg() < oDBF.size; rNum++)
	{	outDBF.put(inDBF.get()); // ' ' or '*' precedes record contents
		for (unsigned int fNum = 0; fNum < oDBF.NumFields; fNum++)
		{	for (char c = 0; c < tDBF.fArr[fNum].len; c++) outDBF.put(inDBF.get());
			inDBF.seekg(oDBF.fArr[fNum].len-tDBF.fArr[fNum].len, ios::cur);
		}
	}
	if (!tDBF.borderline) outDBF.put(0x1A); // EOF marker */
}
