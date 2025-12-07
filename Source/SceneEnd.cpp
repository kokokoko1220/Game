#include "Graphics/Graphics.h"
#include "SceneEnd.h"
#include"Input/Input.h"
#include "SceneManager.h"
#include "SceneLoading.h"
#include "SceneTitle.h"
#include"Camera.h"
#include"GameUI.h"
#include"SoundManager.h"
//初期化
void SceneEnd::Initialize()
{
	//スプライト初期化
	sprite = new Sprite("Data/Sprite/GameClear.png");
	face = new Sprite("Data/Font/font1.png");   // ← 追加
    SoundManager::Instance().Initialize();
    SoundManager::Instance().LoadSE("title", L"Data/Audio/gole.wav");
    SoundManager::Instance().PlaySELoop("title", 0.5f);
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
	if (face) { delete face;   face = nullptr; } // ← 追加
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
        SoundManager::Instance().StopAll();
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTitle));
	}
}

//描画処理
void SceneEnd::Render()
{
	//// ======== 2Dスプライト描画 ========
	//{
	//	Graphics& graphics = Graphics::Instance();
	//	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	//	RenderState* renderState = graphics.GetRenderState();

	//	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	//	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	//	// 画面クリア＆レンダーターゲット設定
	//	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	//	dc->ClearRenderTargetView(rtv, color);
	//	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//	dc->OMSetRenderTargets(1, &rtv, dsv);

	//	// 2Dスプライト描画
	//	{
	//		// タイトル（スプライト）描画
	//		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	//		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
	//		sprite->Render(dc,				//&rc
	//			0, 0, 						//dx , dy , dz
	//			screenWidth, screenHeight,	//dw , dh
	//			0,							//angle
	//			1, 1, 1, 1);				//color
	//	}
	//}
  
        // ======== 2Dスプライト描画 ========
        Graphics& graphics = Graphics::Instance();
        ID3D11DeviceContext* dc = graphics.GetDeviceContext();
        ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
        ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

        // 画面クリア＆レンダーターゲット設定
        const FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f }; // RGBA(0.0～1.0)
        dc->ClearRenderTargetView(rtv, color);
        dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        dc->OMSetRenderTargets(1, &rtv, dsv);

        // 画面サイズ
        float screenWidth = static_cast<float>(graphics.GetScreenWidth());
        float screenHeight = static_cast<float>(graphics.GetScreenHeight());

        // 背景スプライト
        if (sprite)
        {
            sprite->Render(dc,
                0, 0,                         // x, y
                screenWidth, screenHeight,    // w, h
                0,                            // angle
                1, 1, 1, 1);                  // rgba
        }

        // ===== テキスト描画（font1.png / face->textout）=====
	   // ======== 時間表示（font1.png / textout）========
    // --- ここから追加（背景を描いた“直後”に置く） ---
        if (face) {
            // 1) アルファブレンドを有効化（GameUIと同じ）
            ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();
            ID3D11BlendState* alpha = nullptr;
            D3D11_BLEND_DESC bd{};
            bd.RenderTarget[0].BlendEnable = TRUE;
            bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            Graphics::Instance().GetDevice()->CreateBlendState(&bd, &alpha);
            const float bf[4] = { 0,0,0,0 };
            dc->OMSetBlendState(alpha, bf, 0xffffffff);

            // 2) 表示用テキストを作る（finalTime 方式の場合）
            char buf[64];
            std::snprintf(buf, sizeof(buf), "%.2f", finalTime);  // ← UI参照なら ui->gametimer に

            // 3) 画面中央に描く（中央寄せ計算）
            float W = (float)Graphics::Instance().GetScreenWidth();
            float H = (float)Graphics::Instance().GetScreenHeight();
            const float cw = 64.0f;   // 文字幅
            const float ch = 64.0f;   // 文字高さ
            size_t n = std::strlen(buf);
            float x = (W - (float)n * cw) * 0.5f;
            float y = H * 0.55f;

            // 4) 描画（影→本体）
            face->textout(dc, buf, x + 3, y + 3, cw, ch, 0, 0, 0, 1);
            face->textout(dc, buf, x, y, cw, ch, 1, 1, 1, 1);

            // 5) 後始末
            dc->OMSetBlendState(nullptr, bf, 0xffffffff);
            if (alpha) alpha->Release();
        }
        // --- ここまで追加 ---


    

}

//GUI描画
void SceneEnd::DrawGUI()
{

}