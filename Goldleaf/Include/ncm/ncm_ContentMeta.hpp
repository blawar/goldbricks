
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
#include <cstring>
#include <Types.hpp>
#include <ByteBuffer.hpp>
#include <ncm/ncm_Types.hpp>

namespace ncm
{
    class ContentMeta
    {
        public:
            ContentMeta();
            ContentMeta(u8 *Data, size_t Size);
            ~ContentMeta();
            ContentMetaHeader GetContentMetaHeader();
            NcmMetaRecord GetContentMetaKey();
            std::vector<ContentRecord> GetContentRecords();
            void GetInstallContentMeta(ByteBuffer &CNMTBuffer, ContentRecord &CNMTRecord, bool IgnoreVersion);
        private:
            ByteBuffer buf;
    };
}