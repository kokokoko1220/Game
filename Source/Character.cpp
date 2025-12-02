#include"Character.h"
#include"Stage.h"

//行列更新処理
void Character::UpdateTransform() {

	//スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y,scale.z);

   //回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x,angle.y,angle.z);

	//位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//3つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;

	//計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}


//移動処理
void Character::Move(float elapsedTime, float vx, float vz, float speed) {
	speed *= elapsedTime;
	position.x += vx * speed;
	position.z += vz * speed;
//void Character::Move(float vx, float vz, float speed)
//{
//		////�ړ������x�N�g����ݒ�
//		moveVecX = vx;
//		moveVecZ = vz;
//
//		//�ő呬�x�ݒ�
//		maxMoveSpeed = speed;
}

void Character::Turn(float elapsedTime, float vx, float vz, float speed) {
	speed += elapsedTime;

	//進行ベクトルがゼロベクトルの場合は処理する必要なし
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f)return;

	//進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;

	//自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);


	//回転角を求める為、2つの単位ベクトルの内積を計算する
	float dot = (frontX * vx) + (frontZ * vz);

	//内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度が
	//小さいほどに1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot;
	if (rot > speed)rot = speed;

	//左右判定を行うために2つの単位ベクトルの外積を計算する
	float cross = (frontZ * vx) - (frontX * vz);

	//2Dの外積値が正の場合か負の場合によって左右判定が行える
	//左右判定を行うことによって左右回転を選択する

	if (cross < 0.0f) {

		//angle.y -= speed;
		angle.y -= rot;
	}
	else {

		//angle.y += speed;
		angle.y += rot;
	}

}

//速力処理更新
void Character::UpdateVelocity(float elapsedTime)
{
	//経過フレーム
	float elapsedFrame = 60.0f * elapsedTime;

	//垂直速力更新処理
	UpdateVerticalVelocity(elapsedFrame);

	//水平速力更新処理
	/*UpdateHorizontalVelocity(elapsedFrame);*/

	//垂直移動更新処理
	UpdateVerticalMove(elapsedTime);
	//水平移動更新処理
	UpdateHorizontalMove(elapsedTime);
	//地面判定
	if (position.y < 0.0f)
	{
		position.y = 0.0f;
		velocity.y = 0.0f;

		//着地した
		if (!isGround)
		{
			OnLanding();
		}
		isGround = true;
	}
	else
	{
		isGround = false;
	}

}

//垂直速力更新処理
void Character::UpdateVerticalVelocity(float elapsedFrame)
{

	//重力処理
	velocity.y += gravity * elapsedFrame;

}


//垂直移動更新処理
void Character::UpdateVerticalMove(float elapsedTime)
{
	//垂直方向の移動量
	float my = velocity.y * elapsedTime;

	//落下中
	if (my < 0.0f) {
		//レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x,position.y + stepOffset,position.z };
		//レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x,position.y + my, position.z };

		//レイキャストによる地面判定
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit)) 
		{
			//地面に接地している
			position.y = hit.position.y;
		
			//着地した
			if (!isGround) {
				OnLanding();
			}
			isGround = true;
			velocity.y = 0.0f;
		}
		else
		{
		//空中に浮いている
			position.y += my;
			isGround = false;
		}
	}
	//上昇中
	else if (my > 0.0f) {
		position.y += my;
		isGround = false;
	}
}

//水平速力更新処理
//void Character::UpdateHorizontalVelocity(float elapsedFrame)
//{
//
//	//XZ平面の速力を減速する
//	float length = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
//	if (length > 0.0f)
//	{
//		//摩擦力
//		float friction = this->friction * elapsedFrame;
//
//		//摩擦による横方向の減速処理
//		if (length > friction)
//		{
//			//単位ベクトル化
//			float vx = velocity.x / length;
//			float vz = velocity.z / length;
//
//			velocity.x -= vx * friction;
//			velocity.z -= vz * friction;
//		}
//		//横方向の速力が摩擦力以下になったので速力を無効か
//		else {
//			velocity.x = 0.0f;
//			velocity.z = 0.0f;
//		}
//	}
//
//	//XZ平面の速力を加速する
//	if (length <= maxMoveSpeed)
//	{
//
//		//移動ベクトルがゼロベクトルでないなら加速する
//		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
//		if (moveVecLength > 0.0f)
//		{
//			//加速力
//			float acceleration = this->acceleration * elapsedFrame;
//			//移動ベクトルによる加速処理
//			velocity.x += moveVecX * acceleration;
//			velocity.z += moveVecZ * acceleration;
//
//			//最大速度制限
//			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
//			if (length > maxMoveSpeed)
//			{
//				float vx = velocity.x / length;
//				float vz = velocity.z / length;
//
//				velocity.x = vx * maxMoveSpeed;
//				velocity.z = vz * maxMoveSpeed;
//			}
//			//下り坂でガタガタしないようにする
//		/*	if (isGround && slopeRate > 0.0f) {
//				velocity.y -= length * slopeRate * elapsedFrame;
//			}*/
//		}
//	}
//	//移動ベクトルをリセット
//	moveVecX = 0.0f;
//	moveVecZ = 0.0f;
//}



void Character::UpdateHorizontalMove(float elapsedTime) {

	// 移動処理
	/*position.x += velocity.x * elapsedTime;
	position.z += velocity.z * elapsedTime;*/

	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		//水平移動値
		float mx = velocity.x * elapsedTime;
		float mz = velocity.z * elapsedTime;

		//レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx, position.y + stepOffset, position.z + mz };

		//レイキャストによる壁判定
		HitResult hit;
		if (Stage::Instance().RayCast(start, end, hit))
		{
			//壁までのベクトル
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&hit.position);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			//壁の法線を取得
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);

			//入射ベクトルを法線に射影
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
			Dot = DirectX::XMVectorScale(Dot, 1.1f);


			//補正位置
			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
			DirectX::XMFLOAT3 collectPosition;
			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

			//壁ずり方向へレイキャスト
			HitResult hit2;
			if (!Stage::Instance().RayCast(start, collectPosition, hit2))
			{
				//壁ずり方向で壁に当たらなかったら補正位置に移動
				position.x = collectPosition.x;
				position.z = collectPosition.z;


			}
			else
			{
				position.x = hit2.position.x;
				position.z = hit2.position.z;
			}
		}
		else
		{
			//壁に当たっていなければ通常移動
			position.x += mx;
			position.z += mz;
		}
	}


}