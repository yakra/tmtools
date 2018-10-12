#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class vertex
{	public:
	string label;
	double lat, lon;

	vertex(string &ConstLine)
	{	char *line = new char[ConstLine.size()+1];
		strcpy(line, ConstLine.data());
		label = strtok(line, " ");
		lat = stod(strtok(0, " "));
		lon = stod(strtok(0, " "));
	}	
};

class edge
{	public:
	unsigned int BegI, EndI, qty;
	vertex *BegP, *EndP;
	char *label;
	vector<double> shape;

	edge(string &ConstLine, vertex **vertices)
	{	char *line = new char[ConstLine.size()+1];
		strcpy(line, ConstLine.data());
		BegI = stoi(strtok(line, " "));	BegP = vertices[BegI];
		EndI = stoi(strtok(0, " "));	EndP = vertices[EndI];
		label = strtok(0, " ");
		for (char *val = strtok(0, " "); val; val = strtok(0, " "))
		{	shape.push_back(stod(val));
		}
		qty = 1;
		for (char *comma = strchr(label, ','); comma; qty++) comma = strchr(comma+1, ',');
	}
};

int main(int argc, char *argv[])
{	ifstream tmg(argv[1]);
	if (argc < 3)	{ cout << "usage: ./tmg2html <InputFile> <OutputFile>\n"; return 0; }
	if (!tmg)	{ cout << argv[1] << " not found.\n"; return 0; }

	const char *filename = &argv[1][string(argv[1]).find_last_of("/\\")+1];

	unsigned int NumVertices, NumEdges;
	string tmgline;
	getline(tmg, tmgline);
	if (tmgline.substr(0, 7) != "TMG 1.0") { cout << '\"' << tmgline << "\" unsupported.\n"; return 0; }
	tmg >> NumVertices;
	tmg >> NumEdges;
	vertex **vertices = new vertex*[NumVertices];
	edge **edges = new edge*[NumEdges];
	getline(tmg, tmgline); // seek to end of NumVertices, NumEdges line

	// read vertices
	for (unsigned int i = 0; i < NumVertices; i++)
	{	getline(tmg, tmgline);
		vertices[i] = new vertex(tmgline);
		//cout << vertices[i]->label << ' ' << to_string(vertices[i]->lat) << ' ' << to_string(vertices[i]->lon) << '\n';
	}

	// read edges
	for (unsigned int i = 0; i < NumEdges; i++)
	{	getline(tmg, tmgline);
		edges[i] = new edge(tmgline, vertices);
		/*cout << edges[i]->BegI << ' ' << edges[i]->EndI << ' ' << edges[i]->label;
		  for (unsigned int j = 0; j < edges[i]->shape.size(); j++) cout << ' ' << edges[i]->shape[j];
		  cout << '\n';//*/
	}

	// HTML output
	ofstream html(argv[2]);

	// html elements
	html << "<!doctype html>\n";
	html << "<html>\n";
	html << "<head>\n";
	html << "	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n";
	html << "	<title>yakra's HDX-Lite: " << filename << "</title>\n";
	html << "	<style type=\"text/css\">\n";
	html << "	body\n";
	html << "	{	font-family:verdana,helvetica,arial,sans-serif;\n";
	html << "		overflow:hidden;\n";
	html << "		font-size:9pt;\n";
	html << "	}\n";
	html << "\n";
	html << "	.button\n";
	html << "	{	color:black;\n";
	html << "		background-color: #FFD864;\n";
	html << "		border: 1px solid black;\n";
	html << "		padding: 2px 5px;\n";
	html << "	}\n";
	html << "\n";
	html << "	a.button:hover,  a.button:active\n";
	html << "	{	color:black;\n";
	html << "		background-color: #E8B000;\n";
	html << "		border: 1px solid black;\n";
	html << "		cursor:pointer;\n";
	html << "	}\n";
	html << "\n";
	html << "	.vtr\n";
	html << "	{	background-color:#F0F0F0;\n";
	html << "	}\n";
	html << "\n";
	html << "	.vtr:hover\n";
	html << "	{	background-color:#D0D0D0;\n";
	html << "	}\n";
	html << "\n";
	html << "	#vInfo\n";
	html << "	{	position:fixed;\n";
	html << "		left: 700px;\n";
	html << "		right: 0px;\n";
	html << "		top: 200px;\n";
	html << "		height: 500px;\n";
	html << "		overflow:scroll;\n";
	html << "	}\n";
	html << "\n";
	html << "	</style>\n";
	html << "</head>\n";
	html << "\n";
	html << "<body style=\"margin:0px;\" bgcolor=\"#F0F0F0\">\n";
	html << "\n";
	html << "<canvas width=700 height=700 style=\"float:left\" draggable=\"true\"\n";
	html << "	onclick=\"ClickMap(event)\"\n";
	html << "	ondblclick=\"PanToXY(event.clientX, event.clientY)\"\n";
	html << "	ondragend=\"ClickMap(event)\"\n";
	html << "	ondragstart=\"PanDrag(event)\"\n";
	html << "	onmousemove=\"ShowCoords(event)\"\n";
	html << "	onwheel=\"ZoomWheel(event)\">\n";
	html << "(To see Map, upgrade browser.)\n";
	html << "</canvas>\n";
	html << "\n";
	html << "<div id=\"CoordDisp\"><br><br></div>\n";
	html << "<br>\n";
	html << "<a onclick=\"ZoomIn();\" class=\"button\"><b>+</b></a>\n";
	html << "<a onclick=\"ZoomOut();\" class=\"button\"><b>-</b></a>\n";
	html << "<!--<a onclick=\"ClearCanvas();\" class=\"button\">Clear</a>\n";
	html << "<a onclick=\"RenderMap();\" class=\"button\">Render</a>-->\n";
	html << "<a onclick=\"reset();\" class=\"button\"><u>R</u>eset</a>\n";
	html << "<!--<a onclick=\"NullXform();\" class=\"button\">NullXform</a>\n";
	html << "<a onclick=\"BoundBox();\" class=\"button\">BoundBox</a>-->\n";
	html << "<br>\n";
	html << "<!--<div id=\"bounds\"><br><br><br><br></div>-->\n";
	html << "<br>\n";
	html << "<table bgcolor=D0D0D0 width=200>\n";
	html << "<tr><td align=center><b><big>Info</big></b>\n";
	html << "    <a onclick=\"ClearInfo();\" class=\"button\" style=\"float:right\">X</a></td></tr>\n";
	html << "<tr><td><div id=\"InfoBox\"></div></td></tr>\n";
	html << "</table>\n";
	html << "<div id=\"vInfo\"></div>\n";
	html << "\n";
	html << "<script>\n";

	// vertices
	html << "var vertex = [";
	for (int vNum = 0; vNum < NumVertices; vNum++)
		html << "\n{lat:" << to_string(vertices[vNum]->lat)\
		     << ",lon:" << to_string(vertices[vNum]->lon)\
		     << ",label:'" << vertices[vNum]->label << "'},";
	html.seekp(-1, ios::cur);
	html << "\n];\n\n";

	// edges
	html << "var edge = [";
	for (int eNum = 0; eNum < NumEdges; eNum++)
	{	html << "\n{begin:" << edges[eNum]->BegI\
		     << ",end:" << edges[eNum]->EndI\
		     << ",qty:" << edges[eNum]->qty\
		     << ",label:'" << edges[eNum]->label\
		     << "',shape:[";
		for (unsigned int sNum = 0; sNum < edges[eNum]->shape.size(); sNum++)
		{	html << to_string(edges[eNum]->shape[sNum]);
			if (sNum < edges[eNum]->shape.size()-1) html << ", ";
		}
		html << "]},";
	}
	html.seekp(-1, ios::cur);
	html << "\n];\n\n";

	// javascript functions
	html << "var MinLat, MinLon, MaxLat, MaxLon;\n";
	html << "var MinMerc, ScaleFac;\n";
	html << "var selectedV = -1;\n";
	html << "var selectedE = -1;\n";
	html << "var PanDragX = -1;\n";
	html << "var PanDragY = -1;\n";
	html << "\n";
	html << "document.addEventListener(\"keydown\", function(event)\n";
	html << "{	if (event.key == \"+\") ZoomIn();\n";
	html << "	if (event.key == \"-\") ZoomOut();\n";
	html << "	if (event.key == \"r\" || event.key == \"R\") reset();\n";
	html << "	if (event.keyCode == 37) PanToXY(canvas.width/4, canvas.height/2);\n";
	html << "	if (event.keyCode == 38) PanToXY(canvas.width/2, canvas.height/4);\n";
	html << "	if (event.keyCode == 39) PanToXY(canvas.width*0.75, canvas.height/2);\n";
	html << "	if (event.keyCode == 40) PanToXY(canvas.width/2, canvas.height*0.75);\n";
	html << "});\n";
	html << "\n";
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
	html << "}\n";
	html << "\n";
	html << "function ClearCanvas()\n";
	html << "{	c = canvas.getContext(\"2d\");\n";
	html << "	c.fillStyle = \"#FFFFFF\";\n";
	html << "	c.fillRect(0,0,canvas.width,canvas.height);\n";
	html << "}\n";
	html << "\n";
	html << "function ClearInfo()\n";
	html << "{	document.getElementById(\"InfoBox\").innerHTML = \"\";\n";
	html << "	// selected V & selected E should never have a value >= 0 at the same time\n";
	html << "	if (selectedV >= 0) VertexMarker(selectedV, \"#FFFF00\"); // faster than re-rendering map altogether\n";
	html << "	if (selectedE >= 0) { selectedE = -1; ClearCanvas(); RenderMap(); }\n";
	html << "	selectedV = -1;\n";
	html << "}\n";
	html << "\n";
	html << "function ClickMap(e)\n";
	html << "{	if (PanDragX < 0 || PanDragY < 0) // just clicking, not dragging\n";
	html << "	{	ClearInfo();\n";
	html << "\n";
	html << "		// check for click on vertex marker\n";
	html << "		MinDist = measure(0, 0, canvas.width, canvas.height);\n";
	html << "		for (vNum = 0; vNum < vertex.length; vNum++)\n";
	html << "		  if (measure(e.clientX, e.clientY, LonToX(vertex[vNum].lon), LatToY(vertex[vNum].lat)) <= 7)\n";
	html << "	  	    if (measure(e.clientX, e.clientY, LonToX(vertex[vNum].lon), LatToY(vertex[vNum].lat)) < MinDist)\n";
	html << "		    {	selectedV = vNum;\n";
	html << "			MinDist = measure(e.clientX, e.clientY, LonToX(vertex[vNum].lon), LatToY(vertex[vNum].lat));\n";
	html << "		    }\n";
	html << "		if (selectedV >= 0) { VertexSelect(selectedV); return; }\n";
	html << "\n";
	html << "		// check for click on/near edge\n";
	html << "		MinDist = measure(0, 0, canvas.width, canvas.height);\n";
	html << "		var CurDist;\n";
	html << "		for (eNum = 0; eNum < edge.length; eNum++)\n";
	html << "		  if (EdgeInViewport(eNum))\n";
	html << "		  {	if (edge[eNum].shape.length == 0)\n";
	html << "			{	CurDist = EdgeSegDist(LonToX(vertex[edge[eNum].begin].lon),\n";
	html << "						      LatToY(vertex[edge[eNum].begin].lat),\n";
	html << "						      LonToX(vertex[edge[eNum].end].lon),\n";
	html << "						      LatToY(vertex[edge[eNum].end].lat),\n";
	html << "						      e.clientX, e.clientY);\n";
	html << "				if (MinDist > CurDist) { MinDist = CurDist; selectedE = eNum; }\n";
	html << "			}\n";
	html << "			else {	// do first segment\n";
	html << "				CurDist = EdgeSegDist(LonToX(vertex[edge[eNum].begin].lon),\n";
	html << "						      LatToY(vertex[edge[eNum].begin].lat),\n";
	html << "						      LonToX(edge[eNum].shape[1]),\n";
	html << "						      LatToY(edge[eNum].shape[0]),\n";
	html << "						      e.clientX, e.clientY);\n";
	html << "				if (MinDist > CurDist) { MinDist = CurDist; selectedE = eNum; }\n";
	html << "				// do intermediate segments\n";
	html << "				for (s = 2; s < edge[eNum].shape.length; s+=2)\n";
	html << "				{	CurDist = EdgeSegDist(LonToX(edge[eNum].shape[s-1]),\n";
	html << "							      LatToY(edge[eNum].shape[s-2]),\n";
	html << "							      LonToX(edge[eNum].shape[s+1]),\n";
	html << "							      LatToY(edge[eNum].shape[s]),\n";
	html << "							      e.clientX, e.clientY);\n";
	html << "					if (MinDist > CurDist) { MinDist = CurDist; selectedE = eNum; }\n";
	html << "				}\n";
	html << "				// do last segment\n";
	html << "				CurDist = EdgeSegDist(LonToX(edge[eNum].shape[edge[eNum].shape.length-1]),\n";
	html << "						      LatToY(edge[eNum].shape[edge[eNum].shape.length-2]),\n";
	html << "						      LonToX(vertex[edge[eNum].end].lon),\n";
	html << "						      LatToY(vertex[edge[eNum].end].lat),\n";
	html << "						      e.clientX, e.clientY);\n";
	html << "				if (MinDist > CurDist) { MinDist = CurDist; selectedE = eNum; }\n";
	html << "			     }\n";
	html << "		  }\n";
	html << "		if (MinDist <= 7) EdgeSelect(selectedE);\n";
	html << "		else selectedE = -1;\n";
	html << "	} // end if (just clicking, not dragging)\n";
	html << "	else {	PanToXY(Math.round(canvas.width/2)+PanDragX-e.clientX, Math.round(canvas.height/2)+PanDragY-e.clientY);\n";
	html << "		PanDragX = -1;\n";
	html << "		PanDragY = -1;\n";
	html << "	     }\n";
	html << "}\n";
	html << "\n";
	html << "function CoordsInViewport(lat, lon)\n";
	html << "{ return lat >= MinLat.toFixed(6) && lat <= MaxLat.toFixed(6) && lon >= MinLon.toFixed(6) && lon <= MaxLon.toFixed(6); }\n";
	html << "\n";
	html << "function DrawEdge(eNum, width, ColOv)\n";
	html << "{	c = canvas.getContext(\"2d\");\n";
	html << "	c.save();\n";
	html << "	c.beginPath();\n";
	html << "	if (ColOv) c.strokeStyle = ColOv;\n";
	html << "	else	switch(edge[eNum].qty)\n";
	html << "		{   case 1: c.strokeStyle = 'rgba(0, 0, 255, 0.33)'; break;\n";
	html << "		    case 2: c.strokeStyle = 'rgba(0, 255, 0, 0.33)'; break;\n";
	html << "		    case 3: c.strokeStyle = 'rgba(255, 0, 255, 0.33)'; break;\n";
	html << "		    case 4: c.strokeStyle = 'rgba(255, 255, 0, 0.33)'; break;\n";
	html << "		    default: c.strokeStyle = 'rgba(255, 0, 0, 0.33)'; break;\n";
	html << "		}\n";
	html << "	c.lineWidth=width;\n";
	html << "	c.moveTo(LonToX(vertex[edge[eNum].begin].lon), LatToY(vertex[edge[eNum].begin].lat));\n";
	html << "	for (s = 0; s < edge[eNum].shape.length; s+=2)\n";
	html << "		c.lineTo(LonToX(edge[eNum].shape[s+1]), LatToY(edge[eNum].shape[s]));\n";
	html << "	c.lineTo(LonToX(vertex[edge[eNum].end].lon), LatToY(vertex[edge[eNum].end].lat));\n";
	html << "	c.stroke();\n";
	html << "	c.restore();\n";
	html << "}\n";
	html << "\n";
	html << "function EdgeHighlight(eNum)\n";
	html << "{	DrawEdge(eNum, 15, 'rgba(255, 0, 0, 1)');\n";
	html << "	DrawEdge(eNum, 12, 'rgba(255, 255, 255, 1)');\n";
	html << "	DrawEdge(eNum, 9, 0);\n";
	html << "	VertexMarkers();\n";
	html << "}\n";
	html << "\n";
	html << "function EdgeInViewport(eNum)\n";
	html << "{	if (CoordsInViewport(vertex[edge[eNum].begin].lat, vertex[edge[eNum].begin].lon)) return 1;\n";
	html << "	if (CoordsInViewport(vertex[edge[eNum].end].lat, vertex[edge[eNum].end].lon)) return 1;\n";
	html << "	for (s = 0; s < edge[eNum].shape.length; s+=2)\n";
	html << "		if (CoordsInViewport(edge[eNum].shape[s], edge[eNum].shape[s+1])) return 1;\n";
	html << "	return 0;\n";
	html << "}\n";
	html << "\n";
	html << "function EdgeSegDist(BegX, BegY, EndX, EndY, ClX, ClY)\n";
	html << "// find nearest point on edge segment; return distance from clicked point\n";
	html << "{	// horizontal / slope 0\n";
	html << "	if (BegY == EndY)\n";
	html << "	  if      (ClX < Math.min(BegX, EndX)) return measure(ClX, ClY, Math.min(BegX, EndX), BegY);\n";
	html << "	  else if (ClX > Math.max(BegX, EndX)) return measure(ClX, ClY, Math.max(BegX, EndX), BegY);\n";
	html << "	  else return measure(ClX, ClY, ClX, BegY);\n";
	html << "	// vertical / slope infinite\n";
	html << "	if (BegX == EndX)\n";
	html << "	  if      (ClY < Math.min(BegY, EndY)) return measure(ClX, ClY, BegX, Math.min(BegY, EndY));\n";
	html << "	  else if (ClY > Math.max(BegY, EndY)) return measure(ClX, ClY, BegX, Math.max(BegY, EndY));\n";
	html << "	  else return measure(ClX, ClY, BegX, ClY);\n";
	html << "	// diagonal\n";
	html << "	m1 = (EndY-BegY)/(EndX-BegX);\n";
	html << "	m2 = -1/m1;\n";
	html << "	b1 = BegY-m1*BegX;\n";
	html << "	b2 = ClY-m2*ClX;\n";
	html << "	x = (b1-b2)/(m2-m1);\n";
	html << "	if (x < Math.min(BegX, EndX)) x = Math.min(BegX, EndX);\n";
	html << "	if (x > Math.max(BegX, EndX)) x = Math.max(BegX, EndX);\n";
	html << "	y = m1*x+b1;\n";
	html << "	return measure(ClX, ClY, x, y);\n";
	html << "}\n";
	html << "\n";
	html << "function EdgeSelect(eNum)\n";
	html << "{	ClearInfo();\n";
	html << "	document.getElementById(\"InfoBox\").innerHTML =\n";
	html << "		\"<hr><big>\" + edge[eNum].label + \"</big>\" +\n";
	html << "		\"<br><small>Edge \" + eNum;\n";
	html << "	EdgeHighlight(eNum);\n";
	html << "	selectedE = eNum;\n";
	html << "}\n";
	html << "\n";
	html << "function EdgeTable(eNum)\n";
	html << "{	EdgeSelect(eNum);\n";
	html << "	// Pan to edge\n";
	html << "	eMinLat = vertex[edge[eNum].begin].lat; eMaxLat = eMinLat;\n";
	html << "	eMinLon = vertex[edge[eNum].begin].lon; eMaxLon = eMinLon;\n";
	html << "	if (eMinLat > vertex[edge[eNum].end].lat) eMinLat = vertex[edge[eNum].end].lat;\n";
	html << "	if (eMaxLat < vertex[edge[eNum].end].lat) eMaxLat = vertex[edge[eNum].end].lat;\n";
	html << "	if (eMinLon > vertex[edge[eNum].end].lon) eMinLon = vertex[edge[eNum].end].lon;\n";
	html << "	if (eMaxLon < vertex[edge[eNum].end].lon) eMaxLon = vertex[edge[eNum].end].lon;\n";
	html << "	for (s = 0; s < edge[eNum].shape.length; s+=2)\n";
	html << "	{	if (eMinLat > edge[eNum].shape[s]) eMinLat = edge[eNum].shape[s];\n";
	html << "		if (eMaxLat < edge[eNum].shape[s]) eMaxLat = edge[eNum].shape[s];\n";
	html << "		if (eMinLon > edge[eNum].shape[s+1]) eMinLon = edge[eNum].shape[s+1];\n";
	html << "		if (eMaxLon < edge[eNum].shape[s+1]) eMaxLon = edge[eNum].shape[s+1];\n";
	html << "	}\n";
	html << "	eMaxX = LonToX(eMaxLon);\n";
	html << "	eMinX = LonToX(eMinLon);\n";
	html << "	eMaxY = LatToY(eMaxLat);\n";
	html << "	eMinY = LatToY(eMinLat);\n";
	html << "	PanToXY((eMaxX+eMinX)/2, (eMaxY+eMinY)/2);\n";
	html << "}\n";
	html << "\n";
	html << "function InitBounds()\n";
	html << "//EDB - get max/min lat/lon for a quick-n-dirty scale of routes trace to fill canvas\n";
	html << "{	MinLat = 90;\n";
	html << "	MinLon = 180;\n";
	html << "	MaxLat = -90;\n";
	html << "	MaxLon = -180;\n";
	html << "	for (vNum = 0; vNum < vertex.length; vNum++)\n";
	html << "	{	if (vertex[vNum].lat < MinLat) MinLat = vertex[vNum].lat;\n";
	html << "		if (vertex[vNum].lon < MinLon) MinLon = vertex[vNum].lon;\n";
	html << "		if (vertex[vNum].lat > MaxLat) MaxLat = vertex[vNum].lat;\n";
	html << "		if (vertex[vNum].lon > MaxLon) MaxLon = vertex[vNum].lon;\n";
	html << "	}\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	//ShowBounds();\n";
	html << "}\n";
	html << "\n";
	html << "function LatToY(lat)		 { return canvas.height-1-(merc(lat)-MinMerc)*ScaleFac; }\n";
	html << "function LonToX(lon)		 { return (lon-MinLon)*ScaleFac; }\n";
	html << "function measure(x1, y1, x2, y2) { return Math.sqrt(Math.pow(x1-x2,2) + Math.pow(y1-y2,2)); }\n";
	html << "function merc(lat)		 { return Math.log(Math.tan(0.785398163+lat*3.1415926535898/360))*180/3.1415926535898; }\n";
	html << "function amerc(y)		 { return (Math.atan(Math.pow(2.718281828459, y/180*3.1415926535898))-0.785398163)/3.1415926535898*360 }\n";
	html << "\n";
	html << "function NullXform()\n";
	html << "{	MinLat = YToLat(canvas.height-1);\n";
	html << "	MaxLat = YToLat(0);\n";
	html << "	MinLon = XToLon(0);\n";
	html << "	MaxLon = XToLon(canvas.width-1);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	//ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n";
	html << "\n";
	html << "function PanDrag(e)\n";
	html << "{	PanDragX = e.clientX;\n";
	html << "	PanDragY = e.clientY;\n";
	html << "}\n";
	html << "\n";
	html << "function PanToXY(x,y)\n";
	html << "{	MinLat = YToLat(y+canvas.height/2-1);\n";
	html << "	MaxLat = YToLat(y-canvas.height/2);\n";
	html << "	MaxLon = XToLon(x+canvas.width/2-1);\n";
	html << "	MinLon = XToLon(x-canvas.width/2);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	//ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n";
	html << "\n";
	html << "function RenderMap()\n";
	html << "{	for (eNum = 0; eNum < edge.length; eNum++) DrawEdge(eNum, 6, 0);\n";
	html << "	// selected V & selected E should never have a value >= 0 at the same time\n";
	html << "	if (selectedE >= 0) EdgeHighlight(selectedE);\n";
	html << "	else VertexMarkers();\n";
	html << "	if (selectedV >= 0) VertexMarker(selectedV, \"#FF0000\");\n";
	html << "}\n";
	html << "\n";
	html << "function reset()\n";
	html << "{	InitBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n";
	html << "\n";
	html << "function SetScaleFac()\n";
	html << "{	ScaleFac = Math.min((canvas.width-1)/(MaxLon-MinLon), (canvas.height-1)/(merc(MaxLat)-merc(MinLat)));\n";
	html << "}\n";
	html << "\n";
	html << "function ShowBounds()\n";
	html << "{	document.getElementById(\"bounds\").innerHTML=\n";
	html << "	\"<br><table border=1 cellspacing=0>\"+\n";
	html << "	\"<tr><td>MinLat: </td><td>\" + MinLat.toFixed(6)+ \"</td><td>(\"+ LatToY(MinLat).toFixed(0)+ \")</td><td>\"+ merc(MinLat).toFixed(6)+\n";
	html << "	\"<tr><td>MaxLat: </td><td>\" + MaxLat.toFixed(6)+ \"</td><td>(\"+ LatToY(MaxLat).toFixed(0)+ \")</td><td>\"+ merc(MaxLat).toFixed(6)+\n";
	html << "	\"<tr><td>MinLon: </td><td>\" + MinLon.toFixed(6)+ \"</td><td colspan=2>(\"+ LonToX(MinLon).toFixed(0)+ \")\"+\n";
	html << "	\"<tr><td>MaxLon: </td><td>\" + MaxLon.toFixed(6)+ \"</td><td colspan=2>(\"+ LonToX(MaxLon).toFixed(0)+ \")\"+\n";
	html << "	\"<tr><td>ScaleFac: </td><td colspan=3>\" + ScaleFac.toFixed(4)+\n";
	html << "	\"</table>\";\n";
	html << "}\n";
	html << "\n";
	html << "function ShowCoords(e)\n";
	html << "{	lon = XToLon(e.clientX);\n";
	html << "	lat = YToLat(e.clientY);\n";
	html << "	document.getElementById(\"CoordDisp\").innerHTML=\"Mouse: \" + lat.toFixed(6) + \",\" + lon.toFixed(6) +\n";
	html << "	\" (y:\" + e.clientY + \", x:\" + e.clientX + \")<br>\"+\n";
	html << "	\"<a href=http://www.openstreetmap.org/?lat=\"+lat.toFixed(6)+\"&lon=\"+lon.toFixed(6)+\"&zoom=15>OSM</a> \"+\n";
	html << "	\"<a href=http://maps.google.com/?ll=\"+lat.toFixed(6)+\",\"+lon.toFixed(6)+\"&z=15>Google</a>\";\n";
	html << "}\n";
	html << "\n";
	html << "function VertexInViewport(vNum) { return CoordsInViewport(vertex[vNum].lat, vertex[vNum].lon); }\n";
	html << "\n";
	html << "function VertexMarker(vNum, color)\n";
	html << "{	c = canvas.getContext(\"2d\");\n";
	html << "	c.beginPath();\n";
	html << "	c.arc(LonToX(vertex[vNum].lon),LatToY(vertex[vNum].lat),7,0,2*Math.PI);\n";
	html << "	c.fillStyle = \"#000000\";\n";
	html << "	c.fill();\n";
	html << "\n";
	html << "	c.beginPath();\n";
	html << "	c.arc(LonToX(vertex[vNum].lon),LatToY(vertex[vNum].lat),6,0,2*Math.PI);\n";
	html << "	c.fillStyle = color;\n";
	html << "	c.fill();\n";
	html << "\n";
	html << "	c.beginPath();\n";
	html << "	c.moveTo(LonToX(vertex[vNum].lon),LatToY(vertex[vNum].lat)-7);\n";
	html << "	c.lineTo(LonToX(vertex[vNum].lon),LatToY(vertex[vNum].lat)+7);\n";
	html << "	c.moveTo(LonToX(vertex[vNum].lon)-7,LatToY(vertex[vNum].lat));\n";
	html << "	c.lineTo(LonToX(vertex[vNum].lon)+7,LatToY(vertex[vNum].lat));\n";
	html << "	c.stroke();\n";
	html << "	c.restore();\n";
	html << "}\n";
	html << "\n";
	html << "function VertexMarkers()\n";
	html << "{	for (vNum = 0; vNum < vertex.length; vNum++)\n";
	html << "	  if (VertexInViewport(vNum)) VertexMarker(vNum, \"#FFFF00\");\n";
	html << "}\n";
	html << "\n";
	html << "function VertexSelect(vNum)\n";
	html << "{	ClearInfo();\n";
	html << "	document.getElementById(\"InfoBox\").innerHTML =\n";
	html << "		\"<hr><big>\" + vertex[vNum].label + \"</big>\" +\n";
	html << "		\"<br><small>Vertex \" + vNum +\n";
	html << "		\"<br><a href='http://www.openstreetmap.org/?lat=\" +\n";
	html << "			vertex[vNum].lat.toFixed(6) + \"&lon=\" +\n";
	html << "			vertex[vNum].lon.toFixed(6) + \"'>Coords:</a> \" +\n";
	html << "		vertex[vNum].lat.toFixed(6) + \",\" + vertex[vNum].lon.toFixed(6) +\n";
	html << "		\"</small>\";\n";
	html << "	VertexMarker(vNum, \"#FF0000\");\n";
	html << "	selectedV = vNum;\n";
	html << "}\n";
	html << "\n";
	html << "function VertexTable(vNum)\n";
	html << "{	VertexSelect(vNum);\n";
	html << "	PanToXY(LonToX(vertex[vNum].lon), LatToY(vertex[vNum].lat));\n";
	html << "}\n";
	html << "\n";
	html << "function vInfo()\n";
	html << "{	var html = \"\";\n";
	html << "	// vertex table\n";
	html << "	html += \"<table border=1 cellspacing=0>\";\n";
	html << "	html += \"<thead><tr><th>#</th><th>Coords</th><th>Vertex Name</th></tr></thead>\"\n";
	html << "	html += \"<tbody>\";\n";
	html << "	for (vNum = 0; vNum < vertex.length; vNum++)\n";
	html << "		html += \"<tr onclick='VertexTable(\"+vNum+\")' class='vtr'><td>\"\n";
	html << "		+vNum+\"</td><td>\"+vertex[vNum].lat+\",<br>\"+vertex[vNum].lon+\"</td><td>\"+vertex[vNum].label+\"</td></tr>\";\n";
	html << "	html += \"</tbody></table><br>\";\n";
	html << "	// edge table\n";
	html << "	html += \"<table border=1 cellspacing=0>\";\n";
	html << "	html += \"<thead><tr><th>#</th><th>Route Name(s)</th><th>Endpoints</th></tr></thead>\"\n";
	html << "	html += \"<tbody>\";\n";
	html << "	for (eNum = 0; eNum < edge.length; eNum++)\n";
	html << "		html += \"<tr onclick='EdgeTable(\"+eNum+\")' class='vtr'><td>\"\n";
	html << "		+eNum+\"</td><td>\"+edge[eNum].label+\"</td><td>\"\n";
	html << "		+edge[eNum].begin+\": \"+vertex[edge[eNum].begin].label+\" <-> \"\n";
	html << "		+edge[eNum].end+\": \"+vertex[edge[eNum].end].label+\"</td></tr>\";\n";
	html << "	html += \"</tbody></table>\";\n";
	html << "\n";
	html << "	document.getElementById(\"vInfo\").innerHTML = html;\n";
	html << "}\n";
	html << "\n";
	html << "function XToLon(x)	{ return x/ScaleFac+MinLon; }\n";
	html << "function YToLat(y)	{ return amerc((canvas.height-y-1)/ScaleFac+MinMerc); }\n";
	html << "\n";
	html << "function ZoomIn()\n";
	html << "{	MinLat = YToLat(0.75*canvas.height-0.5);\n";
	html << "	MaxLat = YToLat(0.25*canvas.height);\n";
	html << "	MaxLon = XToLon(0.75*canvas.width-0.5);\n";
	html << "	MinLon = XToLon(0.25*canvas.width);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	//ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n";
	html << "\n";
	html << "function ZoomOut()\n";
	html << "{	MinLat = YToLat(1.5*canvas.height-2);\n";
	html << "	MaxLat = YToLat(-0.5*canvas.height);\n";
	html << "	MaxLon = XToLon(1.5*canvas.width-2);\n";
	html << "	MinLon = XToLon(-0.5*canvas.width);\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	//ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n";
	html << "\n";
	html << "function ZoomWheel(e)\n";
	html << "{	e.preventDefault();\n";
	html << "\n";
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
	html << "	}\n";
	html << "\n";
	html << "	SetScaleFac();\n";
	html << "	MinMerc = merc(MinLat);\n";
	html << "	//ShowBounds();\n";
	html << "	ClearCanvas();\n";
	html << "	RenderMap();\n";
	html << "}\n";
	html << "\n";
	html << "//John Pound - initialize canvas\n";
	html << "var canvas = document.getElementsByTagName(\"canvas\")[0];\n";
	html << "\n";
	html << "reset();\n";
	html << "vInfo();\n";
	html << "\n";
	html << "// JavaScript ends here\n";
	html << "</script>\n";
	html << "</body>\n";
	html << "</html>\n";
}
