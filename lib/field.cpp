#include "dbf.cpp"

void field::GetMax(DBF& tDBF, unsigned int fNum, char* fVal)
{	unsigned char pad;
	char* NewVal;
	switch (type)
	{   case 'C':
		// init
		if (!tDBF.MaxVal[fNum])
		{	tDBF.fArr[fNum].len = 0;
			tDBF.MaxVal[fNum] = new char[1]; tDBF.MaxVal[fNum][0] = 0;
		}
		// trim whitespace
		while (fVal[strlen(fVal)-1] <= ' ' && fVal[strlen(fVal)-1] > 0) fVal[strlen(fVal)-1] = 0;
		// compare
		if (strlen(fVal) > tDBF.fArr[fNum].len)
		{	tDBF.fArr[fNum].len = strlen(fVal);
			delete[] tDBF.MaxVal[fNum];
			tDBF.MaxVal[fNum] = fVal;
		}
		else delete[] fVal;
		return;

	    case 'F':
		// init
		if (!tDBF.MaxVal[fNum])
		{	tDBF.fArr[fNum].len = 0;
			tDBF.MaxVal[fNum] = new char[1]; tDBF.MaxVal[fNum][0] = 0;
		}
		// trim whitespace
		for (pad = 0; (fVal[pad] <= ' ') && pad < len; pad++);
		NewVal = new char[strlen(fVal+pad)+1];
		strcpy(NewVal, fVal+pad);
		delete[] fVal;
		fVal = NewVal;
		// compare
		if (strlen(fVal) > tDBF.fArr[fNum].len)
		{	tDBF.fArr[fNum].len = strlen(fVal);
			delete[] tDBF.MaxVal[fNum];
			tDBF.MaxVal[fNum] = fVal;
		}
		else delete[] fVal;
		return;

	    case 'N':
		// init
		if (!tDBF.MaxVal[fNum])
		{	tDBF.fArr[fNum].len = 0;
			tDBF.MaxVal[fNum] = new char[1]; tDBF.MaxVal[fNum][0] = 0;
			if (strchr(fVal, '.')) MinEx0 = 255;
		}
		// trim leading whitespace
		for (pad = 0; (fVal[pad] <= ' ') && pad < len; pad++);
		NewVal = new char[strlen(fVal+pad)+1];
		strcpy(NewVal, fVal+pad);
		delete[] fVal;
		fVal = NewVal;
		// trim extraneous trailing zeros
		if (strchr(fVal, '.'))
		{	pad = 0;
			for (unsigned char i = strlen(fVal)-1; fVal[i] == '0'; i--) pad++;
			if (pad < MinEx0)
			{	MinEx0 = pad;
				tDBF.fArr[fNum].DecCount = DecCount-MinEx0;
				if (MinEx0 == DecCount) MinEx0++; // decimal point itself is extraneous
			}
		}
		// compare
		if (strlen(fVal) > tDBF.fArr[fNum].len+MinEx0)
		{	tDBF.fArr[fNum].len = strlen(fVal)-MinEx0;
			delete[] tDBF.MaxVal[fNum];
			tDBF.MaxVal[fNum] = fVal;
			tDBF.MaxVal[fNum][tDBF.fArr[fNum].len] = 0;
		}
		else delete[] fVal;
		return;

	    default:
		delete[] fVal;
		if (!tDBF.MaxVal[fNum]) // ELSE case should only ever be "  <Type ? fields unsupported>", where '?' is field type
		{	tDBF.MaxVal[fNum] = new char[30];
			strcpy(tDBF.MaxVal[fNum], "  <Type ? fields unsupported>");
			tDBF.MaxVal[fNum][8] = type;
		}
	}
}
