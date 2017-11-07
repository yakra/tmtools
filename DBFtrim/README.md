# DBFtrim

**Purpose:**<br>
Trims unused empty space from data fields in a DBF file.<br>
This often results in substantial file size savings, which in turn can lead to faster rendering and seeking times in a GIS application.

**Usage:**<br>
`DBFtrim InputFile OutputFile`

**Notes:**<br>
Program requires 64-bit memory addressing and little-endian (Intel) byte order.<br>
It will not work in other environments.<br>
This could be fixed/improved, but is a low priority.
