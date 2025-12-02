#pragma once

#include <DirectXMath.h>

#include "RenderState.h"

// レンダーコンテキスト
struct RenderContext
{
	ID3D11DeviceContext* deviceContext;
	const RenderState* renderState = nullptr;
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;
	DirectX::XMFLOAT4		lightDirection;
};
