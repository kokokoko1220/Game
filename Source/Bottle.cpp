#include"ItemManager.h"
#include"Item.h"
#include"Graphics/Graphics.h"
#include "Bottle.h"
#include <cmath> // sin関数用
using namespace DirectX;
//デバックプリミティブ描画
void Item::DrawDebugPrimitive() {
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	//衝突判定用のデバック球を描画
	debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));
}

//コンストラクタ
Bottle::Bottle() {
	//モデルを読み込み

	model = new Model("Data/Model/Bottle_of_Rum/Bottle1.mdl");//ボトル

	//スケーリング
	scale.x = scale.y = scale.z = 0.004f;
	angle.z = 25;
	//position.x = 70.0f;
	//position.y = 8.0f;
}

//デストラクタ
Bottle::~Bottle() {
	//モデルを破棄
	delete model;
}

//更新処理
void Bottle::Update(float elapsedTime) {
	
	// 経過時間を蓄積
	static float totalTime = 0.0f;
	totalTime += elapsedTime;

	// === 見た目だけ上下に揺らす ===
	float floatHeight = 0.3f;   // 揺れの振幅（上下の高さ）
	float floatSpeed = 1.5f;    // 揺れる速さ
	float offsetY = sinf(totalTime * floatSpeed) * floatHeight;

	// 実際の当たり判定用の位置は動かさない
	XMFLOAT3 visualPos = position;
	visualPos.y += offsetY; // モデルだけ上下させる

	// 行列更新（見た目用位置を使用）
	XMMATRIX translation = XMMatrixTranslation(visualPos.x, visualPos.y, visualPos.z);
	XMMATRIX rotationZ = XMMatrixRotationZ(XMConvertToRadians(angle.z));
	XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);

	DirectX::XMMATRIX world = scaling * rotationZ * translation;
	
	DirectX::XMStoreFloat4x4(&transform, world);

	angle.y += rotationSpeed * elapsedTime;
	// モデル行列を更新
	model->UpdateTransform(transform);

	////オブジェクト行列を更新
	//UpdateTransform();
	////モデル行列更新
	//model->UpdateTransform(transform);
}

//描画処理
void Bottle::Render(ID3D11DeviceContext* dc, Shader* shader) {
	shader->Draw(dc, model);
}
