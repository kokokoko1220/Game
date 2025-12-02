#pragma once
#include "Scene.h"
#include"Stage.h"
#include"Player.h"
#include"CameraController.h"
#include"GameUI.h"
// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() {}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render();

	void DrawGUI() {};
private:
	Stage* stage = nullptr;
	Player* player = nullptr;
	CameraController* cameraController = nullptr;
	GameUI* gameUI = nullptr;

	//int MAX_ENEMY = 5;
	const int MAX_CAR = 3;
	const int MAX_POLICE = 2;
	int MAX_ITEM = 14; // 同時出現数の上限
	float lastIndex = 0;// スポーン位置履歴
	float lastIndexP = 0;// スポーン位置履歴
	float lastIndexBO = 0;// ボトルスポーン位置履歴
	float spawnInterval = 2.0f;  // スポーン間隔（秒）
	float spawnIntervalBO = 1.5f;  // スポーン間隔（秒）
	float spawnTimer = 0.0f;// 経過時間
	float spawnTimerBO = 0.0f;// ボトル経過時間
};