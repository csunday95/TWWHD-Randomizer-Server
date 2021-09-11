#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include "../utility/byteswap.hpp"

enum struct MSBTError
{
	NONE = 0,
	COULD_NOT_OPEN,
	NOT_MSBT,
	UNKNOWN_VERSION,
	UNEXPECTED_VALUE,
	UNKNOWN_SECTION,
	NOT_LBL1,
	NOT_ATR1,
	NOT_TSY1,
	NOT_TXT2,
	REACHED_EOF,
	UNKNOWN
};

struct MSBTHeader {
	char magicMsgStdBn[8];
	uint16_t byteOrderMarker;
	uint16_t unknown_0x00;
	uint8_t encoding_0x01;
	uint8_t version_0x03;
	uint16_t sectionCount;
	uint16_t unknown2_0x00;
	uint32_t fileSize;
	uint8_t padding_0x00[10];
};

struct Label {
	uint32_t checksum;
	uint8_t length;
	std::string string;
	uint32_t messageIndex;
};

struct LBLEntry {
	uint32_t stringCount;
	uint32_t stringOffset;
	std::vector<Label> labels;
};

struct LBL1Header {
	int offset;
	char magicLBL1[4];
	uint32_t tableSize;
	uint8_t padding_0x00[8];
	uint32_t entryCount;
	std::vector<LBLEntry> entries;
};

struct Attributes {
	uint8_t character; //The NPC (or similar entity) that it's attached to, internally "CharacterName"
	uint8_t boxStyle; //internally "BalloonType"
	uint8_t drawType; //internally "DisplayStyle"
	uint8_t screenPos; //internally "BalloonPlacement"
	uint16_t price; //internally "Price"
	uint16_t nextNo; //internally "NextNo", unknown purpose
	uint8_t item; //Matches with SD, is ignored, internally "Item"
	uint8_t lineAlignment; //internally "LineAlignment"
	uint8_t soundEffect; //internally "SE"
	uint8_t camera; //internally "Camera"
	uint16_t demoID; //internally "DemoID", extra ID used for cutscenes 
	uint8_t animation; //internally "Animation"
	uint32_t commentE_1; //internally "Comment(E-1)", doesnt seem to align as an offset?
	uint32_t commentE_2; //internally "Comment(E-2)", doesnt seem to align as an offset?
	uint8_t checkOriginal; //internally "CheckOriginal", unknown purpose, seemingly cut off by attribute entry length
	uint8_t checkRev; //internally "MCT Check Rev", unknown purpose, seemingly cut off by attribute entry length
	uint8_t mctTester; //internally "MCT Tester", unknown purpose, seemingly cut off by attribute entry length
};

struct ATR1Header {
	int offset;
	char magicATR1[4];
	uint32_t tableSize;
	uint8_t padding_0x00[8];
	uint32_t entryCount;
	uint32_t entrySize;
	std::vector<Attributes> entries;
};

struct TSY1Entry {
	uint32_t styleIndex; //Index in the MSBP style list
};

struct TSY1Header {
	int offset;
	char magicTSY1[4];
	uint32_t tableSize;
	uint8_t padding_0x00[8];
	std::vector<TSY1Entry> entries;
};

struct TXT2Entry {
	uint32_t offset;
	uint32_t nextOffset;
	std::string message;
};

struct TXT2Header {
	int offset;
	char magicTXT2[4];
	uint32_t tableSize;
	uint8_t padding_0x00[8];
	uint32_t entryCount;
	std::vector <TXT2Entry> entries;
};

struct Message {
	Label label;
	Attributes attributes;
	TSY1Entry TSYEntry;
	TXT2Entry text;
};
namespace FileTypes {

	const char* MSBTErrorGetName(MSBTError err);

	class MSBTFile {
	public:
		MSBTHeader header;
		LBL1Header LBL1;
		ATR1Header ATR1;
		TSY1Header TSY1;
		TXT2Header TXT2;
		std::unordered_map<std::string, Message> messages_by_label;

		MSBTFile();
		static MSBTFile createNew(const std::string& filename);
		MSBTError readSection(std::istream& msbt);
		MSBTError loadFromBinary(std::istream& msbt);
		MSBTError loadFromFile(const std::string& filePath);
		Message& addMessage(std::string label, Attributes attributes, TSY1Entry TSY, std::string message);
		MSBTError writeToStream(std::ostream& out);
		MSBTError writeToFile(const std::string& outFilePath);
	private:
		void initNew();
	};
}