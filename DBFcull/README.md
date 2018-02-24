# DBFcull

**Purpose:**<br>
Culls unwanted data fields from a DBF file.<br>
This often results in substantial file size savings, which in turn can lead to faster rendering and seeking times in a GIS application.

**Usage:**<br>
`DBFcull InputFile OutputFile Field1 (Field2) (...)`<br>
Field1 is required. Field2 and onwards are optional. List as many fields as you want to cull.<br>
If a listed field is not contained in the DBF file, it is ignored.<br>
If no listed fields are contained in the DBF file, the output file will be identical to the input file.

**Notes:**<br>
Program requires little-endian (Intel) byte order.<br>
Adapting it to big-endian (Motorola) is not a priority.
