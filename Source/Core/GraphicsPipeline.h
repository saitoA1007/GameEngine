#pragma once
#include <d3d12.h>
#include<dxcapi.h>
#include<iostream>
#include <wrl.h>
#include"Source/Common/LogManager.h"

#pragma comment(lib,"dxcompiler.lib")

class GraphicsPipeline {
public:

    GraphicsPipeline() = default;
    ~GraphicsPipeline() = default;

    // 初期化
    void Initialize(ID3D12Device* device, LogManager* logManager);

    // 解放処理
    void Finalize();

    ID3D12PipelineState* GetPipelineState() { return graphicsPipelineState_.Get(); }
    ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

private:
    GraphicsPipeline(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

    ID3D12Device* device_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;

    // Shaderをコンパイルする
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    // ログ
    LogManager* logManager_;

private:

    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
        // CompilerするShaderファイルへのパス
        const std::wstring& filePath,
        // Compilerに使用するProfile
        const wchar_t* profile,
        // 初期化で生成したものを3つ
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler);
};
