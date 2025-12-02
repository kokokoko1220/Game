
#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

class SceneTutorial :public Scene
{
public:
	SceneTutorial() {}
	~SceneTutorial() override {}

	//������
	void Initialize() override;

	//�I����
	void Finalize() override;

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render() override;

	//GUI�`��
	void DrawGUI() override;

private:
	Sprite* sprite1 = nullptr;
	Sprite* sprite2 = nullptr;

	int page = 0;
};