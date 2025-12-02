#pragma once
#include<DirectXMath.h>
#include"Graphics/Model.h"

//ヒット結果
struct HitResult {
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 normal = { 0,0,0 };
	float distance = 0.0f;
	int materialIndex = -1;
};

//コリジョン
class Collision {
	public :

		//球と球の当たり判定
		static bool IntersectSphereVsSphere(
			const DirectX::XMFLOAT3& positionA,
			float radiusA,
			const DirectX::XMFLOAT3& positionB,
			float radiusB,
			DirectX::XMFLOAT3& outPosition
		);

		//球と円柱の交差判定
		static	bool IntersectSphereVsCylinder(
			const DirectX::XMFLOAT3& spherePosition,
			float sphereRadius,
			const DirectX::XMFLOAT3& cylinderPosition,
			float cylinderRadius,
			float cylinderHeight,
			DirectX::XMFLOAT3& outCylinderPosition
		);

		////円柱と円柱の交差判定
		//static bool IntersectCylinderVsCylinder(
		//	const DirectX::XMFLOAT3& positionA,
		//	float radiusA,
		//	float heightA,
		//	const DirectX::XMFLOAT3& positionB,
		//	float radiusB,
		//	float heightB,
		//	DirectX::XMFLOAT3& outPositionB
		//);

		//レイとモデルの交差判定
		static	bool IntersectRayVsModel(
			const DirectX::XMFLOAT3& start,
			const DirectX::XMFLOAT3& end,
			const Model* model,
			HitResult& result
		);
		
		};