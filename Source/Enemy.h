#pragma once

#include "Graphics/Shader.h"
#include "Character.h"

//エネミー
class Enemy : public Character {
public:
	Enemy() {};
	virtual ~Enemy() override {};
	DirectX::XMFLOAT3 knockbackVel = { 0.0f, 0.0f, 0.0f };//ノックバック用
	float knockbackTimer = 0.0f;

	// 追加
	float disappearTimer = 0.0f;  // ノックバック後の消滅タイマー
	bool toDisappear = false;     // 消滅フラグ

	//更新処理
	virtual void Update(float elapsedTime) = 0;
	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//デバックプリミティブ描画
	virtual void DrawDebugPrimitive();

	bool isDead = false;
	void Destroy() { isDead = true; }
	bool IsDead() const { return isDead; }
};