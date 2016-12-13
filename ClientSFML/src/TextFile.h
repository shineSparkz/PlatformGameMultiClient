#ifndef __TEXT_FILE_H__
#define __TEXT_FILE_H__

#include "types.h"

class TextFile
{
public:
	TextFile();
	~TextFile();

	bool LoadFileAsLinesToBuffer(const std::string& filename);
	static std::string LoadFileIntoStr(const std::string& filename);

	std::string GetDataBuffer(dword index) const;
	int32 GetDataAsInt(dword idx) const;

	const std::vector<std::string>& GetBuffer() const { return buffer; }

private:
	std::vector<std::string> buffer;
};

struct BinaryLevelData
{
	int tilesize;
	int mapwidth;
	int mapheight;
	std::vector<int> object_ids;
};

#endif

