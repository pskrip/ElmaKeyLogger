#include "KeyLogger.h"
#include <fstream>
#include <Windows.h>
#include <iomanip>

KeyLogger logger;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	logger.Log(nCode, wParam, lParam);
	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

using namespace std;

const string CONFIG_FILE = "config.ini";

void main()
{
	// map names to controls
	map<string, Control> nameControlMap;
	for (const auto& control : CONTROL_NAMES)
		nameControlMap[control.second] = control.first;

	// read key mapping from file
	auto keyMapFile = ifstream(CONFIG_FILE);
	if (!keyMapFile.is_open())
	{
		cout << "Config file '" << CONFIG_FILE << "' not found!" << endl;
		system("pause");
		return;
	}

	map<int, Control> keys;
	string name;
	int key = -1;
	while (keyMapFile >> name >> hex >> key)
	{
		if (key <= 0 || nameControlMap.find(name) == nameControlMap.end())
		{
			cout << "Error in config file!" << endl;
			system("pause");
			return;
		}
		keys[key] = nameControlMap[name];
	}

	logger.MapKeys(keys);

	auto file = ofstream("keylog.txt", ofstream::app);
	file << fixed << setprecision(3);
	logger.SetOutput(&file);

	SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
	logger.StartRecording();

	// TODO: переделать без этого сообщения (использовать консоль)
	MessageBox(nullptr, L"Press OK to stop logging.", L"Information", MB_OK);

	logger.StopRecording();
}
