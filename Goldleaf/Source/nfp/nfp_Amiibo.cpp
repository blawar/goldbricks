
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

#include <nfp/nfp_Amiibo.hpp>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

namespace nfp
{
    static HidControllerID dhandle;
    static Event available;
    static Event activate;
    static Event deactivate;
    static bool init;

    Result Initialize()
    {
        if(init) return 0;
        Result rc = nfpuInitialize(NULL);
        if(rc == 0)
        {
            rc = nfpuAttachAvailabilityChangeEvent(&available);
            if(rc == 0)
            {
                HidControllerID devs[9];
                rc = nfpuListDevices(NULL, devs, 9);
                if(rc == 0)
                {
                    dhandle = devs[0];
                    rc = nfpuAttachActivateEvent(dhandle, &activate);
                    if(rc == 0)
                    {
                        rc = nfpuAttachDeactivateEvent(dhandle, &deactivate);
                        if(rc == 0)
                        {
                            rc = nfpuStartDetection(dhandle);
                            init = true;
                        }
                    }
                }
            }
        }
        return rc;
    }

    bool IsReady()
    {
        if(!init) return false;
        NfpuDeviceState dst;
        nfpuGetDeviceState(dhandle, &dst);
        return (dst == NfpuDeviceState_TagFound);
    }

    Result Open()
    {
        return nfpuMount(dhandle, NfpuDeviceType_Amiibo, NfpuMountTarget_All);
    }

    NfpuTagInfo GetTagInfo()
    {
        NfpuTagInfo tinfo = {0};
        nfpuGetTagInfo(dhandle, &tinfo);
        return tinfo;
    }

    NfpuRegisterInfo GetRegisterInfo()
    {
        NfpuRegisterInfo rinfo = {0};
        nfpuGetRegisterInfo(dhandle, &rinfo);
        return rinfo;
    }

    NfpuCommonInfo GetCommonInfo()
    {
        NfpuCommonInfo cinfo = {0};
        nfpuGetCommonInfo(dhandle, &cinfo);
        return cinfo;
    }

    NfpuModelInfo GetModelInfo()
    {
        NfpuModelInfo minfo = {0};
        nfpuGetModelInfo(dhandle, &minfo);
        return minfo;
    }

    Result DumpToEmuiibo(NfpuTagInfo &tag, NfpuRegisterInfo &reg, NfpuCommonInfo &common, NfpuModelInfo &model)
    {
        auto outdir = "sdmc:/emuiibo/amiibo/" + pu::String(reg.amiibo_name);
        fsdevDeleteDirectoryRecursively(outdir.AsUTF8().c_str());

        mkdir("sdmc:/emuiibo", 777);
        mkdir("sdmc:/emuiibo/amiibo", 777);
        mkdir(outdir.AsUTF8().c_str(), 777);

        auto jtag = JSON::object();
        std::stringstream strm;
        for(u32 i = 0; i < 9; i++) strm << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << (int)tag.uuid[i];
        jtag["uuid"] = strm.str();
        std::ofstream ofs((outdir + "/tag.json").AsUTF8());
        ofs << std::setw(4) << jtag;
        ofs.close();

        auto jmodel = JSON::object();
        strm.str("");
        strm.clear();
        for(u32 i = 0; i < 8; i++) strm << std::hex << std::setw(2) << std::uppercase << std::setfill('0') << (int)model.amiibo_id[i];
        jmodel["amiiboId"] = strm.str();
        ofs = std::ofstream((outdir + "/model.json").AsUTF8());
        ofs << std::setw(4) << jmodel;
        ofs.close();

        FILE *f = fopen((outdir + "/mii-charinfo.bin").AsUTF8().c_str(), "wb");
        fwrite(&reg.mii, 1, sizeof(NfpuMiiCharInfo), f);
        fclose(f);

        auto jreg = JSON::object();
        jreg["name"] = std::string(reg.amiibo_name);
        jreg["miiCharInfo"] = "mii-charinfo.bin";
        strm.str("");
        strm.clear();
        strm << std::dec << reg.first_write_year << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)reg.first_write_month << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)reg.first_write_day;
        jreg["firstWriteDate"] = strm.str();
        ofs = std::ofstream((outdir + "/register.json").AsUTF8());
        ofs << std::setw(4) << jreg;
        ofs.close();

        auto jcommon = JSON::object();
        strm.str("");
        strm.clear();
        strm << std::dec << common.last_write_year << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)common.last_write_month << "-";
        strm << std::dec << std::setw(2) << std::setfill('0') << (int)common.last_write_day;
        jcommon["lastWriteDate"] = strm.str();
        jcommon["writeCounter"] = (int)common.write_counter;
        jcommon["version"] = (int)common.version;
        ofs = std::ofstream((outdir + "/common.json").AsUTF8());
        ofs << std::setw(4) << jcommon;
        ofs.close();

        return 0;
    }

    void Close()
    {
        nfpuUnmount(dhandle);
    }

    void Finalize()
    {
        if(!init) return;
        nfpuStopDetection(dhandle);
        nfpuExit();
        init = false;
    }
}