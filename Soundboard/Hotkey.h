#include<map>
#include<vector>
#include<Windows.h>
using namespace std;

class Hotkey {
public:
	map<string, string> input;
	vector<string> key;

	const map<string, UINT> inputModifier = {
		{ "Shift", MOD_SHIFT },
		{ "Ctrl", MOD_CONTROL },
		{ "Alt", MOD_ALT }
	};

	const map<string, UINT> inputKey = {
		{ "0", 0x30 },{ "1", 0x31 },{ "2", 0x32 },{ "3", 0x33 },{ "4", 0x34 },{ "5", 0x35 },{ "6", 0x36 },
		{ "7", 0x37 },{ "8", 0x38 },{ "9", 0x39 },
		{ "A", 0x41 },{ "B", 0x42 },{ "C", 0x43 },{ "D", 0x44 },{ "E", 0x45 },{ "F", 0x46 },{ "G", 0x47 },
		{ "H", 0x48 },{ "I", 0x49 },{ "J", 0x4A },{ "K", 0x4B },{ "L", 0x4C },{ "M", 0x4D },{ "N", 0x4E },
		{ "O", 0x4F },{ "P", 0x50 },{ "Q", 0x51 },{ "R", 0x52 },{ "S", 0x53 },{ "T", 0x54 },{ "U", 0x55 },
		{ "V", 0x56 },{ "W", 0x57 },{ "X", 0x58 },{ "Y", 0x59 },{ "Z", 0x5A },
		{ ".", VK_OEM_PERIOD },{ ",", VK_OEM_COMMA },{ ";", VK_OEM_1 },{ "/", VK_OEM_2 },
		{ "[", VK_OEM_4 },{ "]", VK_OEM_6 },{ "`", VK_OEM_3 },{ "|", VK_OEM_5 },{ "'", VK_OEM_7 },
		{ "Home", VK_HOME },{ "End", VK_END },{ "PageUp", VK_PRIOR },{ "PageDown", VK_NEXT },
		{ "Enter", VK_RETURN },{ "Space", VK_SPACE },{ "Backspace", VK_BACK },{ "Esc", VK_ESCAPE },{ "Caps", VK_CAPITAL },
		{ "Left", VK_LEFT },{ "Right", VK_RIGHT },{ "Up", VK_UP },{ "Down", VK_DOWN },
		{ "LShift", VK_LSHIFT },{ "RShift", VK_RSHIFT },
		{ "LCtrl", VK_LCONTROL },{ "RCtrl", VK_RCONTROL },
		{ "LAlt", VK_LMENU },{ "RAlt", VK_RMENU },
		{ "LClick", VK_LBUTTON },{ "RClick", VK_RBUTTON },{ "MClick", VK_MBUTTON },
		{ "X1Click", VK_XBUTTON1 },{ "X2Click", VK_XBUTTON2 },
		{ "Add", VK_ADD },{ "Subtract", VK_SUBTRACT },{ "Multiply", VK_MULTIPLY },
		{ "Tab", VK_TAB } ,{ "Insert", VK_INSERT },{ "Delete", VK_DELETE } ,{ "Pause", VK_PAUSE },
		{ "BrowserHome", VK_BROWSER_HOME }, 
		{ "F1", 0x70 },{ "F2", 0x71 },{ "F3", 0x72 },{ "F4", 0x73 },{ "F5", 0x74 },{ "F6", 0x75 },
		{ "F7", 0x76 },{ "F8", 0x77 },{ "F9", 0x78 },{ "F10", 0x79 },{ "F11", 0x7A },{ "F12", 0x7B },
		{ "Numpad0", 0x60 },{ "Numpad1", 0x61 },{ "Numpad2", 0x62 },{ "Numpad3", 0x63 },{ "Numpad4", 0x64 },
		{ "Numpad5", 0x65 },{ "Numpad6", 0x66 },{ "Numpad7", 0x67 },{ "Numpad8", 0x68 },{ "Numpad9", 0x69 },
	};

	void registerHotkey() {
		int count = 0;
		for (map<string, string>::iterator iterator = input.begin(); iterator != input.end(); iterator++) {
			key.push_back(iterator->first);
			size_t delimiter = key.back().find_first_of("+");
			if (delimiter == string::npos) {
				RegisterHotKey(0, count, NULL | MOD_NOREPEAT, inputKey.at(key.back().substr(delimiter + 1)));
			}
			else {
				RegisterHotKey(0, count, inputModifier.at(key.back().substr(0, delimiter)) | MOD_NOREPEAT, inputKey.at(key.back().substr(delimiter + 1)));
			}			
			count++;
		}
	}

	void unregisterHotkey() {
		for (int i = 0; i < (int) key.size(); i++) {
			UnregisterHotKey(0, i);
		}
		input.clear();
		key.clear();
	}
};
