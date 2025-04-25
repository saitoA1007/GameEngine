#pragma once
#include <d3d12.h>
#include<iostream>
#include<array>
#include <wrl.h>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

#include"DirectXCommon.h"
#include<format>
#include"Source/Common/LogManager.h"

namespace GameEngine {

	class TextureManager {
	public:
		TextureManager() = default;
		~TextureManager() = default;

		// テクスチャ
		struct Texture {
			// テクスチャリソース
			Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;

			DirectX::ScratchImage mipImage{};
			// シェーダリソースビューのハンドル(CPU)
			CD3DX12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
			// シェーダリソースビューのハンドル(CPU)
			CD3DX12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
			// 名前
			std::string fileName;
		};

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="dxCommon"></param>
		void Initialize(DirectXCommon* dxCommon, LogManager* logManager);

		// 解放処理
		void Finalize();

		/// <summary>
		/// テクスチャを読み込んで転送する処理
		/// </summary>
		/// <param name="fileName">ファイル名</param>
		/// <param name="logStream">ログファイル</param>
		/// <returns></returns>
		uint32_t Load(const std::string& fileName);


		D3D12_GPU_DESCRIPTOR_HANDLE& GetTextureSrvHandlesGPU(const uint32_t& textureHandle);

	private:
		//TextureManager() = default;
		//~TextureManager() = default;
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) = delete;

		// テクスチャの数
		static const int kTextureNum_ = 128;

		// DirectXCommon
		DirectXCommon* dxCommon_ = nullptr;

		// テクスチャデータを管理する変数
		std::array<Texture, kTextureNum_> textures_;

		const DirectX::TexMetadata* metadata_{};

		// metaDataを基にSRVの設定の変数
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc_{};

		// テクスチャアップロードするための変数
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> intermediateResources_;

		// テクスチャパス
		std::string texturePath_;

		// 配列の管理
		int32_t index_ = -1;

		// ログ
		LogManager* logManager_;

	private:

		DirectX::ScratchImage LoadTexture(const std::string& filePath);

		Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

		[[nodiscard]] Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
			ID3D12GraphicsCommandList* commandList);
	};
}
