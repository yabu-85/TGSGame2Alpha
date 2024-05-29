#include "Triangle.h"
#include "../Engine/Fbx.h"
#include "../Engine/Direct3D.h"

Triangle::Triangle(XMFLOAT3& p1, XMFLOAT3& p2, XMFLOAT3& p3)
{
	position[0] = XMLoadFloat3(&p1);
	position[1] = XMLoadFloat3(&p2);
	position[2] = XMLoadFloat3(&p3);

	XMVECTOR edge1 = position[1] - position[0];
	XMVECTOR edge2 = position[2] - position[0];
	normal = XMVector3Normalize(XMVector3Cross(edge1, edge2));

}

XMVECTOR Triangle::GetPosition(int index)
{
	return position[index];
}

XMVECTOR Triangle::GetNormal()
{
	return normal;
}

// 30“x‚ðƒ‰ƒWƒAƒ“‚É•ÏŠ·
const float MaxAngleRad = XMConvertToRadians(30.0f);
const XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

bool Triangle::IsMovable()
{
	float dotProduct = XMVectorGetX(XMVector3Dot(normal, up));
	float angleRad = acosf(dotProduct);
	bool b = angleRad <= MaxAngleRad;
	return b;
}

void Triangle::RayCast(RayCastData* data)
{
	BOOL  hit = FALSE;
	float dist = 0.0f;
	XMVECTOR normal = XMVectorZero();
	hit = Direct3D::Intersect(data->start, data->dir, position[0], position[1], position[2], &dist, &normal);

	if (hit && dist < data->dist)
	{
		data->hit = TRUE;
		data->dist = dist;
	}
}
