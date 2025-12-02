#include"EnemyPolice.h"
#include"Enemy.h"
#include "Player.h"
#include"Graphics/Graphics.h"
#include "Collision.h"
#include "Stage.h"

//コンストラクタ
EnemyPolice::EnemyPolice() {
    //車モデルを読み込み

  //  model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");
    model = new Model("Data/Model/Ptcar/Ptcar2.mdl");//koko//パトカー
    //スケーリング
    scale.x = scale.y = scale.z = 0.01f;

    //幅、高さ設定
    radius = 0.5f;
    height = 1.0f;
}

//デストラクタ
EnemyPolice::~EnemyPolice() {
    //車モデルを破棄
    delete model;
}

//更新処理
void EnemyPolice::Update(float elapsedTime) {
    //オブジェクト行列を更新
    UpdateTransform();
    //モデル行列更新
    model->UpdateTransform(transform);

    switch (State) {
    case 0:
    {
        angle.y += 0.01f;   //仮の動作

        if (EnemyView())
        {
            State = 1;
        }
        break;
    }
    case 1:
    {
        DirectX::XMFLOAT3 playerPos = Player::Instance().GetPosition();

        float vx = playerPos.x - position.x;
        float vz = playerPos.z - position.z;

        float turnSpeed = 2.0f;
        Turn(elapsedTime, vx, vz, turnSpeed);

        float forwardX = sinf(angle.y);
        float forwardZ = cosf(angle.y);

        float moveSpeed = 25.0f;
        Move(elapsedTime, forwardX, forwardZ, moveSpeed);

        if (!EnemyView())
        {
            State = 0;
        }
        break;
    }

   }
    frameCounter++;
    if (frameCounter >= RAYCAST_INTERVAL) {
        frameCounter = 0;
 //    足元にレイを飛ばして地面の高さを取得
    HitResult hit;
    DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
    DirectX::XMFLOAT3 end = { position.x, position.y - 100.0f, position.z };
    if (Stage::Instance().RayCast(start, end, hit)) {
        position.y = hit.position.y;
    }
    }
}

    //描画処理
void EnemyPolice::Render(ID3D11DeviceContext* dc, Shader* shader) {
    shader->Draw(dc, model);
}

bool EnemyPolice::EnemyView() {
    using namespace DirectX;

    // プレイヤー位置
    XMFLOAT3 playerPos = Player::Instance().GetPosition();

    // 敵 → プレイヤー のベクトル
    XMFLOAT3 toPlayer = {
        playerPos.x - position.x,
        playerPos.y - position.y,
        playerPos.z - position.z
    };

    // 距離判定
    float distance = sqrtf(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y + toPlayer.z * toPlayer.z);
    if (distance > ViewRadius) return false;

    // -------------------------------
    // ▼ 敵の前方向ベクトルを作る部分
    // -------------------------------
    float yaw = angle.y;   // ← ここはあなたの Enemy の Y軸回転に合わせて

    XMFLOAT3 forwardDir = {
        sinf(yaw),    // x
        0.0f,         // y
        cosf(yaw)     // z
    };
    // -------------------------------

    // 正規化
    XMVECTOR dirToPlayer = XMVector3Normalize(XMLoadFloat3(&toPlayer));
    XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&forwardDir));

    // 内積
    float dot = XMVectorGetX(XMVector3Dot(forward, dirToPlayer));

    // 視野角（例: 90° → 45°）
    float cosHalfAngle = cosf(XMConvertToRadians(ViewAngle * 0.5f));

    if (dot >= cosHalfAngle) {
        return true;
    }

    return false;
}