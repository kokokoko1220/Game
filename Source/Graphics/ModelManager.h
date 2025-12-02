#pragma once

#include <memory>
#include <string>
#include <map>
#include "ModelResource.h"

// リソースマネージャー
class ModelManager
{
private:
	ModelManager() {}
	~ModelManager() {}

public:
	// 唯一のインスタンス取得
	static ModelManager& Instance()
	{
		static ModelManager instance;
		return instance;
	}

	// モデルリソース読み込み
	std::shared_ptr<ModelResource> LoadModelResource(const char* filename);

	void DrawDebugGUI();

private:
	using ModelMap = std::map<std::string, std::weak_ptr<ModelResource>>;

	ModelMap		models;
};
