# tmtools

Tools to aid in development of the [TravelMapping](https://github.com/TravelMapping/) project.
* `autoshape` adds shaping points to the least-offset coords along WPT segments, to break false concurrencies.
* `canvas` produces interactive "CHM-style" Javascript maps using the HTML5 canvas.
* `DBFcull` culls unwanted data fields from a DBF file, for improved performance in GIS applictions.
* `DBFmine` mines unique values of a specified data field from a DBF file.
* `DBFtrim` trims unused empty space from data fields in a DBF file, for improved performance in GIS applictions.
* `gisplunge` extracts coordinates from GIS shapefiles for use in WPT files.
* `lib` contains C++ header and source files shared between multiple tmtools projects.
* `tmg2html` converts .tmg graphs into Javascript "HDX-Lite" web pages.
