
/*

    Goldleaf - Multipurpose homebrew tool for Nintendo Switch
    Copyright (C) 2018-2019  XorTroll

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <usb/usb_Commands.hpp>
#include <usb/usb_Communications.hpp>
#include <sstream>
#include <iomanip>
#include "rapidjson/document.h"
#include <malloc.h>

namespace usb
{
	class Packet
	{
	public:
		class Header
		{
		public:
			static const u32 MAGIC = 0x12121212;
			u32 magic = 0;
			u32 command = 0;
			u64 size = 0;
			u32 threadId = 0;
			u16 packetIndex = 0;
			u16 packetCount = 0;
			u64 timestamp = 0;

			bool isValid()
			{
				return magic == MAGIC;
			}

			void clear()
			{
				command = 0;
				size = 0;
				threadId = 0;
				packetIndex = 0;
				packetCount = 0;
				timestamp = 0;
				magic = 0;
			}
		} PACKED;
	};
	
	void send(const char* data, u32 channel)
	{
		if(data)
		{
			send((const u8*)data, strlen(data), channel);
		}
	}
	
	void send(const u8* data, u64 sz, u32 channel)
	{
		Packet::Header header;
		header.magic = Packet::Header::MAGIC;
		header.size = sz;
		header.command = 1;
		header.threadId = channel;
		
		Write(&header, sizeof(header));
	

		if (sz)
		{
			Write(data, sz);
		}
	}
	
	void recv(std::vector<u8>& payload, u32 channel, u64 timeout)
	{
		Packet::Header* header = (Packet::Header*)memalign(0x1000, sizeof(Packet::Header));
		
		Read(header, sizeof(Packet::Header));		

		if (header->size > 32000000)
		{
			free((void*)header);
			return;
		}

		payload.resize(header->size);
		
		void* buffer = memalign(0x1000, header->size);

		Read(buffer, header->size);
		memcpy((void*)payload.data(), buffer, header->size);

		free(buffer);
		free((void*)header);
	}
	
	enum EntryType : u8
	{
		Entry_None = 0,
		Entry_Files = 1,
		Entry_Dirs = 2
	};
	
	std::string urlEncode(std::string value)
	{
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
		{
			std::string::value_type c = (*i);


			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '/')
			{
				escaped << c;
				continue;
			}

			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char) c);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}
	
	std::string resolvePath(std::string Path)
	{
		Path.erase(std::remove(Path.begin(), Path.end(), ':'), Path.end());
		return (Path);
	}
	
	std::vector<pu::String> GetEntires(const std::string& Path, EntryType type)
    {
		std::string pathRequest = "/api/directoryList/";
		pathRequest += resolvePath(Path);
        std::vector<pu::String> results;
		std::vector<u8> buffer;
		send(pathRequest.c_str());
		recv(buffer);
		buffer.push_back(0);
		
		rapidjson::Document entries;
		entries.Parse((const char*)buffer.data());
		
		if (entries.IsObject())
		{
			if ((type == Entry_Files) && entries.HasMember("files") && entries["files"].IsArray())
			{
				auto& data = entries["files"];

				for (auto itr = data.Begin(); itr != data.End(); ++itr)
				{
					auto& e = *itr;

					if (!e.HasMember("name") || !e["name"].IsString())
					{
						continue;
					}

					results.push_back(pu::String(e["name"].GetString()));
				}
			}

			if ((type == Entry_Dirs) && entries.HasMember("dirs") && entries["dirs"].IsArray())
			{
				auto& data = entries["dirs"];

				for (auto itr = data.Begin(); itr != data.End(); ++itr)
				{
					auto& e = *itr;

					if (!e.HasMember("name") || !e["name"].IsString())
					{
						continue;
					}
					results.push_back(pu::String(e["name"].GetString()));
				}
			}
		}

        return results;
    }
	
	std::vector<pu::String> GetDirectories(pu::String Path)
	{
		return GetEntires(Path.AsUTF8(), Entry_Dirs);
	}

    std::vector<pu::String> GetFiles(pu::String Path)
    {
        return GetEntires(Path.AsUTF8(), Entry_Files);
    }
}