#include "KeyLogger.h"
#include <iomanip>

using namespace std;
using namespace std::chrono;

namespace
{
	const map<pair<Control, bool>, string> CONTROL_LOG_NAMES{
		{{Control::Throttle, true}, "Gas"},
		{{Control::Brake, true}, "Brake"},
		{{Control::Right, true}, "Right"},
		{{Control::Left, true}, "Left"},
		{{Control::Turn, true}, "Turn"},
		{{Control::Alo, true}, "Alo"},
		{{Control::Enter, true}, "Enter"},
		{{Control::Escape, true}, "Escap"},
		{{Control::Throttle, false}, "Gas_"},
		{{Control::Brake, false}, "Brake_"},
	};
}

void KeyLogger::SetOutput(std::ostream* output)
{
	if (output)
		_output = output;
	else
		_output = &std::cout;
}

void KeyLogger::StartRecording()
{
	Reset();
	_isRecording = true;
}

void KeyLogger::StopRecording()
{
	_isRecording = false;
}

void KeyLogger::MapKeys(const std::map<int, Control>& keys)
{
	_mapping = keys;
}

void KeyLogger::Log(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!_isRecording || nCode != HC_ACTION)
		return;

	bool isDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
	// scancode
	int code = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam)->scanCode;

	// add digit to mark number
	if (code > 1 && code <= 11)
	{
		if (isDown)
			_digits.push_back(code == 11 ? 0 : code - 1);
		return;
	}

	// check if key is mapped to control
	auto key = _mapping.find(code);
	if (key == _mapping.end())
		return;

	auto control = key->second;

	// discard key repetition
	if (isDown && _states[control])
		return;
	_states[control] = isDown;

	// log key up only for throttle and brake
	if (!isDown && control != Control::Throttle && control != Control::Brake)
		return;

	// start new event sequence
	if (control == Control::Enter)
	{
		Enter();
		return;
	}

	// event sequence stoped
	if (!_events.empty() && _events.back().control == Control::Escape)
		return;

	// store event
	_events.emplace_back(Event{ control, isDown, high_resolution_clock::now() });
}

void KeyLogger::Enter()
{
	// put it first just in case (save time)
	auto newTime = high_resolution_clock::now();

	LogEvents();
	Reset();

	_enterTime = newTime;
	_enterTimeApprox = system_clock::now();
}

void KeyLogger::Reset()
{
	_events.clear();
	_digits.clear();
	// reset states
	_states[Control::Throttle] = false;
	_states[Control::Brake] = false;
	_states[Control::Right] = false;
	_states[Control::Left] = false;
	_states[Control::Turn] = false;
	_states[Control::Alo] = false;
	_states[Control::Enter] = false;
	_states[Control::Escape] = false;
}

void KeyLogger::LogEvents()
{
	// ignore empty sequence
	if ((_events.empty() || _events.back().control == Control::Escape) && _digits.empty())
		return;

	// log number mark
	if (!_digits.empty())
		*_output << "\n### ";
	for (const auto& digit : _digits)
		*_output << digit;

	// time of sequence enter
	auto tt = system_clock::to_time_t(_enterTimeApprox);
	tm tm{}; localtime_s(&tm, &tt);
	*_output << "\n" << put_time(&tm, "%T");

	// log sequence
	for (auto event = _events.begin(); event != _events.end(); ++event)
	{
		if (!event->isDown && event->control == Control::Brake)
			continue;
		duration<float> diff = event->time - _enterTime;

		*_output << "  " << CONTROL_LOG_NAMES.at({ event->control, event->isDown });
		*_output << " " << diff.count();

		if (!event->isDown || event->control != Control::Brake)
			continue;
		auto brakeOff = find_if(event, _events.end(), [](const Event& ev) {return ev.control == Control::Brake && ev.isDown == false; });
		if (brakeOff == _events.end())
			continue;

		duration<float> dur = brakeOff->time - event->time;
		*_output << "(+" << dur.count() << ")";
	}
}
