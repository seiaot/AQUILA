#include "Lib/Graphics/ResMesh.h"

#ifdef _DEBUG
#pragma comment( lib, "assimp-vc142-mtd.lib" )
#else
#pragma comment( lib, "assimp-vc142-mt.lib" )
#endif

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <codecvt>
#include <cassert>

using namespace DirectX;
using namespace std;

#define FMT_FLOAT4 DXGI_FORMAT_R32G32B32A32_FLOAT
#define FMT_FLOAT3 DXGI_FORMAT_R32G32B32_FLOAT
#define FMT_FLOAT2 DXGI_FORMAT_R32G32_FLOAT
#define APPEND D3D12_APPEND_ALIGNED_ELEMENT
#define IL_VERTEX D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA

namespace Lib {
namespace Graphics {

const D3D12_INPUT_ELEMENT_DESC MeshVertex::InputElements[] = {
	{"POSITION",0,FMT_FLOAT3,0, APPEND,IL_VERTEX,0},
	{"NORMAL"  ,0,FMT_FLOAT3,0, APPEND,IL_VERTEX,0},
	{"COLOR"  ,0,FMT_FLOAT4,0, APPEND,IL_VERTEX,0},
	{"TEXCOORD",0,FMT_FLOAT2,0, APPEND,IL_VERTEX,0},
	{"TANGENT" ,0,FMT_FLOAT3,0, APPEND,IL_VERTEX,0},
	{"ADD_COLOR" ,0,FMT_FLOAT3,0, APPEND,IL_VERTEX,0},
};

const D3D12_INPUT_LAYOUT_DESC MeshVertex::InputLayout = {
	MeshVertex::InputElements,
	MeshVertex::InputElementCount,
};

static_assert(sizeof(MeshVertex) == 72, "Vertex struct/layout missmatch");

string ToUTF8(const std::wstring& value)
{
	auto length = WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, nullptr, 0, nullptr, nullptr);
	auto buffer = new char[length];

	WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, buffer, length, nullptr, nullptr);

	string result(buffer);
	delete[] buffer;
	buffer = nullptr;

	return result;
}

namespace {

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();

	bool Load(
		const wchar_t* filename,
		vector<ResMesh>& meshes,
		vector<ResMaterial>& materials
	);

private:

	void ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh);
	void ParseMaterial(ResMaterial& dstMaterial, const aiMaterial* pSrcMaterial);
};

MeshLoader::MeshLoader()
{

}

MeshLoader::~MeshLoader()
{

}

bool MeshLoader::Load(
	const wchar_t* filename,
	vector<ResMesh>& meshes,
	vector<ResMaterial>& materials
)
{
	if (filename == nullptr)
		return false;

	auto path = ToUTF8(filename);

	Assimp::Importer importer;

	int flag = 0;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_PreTransformVertices;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_GenSmoothNormals;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_RemoveRedundantMaterials;
	flag |= aiProcess_OptimizeMeshes;

	//ファイルを読み込み
	auto pScene = importer.ReadFile(path, flag);

	if (pScene == nullptr)
		return false;

	//メッシュのメモリを確保
	meshes.clear();
	meshes.resize(pScene->mNumMeshes);

	//メッシュデータを変換
	for (size_t i = 0; i < meshes.size(); ++i) {
		const auto pMesh = pScene->mMeshes[i];
		ParseMesh(meshes[i], pMesh);
	}

	//マテリアルのメモリを確保
	materials.clear();
	materials.resize(pScene->mNumMaterials);

	//マテリアルデータを変換
	for (size_t i = 0; i < materials.size(); ++i) {
		const auto pMaterial = pScene->mMaterials[i];
		ParseMaterial(materials[i], pMaterial);
	}

	//不要になったのでクリア
	pScene = nullptr;

	return true;
}

void MeshLoader::ParseMesh(ResMesh& dstMesh, const aiMesh* pSrcMesh)
{
	//マテリアル番号を設定
	dstMesh.MaterialId = pSrcMesh->mMaterialIndex;

	aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	XMFLOAT4 color = { 1.0f,1.0f ,1.0f ,1.0f };
	XMFLOAT3 addcolor = { 0.0f,0.0f ,0.0f };

	//頂点データのメモリを確保
	dstMesh.Vertices.resize(pSrcMesh->mNumVertices);

	for (UINT i = 0; i < pSrcMesh->mNumVertices; ++i)
	{
		auto pPosition = &(pSrcMesh->mVertices[i]);
		auto pNormal = &(pSrcMesh->mNormals[i]);
		auto pTexCoord = (pSrcMesh->HasTextureCoords(0)) ? &(pSrcMesh->mTextureCoords[0][i]) : &zero3D;
		auto pTangent = (pSrcMesh->HasTangentsAndBitangents()) ? &(pSrcMesh->mTangents[i]) : &zero3D;

		dstMesh.Vertices[i] = MeshVertex(
			XMFLOAT3(pPosition->x, pPosition->y, pPosition->z),
			XMFLOAT3(pNormal->x, pNormal->y, pNormal->z),
			color,
			XMFLOAT2(pTexCoord->x, pTexCoord->y),
			XMFLOAT3(pTangent->x, pTangent->y, pTangent->z),
			addcolor
		);
	}

	//頂点インデックスのメモリを確保
	dstMesh.Indices.resize(pSrcMesh->mNumFaces * 3);

	for (UINT i = 0; i < pSrcMesh->mNumFaces; ++i) {
		const auto& face = pSrcMesh->mFaces[i];
		assert(face.mNumIndices == 3);

		UINT n = i * 3;
		dstMesh.Indices[n + 0] = face.mIndices[0];
		dstMesh.Indices[n + 1] = face.mIndices[1];
		dstMesh.Indices[n + 2] = face.mIndices[2];
	}
}

void MeshLoader::ParseMaterial(ResMaterial& dstMaterial, const aiMaterial* pSrcMaterial)
{
	//拡散反射成分
	{
		aiColor3D color(0.0f, 0.0f, 0.0f);

		if (pSrcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
		{
			dstMaterial.Diffuse.x = color.r;
			dstMaterial.Diffuse.y = color.g;
			dstMaterial.Diffuse.z = color.b;
		}
		else {
			dstMaterial.Diffuse.x = 0.5f;
			dstMaterial.Diffuse.y = 0.5f;
			dstMaterial.Diffuse.z = 0.5f;
		}
	}

	//鏡面反射成分
	{
		aiColor3D color(0.0f, 0.0f, 0.0f);

		if (pSrcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
		{
			dstMaterial.Specular.x = color.r;
			dstMaterial.Specular.y = color.g;
			dstMaterial.Specular.z = color.b;
		}
		else {
			dstMaterial.Specular.x = 0.0f;
			dstMaterial.Specular.y = 0.0f;
			dstMaterial.Specular.z = 0.0f;
		}
	}

	//鏡面反射強度
	{
		auto shininess = 0.0f;

		if (pSrcMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
		{
			dstMaterial.Shininess = shininess;
		}
		else {
			dstMaterial.Shininess = 0.0f;
		}
	}

	//ディフューズマップ
	{
		aiString path;

		if (pSrcMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
		{
			dstMaterial.DiffuseMap = string(path.C_Str());
		}
		else {
			dstMaterial.DiffuseMap.clear();
		}
	}
}
}

bool LoadMesh(
	const wchar_t* filename,
	vector<ResMesh>& meshes,
	vector<ResMaterial>& materials
)
{
	MeshLoader meshLoader;
	return meshLoader.Load(filename, meshes, materials);
}

}//namespace Graphics
}//namespace Lib