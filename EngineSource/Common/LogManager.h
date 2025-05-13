#pragma once
#include<iostream>
#include<fstream>
#include <Windows.h>
#include<filesystem>
#include<chrono>

class LogManager {
public:

	/// <summary>
	/// ログを書き込むため準備
	/// </summary>
	static void Create();

	/// <summary>
	/// ログを出力
	/// </summary>
	/// <param name="message">出力するメッセージ</param>
	void Log(const std::string& message);

	std::ofstream GetLogStream() {}

private:
	static std::ofstream logStream_;
};