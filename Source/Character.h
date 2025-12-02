#pragma once
#include<DirectXMath.h>

//キャラクター
class Character {
public:
	Character() {}
	virtual ~Character() {}

	//位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	//位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }


	//回転取得
	const DirectX::XMFLOAT3& GetAngle()const { return angle; }

	//回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//スケール取得
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

//行列更新処理
	void UpdateTransform();

	// 地面に接地しているか
	bool IsGround() const { return isGround; }

	

	//半径取得
	float GetRadius() const { return radius; }

	//09
	//速力処理更新
	void UpdateVelocity(float elapsedTime);

	

	//// 水平移動更新処理
	//void UpdateHorizontalMove(float elapsedTime);

	//高さ取得
	float GrtHeigth()const { return height; }

private:

	//垂直速力更新処理
	void UpdateVerticalVelocity(float elapsedFrame);


	//垂直移動更新処理
	void Character::UpdateVerticalMove(float elapsedTime);


protected:
	//移動処理
	void Move(float elapsedTime, float vx, float vz, float speed);
	//void Move(float vx, float vz, float speed);
	//旋回処理
	void Turn(float elapsedTime, float vx, float vz, float speed);

	// 着地した時に呼ばれる
	virtual void OnLanding() {}
	//void UpdateHorizontalVelocity(float elapsedFrame);
	//// 水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1 };
	bool				isGround = false;
	float gravity = -1.0f;
	DirectX::XMFLOAT3	velocity = { 0, 0, 0 };
	float height = 2.0f;
	float radius = 0.5f;
	float stepOffset = 1.0f;

	float friction = 0.5f;
	float acceleration = 1.0f;
	float maxMoveSpeed = 5.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
};
