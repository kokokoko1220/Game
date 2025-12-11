#include"Player.h"
#include"Input/Input.h"
#include<imgui.h>
#include"Camera.h"
#include"Graphics/Graphics.h"
#include"EnemyManager.h"
#include"Collision.h"
#include "ItemManager.h"
#include "SceneEnd.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneGameOver.h"
#include <Stage.h>
#include"GameUI.h"
#include"SoundManager.h"

template <typename T>
T clamp(T value, T minVal, T maxVal)
{
	if (value < minVal) return minVal;
	if (value > maxVal) return maxVal;
	return value;
}
template <typename T>
T lerp(T a, T b, T t)
{
	return a + (b - a) * t;
}
//コンストラクタ
Player::Player()
{
	
	model = new Model("Data/Model/Scooter/alcS.mdl");//キャラクターモデル
	
	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 0.004f;
	
	position.x = 73.0f;
	HP = 3;
	hitEffect = new Effect("Data/Effect/Hit.efk");
	
}

//デストラクタ
Player::~Player() {
	delete model;
	delete hitEffect;
}

//更新処理
void Player::Update(float elapsedTime) {

	{
		GamePad& gamePad = Input::Instance().GetGamePad();
		axisX = gamePad.GetAxisLX();
		axisY = gamePad.GetAxisLY();
	}
	
	// ★ 停止中なら移動処理をスキップ
	if (stopTimer > 0.0f)
	{
		stopTimer -= elapsedTime;

		// モデル行列などは更新したい場合は残す
		UpdateTransform();
		model->UpdateTransform(transform);

		return;  // ★ 移動・衝突など一切しない
	}
	drunkenness(elapsedTime);
	//移動入力処理
	/*InputMove(elapsedTime);*/
	if (ui->start_switch == true)
	{
		InputMove(elapsedTime);
	}
	//プレイヤーと敵との衝突処理
	CollisionPlayerVsEnemies();

	CollisionPlayerVsBottleDelete();

	//オブジェクト行列を更新
	UpdateTransform();

	//モデル行列更新
	model->UpdateTransform(transform);

	//ダメージ判定
	PlayerDamage(elapsedTime);

	//クリア判定　条件は後ほど変えることを前提
	
	if (HP == 0)//kokoko//enemyにぶつかってHP減った時だけに変更
	{
		SoundManager::Instance().StopAll();
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGameOver));
	}

	raycast_counter++;

	// RAYCAST_INTERVAL フレームごとに処理を実行
	if (raycast_counter >= RAYCAST_INTERVAL)
	{
		raycast_counter = 0; // カウンターをリセット

		// 足元にレイを飛ばして地面の高さを取得 (レイキャスト処理)
		HitResult hit;
		DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
		DirectX::XMFLOAT3 end = { position.x, position.y - 100.0f, position.z };

		if (Stage::Instance().RayCast(start, end, hit)) {
			position.y = hit.position.y; // 地面に吸着
		}
	}
	
}

//描画処理
void Player::Render(ID3D11DeviceContext* dc, Shader* shader) {
	shader->Draw(dc, model);
}

// デバッグプリミティブ描画
void Player::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	

	//衝突判定用のデバック円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}



//デバック用GUI描画
void Player::DrawDebugGUI() {
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None)) {

		//トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)){
			//位置
			ImGui::InputFloat3("Position", &position.x);
			//回転
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			//スケール
			ImGui::InputFloat3("Scale", &scale.x);
			//消したボトルの数
			ImGui::InputInt("DeleteCount", &DeleteCount);
			//HP関連
			ImGui::InputInt("HP", &HP);
			ImGui::InputInt("invincibleTime", &invincibleTime);
			ImGui::Checkbox("isDamage", &isDamage);
		}
	}
	ImGui::End();
}

//スティック入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::GetMoveVec() const {


	float ax = axisX;  // ← メンバーを使う！
	float ay = axisY;

	Camera& camera = Camera::Instance();
	const auto& R = camera.GetRight();
	const auto& F = camera.GetFront();

	float rx = R.x, rz = R.z;
	float rl = sqrtf(rx * rx + rz * rz);
	if (rl > 0.0f) { rx /= rl; rz /= rl; }

	float fx = F.x, fz = F.z;
	float fl = sqrtf(fx * fx + fz * fz);
	if (fl > 0.0f) { fx /= fl; fz /= fl; }

	DirectX::XMFLOAT3 v;
	v.x = (rx * ax) + (fx * ay);
	v.z = (rz * ax) + (fz * ay);
	v.y = 0.0f;
	return v;
}



//Player初期化
void Player::Reset()
{
	HP = 3;
	position.x = 73.0f;
	position.y = 0.0f;
	position.z = 0.0f;
	angle.x = angle.y = angle.z = 0.0f;
	moveSpeed = 3.5f;
	turnSpeed = DirectX::XMConvertToRadians(720);
	// その他のステータスもリセットが必要
	// 無敵時間もリセット
	invincibleTime = 0;
	isDamage = false;
	// 酔いシステムのタイマー/値もリセット
	randomTimer = 0.0f;
	axisX = 0.0f;
	axisY = 0.0f;
	
}

//移動入力処理
void Player::InputMove(float elapsedTime) {
	using namespace DirectX;
	
		
	// === 前後入力：axisY の符号で 前進/後退 を決める ===
	float forwardIn = clamp(axisY, -1.0f, 1.0f);     // 上(前)＝+1, 下(後)＝-1
	float forwardMag = fabsf(forwardIn);              // 押し込み量 0..1
	float forwardSign = (forwardIn >= 0.0f) ? 1.0f : -1.0f;


	

	// ★ 基本パラメータ（今までの値）
	const float baseMaxSpeed = moveSpeed * 4.0f; // もともとの最高速
	const float baseAccelRate = 4.0f;             // もともとの加速力

	// ★ ゲージ(t)でブースト
	float accelScale = lerp(1.0f, 4.0f, t);   // t=1で加速4倍
	float speedScale = lerp(1.0f, 3.0f, t);   // t=1で最高速3倍

	// 前進/後退の別最高速（後退は遅めに抑える）
	float maxSpeedFwd = baseMaxSpeed * speedScale * 0.5f;   // 前進最高速
	float maxSpeedRev = maxSpeedFwd * 0.4f;          // 後退最高速（40%に設定：好みで）
	
	float accelRateNow = baseAccelRate * accelScale;

	// ★ 目標速度（符号付き）：前進は +、後退は -
	float signedMaxNow = (forwardSign > 0.0f) ? maxSpeedFwd : maxSpeedRev;
	float targetSpeed = forwardSign * (signedMaxNow + drunkenPower) * forwardMag;

	// ===== ここから下はハンドル・旋回処理 =====
	
	// ★ 加速：符号ごと target に追従（減速もこの一式でOK）
	speed += (targetSpeed - speed) * accelRateNow * elapsedTime;

	// ===== ハンドル・旋回（後退時は左右が反転するのを考慮） =====
	float steerScale = 1.0f - 0.85f * t;      // ブースト中は舵角を少し弱める
	float rateScale = 1.0f - 0.5f * t;      // 旋回レートも弱める

	float steerInput = clamp(axisX, -1.0f, 1.0f);
	float targetSteer = (maxSteer * steerScale) * steerInput;

	float maxDelta = steerRate * rateScale * elapsedTime;

	float delta = clamp(targetSteer - steerAngle, -maxDelta, maxDelta);
	steerAngle += delta;

	// 速度の符号で旋回向きが反転（車のバックと同じ挙動）
	float dirSign = (speed >= 0.0f) ? 1.0f : -1.0f;

	float yawRate = ((fabsf(speed) / wheelBase) * steerAngle) * dirSign;
	float maxYaw = maxYawRate * (1.0f - 0.4f * t);//kokoko
	yawRate = clamp(yawRate, -maxYaw, maxYaw);

	yaw += yawRate * elapsedTime;
	angle.y = yaw;

	// ---- ここから Character::UpdateHorizontalMove を移植した壁当たり判定 ----
	XMFLOAT3 fwd = { sinf(yaw), 0, cosf(yaw) };

	float mx = fwd.x * speed * elapsedTime;
	float mz = fwd.z * speed * elapsedTime;

	XMFLOAT3 start = { position.x, position.y, position.z };
	XMFLOAT3 end = { position.x + mx,     position.y  ,position.z + mz };


	HitResult hit;
	if (Stage::Instance().RayCast(start, end, hit))
	{
		// 壁までのベクトル
		XMVECTOR Start = XMLoadFloat3(&hit.position);
		XMVECTOR End = XMLoadFloat3(&end);
		XMVECTOR Vec = XMVectorSubtract(End, Start);

		// 壁の法線
		XMVECTOR Normal = XMLoadFloat3(&hit.normal);

		// 入射ベクトルを法線に射影
		XMVECTOR Dot = XMVector3Dot(XMVectorNegate(Vec), Normal);
		Dot = XMVectorScale(Dot, 1.1f); // 少し余裕をもたせて押し出す

		// 補正位置（壁ずり方向）
		XMVECTOR CollectPosition = XMVectorMultiplyAdd(Normal, Dot, End);
		XMFLOAT3 collectPosition;
		XMStoreFloat3(&collectPosition, CollectPosition);

		// 壁ずり方向でもう一回レイキャスト（2回目）
		HitResult hit2;
		if (!Stage::Instance().RayCast(start, collectPosition, hit2))
		{
			// 壁に当たらなければ補正位置に移動
			position.x = collectPosition.x;
			position.z = collectPosition.z;
		}
		else
		{
			// まだ壁に当たるなら、そこまでで止める
			position.x = hit2.position.x;
			position.z = hit2.position.z;
		}
	}
	else
	{
		// 壁に当たっていなければ通常移動
	position.x += mx;
	position.z += mz;
		
	}

	//（今は gripNow 使ってないけど、後で横滑り計算に使うなら残してOK）
	float gripNow = lerp(grip, 3.0f, t);
}


// プレイヤーとエネミーとの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	if (invincibleTime > 0) {
		return;
	}
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての敵と総当たりで衝突処理
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsSphere(
			position,radius,
			enemy->GetPosition(),
			enemy->GetRadius(),
			outPosition))
		{
			SoundManager::Instance().PlaySE("SMASH");
		// 押し出し後の位置設定
		
			DirectX::XMFLOAT3 dir = {
			 enemy->GetPosition().x - position.x,
			 0.0f,
			 enemy->GetPosition().z - position.z
			};

			// 正規化
			float len = sqrtf(dir.x * dir.x + dir.z * dir.z);
			if (len > 0.001f) {
				dir.x /= len;
				dir.z /= len;
			}

			// ノックバック速度
			enemy->knockbackVel.x = dir.x * 300.0f;  // 強さは調整
			enemy->knockbackVel.y = 0.0f;          // 今はXZだけ
			enemy->knockbackVel.z = dir.z * 300.0f;

			// ノックバック時間
			enemy->knockbackTimer = 0.5f; // 0.5秒ノックバック


			// ★ ここでエフェクト再生 ★
			if (hitEffect) {
				hitEffect->Play(position, 1.0f); // 1.0f はスケール
			}


			switch (enemy->id)
			{
			case Enemy::Police:
				isDamage = true;
		
				break;
			case Enemy::Slime:
				stopTimer = 0.5f;   // ★ ここで停止タイマー開始
				break;

			default:
				break;
			}
		}
	}
}

void Player::CollisionPlayerVsBottleDelete()
{
	ItemManager& itemManager = ItemManager::Instance();

	// 全てのアイテムと総当たりで衝突処理
	int itemCount = itemManager.GetItemCount();
	for (int i = 0; i < itemCount; ++i)
	{
		Item* item = itemManager.GetItem(i);

		// 衝突処理
		DirectX::XMFLOAT3 outPosition;
		if (Collision::IntersectSphereVsSphere(
			position, radius,
			item->GetPosition(),
			item->GetRadius(),
			outPosition))
		{// 押し出し後の位置設定
		
			item->Destroy();
			DeleteCount++;
			SoundManager::Instance().PlaySE("HIT");
			
			ui->iteam++;
		}
	}
}

// 着地した時に呼ばれる
void Player::OnLanding()
{
	jumpCount = 0;
}

// ダメージ処理
void Player::PlayerDamage(float elapsedTime)
{
	// ダメージ中は処理しない
	if (invincibleTime == 0) {
		if (isDamage) {
			HP -= 1;
			invincibleTime = 70;
			// ★★★ ここで0.7秒停止開始 ★★★
			
			return;
		}
	}
	else
	{
		invincibleTime -= 1;

		auto* res = const_cast<ModelResource*>(model->GetResource());
		for (auto& mat : res->GetMaterials()) {
			mat.color.w = 0.4f;
		}

		if (invincibleTime <= 0)
		{
			invincibleTime = 0;
			isDamage = false;

			for (auto& mat : res->GetMaterials()) {
				mat.color.w = 1.0;
			}
		}
	}

}
void Player::drunkenness(float elapsedTime)
{

	

	randomTimer += elapsedTime;
	if (randomTimer >= 2.0f)
	{
		randomTimer = 0.0f;
		drunkennessX = ((rand() % 200) - 100) / 100.0f; // -1～1
		drunkennessY = ((rand() % 200) - 100) / 100.0f;


		// --- GameUIからゲージ値を取得 ---

		if (ui) {
			float gauge = ui->gauge;  // 現在のゲージ値
			float gaugeMin = ui->gauge_MIN;
			float gaugeMax = ui->gauge_MAX;

			// --- ゲージ比率（0～1に正規化） ---
			t = (gauge - gaugeMin) / (gaugeMax - gaugeMin);
			t = clamp(t, 0.0f, 1.0f);
		}
	}
	float drunkenPower = 0.05f + (t * 0.30f);
	// 操作入力に揺らぎを加える
	
	axisX += drunkennessX * drunkenPower;
	axisY += drunkennessY * drunkenPower;
	axisX = clamp(axisX, -1.0f, 1.0f);
	axisY = clamp(axisY, -1.0f, 1.0f);

}