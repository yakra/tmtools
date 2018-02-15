#include "../lib/dbf.cpp"	// includes cstring, fstream, iostream
using namespace std;

inline void ProgBar(unsigned int numerator, unsigned int denominator)
{	cout << numerator << '/' << denominator << char(0x0D);
}

class entry
{	public:
	char *value;
	entry *prev, *next;

	//init alpha & omega
	entry(unsigned char FieldLength, char letter)
	{	value = new char[FieldLength+1];
		for (unsigned char index = 0; index < FieldLength; index++) value[index] = letter;
		value[FieldLength] = 0;
	}
	//insertions
	entry(entry *cursor, char *KFieldVal)
	{	prev = cursor->prev;
		prev->next = this;
		next = cursor;
		next->prev = this;
		value = KFieldVal;
	}
};

void mine(DBF& rDBF)
{	cout << "Mining...\n";

	//init alpha & omega
	char *KFieldVal;
	entry alpha(rDBF.KeyLen, 0), omega(rDBF.KeyLen, 255), *cursor;
	alpha.prev = 0; alpha.next = &omega;
	omega.prev = &alpha; omega.next = 0;

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
		cursor = &alpha;
		while (strcmp(KFieldVal, cursor->value) > 0) cursor = cursor->next;
		if (strcmp(KFieldVal, cursor->value) < 0)
			new entry(cursor, KFieldVal);	// insert new entry into list before cursor
		else delete KFieldVal;			// strcmp == 0; value already on list

		ProgBar(RecNum+1, rDBF.NumRec); //zeroth vs first
	}
	fDBF.close();
	for (cursor = alpha.next; cursor->next; cursor = cursor->next) file << cursor->value << endl;
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
