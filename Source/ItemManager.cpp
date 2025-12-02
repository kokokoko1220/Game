#include"ItemManager.h"
#include"Item.h"
#include"Collision.h"

//更新処理
void ItemManager::Update(float elapsedTime) {
	for (Item* item : itemes) {
		item->Update(elapsedTime);
	}

	for (Item* item : removes)
	{
		std::vector<Item*>::iterator it = std::find(itemes.begin(), itemes.end(), item);
		if (it != itemes.end())
		{
			itemes.erase(it);
		}
		delete item;
	}
	removes.clear();

	//アイテム同士の衝突判定
	CollisionEnemiVsItemes();
}

//描画処理
void ItemManager::Render(ID3D11DeviceContext* context, Shader* shader) {
	for (Item* item : itemes) {
		item->Render(context, shader);
	}
}

//アイテム登録
void ItemManager::Register(Item* item) {
	itemes.emplace_back(item);
}

//アイテム全削除
void ItemManager::Clear() {
	for (Item* item : itemes) {
		delete item;
	}
	itemes.clear();
}

//デバックプリミティブ描画
void ItemManager::DrawDebugPrimitive() {
	for (Item* item : itemes) {
		item->DrawDebugPrimitive();
	}
}

// アイテム同士の衝突処理
void ItemManager::CollisionEnemiVsItemes()
{
	size_t itemCount = itemes.size();
	for (int i = 0; i < itemCount; ++i)
	{
		Item* itemA = itemes.at(i);
		for (int j = i + 1; j < itemCount; ++j)
		{
			Item* itemB = itemes.at(j);

			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsSphere(
				itemA->GetPosition(),
				itemA->GetRadius(),
				itemB->GetPosition(),
				itemB->GetRadius(),
				outPosition))
			{
				itemB->SetPosition(outPosition);
			}
		}
	}
}

void ItemManager::Remove(Item* item)
{
	removes.insert(item);
}