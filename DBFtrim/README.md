# DBFtrim

**Purpose:**
Trims unused whitespace from data fields in a DBF file.
This often results in substantial file size savings, which in turn can lead to faster rendering and seeking times in a GIS application.

**Usage:**
*DBFtrim InputFile OutputFile*

**Notes:**
Program requires 64-bit memory addressing and little-endian (Intel) byte order. It will not work in other environments.
This could be fixed/improved, but is a low priority.
