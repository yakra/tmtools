#include "dbf.cpp"

void fTrim(char* fVal, unsigned char &vlen)
{	unsigned char pad;
	// trim whitespace RIGHT
	for (int i = vlen-1; fVal[i] <= ' ' && fVal[i] > 0 && i >= 0; i--) { fVal[i] = 0; vlen--; }
	// trim whitespace LEFT
	for (pad = 0; fVal[pad] <= ' ' && pad < vlen; pad++);
	vlen -= pad;
	for (unsigned char i = 0; i <= vlen; i++) fVal[i] = fVal[pad+i];
}

void field::GetMax(DBF& tDBF, unsigned int fNum, char* fVal)
{	unsigned char vlen = strlen(fVal);
	unsigned char IntD;
	fTrim(fVal, vlen);
	// trim extraneous trailing zeros
	switch (subtype(fVal))
	{   case 1: // integer
		if (MaxIntD < vlen) MaxIntD = vlen;
		break;
	    case 2: // decimal
		IntD = strchr(fVal, '.') - fVal;
		if (MaxIntD < IntD) MaxIntD = IntD;
		unsigned char fI;
		for (fI = vlen-1; fVal[fI] == '0'; fI--);
		if (tDBF.fArr[fNum].DecCount < fI-IntD) tDBF.fArr[fNum].DecCount = fI-IntD;
		break;
	    case 3: // scientific notation float
		tDBF.fArr[fNum].DecCount = DecCount;
	}

	// compare & set field length
	switch (subtype(fVal))
	{   case 2: // decimal
		vlen = MaxIntD + (tDBF.fArr[fNum].DecCount > 0) + tDBF.fArr[fNum].DecCount; // reuse lame-duck variable rather than declare a new one
		if (vlen > len) vlen = len;
		if (tDBF.fArr[fNum].len < vlen)
		{	tDBF.fArr[fNum].len = vlen;
			delete[] tDBF.MaxVal[fNum];
			tDBF.MaxVal[fNum] = fVal;
			x0term = IntD + (tDBF.fArr[fNum].DecCount > 0) + tDBF.fArr[fNum].DecCount;
			if (x0term == len) x0term = 0;
		}
		else delete[] fVal;
		break;
	    case 1: // integer
	    case 3: // scientific notation float
	    case 'C':
		if (tDBF.fArr[fNum].len < vlen)
		{	tDBF.fArr[fNum].len = vlen;
			delete[] tDBF.MaxVal[fNum];
			tDBF.MaxVal[fNum] = fVal;
			x0term = 0;
		}
		else delete[] fVal;
	}
}

unsigned char field::subtype(char* fVal)
{	if (type == 'F' || type == 'N')
	{	if (strchr(fVal, 'e') || strchr(fVal, 'E'))	return 3; // scientific notation float
		else if (strchr(fVal, '.'))			return 2; // decimal
		else						return 1; // integer
	}
	else return type;
}
