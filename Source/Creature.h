#pragma once

#include "Graphics/Shader.h"
#include "Character.h"

//物
class Creature : public Character {
public:
	Creature() {};
	~Creature() override {};
	//更新処理
	virtual void Update(float elapsedTime) = 0;
	//描画処理
	virtual void Render(ID3D11DeviceContext* dc, Shader* shader) = 0;

	//デバックプリミティブ描画
	virtual void DrawDebugPrimitive();

};