# gisplunge

**What is gisplunge?:**
* The purpose is to line up coordinates from input WPT files to a specified set of shapefiles.<br>
* It can also extract shapefile records to WPT.
* Why "gisplunge"? I needed something to name the INI file when first writing the program, and
[splunge](https://www.youtube.com/watch?v=3v0I4OQi7CQ) was the first thing that popped into my head. I pronounce it "Ji-SPLUNGE".

**Compiling:**
* C++11 and thread support are required.<br>
With GCC, I use the commandline `g++ gisplunge.cpp -std=c++11 -pthread -o gisplunge`

**Compatibility:**
* gisplunge only works with shapefiles that have their coordinates stored as double floating-point longitude and latitude values.
Converting between different Coordinate Reference Systems is not and will not be supported. If you need to convert a shapefile
set from another CRS, I recommend using a GIS application such as [QGIS](http://qgis.org/).
* Program requires little-endian (Intel) byte order. Adapting it to big-endian (Motorola) is not a priority.

**Usage:**
* You will need an `output/` subdirectory in the same directory as the executable.
From there, create `output/off/` and `output/on/`. A bit more detail on this below...
* Program parameters are controlled by the `splunge.ini` file, located in the same directory as the executable.<br>
There are no commandline options.
* An example `splunge.ini` file is below:
```INI
#################
#_Basic_options_#
#################

InputFile /home/yakra/TravelMapping/hwy_data/MB/canmbp/draft/mb.mb234.wpt
ShapeRoot /home/yakra/gis/data/mb/nrn_rrn_mb_shp_en_6.0b/NRN_MB_6_0_ROADSEG
KeyField RTNUMBER1
KeyValue 234

###################
#_CSVmode_options_#
###################

SourceDir /home/yakra/TravelMapping/hwy_data/MB/canmbp/draft/
CharSkip 2
Threads 4

####################
#_Advanced_options_#
####################

JctSnap OFF
SegDump OFF

#########
#_Debug_#
#########

WriteOrig OFF
DoubleBug OFF
```
Parameters (all of which are case sensitive) explained:

**Basic options:**
* `InputFile` contains the input file to be processed. It is assumed to be a WPT file, unless the filename ends in `.csv`, in
which case gisplunge will enter "CSV mode" (see below), parse the input file as a "chopped routes" CSV, and process every route
on that list.
* `ShapeRoot` contains the root of the shapefile name, with no extension. *.shp*, *.shx*, & *.dbf* will be internally appended in
order to load the corresponding files.<br>
Spaces in filenames & paths are not supported in either `InputFile` or `ShapeRoot`. Who uses spaces in filenames anyway? ;)
* `KeyField` contains the data attribute in the shapefiles' data table (DBF file) to be considered. Shapefile records where the
value in the key field matches `KeyValue` will have their points compared against the points in the input WPT, replacing the WPT
coordinates as matches are found.<br> This will usually be a field used to store route numbers.
* `KeyValue` contains the value of `KeyField` for the particular route being processed. This will usually be a route
number. If gisplunge is operating in CSV mode, this parameter will be ignored, with the key value instead being derived from
the data in the CSV file.

**CSV mode options:**
* `SourceDir` contains the directory where the WPT files are found. The trailing slash (or backslash in Windows) is required.<br>
Again, whitespace is unsupported.
* `CharSkip` is the number of characters to be ignored at the beginning of the CSV's *Route* field. The remainder of the *Route*
field becomes the new KeyValue for each route being processed. For example, a Route listed as `MB234` with a `CharSkip` of `2`
will have the *MB* ignored, and *234* passed on as the new KeyValue.
* `Threads` contains the maximum number of highways to process at once. If set to 0, one thread will be created for each valid
highway on the CSV list. I recommend setting this to the number of CPU cores in your computer. Increasing it too far beyond that
can degrade performance slightly.

**Advanced options:**
* `JctSnap` has allowable values of `OFF`, `ON`, `BOTH`, or `NONE`. JctSnap attempts to line WPT points up to road junctions, by
using the points at the ends of individual shapefile records. This can help cut down on NMPs. On the downside, not all
jurisdictions' shapefiles will actually have records ending at each road junction, or shapefiles may not have junctions / record
ends where expected, resulting in output waypoints that are significantly out of whack. These points can be manually replaced
with their counterparts from the OFF-flavored files (All gisplunge output should be manually checked, regardless). `OFF` and `ON`
each write files to their respective `output` directories; `BOTH` writes both sets of files. The `NONE` option is useful if you
only want to *SegDump* individual shapefile records, and don't care about having the *gisplunged* route files themselves.<br>
For beginners, I recommend just leaving this option `OFF` until you become familiar with the mojo of the particular shapefile set
you're working with, in your GIS application.
* `SegDump` has allowable values of `OFF` or `ON`. SegDump extracts individual shapefile records where `KeyField` contains
`KeyValue`, and writes them to disk as WPT files, in the `output/segdump` subdirectory. This should be left `OFF` unless you
really know what you're doing; it can crap up your hard disk with untold tons of WPT files if not used *very* carefully.

**Debug options:**
* `WriteOrig` can be `ON` or `OFF`. This writes a file to the `output/orig/` subdirectory, using the unchanged coordinates as
read from the original WPT file. Useful for seeing how malformed URLs are handled, or that the text -> binary and binary -> text
conversion processes are working as intended.
* `DoubleBug` can be `ON` or `OFF`. This recreates a floating-point binary -> text conversion bug found in earlier *gisplunge*
versions, that resulted in the least significant digit sometimes being 1 too low. Only really useful for the original author,
in checking out DIFFs from earlier versions' output.
