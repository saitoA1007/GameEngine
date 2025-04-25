#pragma once
#include"Source/Core/WindowsApp.h"
#include"Source/Core/DirectXCommon.h"
#include"Source/Core/GraphicsPipeline.h"
#include"Source/Core/TextureManager.h"

#include"Source/Common/LogManager.h"
#include"Source/Common/ResourceLeakChecker.h"
#include"Source/Common/CrashHandle.h"

#include"Source/3D/Camera.h"
#include"Source/3D/DebugCamera.h"
#include"Source/3D/Model.h"
#include"Source/3D/DirectionalLight.h"

#include"Source/2D/ImGuiManager.h"
#include"Source/2D/Sprite.h"

#include"Source/Math/TransformationMatrix.h"

#include"Source/Input/InPut.h"

#include"Source/Audio/AudioManager.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")