#pragma once

#include<vector>
#include"Creature.h"

//エネミーマネージャー
class CreatureManager {

private:
	 CreatureManager() {}
	~CreatureManager() {}

public:
	//唯一のインスタンスを取得
	static CreatureManager& Instance() {
		static CreatureManager instance;

		return instance;
	}
	//更新処理
	void Update(float elapsedTime);

	//描画処理
	void Render(ID3D11DeviceContext* dc, Shader* shader);

	//エネミー登録
	void Register(Creature* creature);


	//デバックプリミティブ描画
	void DrawDebugPrimitive();

	// エネミー数取得
	int GetCreatureCount() const { return static_cast<int>(creatures.size()); }

	// エネミー取得
	Creature* GetCreature(int index) { return creatures.at(index); }

	//エネミー全削除
	void Clear();

private:
	//エネミー同士の衝突判定
	void CollisionEnemiVsCreatures();

	std::vector<Creature*> creatures;

};