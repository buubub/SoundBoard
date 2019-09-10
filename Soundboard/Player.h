#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<algorithm>
#include"Hotkey.h"
#include"bass.h"
using namespace std;

class FileHandler {
public:
	ifstream file;

	string getConfig(string value) {
		file.open("config.conf");
		string line;
		if (file.fail()) {
			cout << "config.conf not found!" << endl;
			system("pause");
			exit(0);
		}
		else if (file.is_open()) {
			while (getline(file, line)) {
				if (line.compare("") == 0 || line.at(0) == '#') {
					continue;
				}
				size_t delimiter = line.find_first_of(' ');
				if (line.substr(0, delimiter) == value) {
					file.close();
					return line.substr(delimiter + 1);
				}
			}
			file.close();
			cout << "Value " << value << " not found in config.conf" << endl;
			system("pause");
			exit(0);
		}
	}

	map<string, string> getInput() {
		map<string, string> input;
		file.open("input.conf");
		string line;
		if (file.fail()) {
			cout << "input.conf not found!" << endl;
			system("pause");
			exit(0);
		}
		else if (file.is_open()) {
			while (getline(file, line)) {
				if (line.compare("") == 0 || line.at(0) == '#') {
					continue;
				}
				size_t delimiter = line.find_first_of(' ');
				input[line.substr(0, delimiter)] = line.substr(delimiter + 1);
			}
			file.close();
		}
		return input;
	}

	void openDirectory(string dir) {
		string directory = "explorer ";
		directory.append(dir);
		system(directory.c_str());
		cout << "Opening Directory" << endl;
	}	
};

class Player {
private:
	string dir;
	bool paused = false;
	vector<int> device;
	DWORD vol;
	HSTREAM stream;
	FileHandler handler;
	Hotkey hotkey;
	MSG msg;

	void initDirectory() {
		dir = handler.getConfig("dir");
		dir.append("\\");
	}

	void showDirectory() {
		cout << "Directory: " << dir << endl;
	}

	void initVolume() {
		string strVol = handler.getConfig("vol");
		vol = (DWORD) stoi(strVol);
	}

	void initHotkey() {
		hotkey.input = handler.getInput();
	}

	void initDevice() {
		string strdevice = handler.getConfig("device");
		size_t count = std::count(strdevice.begin(), strdevice.end(), ',');
		size_t offset = 0;
		for (int i = 0; i < (int) count+1; i++) {
			size_t delimiter = strdevice.find_first_of(',', offset);
			device.push_back(stoi(strdevice.substr(offset, delimiter)));
			offset = delimiter + 1;
			BASS_Init(device[i], 44100, 0, 0, NULL);
		}
	}

	void showDevice() {
		BASS_DEVICEINFO info;
		cout << endl << "Used Device: " << endl;
		for (int i = 0; i < (int) device.size(); i++) {
			BASS_GetDeviceInfo(device[i], &info);
			cout << "Device ID: " << device[i] << endl;
			cout << "Device Name: " << info.name << endl;
			cout << "Device Driver: " << info.driver << endl << endl;
		}
	}

	void showAvailableDevice() {
		BASS_DEVICEINFO info;
		cout << endl << "Available Device: " << endl;
		for (int i = 1; BASS_GetDeviceInfo(i, &info); i++) {
			if (info.flags&BASS_DEVICE_ENABLED) {
				cout << "Device ID: " << i << endl;
				cout << "Device Name: " << info.name << endl;
				cout << "Device Driver: " << info.driver << endl << endl;
			}
		}
	}

	void showHotkey() {
		cout << endl << "Hotkey: " << endl;
		for (map<string, string>::iterator iterator = hotkey.input.begin(); iterator != hotkey.input.end(); iterator++) {
			cout << iterator->first << " : " << iterator->second << endl;
		}
		cout << endl;
	}

	void showAvailableHotkey() {
		cout << endl << "Available Modifier:" << endl;
		for (map<string, UINT>::const_iterator iterator = hotkey.inputModifier.begin(); iterator != hotkey.inputModifier.end(); iterator++) {
			cout << iterator->first << endl;
		}
		cout << endl << "Available Key:" << endl;
		for (map<string, UINT>::const_iterator iterator = hotkey.inputKey.begin(); iterator != hotkey.inputKey.end(); iterator++) {
			cout << iterator->first << endl;
		}
		cout << endl;
	}

	void remapHotkey() {
		cout << "Remap Hotkey" << endl;
		hotkey.unregisterHotkey();
		initHotkey();
		hotkey.registerHotkey();
		showHotkey();
	}

	void play(string audio) {
		cout << "Playing: " << audio << endl;
		const char* file;
		string directory = dir;
		file = directory.append(audio).c_str();
		for (int i = 0; i < (int) device.size(); i++) {
			BASS_SetDevice(device[i]);
			stream = BASS_StreamCreateFile(FALSE, file, 0, 0, BASS_STREAM_AUTOFREE);
			if (BASS_ErrorGetCode() != 0) {
				cout << "Device " << device[i] << " Error: File not found" << endl;
			}
			BASS_ChannelPlay(stream, FALSE);
			if (BASS_ErrorGetCode() != 0) {
				cout << "Device " << device[i] << " Error: Handle Error" << endl;
			}
		}
	}

	void pause() {
		if (paused) {
			for (int i = 0; i < (int) device.size(); i++) {
				BASS_SetDevice(device[i]);
				BASS_Start();
			}
			cout << "Resumed" << endl;
			paused = false;
		}
		else {
			for (int i = 0; i < (int) device.size(); i++) {
				BASS_SetDevice(device[i]);
				BASS_Pause();
			}
			cout << "Paused" << endl;
			paused = true;
		}
	}

	void stop() {
		for (int i = 0; i < (int)device.size(); i++) {
			BASS_SetDevice(device[i]);
			BASS_Stop();
			BASS_Start();
		}
		cout << "Stopped" << endl;
	}

	void setVol(bool up) {
		if (up) {
			vol += 1000;
			if (vol > 10000) {
				vol = 10000;
			}
		}
		else {
			vol -= 1000;
			if (vol < 0) {
				vol = 0;
			}

		}
		cout << "Volume: " << vol << endl;
		BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, vol);
	}

	void refreshConfig() {
		cout << "Refresh Config" << endl;
		free();
		device.clear();
		initDevice();
		showDevice();
		initDirectory();
		showDirectory();
		initVolume();
	}

public:
	Player() {
		initDevice();
		showDevice();
		initDirectory();
		showDirectory();
		initVolume();
		initHotkey();
		showHotkey();
		hotkey.registerHotkey();
	}

	void run() {
		while (GetMessage(&msg, 0, 0, 0)) {
			if (msg.message == WM_HOTKEY) {
				for (int i = 0; i < (int) hotkey.key.size(); i++) {
					if (msg.wParam == i) {
						if (hotkey.input.find(hotkey.key.at(i))->second == "pause") {
							pause();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "stop") {
							stop();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "set_vol_up") {
							setVol(true);
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "set_vol_down") {
							setVol(false);
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "show_hotkey") {
							showHotkey();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "show_device") {
							showDevice();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "show_available_hotkey") {
							showAvailableHotkey();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "show_available_device") {
							showAvailableDevice();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "show_directory") {
							showDirectory();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "open_directory") {
							handler.openDirectory(dir);
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "remap_hotkey") {
							remapHotkey();
						}
						else if (hotkey.input.find(hotkey.key.at(i))->second == "refresh_config") {
							refreshConfig();
						}
						else {
							play(hotkey.input.find(hotkey.key.at(i))->second);
						}
						break;
					}
				}
			}
		}
	}

	void free() {
		for (int i = 0; i < (int) device.size(); i++) {
			BASS_SetDevice(device[i]);
			BASS_Free();
		}
	}
};