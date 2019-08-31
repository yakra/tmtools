# autoshape

**Purpose:**<br>
Adds hidden points after specified points in WPT files, in order to break false-positive concurrencies.<br>
Coordinates are chosen to six demical places so that the lateral distance from the line directly connecting the points on either side, on a Mercator map, is minimized.

**Compiling:**<br>
C++11 support is required.<br>
With GCC, I use the commandline `g++ autoshape.cpp -std=c++11 -o autoshape`

**Usage:**<br>
`autoshape HwyDataDir IniFile (-n or --NoNMPs)`<br>
* `HwyDataDir` is the directory containing the WPT file(s), organized by Region/System as the hwy/data directory in the HighwayData repo is. The trailing slash (or backslash in Windows) is required.
* `IniFile` is a text file containing a list of files and waypoints to process, descibed in more detail below.
* `-n` or `--NoNMPs` switch is optional. This will only add the nearest hidden point outside a 0.000500° threshold from the target waypoints. If the target waypoints are already < 0.001000° apart in both latitude & langitude, then no hidden point will be added.
* Nota bene: WPTs should already have their waypoint locations lined up to eliminate NMPs before starting the *autoshape* process.

**INI file:**<br>
Each line describes a single highway file to process, and shall contain the following:<br>
* Region
* System
* Root
* Waypoint(s) *after which* new hidden points are to be inserted
* An example file is below:
```INI
NS cannst ns.ns007 NS111(6A)
NJ usanj nj.nj124 SumAve
RI usaus ri.us006 I-95(20)
RI usari ri.ri114 RI15_E
MA usaus ma.us006 BriSt
MA usama ma.ma079 *BriSt
MA usama ma.ma138 US6_E
CT usaus ct.us001 I-95(83) I-95(85)
CT usact ct.ct015 67S 67
CT usact ct.ct032 CT2(26)
NY usai ny.i095 6A
NY usaus ny.us009 I-787(3)
NY usany ny.ny027 I-678 JFKExpy RocBlvd FarBlvd SprBlvd
TX usai tx.i030 45A
TX usai tx.i045 48A
TX usaus tx.us183 TX130 FM1185 SchRd HomTr TX21 OldLocRd TX45/130
TX usaus tx.us287 US60/87 TXLp395 TXLp279 I-40BL/60
TX usausb tx.us083busmca US83_S
TX usatx tx.tx161 I-20 MayRd TXSpr303 MarDr TX180 I-30 EgyWay
TX usatx tx.tx161sir OakRd
TX usatx tx.tx161irv TX183 RocRd NorDr WalHillLn BeltLineRd TX114 MacArtBlvd
TX usatx tx.tx190car I-35E OldDenRd JosLn
TX usatx tx.tx190pla CoitRd IndPkwy CusPkwy
TX usatx tx.tx190 US75 JupRd RenRd LooDr CamRd GarAve
```

**Output:**<br>
* New waypoint labels will consist of `+x4u70-` followed by a random number of up to six digits, E.G. `+x4u70-628135`.
* INPUT FILES WILL BE OVERWRITTEN. It's wise to ensure that your data are backed up, or that you're working with files in a temporary / non-crucial directory.
