#pragma once


#include "Graphics/Shader.h"
#include "Character.h"

//アイテム
class Item : public Character {
public:
	Item() {};
	~Item() override {};
	//更新処理
	virtual void Update(float elapsedTime) = 0;
	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//デバックプリミティブ描画
	virtual void DrawDebugPrimitive();

	void Destroy();
};