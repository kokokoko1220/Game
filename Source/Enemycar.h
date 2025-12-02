#pragma once

#include"Graphics/Model.h"
#include"Enemy.h"

//Ô
class Enemycar : public Enemy {
public:
	Enemycar();
	~Enemycar() override;
	//XVˆ—
	void Update(float elapsedTime) override;
	//•`‰æˆ—
	void Render(ID3D11DeviceContext* dc, Shader* shader) override;

private:
	Model* model = nullptr;

};