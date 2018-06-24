#pragma once
//Giperion September 2016
//xrLC Redux project
//OBJ Exporter for light debugging

typedef xr_map<const Vertex*, DWORD> VertexIndexMap;

//DEFINE_MAP(const Vertex*, DWORD, VertexIndexMap, VertexIndexMapValue)
class PLYExporter
{
public:
	static void ExportAsPly(xr_string FileName, vecVertex& Vertexes, vecFace& Faces);
	static void ExportAsPly(xr_string FileName, xr_vector<Fvector>& Vertexes, xr_vector<PolyIndexes>& Indexes);
};