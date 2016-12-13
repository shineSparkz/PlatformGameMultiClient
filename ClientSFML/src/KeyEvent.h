#ifndef __KEY_EVENT_H__
#define __KEY_EVENT_H__

#include "types.h"
#include <vector>
#include <string>
#include <map>

#define PRESS 1
#define RELEASE 0

struct KeyData
{
	dword key;
	dword action;

	KeyData(dword key_, dword action_) :
		key(key_), action(action_) {}
};

class KeyBindings
{
public:

	static const int ReservedPauseKey;
	static const int ReservedMenuKey;

	enum KeyBinds
	{
		LeftBind,
		RightBind,
		DownBind,
		JumpBind,
		WeaponBind,
		ToggleItemBind,
		UseItemBind,
		InteractItemBind,
		InventoryItemBind,
		NumBindedKeys
	};

	struct KeyState
	{
		int key;
		int pressed;
	};

	static std::vector<KeyState> KeyBindingList;

	static void LoadDefaults();
	static void SaveBindingsData();
	static void SetKeysBackToDefaults();

	// NOTE* Should cache this value if need every frame
	static std::string GetStringFromKey(int);
	static bool IsKeyReserved(int key);

private:
	static std::vector<KeyState> DefaultKeyBindingList;
	static std::map<int, std::string> KeyToStringHash;
	static std::vector<int> ReservedKeys;
};

#endif