#include <ctime>
#include "../lib/tmsystem.cpp"	// includes deque, string, vector
#include "../lib/waypoint.cpp"	// includes cmath, cstring, deque, fstream, iostream, list, string, vector
using namespace std;

class ListEntry
{	public:
	string Region, Name, pl1, pl2;

	ListEntry(string R, string N, string p1, string p2)
	{	Region = R;
		Name = N;
		pl1 = p1;
		pl2 = p2;
	}
};

class envV
{	public:
	string List, Colors, Output, Repo, Width, Height, UnStroke, ClStroke;
	vector<string> N_Colors, UnColors, ClColors;
	vector<string> ExclRg, InclCont, InclCtry, InclRg, InclSysCode;
	deque<tmsystem> SysDeq, InclSysObj;
	list<ListEntry> TravList;
	unsigned short MaxTier;
	short MinLevel;
	bool boundaries;
	bool ReadSysCSV(bool);

	bool set(int argc, char *argv[])
	{	unsigned short envInfo = 0;
		MaxTier = 255;
		MinLevel = 1;
		boundaries = 0;

		// INI file options:
		string iniField;
		ifstream INI("canvas.ini");
		if (!INI) cout << "canvas.ini file not found. All required options must be specified by commandline\n";
		else {	INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Output" && !INI.eof())	INI >> iniField;
			  INI >> Output;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Repo" && !INI.eof())	INI >> iniField;
			  INI >> Repo;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "List" && !INI.eof())	INI >> iniField;
			  INI >> List;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Colors" && !INI.eof())	INI >> iniField;
			  INI >> Colors;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Width" && !INI.eof())	INI >> iniField;
			  INI >> Width;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Height" && !INI.eof())	INI >> iniField;
			  INI >> Height;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Stroke" && !INI.eof())	INI >> iniField;
			  INI >> UnStroke >> ClStroke;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "MaxTier" && !INI.eof())	INI >> iniField;
			  INI >> MaxTier;
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Boundaries" && !INI.eof())	INI >> iniField;
			  if (iniField == "Boundaries") boundaries = 1;

			INI.clear(); INI.seekg(0); iniField.clear();
			while (iniField != "MinLevel" && !INI.eof())	INI >> iniField;
			if (INI >> iniField)
			{	if (iniField == "active")  MinLevel = 4;
				if (iniField == "preview") MinLevel = 3;
				if (iniField == "devel")   MinLevel = 2;
			}

			INI.clear(); INI.seekg(0); iniField.clear();
			while (INI >> iniField)
			{	if (iniField == "Input")
				{	INI >> iniField;
					InclSysObj.emplace_back(iniField);
				}
			}

			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "ExcludeRg" && !INI.eof())	INI >> iniField;
			  string eStr; INI >> eStr;
			   char *eArr = new char[eStr.size()+1];
			    strcpy(eArr, eStr.data());
			     for (char *e = strtok(eArr, ","); e; e = strtok(0, ",")) ExclRg.push_back(e);
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Continent" && !INI.eof())	INI >> iniField;
			  string CStr; INI >> CStr;
			   char *CArr = new char[CStr.size()+1];
			    strcpy(CArr, CStr.data());
			     for (char *C = strtok(CArr, ","); C; C = strtok(0, ",")) InclCont.push_back(C);
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Country" && !INI.eof())	INI >> iniField;
			  string cStr; INI >> cStr;
			   char *cArr = new char[cStr.size()+1];
			    strcpy(cArr, cStr.data());
			     for (char *c = strtok(cArr, ","); c; c = strtok(0, ",")) InclCtry.push_back(c);
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "Region" && !INI.eof())	INI >> iniField;
			  string rStr; INI >> rStr;
			   char *rArr = new char[rStr.size()+1];
			    strcpy(rArr, rStr.data());
			     for (char *rg = strtok(rArr, ","); rg; rg = strtok(0, ",")) InclRg.push_back(rg);
			INI.clear(); INI.seekg(0); iniField.clear();
			 while (iniField != "System" && !INI.eof())	INI >> iniField;
			  string sStr; INI >> sStr;
			   char *sArr = new char[sStr.size()+1];
			    strcpy(sArr, sStr.data());
			     for (char *sys = strtok(sArr, ","); sys; sys = strtok(0, ",")) InclSysCode.emplace_back(sys);
		     }

		// commandline options:
		for (int a = 1; a < argc; a++)
		{	if (!strcmp(argv[a], "-b") || !strcmp(argv[a], "--Boundaries")) boundaries = 1;
			else if (!strcmp(argv[a], "-C") || !strcmp(argv[a], "--Colors"))
			{ if (a+1 < argc)
			  {	Colors = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-c") || !strcmp(argv[a], "--Color"))
			{ if (a+3 < argc)
			{	N_Colors.push_back(argv[a+1]);
				UnColors.push_back(argv[a+2]);
				ClColors.push_back(argv[a+3]);
				a += 3;
			} }
			else if (!strcmp(argv[a], "--Continent"))
			{ if (a+1 < argc)
			  {	InclCont.clear();
				for (char *C = strtok(argv[a+1], ","); C; C = strtok(0, ",")) InclCont.push_back(C);
				a++;
			} }
			else if (!strcmp(argv[a], "--Country"))
			{ if (a+1 < argc)
			  {	InclCtry.clear();
				for (char *c = strtok(argv[a+1], ","); c; c = strtok(0, ",")) InclCtry.push_back(c);
				a++;
			} }
			else if (!strcmp(argv[a], "--ExcludeRg"))
			{ if (a+1 < argc)
			  {	ExclRg.clear();
				for (char *e = strtok(argv[a+1], ","); e; e = strtok(0, ",")) ExclRg.push_back(e);
				a++;
			} }
			else if (!strcmp(argv[a], "-h") || !strcmp(argv[a], "--Height"))
			{ if (a+1 < argc)
			  {	Height = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-i") || !strcmp(argv[a], "--Input"))
			{ if (a+1 < argc)
			  {	InclSysObj.emplace_back(argv[a+1]);
				a++;
			} }
			else if (!strcmp(argv[a], "-l") || !strcmp(argv[a], "--List"))
			{ if (a+1 < argc)
			  {	List = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "--MaxTier"))
			{ if (a+1 < argc)
			  {	try	{ MaxTier = stoi(argv[a+1]); }
				catch	( invalid_argument x )
					{ cout << '\"' << argv[a+1] << "\" cannot be converted to integer; ignoring.\n"; }
				a++;
			} }
			else if (!strcmp(argv[a], "--MinLevel"))
			{ if (a+1 < argc)
			  {	if (!strcmp(argv[a+1], "active"))  MinLevel = 4;
				if (!strcmp(argv[a+1], "preview")) MinLevel = 3;
				if (!strcmp(argv[a+1], "devel"))   MinLevel = 2;
				a++;
			} }
			else if (!strcmp(argv[a], "-o") || !strcmp(argv[a], "--Output"))
			{ if (a+1 < argc)
			  {	Output = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-rg") || !strcmp(argv[a], "--Region"))
			{ if (a+1 < argc)
			  {	InclRg.clear();
				for (char *rg = strtok(argv[a+1], ","); rg; rg = strtok(0, ",")) InclRg.push_back(rg);
				a++;
			} }
			else if (!strcmp(argv[a], "-r") || !strcmp(argv[a], "--Repo"))
			{ if (a+1 < argc)
			  {	Repo = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-s") || !strcmp(argv[a], "--Stroke"))
			{ if (a+2 < argc)
			  {	UnStroke = argv[a+1];
				ClStroke = argv[a+2];
				a += 2;
			} }
			else if (!strcmp(argv[a], "-sys") || !strcmp(argv[a], "--System"))
			{ if (a+1 < argc)
			  {	for (char *sys = strtok(argv[a+1], ","); sys; sys = strtok(0, ",")) InclSysCode.emplace_back(sys);
				a++;
			} }
			else if (!strcmp(argv[a], "-w") || !strcmp(argv[a], "--Width"))
			{ if (a+1 < argc)
			  {	Width = argv[a+1];
				a++;
			} }
			else if (!strcmp(argv[a], "-?") || !strcmp(argv[a], "--help") || !strcmp(argv[a], "--Help"))
			{	cout << "Options:\n";
				cout << "Mandatory arguments to long options are mandatory for short options too.\n";
				cout << "  -b,   --Boundaries                 Show boundaries. No arguments required.\n";
				cout << "  -C,   --Colors <ColorFile>         Color definitions file.\n";
				cout << "  -c,   --Color <Name> <Base> <Clin> Single color definition.\n";
				cout << "        --Continent <Code,Code,Code> Comma-separated continents to include.\n";
				cout << "        --Country <Code,Code,Code>   Comma-separated countries to include.\n";
				cout << "        --ExcludeRg <Code,Code,Code> Comma-separated Regions to exclude.\n";
				cout << "  -h,   --Height <Height>            Canvas height.\n";
				cout << "  -i,   --Input <InputFile>          CSV file listing routes to be plotted.\n";
				cout << "  -l,   --List <ListFile>            Filename of .list file to process.\n";
				cout << "        --MaxTier <Tier>             Maximum Tier to include.\n";
				cout << "        --MinLevel <Level>           Minimum system level to include.\n";
				cout << "                                     Can be active, preview, or devel.\n";
				cout << "  -o,   --Output <OutputFile>        Filename of output HTML file.\n";
				cout << "  -r,   --Repo <Repo>                Path of HighwayData repository.\n";
				cout << "                                     Trailing slash required.\n";
				cout << "  -rg,  --Region <Code,Code,Code>    Comma-separated regions to include.\n";
				cout << "  -s,   --Stroke <Base> <Clinched>   Stroke width of base & clinched segments.\n";
				cout << "                                     Both arguments are required.\n";
				cout << "  -sys, --System <Code,Code,Code>    Comma-separated systems to include.\n";
				cout << "  -w,   --Width <Width>              Canvas width.\n";
				cout << "  -?,   --help, --Help               Display this help and exit.\n\n";

				cout << "INI file:\n";
				cout << "Default options may also be specified in canvas.ini, and will be overridden by\n";
				cout << "commandline switches. Format is one option per line, followed by arguments as\n";
				cout << "listed above. Options are the same as the long options listed above, without the\n";
				cout << "leading dashes, and are case sensitive.\n";
				cout << "Only single color definitions (--Color) are unsupported in canvas.ini.\n";
				cout << "These should be defined in their own separate colors INI file instead.\n";
				return 0;
			}
		}

		// colors:
		ifstream colINI(Colors);
		if (!colINI) cout << Colors << " file not found. All colors not specified by commandline will be colored gray.\n";
		string N_Color, UnColor, ClColor;
		while (colINI >> N_Color >> UnColor >> ClColor)
		{	N_Colors.push_back(N_Color);
			UnColors.push_back(UnColor);
			ClColors.push_back(ClColor);
		}
		if (N_Colors.size() != UnColors.size())
		   { cout << "Oh dear. N_Colors.size() != UnColors.size() in envV::set(). Terminating.\n"; return 0; }
		if (UnColors.size() != ClColors.size())
		   { cout << "Oh dear. UnColors.size() != ClColors.size() in envV::set(). Terminating.\n"; return 0; }
		if (ClColors.size() != N_Colors.size())
		   { cout << "Oh dear. ClColors.size() != N_Colors.size() in envV::set(). Terminating.\n"; return 0; }
		for (unsigned int i = 0; i < InclSysObj.size(); i++) InclSysObj[i].SetColors(N_Colors, UnColors, ClColors);

		if (!List.empty()) SlurpList();
		if (!InclCtry.empty() || !InclCont.empty()) countries_continents();

		ReadSysCSV(InclSysCode.empty() && InclSysObj.empty());
		if (boundaries)
		{	// boundaries	    //System,      CountryCode,  Name,        Color,      Tier, Level;
			SysDeq.emplace_back("b_country", "boundaries", "b_country", "b_country", 255, "boundaries", Repo+"boundaries/b_country.csv");
			SysDeq.back().SetColors(N_Colors, UnColors, ClColors);
			InclSysObj.push_back(SysDeq.back());
			SysDeq.emplace_back("b_subdiv", "boundaries", "b_subdiv", "b_subdiv", 255, "boundaries", Repo+"boundaries/b_subdiv.csv");
			SysDeq.back().SetColors(N_Colors, UnColors, ClColors);
			InclSysObj.push_back(SysDeq.back());
			SysDeq.emplace_back("b_water", "boundaries", "b_water", "b_water", 255, "boundaries", Repo+"boundaries/b_water.csv");
			SysDeq.back().SetColors(N_Colors, UnColors, ClColors);
			InclSysObj.push_back(SysDeq.back());
			if (!InclRg.empty()) InclRg.push_back("_boundaries");
		}

		if (UnStroke.empty())	{ envInfo = 1; cout << "Base stroke thickness unspecified; defaulting to 1.\n"; UnStroke = "1"; }
		if (ClStroke.empty())	{ envInfo = 1; cout << "Clinched stroke thickness unspecified; defaulting to 2.5.\n"; ClStroke = "2.5"; }
		if (Width.empty())	{ envInfo = 1; cout << "Width unspecified; defaulting to 700.\n"; Width = "700"; }
		if (Height.empty())	{ envInfo = 1; cout << "Height unspecified; defaulting to 700.\n"; Height = "700"; }
		if (List.empty())	{ envInfo = 1; cout << ".list file unspecified. Plotting base route traces only.\n"; }
		if (InclSysObj.empty())	{ envInfo = 2; cout << "Input CSV(s) unspecified.\n"; }
		if (Output.empty())	{ envInfo += 1+(envInfo==0); cout << "Output filename unspecified.\n"; }
		if (Repo.empty())	{ envInfo += 1+(envInfo==0); cout << "Repository location unspecified.\n"; }

		if (envInfo)
		{	if (envInfo > 1) cout << envInfo-1 << " fatal error(s).\n";
			cout << "For more info, use --help commandline option.\n";
			if (envInfo > 1) return 0;
		}
		return 1;
	}

	void countries_continents()
	{	ifstream regions(Repo+"regions.csv");
		if (!regions) { cout << Repo+"regions.csv not found!\n"; return; }
		string CSVline;
		getline(regions, CSVline); // skip header row
		while (getline(regions, CSVline))
		{	string code, name, country, continent;
			unsigned int i = 0;
			while (i < CSVline.size() && CSVline[i] != ';') { code.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { name.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { country.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { continent.push_back(CSVline[i]); i++; } i++;
			if ( (StrInVec(country, InclCtry) || InclCtry.empty()) && (StrInVec(continent, InclCont) || InclCont.empty()) && !StrInVec(code, ExclRg) )
			{	InclRg.push_back(code);
				cout << continent << '\t' << country << '\t' << code << '\t' << name << '\n';
			}
		}
	}

	void SlurpList()
	{	ifstream list(List.data());
		if (!list) { cout << "List file \"" << List << "\" not found. Plotting base route traces only.\n"; }
	
		string line;
		while (getline(list, line))
		{	while (line.back() == '\r' || line.back() == ' ' || line.back() == '\t') line.erase(line.end()-1);
			try {	char *LineArr = new char[line.size()+1];
				strcpy(LineArr, line.data());
				char *Region = strtok(LineArr, " \t");
					if (!Region || Region[0] == '#')		throw 'R';
				char *Name = strtok(0, " \t");		if (!Name)	throw 'N';
				char *pl1 = strtok(0, " \t");		if (!pl1)	throw '1';
				char *pl2 = strtok(0, " \t");		if (!pl2)	throw '2';
				char *field5 = strtok(0, " \t");
					if (field5 && field5[0] != '#')			throw '3';
				while (pl1[0] == '+' || pl1[0] == '*') pl1++;
				while (pl2[0] == '+' || pl2[0] == '*') pl2++;
				TravList.emplace_back(caps(Region), caps(Name), pl1, pl2);
			    }
			catch (char err) { if (err != 'R') cout << "Incorrect format .list line: " << line << '\n'; }
		}
		list.close();
	}
};

bool envV::ReadSysCSV(bool PushToDeque)
{	string SysCSV = Repo+"systems.csv";
	ifstream CSV(SysCSV);
	if (!CSV)
	{	cout << SysCSV << " file not found!" << endl;
		return 0;
	}
	CSV.seekg(0, ios::end); unsigned int EoF = CSV.tellg(); CSV.seekg(0);
	while (CSV.get() != '\n' && CSV.tellg() < EoF); //skip header row

	while (CSV.tellg() < EoF)
	{	string System, CountryCode, Name, Color, TierS, Level;
		string CSVline; // read individual line
		for (char charlie = 0; charlie != '\n' && CSV.tellg() < EoF; CSVline.push_back(charlie)) CSV.get(charlie);

		if (CSVline[0] != '#') // if not a comment
		{	while (CSVline.back() == 0x0A || CSVline.back() == 0x0D)	// either DOS or UNIX...
				CSVline.erase(CSVline.end()-1);				// strip out terminal '\n'
			// parse CSV line
			unsigned int i = 0;
			while (i < CSVline.size() && CSVline[i] != ';') { System.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { CountryCode.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { Name.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { Color.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { TierS.push_back(CSVline[i]); i++; } i++;
			while (i < CSVline.size() && CSVline[i] != ';') { Level.push_back(CSVline[i]); i++; } i++;
			try {	unsigned short Tier = stoi(TierS);
				SysDeq.emplace_back(System, CountryCode, Name, Color, Tier, Level, Repo+"hwy_data/_systems/"+System+".csv");
				SysDeq.back().SetColors(N_Colors, UnColors, ClColors);
				if (PushToDeque && SysDeq.back().LevNum >= MinLevel && Tier <= MaxTier || StrInVec(System, InclSysCode))
					InclSysObj.push_back(SysDeq.back());
			    }
			catch (invalid_argument x) { cout << "Bad CSV line in " << SysCSV << ": \"" << CSVline << "\"\n"; }
		}
	}
}

void ProcList(envV &env, ofstream &html, highway &hwy)
{	unsigned int pi1, pi2;
	bool comma = 0;

	string line;
	for (list<ListEntry>::iterator LE = env.TravList.begin(); LE != env.TravList.end(); LE++)
	{	if (LE->Region == hwy.Region && hwy.NameMatch(LE->Name))
		{	pi1 = hwy.GetIndByLabel(LE->pl1);
			pi2 = hwy.GetIndByLabel(LE->pl2);
			if (pi1 < hwy.pt.size() && pi2 < hwy.pt.size())
			{	if (comma)
					if (pi1 < pi2)	html << ", " << pi1 << ", " << pi2;
					else		html << ", " << pi2 << ", " << pi1;
				else	// write the first one
					if (pi1 < pi2)	html << pi1 << ", " << pi2;
					else		html << pi2 << ", " << pi1;
				comma = 1;
			}
			list<ListEntry>::iterator FinishedLine = LE;
			LE--;
			env.TravList.erase(FinishedLine);
		}
	}
}

void HTML(list<highway> &HwyList, envV &env)
{	ofstream html(env.Output.data());

	// html elements
	html << "<!doctype html>\n";
	html << "<html>\n";
	html << "<head>\n";
	html << "	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	html << "	<title>Clinched Highway Map</title>\n";
	html << "	<style type=\"text/css\">\n";
	html << "	body { font-family:verdana,helvetica,arial,sans-serif; }\n\n";

	html << "	.button\n";
	html << "	{	color:black;\n";
	html << "		background-color: #FFD864;\n";
	html << "		border: 1px solid black;\n";
	html << "		padding: 2px 5px;\n";
	html << "	}\n\n";

	html << "	a.button:hover,  a.button:active\n";
	html << "	{	color:black;\n";
	html << "		background-color: #E8B000;\n";
	html << "		border: 1px solid black;\n";
	html << "		cursor:pointer;\n";
	html << "	}\n\n";

	html << "	</style>\n";
	html << "</head>\n\n";

	html << "<body style=\"margin:0px;\" bgcolor=\"#F0F0F0\">\n\n";

	html << "<canvas width=" << env.Width << " height=" << env.Height << " style=\"float:left\" draggable=\"true\"\n";
	html << "	onclick=\"PanClick(event)\"\n";
	html << "	ondragstart=\"PanDrag(event)\"\n";
	html << "	onmousemove=\"ShowCoords(event)\"\n";
	html << "	onwheel=\"ZoomWheel(event)\">\n";
	html << "(To see Map, upgrade browser.)\n";
	html << "</canvas>\n\n";

	html << "<div id=\"CoordDisp\"><br><br></div>\n";
	html << "<br>\n";
	html << "<a onclick=\"ZoomIn();\" class=\"button\"><b>+</b></a>\n";
	html << "<a onclick=\"ZoomOut();\" class=\"button\"><b>-</b></a>\n";
	html << "<a onclick=\"ClearCanvas();\" class=\"button\">Clear</a>\n";
	html << "<a onclick=\"RenderMap();\" class=\"button\">Render</a>\n";
	html << "<a onclick=\"reset();\" class=\"button\"><u>R</u>eset</a>\n";
	html << "<a onclick=\"NullXform();\" class=\"button\">NullXform</a>\n";
	html << "<a onclick=\"BoundBox();\" class=\"button\">BoundBox</a>\n";
	html << "<br>\n";
	html << "<div id=\"bounds\"><br><br><br><br></div>\n";
	html << "<br>\n";
	html << "<table bgcolor=D0D0D0 width=*>\n";
	html << "<tr><td><b><u>Debug Widget</u></b>\n";
	html << "    <a onclick=\"CopyBounds();\" class=\"button\">Bounds</a>\n";
	html << "    <a onclick=\"ClearDebug();\" class=\"button\">Clear</a></td></tr>\n";
	html << "<tr><td><div id=\"debug\"></div></td></tr>\n";
	html << "</table>\n\n";

	html << "<script> // JavaScript starts here\n";
	html << "//EDB - route objects\n";
	html << "var rte = [];\n\n";

	// route objects
	unsigned int num = 0;
	for (list<highway>::reverse_iterator hwy = HwyList.rbegin(); hwy != HwyList.rend(); hwy++)
	{	// not all of this may be necessary but leaving it in nonetheless.
		html << "rte[" << num << "] = {\n";
		html << "System:\"" << hwy->System << "\", ";
		html << "Region:\"" << hwy->Region << "\", ";
		html << "Route:\"" << hwy->Route << "\", ";
		html << "Banner:\"" << hwy->Banner << "\", ";
		html << "Abbrev:\"" << hwy->Abbrev << "\", ";
		html << "City:\"" << hwy->City << "\",\n";

		html << "UnColor:'#" << hwy->HwySys->UnColor << "', ClColor:'#" << hwy->HwySys->ClColor << "',\n";

		html << "//EDB - point latitudes\n";
		html << "lat:[";
		list<waypoint>::iterator point = hwy->pt.begin();
			html << to_string(point->Lat);
		for (point++; point != hwy->pt.end(); point++)
			html << ", " << to_string(point->Lat);
		html << "],\n";

		html << "//EDB - point longitudes\n";
		html << "lon:[";
		point = hwy->pt.begin();
			html << to_string(point->Lon);
		for (point++; point != hwy->pt.end(); point++)
			html << ", " << to_string(point->Lon);
		html << "],\n";

		html << "//vdeane & EDB - indices to .listfile endpoints\ncliSegments:[";
		ProcList(env, html, *hwy);
		html << "]\n} //end object definition\n\n";

		num++;
	} //end for (route objects)

	// javascript functions
	html << "var MinLat, MinLon, MaxLat, MaxLon;\n";
	html << "var MinMerc, ScaleFac;\n";
	html << "var PanDragX = -1;\n";
	html << "var PanDragY = -1;\n\n";

	html << "document.addEventListener(\"keypress\", function(event)\n";
	html << "{	if (event.key == \"+\") ZoomIn();\n";
	html << "	if (event.key == \"-\") ZoomOut();\n";
	html << "	if (event.key == \"r\" || event.key == \"R\") reset();\n";
	html << "	if (event.keyCode == 37) PanToXY(canvas.width/4, canvas.height/2);\n";
	html << "	if (event.keyCode == 38) PanToXY(canvas.width/2, canvas.height/4);\n";
	html << "	if (event.keyCode == 39) PanToXY(canvas.width*0.75, canvas.height/2);\n";
	html << "	if (event.keyCode == 40) PanToXY(canvas.width/2, canvas.height*0.75);\n";
	html << "});\n\n";

	html << "function BoundBox()\n";
	html << "{	c = canvas.getContext(\"2d\");\n";
	html << "	c.save();\n";
	html << "	c.beginPath();\n";
	html << "	c.strokeStyle = \"#FF0000\";\n";
	html << "	c.moveTo(LonToX(MinLon), LatToY(MinLat));\n";
	html << "	c.lineTo(LonToX(MinLon), LatToY(MaxLat));\n";
	html << "	c.lineTo(LonToX(MaxLon), LatToY(MaxLat));\n";
	html << "	c.lineTo(LonToX(MaxLon), LatToY(MinLat));\n";
	html << "	c.lineTo(LonToX(MinLon), LatToY(MinLat));\n";
	html << "	c.moveTo(Math.round(canvas.width/2), Math.round(canvas.height/2)-7);\n";
	html << "	c.lineTo(Math.round(canvas.width/2), Math.round(canvas.height/2)+7);\n";
	html << "	c.moveTo(Math.round(canvas.width/2-7), Math.round(canvas.height/2));\n";
	html << "	c.lineTo(Math.round(canvas.width/2+7), Math.round(canvas.height/2));\n";
	html << "	c.stroke();\n";
	html << "	c.restore();\n";
	html << "}\n\n";

	html << "function ClearCanvas()\n";
	html << "{	c = canvas.getContext(\"2d\");\n";
	html << "	c.fillStyle = \"#FFFFFF\";\n";
	html << "	c.fillRect(0,0,canvas.width,canvas.height);\n";
	html << "}\n\n";

	html << "function ClearDebug()	{ document.getElementById(\"debug\").innerHTML = \"\"; }\n";
	html << "function CopyBounds()	{ document.getElementById(\"debug\").innerHTML += document.getElementById(\"bounds\").innerHTML; }\n\n";

	html << "function InitBounds()\n";
	html << "//EDB - get max/min lat/lon for a quick-n-dirty scale of routes trace to fill canvas\n";
	html << "{	MinLat = 90;\n";
	html << "	MinLon = 180;\n";
	html << "	MaxLat = -90;\n";
	html << "	MaxLon = -180;\n";
	html << "	for (RtNum = 0; RtNum < rte.length; RtNum++)\n";
	html << "	{	for (PtNum = 0; PtNum < rte[RtNum].lat.length; PtNum++)\n";
	html << "		{	if (!(rte[RtNum].Region == \"B\" || rte[RtNum].Region == \"b\" || rte[RtNum].Region == \"_boundaries\"))\n";
	html << "			{	if (rte[RtNum].lat[PtNum] < MinLat) MinLat = rte[RtNum].lat[PtNum];\n";
	html << "				if (rte[RtNum].lon[PtNum] < MinLon) MinLon = rte[RtNum].lon[PtNum];\n";
	html << "				if (rte[RtNum].lat[PtNum] > MaxLat) MaxLat = rte[RtNum].lat[PtNum];\n";
	html << "				if (rte[RtNum].lon[PtNum] > MaxLon) MaxLon = rte[RtNum].lon[PtNum];\n";
	html << "			}\n";
	html << "		}\n";
	html << "	}//*/\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	ShowBounds();\n";
	html << "}\n\n";

	html << "function LatToY(lat)	{ return canvas.height-1-(merc(lat)-MinMerc)*ScaleFac; }\n";
	html << "function LonToX(lon)	{ return (lon-MinLon)*ScaleFac; }\n";
	html << "function merc(lat)	{ return Math.log(Math.tan(0.785398163+lat*3.1415926535898/360))*180/3.1415926535898; }\n";
	html << "function amerc(y)	{ return (Math.atan(Math.pow(2.718281828459, y/180*3.1415926535898))-0.785398163)/3.1415926535898*360 }\n\n";

	html << "function NullXform()\n";
	html << "{	MinLat = YToLat(canvas.height-1);\n";
	html << "	MaxLat = YToLat(0);\n";
	html << "	MinLon = XToLon(0);\n";
	html << "	MaxLon = XToLon(canvas.width-1);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n\n";

	html << "function PanClick(e)\n";
	html << "{	if (PanDragX < 0 || PanDragY < 0)\n";
	html << "		PanToXY(e.clientX,e.clientY);\n";
	html << "	else {	PanToXY(Math.round(canvas.width/2)+PanDragX-e.clientX, Math.round(canvas.height/2)+PanDragY-e.clientY);\n";
	html << "		PanDragX = -1;\n";
	html << "		PanDragY = -1;\n";
	html << "	     }\n";
	html << "}\n\n";

	html << "function PanDrag(e)\n";
	html << "{	PanDragX = e.clientX;\n";
	html << "	PanDragY = e.clientY;\n";
	html << "}\n\n";

	html << "function PanToXY(x,y)\n";
	html << "{	MinLat = YToLat(y+canvas.height/2-1);\n";
	html << "	MaxLat = YToLat(y-canvas.height/2);\n";
	html << "	MaxLon = XToLon(x+canvas.width/2-1);\n";
	html << "	MinLon = XToLon(x-canvas.width/2);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n\n";

	html << "function reset()\n";
	html << "{	InitBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n\n";

	html << "function SetScaleFac()\n";
	html << "{	ScaleFac = Math.min((canvas.width-1)/(MaxLon-MinLon), (canvas.height-1)/(merc(MaxLat)-merc(MinLat)));\n";
	html << "}\n\n";

	html << "function ShowBounds()\n";
	html << "{	document.getElementById(\"bounds\").innerHTML=\n";
	html << "	\"<br><table border=1 cellspacing=0>\"+\n";
	html << "	\"<tr><td>MinLat: </td><td>\" + MinLat.toFixed(6)+ \"</td><td>(\"+ LatToY(MinLat).toFixed(0)+ \")</td><td>\"+ merc(MinLat).toFixed(6)+\n";
	html << "	\"<tr><td>MaxLat: </td><td>\" + MaxLat.toFixed(6)+ \"</td><td>(\"+ LatToY(MaxLat).toFixed(0)+ \")</td><td>\"+ merc(MaxLat).toFixed(6)+\n";
	html << "	\"<tr><td>MinLon: </td><td>\" + MinLon.toFixed(6)+ \"</td><td colspan=2>(\"+ LonToX(MinLon).toFixed(0)+ \")\"+\n";
	html << "	\"<tr><td>MaxLon: </td><td>\" + MaxLon.toFixed(6)+ \"</td><td colspan=2>(\"+ LonToX(MaxLon).toFixed(0)+ \")\"+\n";
	html << "	\"<tr><td>ScaleFac: </td><td colspan=3>\" + ScaleFac.toFixed(4)+\n";
	html << "	\"</table>\";\n";
	html << "}\n\n";

	html << "function ShowCoords(e)\n";
	html << "{	lon = XToLon(e.clientX);\n";
	html << "	lat = YToLat(e.clientY);\n";
	html << "	document.getElementById(\"CoordDisp\").innerHTML=\"Mouse: \" + lat.toFixed(6) + \",\" + lon.toFixed(6) +\n";
	html << "	\" (y:\" + e.clientY + \", x:\" + e.clientX + \")<br>\"+\n";
	html << "	\"<a href=http://www.openstreetmap.org/?lat=\"+lat.toFixed(6)+\"&lon=\"+lon.toFixed(6)+\"&zoom=15>OSM</a> \"+\n";
	html << "	\"<a href=http://maps.google.com/?ll=\"+lat.toFixed(6)+\",\"+lon.toFixed(6)+\"&z=15>Google</a>\";\n";
	html << "}\n\n";

	html << "function XToLon(x)	{ return x/ScaleFac+MinLon; }\n";
	html << "function YToLat(y)	{ return amerc((canvas.height-y-1)/ScaleFac+MinMerc); }\n\n";

	html << "function ZoomIn()\n";
	html << "{	MinLat = YToLat(0.75*canvas.height-0.5);\n";
	html << "	MaxLat = YToLat(0.25*canvas.height);\n";
	html << "	MaxLon = XToLon(0.75*canvas.width-0.5);\n";
	html << "	MinLon = XToLon(0.25*canvas.width);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n\n";

	html << "function ZoomOut()\n";
	html << "{	MinLat = YToLat(1.5*canvas.height-2);\n";
	html << "	MaxLat = YToLat(-0.5*canvas.height);\n";
	html << "	MaxLon = XToLon(1.5*canvas.width-2);\n";
	html << "	MinLon = XToLon(-0.5*canvas.width);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n\n";

	html << "function ZoomWheel(e)\n";
	html << "{	e.preventDefault();\n\n";

	html << "	if (e.deltaY > 0) // zoom out\n";
	html << "	{	MinLat = YToLat(2*canvas.height-e.clientY-2);\n";
	html << "		MaxLat = YToLat(0-e.clientY);\n";
	html << "		MaxLon = XToLon(2*canvas.width-e.clientX-2);\n";
	html << "		MinLon = XToLon(0-e.clientX);\n";
	html << "	}\n";
	html << "	if (e.deltaY < 0) // zoom in\n";
	html << "	{	MinLat = YToLat(0.5*(canvas.height+e.clientY)-0.5);\n";
	html << "		MaxLat = YToLat(e.clientY/2);\n";
	html << "		MaxLon = XToLon(0.5*(canvas.width+e.clientX)-0.5);\n";
	html << "		MinLon = XToLon(e.clientX/2);\n";
	html << "	}\n\n";

	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n\n";

	html << "function RenderMap()\n";
	html << "{	//vdeane & EDB - base route line traces\n";
	html << "	for (RtNum = 0; RtNum < rte.length; RtNum++)\n";
	html << "	{	c = canvas.getContext(\"2d\");\n";
	html << "		c.save();\n";
	html << "		c.beginPath();\n";
	html << "		c.strokeStyle = rte[RtNum].UnColor;\n";
	html << "		c.lineWidth=" << env.UnStroke << ";\n";
	html << "		c.moveTo(LonToX(rte[RtNum].lon[0]), LatToY(rte[RtNum].lat[0]));\n";
	html << "		for (PtNum = 1; PtNum < rte[RtNum].lat.length; PtNum++)\n";
	html << "			c.lineTo(LonToX(rte[RtNum].lon[PtNum]), LatToY(rte[RtNum].lat[PtNum]));\n";
	html << "		c.stroke();\n";
	html << "		c.restore();\n";
	html << "	}\n\n";

	html << "	//vdeane & EDB - begin drawing segments\n";
	html << "	for (RtNum = 0; RtNum < rte.length; RtNum++)\n";
	html << "	{	for (ClSeg = 0; ClSeg < rte[RtNum].cliSegments.length; ClSeg+=2)\n";
	html << "		{	var CliPt = false; //vdeane - track if start or end of segment\n";
	html << "			c = canvas.getContext(\"2d\");\n";
	html << "			c.save();\n";
	html << "			c.beginPath();\n";
	html << "			c.strokeStyle = rte[RtNum].ClColor;\n";
	html << "			c.lineWidth=" << env.ClStroke << ";\n\n";

	html << "			for (PtNum = 0; PtNum < rte[RtNum].lat.length; PtNum++)\n";
	html << "			{	if (rte[RtNum].cliSegments[ClSeg] === PtNum)\n";
	html << "				{	c.moveTo(LonToX(rte[RtNum].lon[PtNum]), LatToY(rte[RtNum].lat[PtNum]));\n";
	html << "					CliPt = true;\n";
	html << "				}\n";
	html << "				if (CliPt === true && PtNum !== 0)\n";
	html << "					c.lineTo(LonToX(rte[RtNum].lon[PtNum]), LatToY(rte[RtNum].lat[PtNum]));\n";
	html << "				if (rte[RtNum].cliSegments[ClSeg+1] === PtNum)\n";
	html << "				{	c.stroke();\n";
	html << "					c.restore();\n";
	html << "					CliPt = false;\n";
	html << "				} //end if (Does label end cliSegment?)\n";
	html << "			} //end for (step thru point coords in route)\n";
	html << "		} // end for (step thru clinched segments)\n";
	html << "	} //end for (step thru routes)//*/\n";
	html << "}\n\n";

	html << "//John Pound - initialize canvas\n";
	html << "var canvas = document.getElementsByTagName(\"canvas\")[0];\n\n";

	html << "reset();\n\n";

	html << "// JavaScript ends here\n";
	html << "</script>\n";
	html << "</body>\n";
	html << "</html>\n";
}

int main(int argc, char *argv[])
{	clock_t RunTime = clock();
	envV env; if (!env.set(argc, argv)) return 0;
	list<highway> HwyList;
	for (unsigned int i = 0; i < env.InclSysObj.size(); i++) //   TODO is filename necessary?
		ChoppedRtesCSV(HwyList, env.InclRg, env.InclSysObj[i].filename, env.Repo+"hwy_data/", 1, env.InclSysObj[i], env.SysDeq, tmSysPtr);
	HwyList.sort();
	HTML(HwyList, env);

	/*if (!env.TravList.empty()) cout << "Unprocessed .list lines:\n";
	for (list<ListEntry>::iterator LE = env.TravList.begin(); LE != env.TravList.end(); LE++)
	{	cout << LE->Region << ' ' << LE->Name << ' ' << LE->pl1 << ' ' << LE->pl2 << '\n';
	}//*/

	RunTime = clock() - RunTime;
	cout << "Total run time: " << float(RunTime)/CLOCKS_PER_SEC << '\n';
}
