#pragma once
#include"Graphics/Model.h"
#include"Enemy.h"

//車
class EnemyPolice : public Enemy {
public:
	EnemyPolice();
	~EnemyPolice() override;
	//更新処理
	void Update(float elapsedTime) override;
	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

	bool EnemyView();

private:
	Model* model = nullptr;
	int State = 0;
	float ViewRadius = 10.0f;
	float ViewAngle = 90.0f;

	int frameCounter = 0; // フレームカウンター
	const int RAYCAST_INTERVAL = 30; // フレームに一度実行する
};