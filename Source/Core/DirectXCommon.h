#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <fstream>
#include"Source/Common/LogManager.h"

class DirectXCommon {
public:
    DirectXCommon()=default;
    ~DirectXCommon()=default;

    // 初期化
    void Initialize(HWND hwnd, uint32_t width, uint32_t height, LogManager* logManager);
    // 描画前処理
    void PreDraw(ID3D12RootSignature* rootSignature, ID3D12PipelineState* graphicsPipelineState);
    // 描画後処理
    void PostDraw();

public:
    ID3D12Device* GetDevice() const { return device_.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }
    IDXGISwapChain4* GetSwapChain() const { return swapChain_.Get(); }

    ID3D12DescriptorHeap* GetRTVHeap() const { return rtvHeap_.Get(); }
    ID3D12DescriptorHeap* GetSRVHeap() const { return srvHeap_.Get(); }
    ID3D12DescriptorHeap* GetDSVHeap() const { return dsvHeap_.Get(); }

    uint32_t GetBackBufferIndex() const { return backBufferIndex_; }

    uint32_t GetSRVDescriptorSize() const { return descriptorSizeSRV_; }
    uint32_t GetRTVDescriptorSize() const { return descriptorSizeRTV_; }
    uint32_t GetDSVDescriptorSize() const { return descriptorSizeDSV_; }

    DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() const { return swapChainDesc; }

    D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() const { return rtvDesc; }

private:
    //DirectXCommon() = default;
    //~DirectXCommon() = default;
    DirectXCommon(const DirectXCommon&) = delete;
    DirectXCommon& operator=(const DirectXCommon&) = delete;

    // DirectXデバイスの生成
    void CreateDevice();
    // 描画命令を記録・実行するためのオブジェクト群を生成
    void CreateCommandObjects();
    // 画面表示のための「表裏バッファ」セット（スワップチェーン）を作成
    void CreateSwapChain(HWND hwnd, uint32_t width, uint32_t height);
    // バックバッファ（SwapChain のリソース）に対して描画可能にするビューを作成
    void CreateRenderTargetViews();
    // Z値（深さ）やステンシル情報を書き込むためのバッファとビューを作成
    void CreateDepthStencilView(uint32_t width, uint32_t height);
    // CPU と GPU 間の同期を取るための Fence オブジェクトを作成
    void CreateFence();
    // GPUを待つ処理 
    void WaitForGPU();

#ifdef _DEBUG
    void DebugLayer();
#endif

private:
    static const uint32_t kSwapChainBufferCount = 2;

    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;

    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kSwapChainBufferCount> swapChainResources_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;

    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    uint64_t fenceValue_ = 0;
    HANDLE fenceEvent_ = nullptr;

    uint32_t backBufferIndex_ = 0;
    uint32_t descriptorSizeRTV_ = 0;
    uint32_t descriptorSizeSRV_ = 0;
    uint32_t descriptorSizeDSV_ = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2] = {};

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

    // ログ
    LogManager* logManager_;
};
