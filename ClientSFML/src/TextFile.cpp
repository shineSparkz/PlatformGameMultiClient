#include "TextFile.h"

#include<fstream>
#include <sstream>

#include "LogFile.h"
#include "types.h"

TextFile::TextFile() :
	buffer()
{
}

TextFile::~TextFile()
{
	if ( !buffer.empty() )
		buffer.clear();
}

bool TextFile::LoadFileAsLinesToBuffer(const std::string& filename)
{
	buffer.clear();
	std::ifstream fileIn(filename);

	if ( fileIn.fail() )
	{
		WRITE_LOG("Error:: TextFile Reader failed to open the file.", "error");
		return false;
	}

	std::string word;
	while ( std::getline(fileIn, word) )
	{
		buffer.push_back(word);
	}

	fileIn.close();
	return true;
}

std::string TextFile::LoadFileIntoStr(const std::string& filename)
{
	std::ifstream fp;
	fp.open(filename, std::ifstream::in);
	if ( fp.is_open() == false )
	{
		return "";
	}

	std::stringstream ss;
	ss << fp.rdbuf();
	return ss.str();
}

std::string TextFile::GetDataBuffer(dword index) const
{
	assert(index < buffer.size());
	return buffer[index];
}

int32 TextFile::GetDataAsInt(dword idx) const
{
	assert(idx < buffer.size());
	return atoi(buffer[idx].c_str());
	/*
	const char* str{ buffer[idx].c_str() };
	size_t size{ 0 };

	while( str[size] != '\0' )
	{
	++size;
	}

	int32 return_value{ 0 };

	for ( size_t i = 0; i < size; ++i )
	{
	if( str[i] < '0' || str[i] > '9' )
	continue;

	return_value = (return_value * 10) + str[i] - '0';
	}

	return return_value;
	*/
}