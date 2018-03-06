class DBF;

class field
{	public:
	char name[11];	// 11 B on disk; 10 B practical storage space. Final element is null terminator.
	char type;	// Field type in ASCII (B, C, D, F, G, L, M, or N).
	char reserved1[4];
	unsigned char len;
	unsigned char DecCount;
	unsigned char MinEx0;	// Minimum extraneous 0s. Not part of DBF spec; stored in reserved bytes for use by DBFtrim.
	unsigned char MaxIntD;	// Maximum Integer digits. Not part of DBF spec; stored in reserved bytes for use by DBFtrim.
	char reserved2[12];
	// No more variables! Must be able to write/read a whole array to/from disk.
	void GetMax(DBF&, unsigned int, char*);
};
