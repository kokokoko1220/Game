#include "Graphics/Graphics.h"
#include "SceneGame.h"
#include"Camera.h"
#include "EnemyManager.h"
#include "Enemycar.h"
#include "EnemyPolice.h"
#include "ItemManager.h"
#include "Bottle.h"
#include "CreatureManager.h"
#include "Dustbox.h"
#include"EffectManager.h"
#include"SceneManager.h"
#include"GameUI.h"
#include"Goal.h"
#include"GoalManager.h"
#include <vector>
#include <random> 

using namespace DirectX;
float GetRandomFloat(float min, float max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

// 敵のスポーン候補座標リスト//kokoko
std::vector<DirectX::XMFLOAT3> enemySpawnPoints = {
	{ 71.0f, 0.0f, -9.0f },   // 家の前（画像①）
	{ 31.0f, 0.0f, -40.0f },  // 画像②の位置
	{ -78.0f, -0.861f, -49.77f }, //家の前2 
	{ -74.0f, -0.877f, 91.987f },  // 曲がり角
	{ 78.404f, -0.474f, 95.122f }    // 右側奥
};
//policeポジ
std::vector<DirectX::XMFLOAT3> enemyPoliceSpawnPoints = {
	//{ 75.375f, 0.0f, 66.5f },
	{ 75.375f, 0.0f, 5.5f },
	{ -79.122f, 0.0f, 95.7f }, // 交差２
};

// ボトルのスポーン候補座標リスト//kokoko
std::vector<DirectX::XMFLOAT3> itemSpawnPoints = {
	{ 75.0f, 0.0f, -36.0f },   // 家の前（画像①）
	{ 50.224f, 0.0f, -34.0f },  // 家の前端
	{ -57.0f, 0.0f, -42.061f }, //車前
	{ -77.707f, 0.0f, -53.786f },  // enemy前
	{ 77.544f, 0.0f, -8.0f },    // 道
	{ 77.517f, 0.0f, -5.0f },   // 道２
	{ -82.882f, 0.0f, 34.0f },  // 家2
	{ -76.068f, 0.0f, 79.241f }, //曲がり角
	{ -71.149f, 0.0f, 105.318f },  // kado前
	{ 19.450f, 0.0f, 81.465f },  // kusa
	{ 51.814f, 0.0f, 48.643f }, //交差
	{ 60.403f, 0.0f, 37.897f },  // 交差２
	{ 77.439f, 0.0f, 15.110f }    // 右側
};

// 初期化
void SceneGame::Initialize()
{
	//ステージ初期化
	stage = new Stage();
	player = new Player();
	gameUI = new GameUI();

	//カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 10, -10),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0));
	camera.SetPerspectiveFoc(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f);
	cameraController = new CameraController();

	

	//プレイヤー初期化
	Player::Instance().Reset();

	//エネミー初期化
	EnemyManager& enemyManager = EnemyManager::Instance();
	/*for (int i = 0; i < 2; i++) {
		Enemycar* car = new Enemycar();
		car->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 5));
		enemyManager.Register(car);
	}*/
	for (const auto& pos : enemySpawnPoints)
	{
		Enemycar* car = new Enemycar();
		car->SetPosition(pos);
		enemyManager.Register(car);
	}
	///一気に出る
	//追尾エネミー
	/*for (int i = 0; i < 1; i++) {
		EnemyPolice* Police = new EnemyPolice();
		Police->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, 10));
		enemyManager.Register(Police);
	}*/
	//Police用
	for (const auto& pos : enemyPoliceSpawnPoints)
	{
		EnemyPolice* police = new EnemyPolice();
		police->SetPosition(pos);
		enemyManager.Register(police);
	}

	//アイテム(ボトル)初期化
	ItemManager& itemManager = ItemManager::Instance();
	for (int i = 0; i < 1; i++) {
		Bottle* rum = new Bottle();
		rum->SetPosition(DirectX::XMFLOAT3(i * 73.5f, 0, 2));
		itemManager.Register(rum);
	}
	//物
	CreatureManager& creatureManager = CreatureManager::Instance();
	for (int i = 0; i < 1; i++) {
		Dustbox* dust = new Dustbox();
		dust->SetPosition(DirectX::XMFLOAT3(i * 0.0f, 0, 2));
		creatureManager.Register(dust);
	}
	player = &Player::Instance();
	//配達位置
	GoalManager& goalManager = GoalManager::Instance();
	for (int i = 0; i <= 2; i++)
	{
		Goal* goal = new Goal();
		goal->SetPosition(DirectX::XMFLOAT3(i * 10 - 10, 0, 0));
		goalManager.Register(goal);
	}
	// UI ������ď�����
	gameUI = new GameUI();
	gameUI->Initialize();
	//Player::Instance().SetUI(gameUI);kokoko
	player->SetUI(gameUI);	//kokokoko
	gameUI->SetPlayer(player);
}

// 終了化
void SceneGame::Finalize()
{
  
	//エネミー終了化
	EnemyManager::Instance().Clear();
	//アイテム終了化
	ItemManager::Instance().Clear();
	//物初期化
	CreatureManager::Instance().Clear();

	GoalManager::Instance().Clear();
	//カメラコントローラー終了化
	if (cameraController != nullptr) {
		delete cameraController;
		cameraController = nullptr;
	}

	//ステージ終了化
	if (stage != nullptr) {
		delete stage;
		stage = nullptr;
	}

	

	gameUI->Finalize();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	//カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = Player::Instance().GetPosition();
	target.y += 0.5f;
	cameraController->SetTarget(target);
	cameraController->Update(elapsedTime);


	//ステージ更新処理
	stage->Update(elapsedTime);
	

	//プレイヤー更新処理
	Player::Instance().Update(elapsedTime);

	//エネミー更新処理
	EnemyManager::Instance().Update(elapsedTime);
	
	//アイテム更新処理
	ItemManager::Instance().Update(elapsedTime);
	//SceneManager::Instance().Update(elapsedTime);
	GoalManager::Instance().Update(elapsedTime, player);
	// ====================== =
	// アイテムのスポーン処理
	// =======================kokoko
	spawnTimerBO += elapsedTime;

	if (spawnTimerBO >= spawnIntervalBO)
	{
		spawnTimerBO = 0.0f; // タイマーリセット

		ItemManager& itemManager = ItemManager::Instance();

		// マップ全体での敵の上限をチェック
		if (itemManager.GetItemCount() >= MAX_ITEM)
			return; // これ以上出現させない

		// ======= ランダムに1つの座標を選ぶ（前回と同じ場所を避ける） =======
		static int lastIndexBO = -1;  // ←★関数内static（1回だけ保持される）
		int index;
		do {
			index = (int)GetRandomFloat(0, (float)itemSpawnPoints.size());
		} while (index == lastIndexBO && itemSpawnPoints.size() > 1);
		lastIndexBO = index;

		// ======= 選ばれたスポーンポイントを取得 =======
		DirectX::XMFLOAT3 pos = itemSpawnPoints[index];

		// ======= 高さを地形に合わせる =======
		float y = 0.0f;
		HitResult hit;
		DirectX::XMFLOAT3 start = { pos.x, 50.0f, pos.z };
		DirectX::XMFLOAT3 end = { pos.x, -50.0f, pos.z };
		if (Stage::Instance().RayCast(start, end, hit))
		{
			y = hit.position.y + 0.3f;
		}

		// ======= アイテム生成 =======
		Bottle* rum = new Bottle();
		rum->SetPosition({ pos.x, y, pos.z });
		itemManager.Register(rum);
	}

	//物の更新処理
	CreatureManager::Instance().Update(elapsedTime);
	
	//エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	gameUI->Update(elapsedTime);
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	//カメラパラメーター設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();


	
	// 3Dモデル描画
	{
		Shader* shader = graphics.GetShader();
		shader->Begin(dc, rc);

		//ステージ描画
		stage->Render(dc, shader);

		//エネミー描画
		EnemyManager::Instance().Render(dc, shader);

		//アイテム描画
		ItemManager::Instance().Render(dc, shader);
		
		//物描画
		CreatureManager::Instance().Render(dc, shader);

		GoalManager::Instance().Render(dc, shader);
		//プレイヤー描画
		Player::Instance().Render(dc, shader);

		//エフェクト描画

		EffectManager::Instance().Render(rc.view, rc.projection);

		//シェーダー終了
		shader->End(dc);


	}

	// 3Dデバッグ描画
	{

		// プレイヤーデバッグプリミティブ描画
		Player::Instance().DrawDebugPrimitive();

		// エネミーデバッグプリミティブ描画
		EnemyManager::Instance().DrawDebugPrimitive();

		// アイテムデバッグプリミティブ描画
		ItemManager::Instance().DrawDebugPrimitive();

		//物デバッグプリミティブ描画
		CreatureManager::Instance().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
	}

	// 2Dスプライト描画
	{
		gameUI->Render();
		//SceneManager::Instance().Render();
	}

	// 2DデバッグGUI描画
	{

	//プレイヤーデバッグ描画
		Player::Instance().DrawDebugGUI();
	
	}
}
