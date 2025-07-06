#ifndef RESOURCEPOOL_H
#define RESOURCEPOOL_H
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

struct TextureDataEntry
{
	unsigned char* RawData;
	int Width, Height, Channels;
	unsigned int TextureID;

	TextureDataEntry(unsigned char* rawData, int width, int height, int channels, unsigned int textureID)
	{
		RawData = rawData;
		Width = width;
		Height = height;
		Channels = channels;
		TextureID = textureID;
	}

	TextureDataEntry()
	{

	}
};

extern std::unordered_map<std::string, TextureDataEntry> TexturePool;
#endif
