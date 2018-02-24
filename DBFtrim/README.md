# DBFtrim

**Purpose:**<br>
Trims unused empty space from data fields in a DBF file.<br>
This often results in substantial file size savings, which in turn can lead to faster rendering and seeking times in a GIS application.

**Usage:**<br>
`DBFtrim InputFile OutputFile`

**Notes:**<br>
Program requires little-endian (Intel) byte order.<br>
Adapting it to big-endian (Motorola) is not a priority.
