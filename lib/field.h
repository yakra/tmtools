class DBF;

class field
{	public:
	char name[11];	// 11 B on disk; 10 B practical storage space. Final element is null terminator.
	char type;	// Field type in ASCII (B, C, D, F, G, L, M, or N).
	unsigned int reserved1;
	unsigned char len;
	unsigned char DecCount;
	unsigned char MinEx0;
	char reserved2[13];
	// No more variables! Must be able to write/read a whole array to/from disk.
	void GetMax(DBF&, unsigned int, char*);
};
