#pragma once


#include"Graphics/Model.h"
#include"Item.h"

//ボトル
class Bottle : public Item {
public:
	Bottle();
	~Bottle() override;
	//更新処理
	void Update(float elapsedTime) override;
	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

private:
	Model* model = nullptr;
	// ==== ここから追加 ====
	float totalTime = 0.0f;   // 経過時間（ふわふわアニメーション用）
	float floatHeight = 0.3f; // 上下の振れ幅（高さ）
	float floatSpeed = 2.0f;  // 揺れる速さ
	float rotationSpeed = 45.0f; // 度/秒
	// ==== ここまで追加 ====
};