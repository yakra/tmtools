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
	char FinalChar;
	bool borderline;	// flags cases where actual input filesize = 1 less than calculated filesize
	char *name;

	DBF(char *filename, bool &OK) // read in header from file
	{	ifstream inDBF(filename);
		if (!inDBF) { OK = 0; cout << filename << " file not found!\n"; }
		else {	OK = 1; cout << filename << " opened.\n";
			name = filename;
			inDBF.read((char*)this, 0x20);
			NumFields = (HeaLen-0x21)/0x20;
			fArr = new field[NumFields];
			inDBF.read((char*)fArr, 0x20*NumFields);
			inDBF.seekg(0, ios::end);	size = inDBF.tellg();
			inDBF.seekg(-1, ios::cur);	inDBF.get(FinalChar);

			cout << "DBF Filesize:\t" << size << " (sanity check ";
			if	(size == NumRec*RecLen+HeaLen+1) { borderline = 0; cout << "pass)\n"; }
			else if	(size == NumRec*RecLen+HeaLen)	 { borderline = 1; cout << "borderline; may be missing terminal 0x1A)\n"; }
			else	{ OK = 0; cout << "fail: " << NumRec*RecLen+HeaLen+1 << " expected)\n"; }
			cout << "Number Records:\t0x" << hex << NumRec << '\t' << dec << NumRec << endl;
			cout << "Header Length:\t0x" << hex << HeaLen << '\t' << dec << HeaLen << endl;
			cout << "Record Length:\t0x" << hex << RecLen << '\t' << dec << RecLen << endl;
			cout << "First char:\t0x" << hex << int(ValidFile) << '\t' << dec << int(ValidFile) << endl;
			cout << "Final char:\t0x" << hex << int(FinalChar) << '\t' << dec << int(FinalChar) << endl;
			cout << NumFields << " fields.\n";
		     }
		inDBF.close();
	}

	void InitCopy(DBF& oDBF)
	{	fArr = new field[NumFields];
		for (unsigned int byte = 0; byte < 32*NumFields; byte++)
			*((char*)fArr+byte) = *((char*)oDBF.fArr+byte);
		for (unsigned int i = 0; i < NumFields; i++)
		  if (fArr[i].MaxVal || fArr[i].MinEx0)
		  {	fArr[i].MaxVal = 0; fArr[i].MinEx0 = 0;
			cout << "Warning: overwriting reserved nonzero bytes in field #" << i << ", " << fArr[i].name << endl;
		  }
	}

	void SetRecLen()
	{	RecLen = 1; // start at ' ' or '*'
		for (unsigned int fNum = 0; fNum < NumFields; fNum++)
			RecLen += fArr[fNum].len;
	}
};
