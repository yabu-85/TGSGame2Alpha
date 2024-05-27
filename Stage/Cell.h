#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;
class Triangle;
struct RayCastData;
class SphereCollider;

class Cell
{
	//cube‚Ìˆê•Ó‚Ì’·‚³
	float length_ = 0;

	XMFLOAT3 min_;
	XMFLOAT3 max_;

	//Triangles‚ÌƒŠƒXƒg
	std::vector<Triangle> floarTriangles_;
	std::vector<Triangle> wallTriangles_;
	
	//Triangle‚ª”ÍˆÍ“à‚©’²‚×‚éŠÖ”
	bool IsPointInAABB(XMFLOAT3& point);
	bool IsTriangleInAABB(Triangle& tri);
	bool IntersectSegmentAABB(XMFLOAT3& p0, XMFLOAT3& p1);
	bool IsPointInTriangle(XMFLOAT3& pt, XMVECTOR& v0, XMVECTOR& v1, XMVECTOR& v2);
	bool IsAABBInsideTriangle(Triangle& triangle);
	bool IntersectTriangleAABB(Triangle& tri);

public:
	Cell();

	void SetPosLeng(XMFLOAT3 pos, float leng);

	//OŠpƒ|ƒŠƒSƒ“‚ªAAB‚ÉG‚ê‚Ä‚¢‚½‚ç‚±‚ÌCELL‚É“o˜^
	bool SetTriangle(Triangle& t);

	//FloarTriangle‚É“–‚½‚Á‚½ê‡‚ÌÅ¬‹——£‚ğ•Ô‚·
	bool SegmentVsFloarTriangle(RayCastData* _data);
	//Wall‚Æ‚â‚é
	bool SegmentVsWallTriangle(RayCastData* _data);

	bool SphereVsTriangle(SphereCollider* collid, XMVECTOR& push);

};