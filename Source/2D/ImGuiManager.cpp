#include"ImGuiManager.h"

ImGuiManager::ImGuiManager() {}
ImGuiManager::~ImGuiManager() {}

void ImGuiManager::Initialize(WindowsApp* windowsApp, DirectXCommon* dxCommon) {

	windowsApp_ = windowsApp;
	dxCommon_ = dxCommon;

	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = windowsApp_->kWindowWidth;
	viewport_.Height = windowsApp_->kWindowHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
	
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = windowsApp_->kWindowWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = windowsApp_->kWindowHeight;

	// ImGuiの初期化。
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowsApp_->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(),
		dxCommon_->GetSwapChainDesc().BufferCount,
		dxCommon_->GetRTVDesc().Format,
		dxCommon_->GetSRVHeap(),
		dxCommon_->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart(),
		dxCommon_->GetSRVHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::BeginFrame() {
	// ImGuiにフレームが始まる旨を伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
	// ImGuiの内部コマンドを生成する
	ImGui::Render();
	// Imguiの描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { dxCommon_->GetSRVHeap() };
	dxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	dxCommon_->GetCommandList()->RSSetViewports(1, &viewport_); // Viewportを設定
	dxCommon_->GetCommandList()->RSSetScissorRects(1, &scissorRect_);      // Scirssorを設定
}

void ImGuiManager::Draw() {
	// 実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
}

void ImGuiManager::Finalize() {
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}