#include "Graphics/Graphics.h"
#include "SceneEnd.h"
#include"Input/Input.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
#include"Camera.h"

//初期化
void SceneEnd::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/Title.png");
}

//終了化
void SceneEnd::Finalize()
{
	//スプライト終了化
	if (sprite != nullptr)
	{
		delete sprite;
		sprite = nullptr;
	}
}

//更新処理
void SceneEnd::Update(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	//なにかボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A
		/* | GamePad::BTN_B
			| GamePad::BTN_X
			| GamePad::BTN_Y*/
		;
	if (gamePad.GetButtonDown() & anyButton)
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
	}
}

//描画処理
void SceneEnd::Render()
{
	// ======== 2Dスプライト描画 ========
	{
		Graphics& graphics = Graphics::Instance();
		ID3D11DeviceContext* dc = graphics.GetDeviceContext();
		RenderState* renderState = graphics.GetRenderState();

		ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
		ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

		// 画面クリア＆レンダーターゲット設定
		FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
		dc->ClearRenderTargetView(rtv, color);
		dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		dc->OMSetRenderTargets(1, &rtv, dsv);

		// 2Dスプライト描画
		{
			// タイトル（スプライト）描画
			float screenWidth = static_cast<float>(graphics.GetScreenWidth());
			float screenHeight = static_cast<float>(graphics.GetScreenHeight());
			sprite->Render(dc,				//&rc
				0, 0, 						//dx , dy , dz
				screenWidth, screenHeight,	//dw , dh
				0,							//angle
				1, 1, 0, 1);				//color
		}
	}
}

//GUI描画
void SceneEnd::DrawGUI()
{

}