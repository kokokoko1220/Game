#include <filesystem>
#include <imgui.h>
#include "Graphics.h"
#include "ModelManager.h"

// モデルリソース読み込み
std::shared_ptr<ModelResource> ModelManager::LoadModelResource(const char* filename)
{
	//すでに読み込まれていた場合は読み込み済みのリソースを返す

	std::string key = filename;

	// すでに読み込まれていた場合は読み込み済みのリソースを返す
	auto it = models.find(key);
	if (it != models.end())
	{
		if (auto shared = it->second.lock())
		{
			return shared; // キャッシュされているリソースを再利用
		}
	}


	//新規モデルリソース作成＆読み込み
	auto resource = std::make_shared<ModelResource>();
	resource->Load(Graphics::Instance().GetDevice(), filename);


	//読み込み管理用の変数に登録

	models[key] = resource;
	//作成したリソースを返す


	return  resource;
	//return nullptr;
}
