#pragma once

#include"Graphics/Model.h"
#include"Creature.h"

//Ô
class Dustbox : public Creature {
public:
	Dustbox();
	~Dustbox() override;
	//XVˆ—
	void Update(float elapsedTime) override;
	//•`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

private:
	Model* model = nullptr;

};