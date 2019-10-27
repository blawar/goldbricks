
/*

    GoldBricks - Multipurpose homebrew tool for Nintendo Switch
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

#pragma once
#include <usb/usb_Communications.hpp>

namespace usb
{
	void send(const u8* data, u64 sz, u32 channel = 0);
	void send(const char* data, u32 channel = 0);
	void recv(std::vector<u8>& payload, u32 channel = 0, u64 timeout = U64_MAX);
	
	std::string urlEncode(std::string value);
	std::string resolvePath(std::string Path);
	
	std::vector<pu::String> GetDirectories(pu::String Path);
    std::vector<pu::String> GetFiles(pu::String Path);
}
