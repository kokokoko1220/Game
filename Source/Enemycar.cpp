#include"Enemycar.h"
#include"Enemy.h"
#include"Graphics/Graphics.h"


//デバックプリミティブ描画
void Enemy::DrawDebugPrimitive() {
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
	
	//衝突判定用のデバック球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

	//衝突判定用のデバック円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

}

//コンストラクタ
Enemycar::Enemycar() {
//車モデルを読み込み
 
model = new Model("Data/Model/Slime/Slime.mdl");

	//スケーリング
	scale.x = scale.y = scale.z = 0.01f;

	//幅、高さ設定
	radius = 0.5f;
	height = 1.0f;

	id = Enemy::Slime;
}

//デストラクタ
Enemycar::~Enemycar() {
//車モデルを破棄
delete model;
	}

//更新処理
void Enemycar::Update(float elapsedTime) {
	//オブジェクト行列を更新
	// ノックバック処理
	if (knockbackTimer > 0.0f)
	{
		knockbackTimer -= elapsedTime;

		// 現在位置にノックバック速度を加算
		position.x += knockbackVel.x * elapsedTime;
		position.z += knockbackVel.z * elapsedTime;

		// 徐々に減衰させる（摩擦）
		knockbackVel.x *= 0.85f;
		knockbackVel.z *= 0.85f;

		// ノックバック中は通常処理を停止
		if (knockbackTimer > 0.0f)
	UpdateTransform();
	//モデル行列更新
	model->UpdateTransform(transform);
			return;
	}
	UpdateTransform();
	//モデル行列更新
	model->UpdateTransform(transform);
}
//描画処理
void Enemycar::Render(ID3D11DeviceContext* dc, Shader* shader) {
	shader->Draw(dc, model);
}