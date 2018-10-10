#include "stdafx.h"
#include "../xrLC_Light/xrface.h"
#include "../xrLC_Light/xrFaceDefs.h"
#include "../xrLC_Light/xrRayDefinition.h"
#include "PLY_Exporter.h"

#define TEXT_LINE(text) text##"\r\n"

const char* CachedNewLine = "\r\n";


DWORD GetVertexID(const Vertex* Vert, VertexIndexMap& VertexIndexes)
{
	auto Iter = VertexIndexes.find(Vert);
	if (Iter == VertexIndexes.end()) return -1;
	return Iter->second;
}


void PLYExporter::ExportAsPly(xr_string FileName, vecVertex& Vertices, vecFace& Faces)
{
	xr_string PlyContent;

	PlyContent += TEXT_LINE("ply");
	PlyContent += TEXT_LINE("format ascii 1.0");
	PlyContent += TEXT_LINE("comment X-Ray Light Compiler Redux debug output");

	string64 ElementDesc; ZeroMemory(ElementDesc, sizeof(string64));
	xr_sprintf(ElementDesc, "element vertex %d\r\n", Vertices.size());
	PlyContent += ElementDesc;
	ZeroMemory(ElementDesc, sizeof(string64));

	PlyContent += TEXT_LINE("property float x");
	PlyContent += TEXT_LINE("property float y");
	PlyContent += TEXT_LINE("property float z");
	PlyContent += TEXT_LINE("property float nx");
	PlyContent += TEXT_LINE("property float ny");
	PlyContent += TEXT_LINE("property float nz");
	PlyContent += TEXT_LINE("property uchar red");
	PlyContent += TEXT_LINE("property uchar green");
	PlyContent += TEXT_LINE("property uchar blue");

	xr_sprintf(ElementDesc, "element face %d\r\n", Faces.size());
	PlyContent += ElementDesc;

	PlyContent += TEXT_LINE("property list uchar uint vertex_indices");
	PlyContent += TEXT_LINE("end_header");

	DWORD IndexCounter = 0;
	VertexIndexMap IndexMap;
	//write all vertices
	string128 valueBuff;
	ZeroMemory(valueBuff, sizeof(string128));
	for (Vertex* Vert : Vertices)
	{
		Vert->normalFromAdj();

		_gcvt_s(valueBuff, Vert->P.x, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, Vert->P.y, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, Vert->P.z, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, Vert->N.x, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, Vert->N.y, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, Vert->N.z, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		float VertexColor = Vert->C.h._r();
		int VertexColorInt = (int)floor ((255.0f * VertexColor));

		xr_sprintf(valueBuff, "%d %d %d", VertexColorInt, VertexColorInt, VertexColorInt);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));

		PlyContent.append(CachedNewLine);

		IndexMap.insert(std::pair<const Vertex*, DWORD>(Vert, IndexCounter));
		IndexCounter++;
	}

	//write poly normals

	for (const Face* FaceObj : Faces)
	{
		DWORD FirstIndex  = GetVertexID(FaceObj->v[0], IndexMap);
		DWORD SecondIndex = GetVertexID(FaceObj->v[1], IndexMap);
		DWORD ThirdIndex  = GetVertexID(FaceObj->v[2], IndexMap);

		xr_sprintf(valueBuff, "3 %d %d %d\r\n", FirstIndex, SecondIndex, ThirdIndex);

		PlyContent.append(valueBuff);
	}

	//save a file
	IWriter* FileDesc = FS.w_open(FileName.c_str());
	FileDesc->w(PlyContent.data(), PlyContent.length());
	FileDesc->flush();
	FS.w_close(FileDesc);
}

void PLYExporter::ExportAsPly(xr_string FileName, xr_vector<Fvector>& Vertexes, xr_vector<PolyIndexes>& Indexes)
{
	xr_string PlyContent;

	PlyContent += TEXT_LINE("ply");
	PlyContent += TEXT_LINE("format ascii 1.0");
	PlyContent += TEXT_LINE("comment X-Ray Light Compiler Redux debug output");

	string64 ElementDesc; ZeroMemory(ElementDesc, sizeof(string64));
	xr_sprintf(ElementDesc, "element vertex %d\r\n", Vertexes.size());
	PlyContent += ElementDesc;
	ZeroMemory(ElementDesc, sizeof(string64));

	PlyContent += TEXT_LINE("property float x");
	PlyContent += TEXT_LINE("property float y");
	PlyContent += TEXT_LINE("property float z");
	PlyContent += TEXT_LINE("property float nx");
	PlyContent += TEXT_LINE("property float ny");
	PlyContent += TEXT_LINE("property float nz");
	PlyContent += TEXT_LINE("property uchar red");
	PlyContent += TEXT_LINE("property uchar green");
	PlyContent += TEXT_LINE("property uchar blue");

	xr_sprintf(ElementDesc, "element face %d\r\n", Indexes.size());
	PlyContent += ElementDesc;

	PlyContent += TEXT_LINE("property list uchar uint vertex_indices");
	PlyContent += TEXT_LINE("end_header");

	//write all vertices
	string128 valueBuff;
	ZeroMemory(valueBuff, sizeof(string128));
	for (const Fvector& Vert : Vertexes)
	{
		_gcvt_s(valueBuff, Vert.x, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, Vert.y, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, Vert.z, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, 0.0f, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, 0.0f, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		_gcvt_s(valueBuff, 0.0f, 6);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));
		PlyContent.append(1, ' ');

		float VertexColor = 1.0f;
		int VertexColorInt = (int)floor((255.0f * VertexColor));

		xr_sprintf(valueBuff, "%d %d %d", VertexColorInt, VertexColorInt, VertexColorInt);
		PlyContent.append(valueBuff);
		ZeroMemory(valueBuff, sizeof(string128));

		PlyContent.append(CachedNewLine);
	}

	//write poly normals

	for (const PolyIndexes& IndexObj : Indexes)
	{
		xr_sprintf(valueBuff, "3 %d %d %d\r\n", IndexObj.Index1, IndexObj.Index2, IndexObj.Index3);

		PlyContent.append(valueBuff);
	}

	//save a file
	IWriter* FileDesc = FS.w_open(FileName.c_str());
	FileDesc->w(PlyContent.data(), PlyContent.length());
	FileDesc->flush();
	FS.w_close(FileDesc);
}
