#include "KeyEvent.h"

#include "LogFile.h"
#include <fstream>
#include <SFML\Graphics.hpp>

std::vector<KeyBindings::KeyState> KeyBindings::KeyBindingList;
std::vector<KeyBindings::KeyState> KeyBindings::DefaultKeyBindingList;

std::map<int, std::string> KeyBindings::KeyToStringHash;
std::vector<int> KeyBindings::ReservedKeys;
const int KeyBindings::ReservedPauseKey = sf::Keyboard::End;// GLFW_KEY_END;
const int KeyBindings::ReservedMenuKey = sf::Keyboard::Escape;

void KeyBindings::SetKeysBackToDefaults()
{
	size_t j = 0;
	for (std::vector<KeyBindings::KeyState>::const_iterator i = DefaultKeyBindingList.begin(); i != DefaultKeyBindingList.end(); ++i)
	{
		KeyBindingList[j].key = (*i).key;
		++j;
	}
}

void KeyBindings::LoadDefaults()
{
	if (KeyBindingList.empty())
	{
		KeyBindingList.resize(KeyBindings::KeyBinds::NumBindedKeys);

		KeyBindingList[KeyBinds::LeftBind] = KeyState{ sf::Keyboard::A, false };
		KeyBindingList[KeyBinds::RightBind] = KeyState{ sf::Keyboard::D };
		KeyBindingList[KeyBinds::DownBind] = KeyState{ sf::Keyboard::S };
		KeyBindingList[KeyBinds::JumpBind] = KeyState{ sf::Keyboard::W };
		KeyBindingList[KeyBinds::WeaponBind] = KeyState{ sf::Keyboard::Space };
		KeyBindingList[KeyBinds::ToggleItemBind] = KeyState{ sf::Keyboard::Q };
		KeyBindingList[KeyBinds::UseItemBind] = KeyState{ sf::Keyboard::E };
		KeyBindingList[KeyBindings::InteractItemBind] = KeyState{ sf::Keyboard::F };
		KeyBindingList[KeyBindings::InventoryItemBind] = KeyState{ sf::Keyboard::Tab };

		DefaultKeyBindingList = KeyBindingList;
	}

	std::ifstream file_in("res/data/key.dat", std::ios::binary);
	if (!file_in)
	{
		WRITE_LOG("Tried to read key data but no dat file", "warning");
		//return;
	}
	else
	{
		file_in.seekg(0);
		file_in.read(reinterpret_cast<char*>(&KeyBindingList[0]), KeyBindingList.size() * sizeof(KeyState));
	}

	if (ReservedKeys.empty())
	{
		ReservedKeys.push_back(ReservedMenuKey);
		ReservedKeys.push_back(ReservedPauseKey);
	}

	if (KeyToStringHash.empty())
	{
		// Setup Hash... Groan
		KeyToStringHash[sf::Keyboard::Unknown] = "unknown";
		KeyToStringHash[sf::Keyboard::Space] = "space";
		KeyToStringHash[sf::Keyboard::Comma] = "comma";
		//KeyToStringHash[sf::Keyboard::M] = "minus";
		KeyToStringHash[sf::Keyboard::Period] = "period";
		KeyToStringHash[sf::Keyboard::Slash] = "slash";
		KeyToStringHash[sf::Keyboard::Num0] = "0";
		KeyToStringHash[sf::Keyboard::Num1] = "1";
		KeyToStringHash[sf::Keyboard::Num2] = "2";
		KeyToStringHash[sf::Keyboard::Num3] = "3";
		KeyToStringHash[sf::Keyboard::Num4] = "4";
		KeyToStringHash[sf::Keyboard::Num5] = "5";
		KeyToStringHash[sf::Keyboard::Num6] = "6";
		KeyToStringHash[sf::Keyboard::Num7] = "7";
		KeyToStringHash[sf::Keyboard::Num8] = "8";
		KeyToStringHash[sf::Keyboard::Num9] = "9";
		KeyToStringHash[sf::Keyboard::SemiColon] = "semicolon";
		KeyToStringHash[sf::Keyboard::Equal] = "equals";
		KeyToStringHash[sf::Keyboard::A] = "A";
		KeyToStringHash[sf::Keyboard::B] = "B";
		KeyToStringHash[sf::Keyboard::C] = "C";
		KeyToStringHash[sf::Keyboard::D] = "D";
		KeyToStringHash[sf::Keyboard::E] = "E";
		KeyToStringHash[sf::Keyboard::F] = "F";
		KeyToStringHash[sf::Keyboard::G] = "G";
		KeyToStringHash[sf::Keyboard::H] = "H";
		KeyToStringHash[sf::Keyboard::I] = "I";
		KeyToStringHash[sf::Keyboard::J] = "J";
		KeyToStringHash[sf::Keyboard::K] = "K";
		KeyToStringHash[sf::Keyboard::L] = "L";
		KeyToStringHash[sf::Keyboard::M] = "M";
		KeyToStringHash[sf::Keyboard::N] = "N";
		KeyToStringHash[sf::Keyboard::O] = "O";
		KeyToStringHash[sf::Keyboard::P] = "P";
		KeyToStringHash[sf::Keyboard::Q] = "Q";
		KeyToStringHash[sf::Keyboard::R] = "R";
		KeyToStringHash[sf::Keyboard::S] = "S";
		KeyToStringHash[sf::Keyboard::T] = "T";
		KeyToStringHash[sf::Keyboard::U] = "U";
		KeyToStringHash[sf::Keyboard::V] = "V";
		KeyToStringHash[sf::Keyboard::W] = "W";
		KeyToStringHash[sf::Keyboard::X] = "X";
		KeyToStringHash[sf::Keyboard::Y] = "Y";
		KeyToStringHash[sf::Keyboard::Z] = "Z";
		KeyToStringHash[sf::Keyboard::LBracket] = "left bracket";
		KeyToStringHash[sf::Keyboard::BackSlash] = "backslash";
		KeyToStringHash[sf::Keyboard::RBracket] = "right bracket";
		//KeyToStringHash[sf::Keyboard::Gra] = "grave accent";
		//KeyToStringHash[sf::Keyboard::] = "world 1";
		//KeyToStringHash[GLFW_KEY_WORLD_2] = "world 2";
		KeyToStringHash[sf::Keyboard::Return] = "enter";
		KeyToStringHash[sf::Keyboard::Tab] = "tab";
		KeyToStringHash[sf::Keyboard::BackSpace] = "backspace";
		KeyToStringHash[sf::Keyboard::Insert] = "insert";
		KeyToStringHash[sf::Keyboard::Delete] = "delete";
		KeyToStringHash[sf::Keyboard::Right] = "right arrow";
		KeyToStringHash[sf::Keyboard::Left] = "left arrow";
		KeyToStringHash[sf::Keyboard::Down] = "down arrow";
		KeyToStringHash[sf::Keyboard::Up] = "up arrow";
		KeyToStringHash[sf::Keyboard::PageUp] = "page up";
		KeyToStringHash[sf::Keyboard::PageDown] = "page down";
		KeyToStringHash[sf::Keyboard::Home] = "home";
		KeyToStringHash[sf::Keyboard::End] = "end";
		//KeyToStringHash[sf::Keyboard::Cap] = "caps";
		//KeyToStringHash[sf::Keyboard::Scro] = "scroll lock";
		//KeyToStringHash[sf::Keyboard::NumLoc] = "num lock";
		//KeyToStringHash[sf::Keyboard::Print] = "prt scn";
		KeyToStringHash[sf::Keyboard::Pause] = "pause";
		KeyToStringHash[sf::Keyboard::F1] = "F1";
		KeyToStringHash[sf::Keyboard::F2] = "F2";
		KeyToStringHash[sf::Keyboard::F3] = "F3";
		KeyToStringHash[sf::Keyboard::F4] = "F4";
		KeyToStringHash[sf::Keyboard::F5] = "F5";
		KeyToStringHash[sf::Keyboard::F6] = "F6";
		KeyToStringHash[sf::Keyboard::F7] = "F7";
		KeyToStringHash[sf::Keyboard::F8] = "F8";
		KeyToStringHash[sf::Keyboard::F9] = "F9";
		KeyToStringHash[sf::Keyboard::F10] = "F10";
		KeyToStringHash[sf::Keyboard::F11] = "F11";
		KeyToStringHash[sf::Keyboard::F12] = "F12";
		KeyToStringHash[sf::Keyboard::F13] = "F13";
		KeyToStringHash[sf::Keyboard::F14] = "F14";
		KeyToStringHash[sf::Keyboard::F15] = "F15";
		//KeyToStringHash[sf::Keyboard::F16] = "F16";
		//KeyToStringHash[sf::Keyboard::F17] = "F17";
		//KeyToStringHash[sf::Keyboard::F18] = "F18";
		//KeyToStringHash[sf::Keyboard::F19] = "F19";
		//KeyToStringHash[sf::Keyboard::F20] = "F20";
		//KeyToStringHash[GLFW_KEY_F21] = "F21";
		//KeyToStringHash[GLFW_KEY_F22] = "F22";
		//KeyToStringHash[GLFW_KEY_F23] = "F23";
		//KeyToStringHash[GLFW_KEY_F24] = "F24";
		//KeyToStringHash[GLFW_KEY_F25] = "F25";
		//KeyToStringHash[sf::Keyboard:] = "KP 0";
		//KeyToStringHash[GLFW_KEY_KP_1] = "KP 1";
		//KeyToStringHash[GLFW_KEY_KP_2] = "KP 2";
		//KeyToStringHash[GLFW_KEY_KP_3] = "KP 3";
		//KeyToStringHash[GLFW_KEY_KP_4] = "KP 4";
		//KeyToStringHash[GLFW_KEY_KP_5] = "KP 5";
		//KeyToStringHash[GLFW_KEY_KP_6] = "KP 6";
		//KeyToStringHash[GLFW_KEY_KP_7] = "KP 7";
		//KeyToStringHash[GLFW_KEY_KP_8] = "KP 8";
		//KeyToStringHash[GLFW_KEY_KP_9] = "KP 9";
		//KeyToStringHash[GLFW_KEY_KP_DECIMAL] = "key pad decimal";
		//KeyToStringHash[GLFW_KEY_KP_DIVIDE] = "key pad divide";
		//KeyToStringHash[GLFW_KEY_KP_MULTIPLY] = "key pad multiply";
		//KeyToStringHash[GLFW_KEY_KP_SUBTRACT] = "key pad subtract";
		//KeyToStringHash[GLFW_KEY_KP_ADD] = "key pad add";
		//KeyToStringHash[GLFW_KEY_KP_ENTER] = "key pad enter";
		//KeyToStringHash[GLFW_KEY_KP_EQUAL] = "key pad equals";
		KeyToStringHash[sf::Keyboard::LShift] = "left shift";
		KeyToStringHash[sf::Keyboard::LControl] = "left ctrl";
		KeyToStringHash[sf::Keyboard::LAlt] = "left alt";
		KeyToStringHash[sf::Keyboard::LSystem] = "left system";
		KeyToStringHash[sf::Keyboard::RShift] = "right shift";
		KeyToStringHash[sf::Keyboard::RControl] = "right ctrl";
		KeyToStringHash[sf::Keyboard::RAlt] = "right alt";
		KeyToStringHash[sf::Keyboard::RSystem] = "right system";
		KeyToStringHash[sf::Keyboard::Menu] = "menu";
	}
}

void KeyBindings::SaveBindingsData()
{
	std::ofstream binout("res/data/key.dat", std::ios::out | std::ios::binary);
	binout.write(reinterpret_cast<char*>(&KeyBindingList[0]), KeyBindingList.size() * sizeof(KeyState));
	binout.close();
}

std::string KeyBindings::GetStringFromKey(int key)
{
	std::map<int, std::string>::iterator mi = KeyToStringHash.find(key);
	return mi == KeyToStringHash.end() ? "unknown" : mi->second;
}

bool KeyBindings::IsKeyReserved(int key)
{
	for (auto i = ReservedKeys.begin(); i != ReservedKeys.end(); ++i)
	{
		if ((*i) == key)
			return true;
	}

	return false;
}

