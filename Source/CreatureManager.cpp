#include"CreatureManager.h"
#include"Collision.h"



//更新処理
void CreatureManager::Update(float elapsedTime) {
	for (Creature* creature : creatures) {
		creature->Update(elapsedTime);
	}
	//エネミー同士の衝突判定
	CollisionEnemiVsCreatures();
}


//描画処理
void CreatureManager::Render(ID3D11DeviceContext* context, Shader* shader) {
	for (Creature* creature : creatures) {
		creature->Render(context, shader);
	}
}

//エネミー登録
void CreatureManager::Register(Creature* creature) {
	creatures.emplace_back(creature);
}



//エネミー全削除
void CreatureManager::Clear() {
	for (Creature* creature : creatures) {
		delete creature;
	}
	creatures.clear();
}

//デバックプリミティブ描画
void CreatureManager::DrawDebugPrimitive() {
	for (Creature* creature : creatures) {
		creature->DrawDebugPrimitive();
	}
}


// エネミー同士の衝突処理
void CreatureManager::CollisionEnemiVsCreatures()
{
	size_t creatureCount = creatures.size();
	for (int i = 0; i < creatureCount; ++i)
	{
		Creature* creatureA = creatures.at(i);
		for (int j = i + 1; j < creatureCount; ++j)
		{
			Creature* creatureB = creatures.at(j);

			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				creatureA->GetPosition(),
				creatureA->GetRadius(),
				creatureB->GetPosition(),
				creatureB->GetRadius(),
				outPosition))
			{
				creatureB->SetPosition(outPosition);
			}
		}
	}
}
