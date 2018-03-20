#include <cstdlib>
#include <thread>
#include "../lib/field.cpp"	// includes cstring, fstream, iostream via dbf.cpp
using namespace std;

inline void ProgBar(unsigned int numerator, unsigned int denominator)
{	cout << numerator << '/' << denominator << char(0x0D);
}

void RecWrite(DBF oDBF, DBF tDBF, char* outFN, unsigned int ThreadNum, unsigned int NumThreads)
{	ifstream inDBF(oDBF.name);
	fstream outDBF(outFN, fstream::in | fstream::out);
	inDBF.seekg(oDBF.HeaLen + ThreadNum*oDBF.RecLen);
	outDBF.seekp(tDBF.HeaLen + ThreadNum*tDBF.RecLen);

	for (unsigned int rNum = ThreadNum; rNum < oDBF.NumRec && inDBF.tellg() < oDBF.size; rNum +=NumThreads)
	{	outDBF.put(inDBF.get()); // ' ' or '*' precedes record contents
		for (unsigned int fNum = 0; fNum < oDBF.NumFields; fNum++)
		{	char* fVal = new char[oDBF.fArr[fNum].len+1]; fVal[oDBF.fArr[fNum].len] = 0;
			inDBF.read(fVal, oDBF.fArr[fNum].len);
			unsigned char vlen = strlen(fVal);
			unsigned char fI; // field value index
			if (oDBF.fArr[fNum].type == 'N' || oDBF.fArr[fNum].type == 'F') fTrim(fVal, vlen);
			switch (oDBF.fArr[fNum].subtype(fVal))
			{   case 2: // decimal
				if (vlen > strchr(fVal, '.') - fVal + 1 + tDBF.fArr[fNum].DecCount)
					vlen = strchr(fVal, '.') - fVal + 1 + tDBF.fArr[fNum].DecCount;
				if (!tDBF.fArr[fNum].DecCount) vlen--; // decimal point itself is extraneous
			    case 1: // integer
			    case 3: // scientific notation float
				for (fI = vlen; fI < tDBF.fArr[fNum].len; fI++) outDBF.put(' ');
				outDBF.write(fVal, vlen);
				break;
			    case 'C':
				fTrim(fVal, vlen);
				outDBF << fVal;
				for (fI = vlen; fI < tDBF.fArr[fNum].len; fI++) outDBF.put(' ');
				break;
			    default:
				outDBF.write(fVal, tDBF.fArr[fNum].len);
			}
			delete[] fVal;
		}

		if (ThreadNum == (oDBF.NumRec+NumThreads-1)%NumThreads)
			ProgBar(rNum+1, oDBF.NumRec);
		inDBF.seekg((NumThreads-1)*oDBF.RecLen, ios::cur);
		outDBF.seekp((NumThreads-1)*tDBF.RecLen, ios::cur);
	}
}

int main(int argc, char *argv[])
{	int NumThreads = 1;
	//ofstream timestamp("Timestamp-Begin"); timestamp.close(); //TEST
	cout << endl;
	if (argc < 3)	{ cout << "usage: DBFtrim InputFile OutputFile (NumThreads)\n\n"; return 0; }
	if (argc > 3)	NumThreads = strtol(argv[3], 0, 10);
	if (NumThreads < 1) NumThreads = 1;
	ifstream filetest(argv[2]);
	if (filetest)
	{	cout << "OutputFile " << argv[2] << " already exists; overwriting not permitted. Aborting.\n\n";
		return 0;
	} filetest.close();

	bool OK;
	DBF oDBF(argv[1], OK);	if (!OK) return 0;	// o is for original
	DBF tDBF(oDBF);		tDBF.InitCopy(oDBF);	// t is for trimmed

	// gather field info
	ifstream inDBF(oDBF.name);
	inDBF.seekg(oDBF.HeaLen);
	cout << "Scanning DBF file...\n";
	for (unsigned int rNum = 0; rNum < oDBF.NumRec && inDBF.tellg() < oDBF.size; rNum++)
	{	inDBF.get(); // seek past leading ' ' or '*'
		for (unsigned int fNum = 0; fNum < oDBF.NumFields; fNum++)
		  if (oDBF.fArr[fNum].type == 'C' || oDBF.fArr[fNum].type == 'F' || oDBF.fArr[fNum].type == 'N')
		  {	char *fVal = new char[oDBF.fArr[fNum].len+1]; fVal[oDBF.fArr[fNum].len] = 0;
			inDBF.read(fVal, oDBF.fArr[fNum].len); // read in value from file
			oDBF.fArr[fNum].GetMax(tDBF, fNum, fVal);
		  }
		  else	inDBF.seekg(oDBF.fArr[fNum].len, ios::cur);
		ProgBar(rNum+1, oDBF.NumRec);
	}//*/
	//timestamp.open("Timestamp-InfoGathered"); timestamp.close(); //TEST
	
	//field info display
	cout << "\nFieldName\tType\tLength\tMax\tData\n";
	for (unsigned int i = 0; i < oDBF.NumFields; i++)
	{	cout << oDBF.fArr[i].name << '\t';	if (strlen(oDBF.fArr[i].name) < 8) cout << '\t'; // tab stop
		cout << oDBF.fArr[i].type << '\t';
		cout << int(oDBF.fArr[i].len) << '\t';
		cout << int(tDBF.fArr[i].len) << '\t';
		unsigned mvSize = oDBF.fArr[i].x0term;
		if (!mvSize) mvSize = strlen(tDBF.MaxVal[i]);
		for (unsigned char n = mvSize; n < tDBF.fArr[i].len; n++) cout << '#';
		cout.write(tDBF.MaxVal[i], mvSize);
		    if (oDBF.fArr[i].x0term) cout << " <- " << tDBF.MaxVal[i];
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
	inDBF.close(); outDBF.close();
	// write records
	thread **thr = new thread*[NumThreads];
	for (unsigned int t = 0; t < NumThreads; t++)
		thr[t] = new thread(RecWrite, oDBF, tDBF, argv[2], t, NumThreads);
	for (unsigned int t = 0; t < NumThreads; t++)
		thr[t]->join();

	outDBF.open(argv[2], ios::app);
	if (!tDBF.borderline) outDBF.put(0x1A); // EOF marker */
	outDBF.close();
	cout << endl;
}
