
#include "Graphics/Graphics.h"
#include "SceneTutorial.h"
#include "Input/Input.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "Camera.h"

//������
void SceneTutorial::Initialize()
{
	//�X�v���C�g������
	sprite1 = new Sprite("Data/Sprite/Tutorial_1.png");
	sprite2 = new Sprite("Data/Sprite/Tutorial_2.png");
}

//�I����
void SceneTutorial::Finalize()
{
	//�X�v���C�g�I����
	if (sprite1 != nullptr)
	{
		delete sprite1;
		sprite1 = nullptr;
	}
}

//�X�V����
void SceneTutorial::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//�Ȃɂ��{�^������������Q�[���V�[���֐؂�ւ�
	const GamePadButton anyButton =
		GamePad::BTN_A
		| GamePad::BTN_B
		| GamePad::BTN_X
		| GamePad::BTN_Y
		;

	if (gamePad.GetButtonDown() & anyButton)
	{
		if (page == 1)
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
		else
			page++;
	}
}

//�`�揈��
void SceneTutorial::Render()
{
	// ======== 2D�X�v���C�g�`�� ========
	{
		Graphics& graphics = Graphics::Instance();
		ID3D11DeviceContext* dc = graphics.GetDeviceContext();
		RenderState* renderState = graphics.GetRenderState();

		ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
		ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

		// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, dsv);

		// 2D�X�v���C�g�`��
		{
			// �^�C�g���i�X�v���C�g�j�`��
			float screenWidth = static_cast<float>(graphics.GetScreenWidth());
			float screenHeight = static_cast<float>(graphics.GetScreenHeight());

			if (page == 0)
				sprite1->Render(dc, 0, 0, screenWidth, screenHeight, 0, 1, 1, 1, 1);
			else
				sprite2->Render(dc, 0, 0, screenWidth, screenHeight, 0, 1, 1, 1, 1);
		}
	}
}

//GUI�`��
void SceneTutorial::DrawGUI()
{

}