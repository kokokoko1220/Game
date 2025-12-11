
#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class SceneTutorial :public Scene
{
public:
	SceneTutorial() {}
	~SceneTutorial() override {}

	
	void Initialize() override;

	
	void Finalize() override;

	
	void Update(float elapsedTime) override;

	
	void Render() override;

	//GUI
	void DrawGUI() override;

private:
	Sprite* sprite1 = nullptr;
	Sprite* sprite2 = nullptr;

	int page = 0;
};