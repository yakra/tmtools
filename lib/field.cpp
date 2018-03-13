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
	unsigned char IntD = 0;
	fTrim(fVal, vlen);
	// trim extraneous trailing zeros
	if (type == 'F' || type == 'N')
	  if (strchr(fVal, '.') && !strchr(fVal, 'E') && !strchr(fVal, 'e'))
	  {	unsigned char pad = 0;
		IntD = strchr(fVal, '.') - fVal;
		if (IntD > MaxIntD) MaxIntD = IntD;
		for (unsigned char i = vlen-1; fVal[i] == '0'; i--) pad++;
		if (pad < MinEx0)
		{	MinEx0 = pad;
			if (MinEx0 >= DecCount)
			{	MinEx0++; // decimal point itself is extraneous
				tDBF.fArr[fNum].DecCount = 0; // prevent wraparound when E.G. "0.0000000000" has a DecCount of 0
			}
			else	tDBF.fArr[fNum].DecCount = DecCount-MinEx0;
		}
	  }
	  else	if (vlen)
		{	MinEx0 = 0;
			tDBF.fArr[fNum].DecCount = DecCount;
		}
	// compare
	if (vlen-IntD+MaxIntD > tDBF.fArr[fNum].len+MinEx0)
	{	tDBF.fArr[fNum].len = vlen-IntD-MinEx0+MaxIntD;
		/*FIXME*/ if (tDBF.fArr[fNum].len > len) tDBF.fArr[fNum].len = len;	// temporary fix to keep left-justified numbers, a la TX, working.
											// eventual implementation of trimming both left AND right should remove need for this.
		delete[] tDBF.MaxVal[fNum];
		tDBF.MaxVal[fNum] = fVal;
		tDBF.MaxVal[fNum][vlen-MinEx0] = 0; // new terminator for when MinEx0
	}
	else delete[] fVal;
}
