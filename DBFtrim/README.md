# DBFtrim

**Purpose:**<br>
Trims unused empty space from data fields in a DBF file.<br>
This often results in substantial file size savings, which in turn can lead to faster rendering and seeking times in a GIS application.

**Compiling:**
* C++11 and thread support are required.<br>
With GCC, I use the commandline `g++ DBFtrim.cpp -std=c++11 -pthread -o DBFtrim`

**Usage:**<br>
`DBFtrim InputFile OutputFile (NumThreads)`
Number of threads is optional. If this argument is absent or cannot be parsed as a positive integer, it will revert to 1.

**Notes:**<br>
Program requires little-endian (Intel) byte order.<br>
Adapting it to big-endian (Motorola) is not a priority.
