#include "GoalManager.h"
#include <algorithm>
#include "Goal.h"
#include "Collision.h"
#include "Graphics/DebugRenderer.h"
#include "Player.h"   // ★ 角括弧でなくダブルクオート推奨

void GoalManager::Update(float elapsedTime, Player* player) {   // ★ ヘッダと一致
    // 1) ゴールの個別更新
    for (Goal* g : goals) {
        g->Update(elapsedTime);
    }

    // 2) プレイヤー vs ゴール
    if (player) {
        CollisionPlayerVsGoals(player);
    }

    // 3) 遅延削除
    for (Goal* g : removes) {
        auto it = std::find(goals.begin(), goals.end(), g);
        if (it != goals.end()) {
            goals.erase(it);
        }
        delete g;
    }
    removes.clear();

    // 必要なら：CollisionGoalVsGoals();
}

void GoalManager::Render(ID3D11DeviceContext* dc, Shader* shader) {
    for (Goal* g : goals) g->Render(dc, shader);
}

void GoalManager::Register(Goal* goal) {
    if (goal) goals.emplace_back(goal);
}

void GoalManager::Remove(Goal* goal) {
    if (goal) removes.insert(goal);
}

void GoalManager::DrawDebugPrimitive() {
    for (Goal* g : goals) g->DrawDebugPrimitive();
}

void GoalManager::Clear() {
    for (Goal* g : goals) delete g;
    goals.clear();
    removes.clear();
}

void GoalManager::CollisionGoalVsGoals() {
    const size_t n = goals.size();
    for (size_t i = 0; i < n; ++i) {
        Goal* a = goals[i];
        for (size_t j = i + 1; j < n; ++j) {
            Goal* b = goals[j];
            DirectX::XMFLOAT3 out;
            if (Collision::IntersectSphereVsSphere(
                a->GetPosition(), a->GetRadius(),
                b->GetPosition(), b->GetRadius(),
                out)) {
                b->SetPosition(out);
            }
        }
    }
}

void GoalManager::CollisionPlayerVsGoals(Player* player) {  // ★ クラスメンバーとして定義
    using namespace DirectX;

    const XMFLOAT3 ppos = player->GetPosition();
    const float    prad = player->GetRadius();

    XMFLOAT3 dummy; // out用（使わない）

    for (Goal* g : goals) {
        if (!g) continue;
        if (Collision::IntersectSphereVsSphere(
            ppos, prad,
            g->GetPosition(), g->GetRadius(),
            dummy)) {
            // 当たったら遅延削除
            Remove(g);
            // ここでスコアやSEなど
            if (ui)ui->clearcount += 1;
        }
    }
}
