
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

#include <ui/ui_PCExploreLayout.hpp>
#include <ui/ui_MainApplication.hpp>

extern set::Settings gsets;

namespace ui
{
    extern MainApplication::Ref mainapp;

    PCExploreLayout::PCExploreLayout() : pu::ui::Layout()
    {
        this->pathsMenu = pu::ui::elm::Menu::New(0, 160, 1280, gsets.CustomScheme.Base, gsets.MenuItemSize, (560 / gsets.MenuItemSize));
        this->pathsMenu->SetOnFocusColor(gsets.CustomScheme.BaseFocus);
        gsets.ApplyScrollBarColor(this->pathsMenu);
        this->Add(this->pathsMenu);
    }

    void PCExploreLayout::UpdatePaths()
    {
        this->names.clear();
        this->paths.clear();
        this->pathsMenu->ClearItems();
		
		std::vector<pu::String> dirs = usb::GetDirectories("");
		
		for(auto& path : dirs)
		{
			this->names.push_back(path);
            this->paths.push_back(path);
		}

        for(u32 i = 0; i < this->names.size(); i++)
        {
            auto itm = pu::ui::elm::MenuItem::New(this->names[i]);
            itm->SetColor(gsets.CustomScheme.Text);
            itm->SetIcon(gsets.PathForResource("/FileSystem/Directory.png"));
            itm->AddOnClick(std::bind(&PCExploreLayout::path_Click, this));
            this->pathsMenu->AddItem(itm);
        }
        auto fselitm = pu::ui::elm::MenuItem::New("Select file from PC");
        fselitm->SetColor(gsets.CustomScheme.Text);
        fselitm->SetIcon(gsets.PathForResource("/FileSystem/File.png"));
        fselitm->AddOnClick(std::bind(&PCExploreLayout::fileSelect_Click, this));
        this->pathsMenu->AddItem(fselitm);
        this->pathsMenu->SetSelectedIndex(0);
    }

    void PCExploreLayout::path_Click()
    {
        u32 idx = this->pathsMenu->GetSelectedIndex();
        mainapp->GetBrowserLayout()->ChangePartitionPCDrive(this->paths[idx]);
        mainapp->LoadLayout(mainapp->GetBrowserLayout());
    }

    void PCExploreLayout::fileSelect_Click()
    {
        //pu::String selfile;
        //auto rc = usb::ProcessCommand<usb::CommandId::SelectFile>(usb::OutString(selfile));
        //if(R_SUCCEEDED(rc)) mainapp->GetBrowserLayout()->HandleFileDirectly(selfile);
    }
}