#pragma once
#include <memory>
#include "Graphics/Sprite.h"
#include "Scene.h" 

class Player;

class GameUI :public Scene
{
public:
    void Initialize();
    void Finalize();
    void Render();
    void DrawGUI();
    void Update(float elapsedTime);
    void SetPlayer(Player* p) { player = p; };
    float gauge = 1;
    int gauge_MAX = 320 - 50;
    int gauge_MIN = 620 - 32;
    int iteam = 0;
    int clearcount = 0;
    bool gauge_UP_switch = false;
    bool cool_time_switch = false;
private:
    //std::unique_ptr<Sprite> sprite; // 安全なRAII管理
    Sprite* sprite = nullptr;
    Sprite* sprite2 = nullptr;
    Sprite* sprite3 = nullptr;

    Sprite* face = nullptr;
    Sprite* white = nullptr;
    Sprite* Bottole = nullptr;
    Sprite* one = nullptr;
    Sprite* two = nullptr;
    Sprite* three = nullptr;
    Sprite* start = nullptr;

    int displayCount;
    float countdown = 3.0f;  // 3秒カウント
    float timer = 0.0f;
    float Density = 0;
    float gauge_UP;
    float time = 0;

    float arrowAngle = 0.0f;
    bool  arrowVisible = false;
    Player* player = nullptr;

    float cool_time = 3;


    bool wasSpacePressed = false; // クラスメンバーに


    float blendFactor[4] = { 0,0,0,0 };
    ID3D11BlendState* blendStateEnable = nullptr;
    ID3D11BlendState* blendStateDisable = nullptr;

    ID3D11BlendState* alphaBlend = nullptr;



};

