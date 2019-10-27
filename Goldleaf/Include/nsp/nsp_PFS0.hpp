
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
#include <fs/fs_Explorer.hpp>
#include <nsp/nsp_Types.hpp>

namespace nsp
{
    class PFS0
    {
        public:
            PFS0(fs::Explorer *Exp, pu::String Path);
            ~PFS0();
            u32 GetCount();
            pu::String GetFile(u32 Index);
            u64 ReadFromFile(u32 Index, u64 Offset, u64 Size, u8 *Out);
            std::vector<pu::String> GetFiles();
            bool IsOk();
            fs::Explorer *GetExplorer();
            u64 GetFileSize(u32 Index);
            void SaveFile(u32 Index, fs::Explorer *Exp, pu::String Path);
            u32 GetFileIndexByName(pu::String File);
			
			static const u32 INVALID_FILE = 0xFFFFFFFF;
        private:
            pu::String path;
            fs::Explorer *gexp;
            u8 *stringtable;
            u32 headersize;
            PFS0Header header;
            std::vector<PFS0File> files;
            bool ok;
    };
}
