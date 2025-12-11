#include"ItemManager.h"
#include"Item.h"
#include"Graphics/Graphics.h"
#include "Goal.h"
#include <cmath> // sin関数用
using namespace DirectX;
// コンストラクタ
Goal::Goal() {
    model = new Model("Data/Model/Man/KCG.mdl");
    
    // モデルが巨大なのでまずは強制縮小（必要に応じて調整）
    scale.x = scale.y = scale.z = 0.004f;  
    angle = { 0, 0, 0 };                     
}

// デストラクタ
Goal::~Goal() {
    delete model;
}

// 更新処理（動かさない）
void Goal::Update(float ) {
    using namespace DirectX;

   
    XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
    XMMATRIX R = XMMatrixRotationZ(XMConvertToRadians(angle.z)); 
    XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);

    XMMATRIX world = S * R * T;

    
    XMStoreFloat4x4(&transform, world);
    model->UpdateTransform(transform);
}

// 描画処理（そのままでOK）
void Goal::Render(ID3D11DeviceContext* dc, Shader* shader) {
    shader->Draw(dc, model);
}
