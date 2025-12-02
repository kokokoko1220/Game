#pragma once

#include"Graphics/Shader.h"
#include"Graphics/Model.h"
#include"Collision.h"

//ステージ
class Stage {
public :
	Stage();
	~Stage();

	//更新処理
	void Update(float elapsedTime);

	//インスタンスを取得
	static Stage& Instance();

	//レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start,const DirectX::XMFLOAT3& end,HitResult& hit);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

private :
	Model* model = nullptr;
};