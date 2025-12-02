#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
//タイトルシーン
class SceneEnd :public Scene
{
public:
	SceneEnd() {}
	~SceneEnd() override {}

	//初期化
	void Initialize() override;

	//終了化
	void Finalize() override;

	//更新時間
	void Update(float elapsedTime) override;

	//描画処理
	void Render() override;

	//GUI描画
	void DrawGUI() override;

private:
	Sprite* sprite = nullptr;
};