#pragma once

#include "Creature.h"
#include"Graphics/Model.h"
#include"Item.h"

//ƒS[ƒ‹
class Goal : public Creature {
public:
	Goal();
	~Goal() override;
	//XVˆ—
	void Update(float elapsedTime) override;
	//•`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

private:
	Model* model = nullptr;

};