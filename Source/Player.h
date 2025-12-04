#pragma once

#include"Graphics/Shader.h"
#include"Graphics/Model.h"
#include"Character.h"
#include"GameUI.h"

//プレイヤー
class Player : public Character {
public:

	static Player& Instance() {
		static Player instance;

		return instance;
	}

	Player();
	~Player() override;

	//デバック用GUI描画
	void DrawDebugGUI();

	//デバックプリミティブ描画
	void DrawDebugPrimitive();

	//更新処理
	void Update(float elapsedTime);
	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);
	void drunkenness(float elapsedTime);
	// シーン開始時にプレイヤーの状態を初期化する関数
	void Reset();
	void SetUI(GameUI* p) { ui = p; };
private :
	//スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec()const;

	//移動処理
	//void Move(float elapsedTime, float vx, float vz, float speed);

	//旋回処理
	//void Turn(float elapsedTime, float vx, float vz, float speed);
	
	//プレイヤーとエネミーとの衝突処理
	void CollisionPlayerVsEnemies();

	//プレイヤーとボトルとの消滅処理
	void CollisionPlayerVsBottleDelete();
	
 
	
	//移動入力処理

	void InputMove(float elapsedTime);

	//ダメージ処理
	void PlayerDamage(float elapsedTime);

	//ジャンプ処理
	//void Jump(float speed);

	//速力処理更新
	//void UpdateVelocity(float elapsedTime);

	//ジャンプ入力処理
	//void InputJump();

protected:

	// 着地した時に呼ばれる
	void OnLanding() override;

private:
	int jumpCount = 0;
	int jumpLimit = 2;

	Model* model = nullptr;
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(720);
	/*float jumpspeed = 20.0f;
	float gravity = -1.0f;
	DirectX::XMFLOAT3 velocity = { 0,0,0 };*/

	int DeleteCount = 0;

	int HP = 0;
	bool isDamage = false;
	int invincibleTime = 0;
	DirectX::XMFLOAT2 stickyMove = { 0, 0 }; // XZ の残存ベクトル（-1..1）
	float stickyAccel = 12.0f;  // 追従の速さ（大きいほど目標に素早く寄る）
	float stickyDecay = 3.0f;   // 離した後の減衰の速さ（大きいほど早くゼロ）
	float stickyMaxMag = 1.0f;   // 残存ベクトルの最大長（1で十分）
	bool  stickyEnabled = true;   // 残存入力のON/OFF
	float drunkennessX;
	
	float axisX = 0.0f;
	float axisY = 0.0f;
	float axisZ = 0.0f;

	float randomTimer = 0.0f;
	int   randomValue = 0;
	int iteam = 0;

	// ==== 車のような旋回用パラメータ ====
	float yaw = 0.0f;              // 現在の向き角（ラジアン）
	float steerAngle = 0.0f;       // ハンドル角
	float speed = 0.0f;            // 現在の前進速度
	float wheelBase = 2.5f;        // 車体長
	float maxSteer = DirectX::XMConvertToRadians(60.0f);   // 最大ハンドル角
	float steerRate = DirectX::XMConvertToRadians(720.0f);  // 1秒で回せるハンドル速度
	float maxYawRate = DirectX::XMConvertToRadians(540.0f);  // 最大旋回速度
	float grip = 8.0f;             // 横滑りを戻す力
	float drunkenPower = 0.0f;
	float accelTime = 4.0f;   // 加速の追従スピード係数（好きな値で調整）

	float raycast_counter = 0; // レイキャスト実行を制御するカウンター
	const float RAYCAST_INTERVAL = 3.7f; // 実行間隔（3.7フレームに1回）

	float drunkennessY;

	float t = 0.0f; // デフォルト
	GameUI* ui = nullptr;
};