#include "GameUI.h"
#include "Graphics/Sprite.h"
#include "Graphics/Graphics.h"
#include "imgui.h"


void GameUI::Initialize()
{
    gauge = gauge_MAX;
    // スプライトの生成
    sprite = new Sprite("Data/Sprite/cage.png");
    sprite2 = new Sprite("Data/Sprite/yazirusi.png");
 
    Bottole = new Sprite("Data/Sprite/Bottole.png");

    face = new Sprite("Data/Font/font1.png");

    // ★ フォントを追加（これだけで文字がくっきり表示されます）
    //ImGuiIO& io = ImGui::GetIO();
   // io.Fonts->AddFontFromFileTTF("Data/Font/MyFont.ttf", 32.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

}

void GameUI::Finalize()
{
    // 安全な解放
    if (sprite)
    {
        delete sprite;
        sprite = nullptr;
    }

    if (sprite2)
    {
        delete sprite2;
        sprite2 = nullptr;
    }
    if (face) { delete face; face = nullptr; }
}

void GameUI::Update(float elapsedTime)
{
    gauge += static_cast<float>(5 * elapsedTime);


    SHORT keyState = GetAsyncKeyState(VK_SPACE);
    bool isPressed = (keyState & 0x8000);
    // 押した瞬間だけ反応
    //if (isPressed && !wasSpacePressed /*&& cool_time_switch == true*/)
    if (isPressed && !wasSpacePressed && item != 0)//kokoko
    {
        // スペース押した瞬間の処理


        gauge_UP_switch = true;
        cool_time_switch = false;

        --item;//kokoko
    }

    if (gauge >= gauge_MIN)
    {
        gauge = gauge_MIN;
    }
    if (gauge_UP_switch == true)
    {

        gauge -= static_cast<float>(80 * elapsedTime);
        time += static_cast<float>(elapsedTime);

        if (time > 1)
        {
            gauge_UP_switch = false;
            time = 0;
        }
    }
    /*if (cool_time_switch == false)
    {
        cool_time += static_cast<float>(elapsedTime);
        if (cool_time >= 5)
        {
            cool_time_switch = true;
            cool_time = 0;
        }
    }*/
    if (gauge >= gauge_MIN)
    {
        gauge = gauge_MIN;
    }
    if (gauge <= gauge_MAX)
    {
        gauge = gauge_MAX;
    }



    if (GetAsyncKeyState('T') & 0x0001)
    {
        clearcount++;   //ここで配達できた個数のカウント
    }
}

void GameUI::Render()
{
    if (!sprite || !sprite2)
        return;

    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

    dc->OMSetRenderTargets(1, &rtv, dsv);

    float screenWidth = static_cast<float>(graphics.GetScreenWidth());
    float screenHeight = static_cast<float>(graphics.GetScreenHeight());

    //=============================================
    // ★ アルファブレンド有効化
    //=============================================
   

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    graphics.GetDevice()->CreateBlendState(&blendDesc, &alphaBlend);

    float blendFactor[4] = { 0, 0, 0, 0 };
    dc->OMSetBlendState(alphaBlend, blendFactor, 0xffffffff);

    //=============================================
    // スプライト描画
    //=============================================

    // cage.png
    sprite->Render(dc,
        screenWidth - 64, 300, 84, 320,  // x, y, 幅, 高さ
        0,
        1, 1, 1, 1);

    Bottole->Render(dc,
        screenWidth / 2, screenHeight - 128 + 20, 128, 128,  // x, y, ��, ����
        0,
        1, 1, 1, 1);

    // yazirusi.png（透過付き）
    sprite2->Render(dc,
        screenWidth - 64 - 64, gauge, 64, 64,
        0,
        1, 1, 1, 1);





    //=========================
   // ★ 文字（Sprite::textout 方式）
   //=========================
    if (face)
    {
        // 表示位置はお好みで
        const float textX = screenWidth - 128 - 20 - 160.0f;
        const float textY = 0;

        // 等幅ASCIIなので 1文字サイズを決める（拡大しないのが一番クッキリ）
        // font0.png が 256x256 の 16x16 グリッドなら、元セルは 16x16 px
        const float cellW = 64.0f;  // ここを 32 にすると拡大（多少ぼけます）
        const float cellH = 64.0f;

        // 文字列を組み立て（ASCIIのみ）
        char buf[128];
        std::snprintf(buf, sizeof(buf),
            "%3d/3",
            clearcount
        );

        // 白文字で描画（RGBA）
        face->textout(dc, buf, textX, textY, 64, 64, 1, 1, 1, 1);

        std::snprintf(buf, sizeof(buf),
            "%3d",
            //clearcount
        item//kokoko
            );
       // face->textout(dc, buf, screenWidth / 2 - 10, screenHeight - 64, 50, 50, 1, 1, 1, 1);
        face->textout(dc, buf, screenWidth / 2 - 30, screenHeight - 45, 50, 50, 1, 1, 1, 1);
        // 例：タイトルも出す
       // face->textout(dc, "POWER", textX, textY - 20.0f, cellW, cellH, 1, 1, 1, 1);
    }







    // 後始末
    dc->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
    if (alphaBlend) alphaBlend->Release();
}

void GameUI::DrawGUI()
{

}
