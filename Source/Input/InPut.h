#pragma once
#include"Source/Math/Vector2.h"
#include<iostream>
#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>
#include <wrl.h>
#include<array>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class Input {
public:
	Input()=default;
	~Input()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HINSTANCE hInstance, HWND hwnd);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_?)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber) const;

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_?)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber) const;

	/// <summary>
	/// マウスの押下をチェック
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(左:0,右:1,中:2,拡張マウスボタン:3~7)</param>
	/// <returns>押されているか</returns>
	bool IsPressMouse(int32_t mouseNumber) const;

	/// <summary>
	/// マウスのトリガーをチェック
	/// </summary>
	/// <param name="buttonNumber">マウスボタン番号(左:0,右:1,中:2,拡張マウスボタン:3~7)</param>
	/// <returns>トリガーか</returns>
	bool IsTriggerMouse(int32_t buttonNumber) const;

	/// <summary>
	/// マウスの位置を取得する（ウィンドウ座標系）
	/// </summary>
	/// <returns>マウスの位置</returns>
	const Vector2& GetMousePosition() const;

	/// <summary>
	/// マウスの移動量を取得（ウィンドウ座標系）
	/// </summary>
	/// <returns></returns>
	const Vector2& GetMouseDelta();

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns>ホイールスクロール量。奥側に回したら+。Windowsの設定で逆にしてたら逆</returns>
	int32_t GetWheel() const;

private:
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;

private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouseDevice_;

	std::array<BYTE, 256> keys_;
	std::array<BYTE, 256> preKeys_;
	DIMOUSESTATE2 mouse_;
	DIMOUSESTATE2 preMouse_;
	HWND hwnd_;
	POINT point_;
	Vector2 mousePosition_;
	Vector2 preMousePosition_;
	Vector2 mouseDelta_;
};