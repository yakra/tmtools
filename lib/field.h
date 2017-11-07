class field
{	public:
	char name[11];	// 11 B on disk; 10 B practical storage space. Final element is null terminator.
	char type;	// Field type in ASCII (B, C, D, F, G, L, M, or N).
	unsigned int DataAddx;
	unsigned char len;
	unsigned char DecCount;
	char padding[5];
	unsigned char MinEx0;
	char *MaxVal; // only works with 64-bit memory addressing, E.G. x86-64
	// No more variables! Must be able to write/read a whole array to/from disk.
	void GetMax(DBF&, unsigned int, char*);
}; //WARNING: functionality is predicated on sizeof(field) being 32 bytes. Meaning, 64-bit memory addressing; see comment for *MaxVal above
