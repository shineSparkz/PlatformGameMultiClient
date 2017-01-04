#ifndef __PLAYER_CREDENTIALS_BIN_H__
#define __PLAYER_CREDENTIALS_BIN_H__

#define MAX_NAME_CHARS 10

struct PlayerCredentials
{
	unsigned short sizeName;
	unsigned short sizePassword;
	char name[MAX_NAME_CHARS];
	char password[MAX_NAME_CHARS];
};

struct PlayerCredentialsFile
{
	static void SaveFile(int sizename, const char* name, int sizePw, const char* pw);

	~PlayerCredentialsFile();
	PlayerCredentials* LoadFile();

private:
	PlayerCredentials* m_Creds = nullptr;
};


#endif