#pragma once
#include <vector>
#include <set>
#include <d3d11.h>

class Goal;
class Shader;
class Player;          // ★ 追加：前方宣言（これがないと Player* が未定義）

class GoalManager {
private:
    GoalManager() = default;
    ~GoalManager() = default;

public:
    static GoalManager& Instance() {
        static GoalManager instance;
        return instance;
    }

    // ★ ここを置き換え：Update に Player* を渡す版にする（古い宣言は消す）
    void Update(float elapsedTime, Player* player);

    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Register(Goal* goal);
    void Remove(Goal* goal);
    void DrawDebugPrimitive();
    void Clear();

    int   GetGoalCount() const { return static_cast<int>(goals.size()); }
    Goal* GetGoal(int index) { return goals.at(index); }

private:
    void CollisionGoalVsGoals();
    void CollisionPlayerVsGoals(Player* player);   // ★ 追加：宣言をヘッダに

private:
    std::vector<Goal*> goals;
    std::set<Goal*>    removes;
};

