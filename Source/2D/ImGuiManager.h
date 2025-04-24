#pragma once
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"

#include"Source/Core/WindowsApp.h"
#include"Source/Core/DirectXCommon.h"

class ImGuiManager {
public:

	ImGuiManager();
	~ImGuiManager();

	void Initialize(WindowsApp* windowsApp, DirectXCommon* dxCommon);

	void BeginFrame();

	void EndFrame();

	void Draw();

	void Finalize();

private:
	//ImGuiManager() = default;
	//~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

	WindowsApp* windowsApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	// ビューポート
	D3D12_VIEWPORT viewport_{};
	// シザー矩形
	D3D12_RECT scissorRect_{};
};
