#include <cstring>
#include <fstream>
#include <iostream>
#include "field.h"

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
	unsigned int size, NumFields, KeyOffset;
	unsigned char KeyLen;
	char FinalChar;
	char **MaxVal;
	bool borderline;	// flags cases where actual input filesize = 1 less than calculated filesize
	const char *name;

	DBF(const char *filename, bool &OK) // read in header from file
	{	std::ifstream inDBF(filename);
		if (!inDBF) { OK = 0; std::cout << filename << " file not found!\n"; }
		else {	OK = 1; std::cout << filename << " opened.\n";
			name = filename;
			inDBF.read((char*)this, 0x20);
			NumFields = (HeaLen-0x21)/0x20;
			fArr = new field[NumFields];
			inDBF.read((char*)fArr, 0x20*NumFields);
			inDBF.seekg(0, std::ios::end);	size = inDBF.tellg();
			inDBF.seekg(-1, std::ios::cur);	inDBF.get(FinalChar);

			std::cout << "DBF Filesize:\t" << size << " (sanity check ";
			if	(size == NumRec*RecLen+HeaLen+1) { borderline = 0; std::cout << "pass)\n"; }
			else if	(size == NumRec*RecLen+HeaLen)	 { borderline = 1; std::cout << "borderline; may be missing terminal 0x1A)\n"; }
			else	{ OK = 0; std::cout << "fail: filesize of " << NumRec*RecLen+HeaLen+1 << " or " << NumRec*RecLen+HeaLen << " expected)\n"; }
			std::cout << "Number Records:\t0x" << std::hex << NumRec << '\t' << std::dec << NumRec << '\n';
			std::cout << "Header Length:\t0x" << std::hex << HeaLen << '\t' << std::dec << HeaLen << '\n';
			std::cout << "Record Length:\t0x" << std::hex << RecLen << '\t' << std::dec << RecLen << '\n';
			std::cout << "First char:\t0x" << std::hex << int(ValidFile) << '\t' << std::dec << int(ValidFile) << '\n';
			std::cout << "Final char:\t0x" << std::hex << int(FinalChar) << '\t' << std::dec << int(FinalChar) << '\n';
			std::cout << NumFields << " fields.\n";
		     }
		inDBF.close();
	}

	void InitCopy(DBF& oDBF)
	{	fArr = new field[NumFields];
		MaxVal = new char*[NumFields];
		memcpy(fArr, oDBF.fArr, 32*NumFields);
		for (unsigned int i = 0; i < NumFields; i++)
		{	fArr[i].DecCount = 0;     // Just in case.
			oDBF.fArr[i].x0term = 0;  // Can potentially read in
			oDBF.fArr[i].MaxIntD = 0; // a nonzero byte from file.
			if   (fArr[i].type == 'C' || fArr[i].type == 'F' || fArr[i].type == 'N')
			     {	MaxVal[i] = new char[1]; MaxVal[i][0] = 0;
				fArr[i].len = 0;
			     }
			else {	MaxVal[i] = new char[30];
				strcpy(MaxVal[i], "  <Type ? fields unsupported>");
				MaxVal[i][8] = fArr[i].type;
			     }
		}
	}

	void SetRecLen()
	{	RecLen = 1; // start at ' ' or '*'
		for (unsigned int fNum = 0; fNum < NumFields; fNum++)
			RecLen += fArr[fNum].len;
	}

	bool SetKey(const char *KeyField)
	{	unsigned int fNum = 0;
		KeyOffset = 0;
		KeyLen = 0;

		//find desired field descriptor
		while (fNum < NumFields)
		{	KeyLen = fArr[fNum].len;
			if (!strcmp(KeyField, fArr[fNum].name)) break;
			KeyOffset += KeyLen;
			fNum++;
		}//*/

		//info display
		if (strcmp(KeyField, fArr[fNum].name)) //if KeyField not found
		{	std::cout << "Field " << KeyField << " not found!\n\n";
			return 0;
		}
		std::cout << KeyField << " is field # " << fNum+1 << ", Offset " << KeyOffset << " in-record, length = " << int(KeyLen) << "\n\n";
		return 1;
	}//*/
};
