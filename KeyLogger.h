#pragma once
#include <chrono>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>
#include <Windows.h>

// Функциональность:
// 1. Задать клавиши управления (возможно несколько для одного действия)
// 2. Записывать время нажатия/отпускания клавиши
// 3. Для торможения расчитывать продолжительность нажатия после отпускания
// 4. Специальные клавиши для управления программой (запуск, остановка, отметки и т.д.)
// 5. Использовать числовые клавиши для отметок

/// Controls
enum class Control
{
	Throttle,
	Brake,
	Right,
	Left,
	Turn,
	Alo,
	Enter,
	Escape
};

const std::map<Control, std::string> CONTROL_NAMES{
	{Control::Throttle, "Throttle"},
	{Control::Brake, "Brake"},
	{Control::Right, "Right"},
	{Control::Left, "Left"},
	{Control::Turn, "Turn"},
	{Control::Alo, "Alo"},
	{Control::Enter, "Enter"},
	{Control::Escape, "Escape"},
};

class KeyLogger
{
public:
	void SetOutput(std::ostream* output);
	void MapKeys(const std::map<int, Control>& keys);
	void StartRecording();
	void StopRecording();
	void Log(int nCode, WPARAM wParam, LPARAM lParam);

private:
	std::ostream* _output = &std::cout;
	bool _isRecording = false;
	std::map<int, Control> _mapping;
	std::map<Control, bool> _states;
	std::vector<int> _digits;
	std::chrono::high_resolution_clock::time_point _enterTime;
	std::chrono::system_clock::time_point _enterTimeApprox;

	struct Event { Control control; bool isDown; std::chrono::high_resolution_clock::time_point time; };
	std::vector<Event> _events;

	void Enter();
	void Reset();
	void LogEvents();
};
