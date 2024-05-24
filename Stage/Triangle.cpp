#include "Triangle.h"
#include "../Engine/Fbx.h"
#include "../Engine/Direct3D.h"

Triangle::Triangle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3)
{
	position[0] = p1;
	position[1] = p2;
	position[2] = p3;

	XMVECTOR edge1 = XMLoadFloat3(&position[1]) - XMLoadFloat3(&position[0]);
	XMVECTOR edge2 = XMLoadFloat3(&position[2]) - XMLoadFloat3(&position[0]);
	XMVECTOR nor = XMVector3Normalize(XMVector3Cross(edge1, edge2));
	normal = nor;
}

std::vector<XMFLOAT3> Triangle::GetAllPosition()
{
	return { position[0], position[1], position[2] };
}

bool Triangle::IsMovable()
{
	XMVECTOR v = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	float dotProduct = XMVectorGetX(XMVector3Dot(normal, v));
	float angle = XMConvertToDegrees(dotProduct);
	return angle <= 30.0f;
}

void Triangle::RayCast(RayCastData* data)
{
	BOOL  hit = FALSE;
	float dist = 0.0f;
	hit = Direct3D::Intersect(data->start, data->dir, position[0], position[1], position[2], &dist);

	if (hit && dist < data->dist)
	{
		data->hit = TRUE;
		data->dist = dist;
	}
}
