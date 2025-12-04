#include"ItemManager.h"
#include"Item.h"
#include"Graphics/Graphics.h"
#include "Goal.h"
#include <cmath> // sin関数用
using namespace DirectX;
// コンストラクタ
Goal::Goal() {
    model = new Model("Data/Model/Slime/slime.mdl");

    // モデルが巨大なのでまずは強制縮小（必要に応じて調整）
    scale.x = scale.y = scale.z = 0.01f;   // ← ここだけ触ればサイズ調整できます
    angle = { 0, 0, 0 };                     // Zだけ25度傾ける（不要なら0）
}

// デストラクタ
Goal::~Goal() {
    delete model;
}

// 更新処理（動かさない）
void Goal::Update(float /*elapsedTime*/) {
    using namespace DirectX;

    // ★ visualPos は使わない。position をそのまま使う
    XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX R = XMMatrixRotationZ(XMConvertToRadians(angle.z)); // 回転不要なら XMMatrixIdentity()
    XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);

    XMMATRIX world = S * R * T;

    // ★ 行列を transform に書き込んでモデルに反映
    XMStoreFloat4x4(&transform, world);
    model->UpdateTransform(transform);
}

// 描画処理（そのままでOK）
void Goal::Render(ID3D11DeviceContext* dc, Shader* shader) {
    shader->Draw(dc, model);
}
