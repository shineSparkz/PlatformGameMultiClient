#include "PlayerCredentialsBin.h"
#include "LogFile.h"
#include "types.h"
#include <fstream>

void PlayerCredentialsFile::SaveFile(int sizename, const char* name, int sizePw, const char* pw)
{
	if (sizename >= MAX_NAME_CHARS || sizePw >= MAX_NAME_CHARS)
	{
		return;
	}

	PlayerCredentials pc;
	pc.sizeName = sizename;
	pc.sizePassword = sizePw;

	memset(&pc.name, 0, MAX_NAME_CHARS);
	memset(&pc.password, 0, MAX_NAME_CHARS);
	strcpy_s(pc.name, name);
	strcpy_s(pc.password, pw);
	
	std::ofstream binout("resources/data/pc.dat", std::ios::out | std::ios::binary);
	binout.write(reinterpret_cast<char*>(&pc), sizeof(pc));
}

PlayerCredentialsFile::~PlayerCredentialsFile()
{
	SAFE_DELETE(m_Creds);
}

PlayerCredentials* PlayerCredentialsFile::LoadFile()
{
	FILE* file;
	byte sizename[2];
	byte sizepw[2];

	//file = fopen("resources/data/pc.dat", "rb");
	fopen_s(&file, "resources/data/pc.dat", "rb");
	
	if (!file)
	{
		WRITE_LOG("Tried to read credetials data but no dat file", "warning");
		return nullptr;
	}

	fread(&sizename, sizeof(byte), 2, file);
	fread(&sizepw, sizeof(byte), 2, file);

	int sizeofname = sizename[0] + sizename[1] * 256;
	int sizeofpw = sizepw[0] + sizepw[1] * 256;

	// Isn't actually needed to create this anymore, as it auto cleans now
	m_Creds = new PlayerCredentials();
	m_Creds->sizeName = sizeofname;
	m_Creds->sizePassword = sizeofpw;
	memset(m_Creds->name, 0, MAX_NAME_CHARS);
	memset(&m_Creds->password, 0, MAX_NAME_CHARS);
	fread(&m_Creds->name, sizeof(char), MAX_NAME_CHARS, file);
	fread(&m_Creds->password, sizeof(char), MAX_NAME_CHARS, file);

	fclose(file);
	return m_Creds;
}
