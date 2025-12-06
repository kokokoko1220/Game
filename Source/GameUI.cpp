#include "GameUI.h"
#include "Graphics/Sprite.h"
#include "Graphics/Graphics.h"
#include "imgui.h"
#include"Camera.h"

#include <cmath>            // ← atan2f
#include <cfloat>
#include <DirectXMath.h>
#include "Goal.h"
#include "GoalManager.h"
#include "Player.h"         // ← Player の位置/向きを使う
#include"SceneManager.h"
#include"SceneEnd.h"
#include"SceneLoading.h"
using namespace DirectX;

namespace {
    inline float WrapPi(float a) {
        while (a > XM_PI)  a -= XM_2PI;
        while (a < -XM_PI) a += XM_2PI;
        return a;
    }
    inline float YawFromVectorXZ(const XMFLOAT3& v) {
        return std::atan2f(v.x, v.z); // +Z基準
    }
    inline Goal* FindNearestGoalXZ(const XMFLOAT3& basePos) {
        auto& gm = GoalManager::Instance();
        int n = gm.GetGoalCount();
        Goal* nearest = nullptr;
        float bestD2 = FLT_MAX;
        for (int i = 0; i < n; ++i) {
            Goal* g = gm.GetGoal(i);
            XMFLOAT3 pos = g->GetPosition();
            float dx = pos.x - basePos.x;
            float dz = pos.z - basePos.z;
            float d2 = dx * dx + dz * dz;
            if (d2 < bestD2) { bestD2 = d2; nearest = g; }
        }
        return nearest;
    }
}





void GameUI::Initialize()
{
    gauge = gauge_MIN;
    // スプライトの生成
    sprite = new Sprite("Data/Sprite/cage.png");
    sprite2 = new Sprite("Data/Sprite/yazirusi.png");
    sprite3 = new Sprite("Data/Sprite/yazirusi2.png");

    Bottole = new Sprite("Data/Sprite/Bottole.png");
    face = new Sprite("Data/Font/font1.png");
    white = new Sprite("Data/Sprite/white.png");

    one = new Sprite("Data/Sprite/1.png");
    two = new Sprite("Data/Sprite/2.png");
    three = new Sprite("Data/Sprite/3.png");
    start = new Sprite("Data/Sprite/start.png");
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
    if (isPressed && !wasSpacePressed && iteam != 0 && cool_time_switch == false)
    {
        // スペース押した瞬間の処理


        gauge_UP_switch = true;
        cool_time_switch = false;
        --iteam;
        cool_time_switch = true;
    }

    if (gauge >= gauge_MIN)
    {
        gauge = gauge_MIN;
    }
    if (gauge_UP_switch == true)
    {

        gauge -= static_cast<float>(120 * elapsedTime);
        time += static_cast<float>(elapsedTime);

        if (time > 1)
        {
            cool_time_switch = false;
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
    // =========================
    // ▼ 一番近いゴールの方向を計算
    // =========================

    arrowVisible = false;

    // カメラ位置・前向き
    const XMFLOAT3 basePos = Camera::Instance().GetEye();
    const XMFLOAT3 forward = Camera::Instance().GetFront();
    const float     baseYaw = YawFromVectorXZ(forward);

    // 最寄りゴール
    Goal* nearest = FindNearestGoalXZ(basePos);
    if (nearest) {
        const XMFLOAT3 goalPos = nearest->GetPosition();

        XMFLOAT3 toGoal{
            goalPos.x - basePos.x,
            0.0f,
            goalPos.z - basePos.z
        };

        const float d2 = toGoal.x * toGoal.x + toGoal.z * toGoal.z;
        if (d2 > 0.01f) { // 近すぎる/ゼロ距離は不安定なので弾く
            float targetYaw = YawFromVectorXZ(toGoal);
            float rel = WrapPi(targetYaw - baseYaw);

            // 画像の基準が「上向き」なら 0.0f のままでOK
            const float spriteOffset = 0.0f;
            arrowAngle = rel + spriteOffset;
            arrowVisible = true;
        }
    }
    if (clearcount >= 3)
    {
        Density += static_cast<float>(1.5 * elapsedTime);
        if (Density >= 1)
        {
            SceneManager::Instance().ChangeScene(new SceneLoading(new SceneEnd));
        }
    }
    timer += static_cast<float>(elapsedTime * 2);

    float remaining = countdown - timer;

    if (remaining < 0.0f)
        remaining = 0.0f;

    // ★ 小数点を切り上げて「3,2,1,0」に変換
    displayCount = static_cast<int>(ceil(remaining));


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
        screenWidth / 2, screenHeight - 128 + 20, 128, 128,  // x, y, 幅, 高さ
        0,
        1, 1, 1, 1);
    // yazirusi.png（透過付き）
    sprite2->Render(dc,
        screenWidth - 64 - 64, gauge, 64, 64,
        0,
        1, 1, 1, 1);

    switch (displayCount)
    {
    case 3:
        // ★ カウントダウン「3」のときの処理
        // 画像1を表示、SE再生など
        three->Render(dc,
            screenWidth / 2 - 256, screenHeight / 2 - 256, 512, 512,  // x, y, 幅, 高さ
            0,
            1, 1, 1, 1);
        break;

    case 2:
        // ★ カウントダウン「2」のときの処理
        two->Render(dc,
            screenWidth / 2 - 256, screenHeight / 2 - 256, 512, 512,  // x, y, 幅, 高さ
            0,
            1, 1, 1, 1);
        break;

    case 1:
        // ★ カウントダウン「1」のときの処理
        one->Render(dc,
            screenWidth / 2 - 256, screenHeight / 2 - 256, 512, 512,  // x, y, 幅, 高さ
            0,
            1, 1, 1, 1);
        break;

    case 0:
        // ★ カウント終了（GO!）
        start->Render(dc,
            screenWidth / 2 - 256, screenHeight / 2 - 256, 512, 512,  // x, y, 幅, 高さ
            0,
            1, 1, 1, 1);
        break;
    }

    /* sprite3->Render(dc,
         screenWidth / 2, 0, 64, 64,
         0,
         1, 1, 1, 1);*/

    if (arrowVisible) {
        const float cx = screenWidth * 0.5f;
        const float cy = screenHeight * 0.15f; // 画面上部に出す例
        //sprite3->Render(dc, cx, cy, 64, 64, arrowAngle, 1, 1, 1, 1);
        // 度に変換して渡す（試してみてください）
        float angleDeg = arrowAngle * 180.0f / DirectX::XM_PI;
        sprite3->Render(dc, cx, cy, 64, 64, angleDeg, 1, 1, 1, 1);
    }
    //if (arrowVisible && sprite3) {
    //    float cx = screenWidth * 0.5f;
    //    float cy = screenHeight * 0.12f; // 上辺付近
    //    sprite3->Render(dc, cx, cy, 64, 64, arrowAngle, 1, 1, 1, 1);
    //}





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
            iteam
        );
        face->textout(dc, buf, screenWidth / 2 - 30, screenHeight - 45, 50, 50, 1, 1, 1, 1);
        // 例：タイトルも出す
       // face->textout(dc, "POWER", textX, textY - 20.0f, cellW, cellH, 1, 1, 1, 1);
    }






    white->Render(dc,
        0, 0, 2000, 1000,
        0,
        1, 1, 1, Density);
    // 後始末
    dc->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
    if (alphaBlend) alphaBlend->Release();
}

void GameUI::DrawGUI()
{

}
