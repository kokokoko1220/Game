#pragma once
#include <vector>
#include <set>
#include <d3d11.h>

class GameUI;
class Goal;
class Shader;
class Player;          
class GoalManager {
private:
    GoalManager() = default;
    ~GoalManager() = default;

public:
    static GoalManager& Instance() {
        static GoalManager instance;
        return instance;
    }

   
    void Update(float elapsedTime, Player* player);

    void Render(ID3D11DeviceContext* dc, Shader* shader);
    void Register(Goal* goal);
    void Remove(Goal* goal);
    void DrawDebugPrimitive();
    void Clear();

    int   GetGoalCount() const { return static_cast<int>(goals.size()); }
    Goal* GetGoal(int index) { return goals.at(index); }
    GameUI* ui = nullptr;
private:
    void CollisionGoalVsGoals();
    void CollisionPlayerVsGoals(Player* player);   

private:
    std::vector<Goal*> goals;
    std::set<Goal*>    removes;
};

