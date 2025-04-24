#include"InPut.h"
#include<cassert>

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {

	hwnd_ = hwnd;

	// DirectInputの初期化
	directInput_ = nullptr;
	HRESULT result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイスの生成
	keyboard_ = nullptr;
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));
	// 排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// マウスデバイスの生成
	mouseDevice_ = nullptr;
	result = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = mouseDevice_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));
	// マウスの排他制御レベルのセット
	result = mouseDevice_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));
}

void Input::Update() {

	// キーボード情報の取得開始
	keyboard_->Acquire();
	// 全キーの前フレームのの入力状態をコピー
	preKeys_ = keys_;
	// 全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(keys_), keys_.data());

	// マウス情報を取得
	mouseDevice_->Acquire();
	// 前フレームのマウスの状態をコピー
	preMouse_ = mouse_;
	mouseDevice_->GetDeviceState(sizeof(mouse_), &mouse_);
	
	// Windowのカーソル位置を取得(スクリーン座標)
	GetCursorPos(&point_);
	// 自分のwindow基準の座標に変換
	ScreenToClient(hwnd_, &point_);
	// 前フレームの位置を取得する
	preMousePosition_ = mousePosition_;
	// Vector2に格納
	mousePosition_.x = static_cast<float>(point_.x);
	mousePosition_.y = static_cast<float>(point_.y);
}

bool Input::PushKey(BYTE keyNumber) const {
	if (keys_[keyNumber] == 0x80) {
		return true;
	} else {
		return false;
	}
}

bool Input::TriggerKey(BYTE keyNumber) const {
	if (keys_[keyNumber] == 0x80 && preKeys_[keyNumber] == 0x00) {
		return true;
	} else {
		return false;
	}
}

bool Input::IsPressMouse(int32_t mouseNumber) const {
	if (mouse_.rgbButtons[mouseNumber] == 0x80) {
		return true;
	} else {
		return false;
	}
}

bool Input::IsTriggerMouse(int32_t buttonNumber) const {
	if (mouse_.rgbButtons[buttonNumber] == 0x80 && preMouse_.rgbButtons[buttonNumber] == 0x00) {
		return true;
	} else {
		return false;
	}
}

const Vector2& Input::GetMousePosition() const {
	return mousePosition_;
}

const Vector2& Input::GetMouseDelta() {
	// マウス位置の差分を取得
	mouseDelta_.x = mousePosition_.x - preMousePosition_.x;
	mouseDelta_.y = mousePosition_.y - preMousePosition_.y;
	return mouseDelta_;
}

int32_t Input::GetWheel() const {
	return static_cast<int32_t>(mouse_.lZ);
}