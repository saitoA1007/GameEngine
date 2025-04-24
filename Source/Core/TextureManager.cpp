#include"TextureManager.h"
#include"Source/Common/ConvertString.h"
#include"Source/Common/CreateBufferResource.h"
#include"Source/Common/DescriptorHandle.h"

TextureManager::TextureManager(){}
TextureManager::~TextureManager() {}

void TextureManager::Initialize(DirectXCommon* dxCommon, LogManager* logManager) {
	dxCommon_ = dxCommon;
	logManager_ = logManager;
}

void TextureManager::Finalize() {

	for (auto& tex : textures_) {
		tex.textureResource.Reset();
	}

	for (auto& inter : intermediateResources_) {
		inter.Reset();
	}

	intermediateResources_.clear();	
}

uint32_t TextureManager::Load(const std::string& fileName) {

	// もし同じテクスチャを読み込んだのであれば、すでに格納されている配列番号を返す。
	for (int i = 0; i < textures_.size(); ++i) {
		if (textures_.at(i).fileName == fileName) {
			return i;
		}
	}

	// テクスチャ読み込みの最大数より小さければ追加する処理
	if (index_ < kTextureNum_) {
		index_++;
	} else {
		return -1;
	}

	textures_.at(index_).fileName = fileName;

		// テクスチャを読み込む
	textures_.at(index_).mipImage = LoadTexture(fileName);
	if (!textures_.at(index_).mipImage.GetImages()) {
		logManager_->Log("Failed to load texture: " + fileName);
		assert(false);
	}
	metadata_ = &textures_.at(index_).mipImage.GetMetadata();
	// テクスチャリソースを作成
	textures_.at(index_).textureResource = CreateTextureResource(dxCommon_->GetDevice(), *metadata_);
	if (!textures_.at(index_).textureResource) {
		logManager_->Log("Failed to create texture resource for: " + fileName);
		assert(false);
	}
	// テクスチャデータをアップロード
	intermediateResources_.push_back(UploadTextureData(textures_.at(index_).textureResource.Get(), textures_.at(index_).mipImage, dxCommon_->GetDevice(), dxCommon_->GetCommandList()));
	if (!intermediateResources_.at(index_)) {
		logManager_->Log("Failed to upload texture data for: " + fileName);
		assert(false);
	}

	// metaDataを基にSRVの設定
	srvDesc_.Format = metadata_->format;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2Dテクスチャ
	srvDesc_.Texture2D.MipLevels = UINT(metadata_->mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める。先頭はImGuiが使っているのでその次を使う
	textures_.at(index_).textureSrvHandleCPU = GetCPUDescriptorHandle(dxCommon_->GetSRVHeap(), dxCommon_->GetSRVDescriptorSize(), index_ + 1);
	textures_.at(index_).textureSrvHandleGPU = GetGPUDescriptorHandle(dxCommon_->GetSRVHeap(), dxCommon_->GetSRVDescriptorSize(), index_ + 1);
	logManager_->Log(std::format("CPU Handle: {}, GPU Handle: {}", textures_.at(index_).textureSrvHandleCPU.ptr, textures_.at(index_).textureSrvHandleGPU.ptr));
	// SRVを作成
	dxCommon_->GetDevice()->CreateShaderResourceView(textures_.at(index_).textureResource.Get(), &srvDesc_, textures_.at(index_).textureSrvHandleCPU);

	// 読み込んだ画像が格納されている配列番号を返す
	return index_;
}

D3D12_GPU_DESCRIPTOR_HANDLE& TextureManager::GetTextureSrvHandlesGPU(const uint32_t& textureHandle) {
	return textures_.at(textureHandle).textureSrvHandleGPU;
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath)
{
	// テクスチャファイルを読み込んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// ミップマップ付きのデータを返す
	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); // Textureの幅
	resourceDesc.Height = UINT(metadata.height); // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行 or 配列Textureの配列数 
	resourceDesc.Format = metadata.format; // TextureのFormat
	resourceDesc.SampleDesc.Count = 1; // ダンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。

	// 利用するHeapの設定。
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし。
		&resourceDesc, // Resourceの設定。
		D3D12_RESOURCE_STATE_COPY_DEST, // データ転送される設定
		nullptr, // Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへ転送後は利用出来るよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}