# DBFmine

**Purpose:**<br>
Mines unique values of a specified data field from a DBF file.<br>
Useful for getting lists of route numbers, seeing if a given field is useful or can be culled, etc.

**Compiling:**<br>
C++11 support is required.<br>
With GCC, I use the commandline `g++ DBFmine.cpp -std=c++11 -o DBFmine`

**Usage:**<br>
`DBFmine DBFname KeyField`

**Notes:**<br>
Program requires little-endian (Intel) byte order.<br>
Adapting it to big-endian (Motorola) is not a priority.
