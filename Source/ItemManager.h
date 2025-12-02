#pragma once

#include<vector>
#include <set>
#include"Item.h"

//エネミーマネージャー
class ItemManager {

private:
	ItemManager() {}
	~ItemManager() {}

public:
	//唯一のインスタンスを取得
	static ItemManager& Instance() {
		static ItemManager instance;

		return instance;
	}
	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//アイテム登録
	void Register(Item* item);

	void Remove(Item* item);

	//デバックプリミティブ描画
	void DrawDebugPrimitive();

	// アイテム数取得
	int GetItemCount() const { return static_cast<int>(itemes.size()); }

	// アイテム取得
	Item* GetItem(int index) { return itemes.at(index); }

	//アイテム全削除
	void Clear();

private:
	//アイテム同士の衝突判定
	void CollisionEnemiVsItemes();

	std::vector<Item*> itemes;
	std::set<Item*>	removes;
};