// License:
// 
// Rosetta
// Scenario Classes
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2025
// 
// 		This program is free software: you can redistribute it and/or modify
// 		it under the terms of the GNU General Public License as published by
// 		the Free Software Foundation, either version 3 of the License, or
// 		(at your option) any later version.
// 
// 		This program is distributed in the hope that it will be useful,
// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// 		GNU General Public License for more details.
// 		You should have received a copy of the GNU General Public License
// 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// 	Please note that some references to data like pictures or audio, do not automatically
// 	fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 25.10.06 VIII
// End License

#include <algorithm>

#include "../Rosetta.hpp"

#include <SlyvTime.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvString.hpp>
#include <SlyvSTOI.hpp>
#include <SlyvDir.hpp>
#include <SlyvDirry.hpp>

#include <TQSE.hpp>


using namespace Slyvina::Units;

namespace Slyvina {
	namespace Rosetta {
		namespace Class {
			//{ Basic

			 CSenario _CEntry::Project() { return Parent->Parent; }

			 //}

			 //{ Entry
			 bool _CEntry::TagExists(string Tag) {
			 	Trans2Upper(Tag);
				//Tag = Tag.ToUpper();
				return LTags.count(Tag);
			}


			void _CEntry::SaveMe() {
				QCol->Doing("Saving",EntryFile);
				QuickStream.SaveString(EntryFile,_Data->ToSource());
				Modified = false;
			}

			String _CEntry::EntryFile(){ return Dirry(Project()->Settings->Value("DIRECTORIES","SCENARIO")+"/"+EntryName+".ini"); }

			GINIE _CEntry::Data() {
				if (_Data == nullptr) {
					QCol->Doing("Loading",EntryFile());
					_Data = LoadOptGINIE(EntryFile());
					_Data->NewValue("::SYS::", "Created", Now());
					_Data->NewValue("::SYS::", "Tool", "Rosetta");
					_Data->NewValue("::SYS::", "Entry", EntryName);
				}
				return _Data;
			}

			String _CEntry::CurrentTagName() {
				Crash("CurrentTagName() not yet implemented");
				return "ERROR";
				//		if (MainWindow.ScenarioTags.SelectedItem == null) return "";
				//		return MainWindow.ScenarioTags.SelectedItem.ToString();
			}

			CTag _CEntry::CurrentTag() {
				auto CTN = CurrentTagName();
				if (CTN == "") return nullptr;
				Trans2Upper(CTN);
				return GT(CTN);

			}

			CTag _CEntry::Adtag(String Tag) {
				Units::Trans2Upper(Tag); //Tag = Tag.ToUpper();
				auto ret {std::shared_ptr<_CTag>(new _CTag(this, Tag))};
				Tags[Tag] = ret;
				return ret;
			}

			_CEntry::_CEntry(_CScenario* _Parent, String EN){
				this->Parent = _Parent;
				EntryName = EN;
				Parent->Entries[EN] = std::shared_ptr<_CEntry>(this);

			}
			//}

			//{ Tag
			_CTag::_CTag (_CEntry* _Parent,string _Tag) {
				Parent = _Parent;
				Tag = _Tag;
				Parent->Tags[Tag] = std::shared_ptr<_CTag>(this);
				auto PC {std::max(1,ToInt(Data()->Value("::PAGES::", Tag))};
				for (int i = 0; i < PC; i++) Page.push_back(Parent->Tags[Tag]); //Page.Add(new CPage(this));
			}

			void _CTag::CurrentPageNumber(int value) {
					auto s {std::max(0, value)};
					if (s >= Page.size()) {
						if (Page.size() == 0 || TQSE::Yes("Add a new page?")) {
							Page->push_back(std::shared_ptr<_CPage>(new _CPage(this)));
							Data->Value("::PAGES::", Tag,(int)Page.size());
						}
						s = Page.size() - 1;
					}
					__currentpagenumber = s;
			}

			_ProjectData* _CTag::Project() { return Parent->Parent->Parent; }

			CPage _CTag::CurrentPage() {
				if (Page.size() == 0) {
					__currentpagenumber = 0;
					//Page.Add(new CPage(this));
					Page.push_back(std::shared_ptr<_CPage>(new _CPage(this)));
				}
				return Page[__currentpagenumber];
			}
			//}

			//{ Page

			void CPMC::operator=(bool value) {Ouwe->Parent->Parent=value;}
			void _CPage::LinkUpdate(ELU FromELU,j19gadget* From=nullptr) {
				//var old = MainWindow.scenario_allowmodify;
				//MainWindow.scenario_allowmodify = false;
				//if (FromELU != ELU::PicDir) MainWindow.ScenarioPicDir = PicDir;
				//if (FromELU != ELU::Lang1 && ChosenLang1 != null) MainWindow.LangHead[1] = ChosenLang1.Header;
				//if (FromELU != ELU::Lang2 && ChosenLang2 != null) MainWindow.LangHead[2] = ChosenLang2.Header;
				//MainWindow.scenario_allowmodify = old;
				Crash("LinkUpdate not yet implemented");
			}

			CSLang* _CPage::GLang() {
				if (lkey == "") return nullptr;
				if (!_Lang.count(lkey)) {
					CSLang[lkey].Parent=this
					CSLang[lkey].Lang=lkey;
				}
				return &_Lang[lkey];
			}

			String _Page::ChosenLangName1() { return Parent->Parent->Parent->Parent->Settings->Value("::SCENARIO::", "LANG1"); }
			String _Page::ChosenLangName2() { return  Parent->Parent->Parent->Parent->Settings->Value("::SCENARIO::
			//}

			//{ Scenario
			void _CScenario::SaveMe(bool force) {
				for(CEntry entry : Entries) { if (force || entry.second->Modified) entry.second->SaveMe(); }
			}

			CEntry _CScenario::GetByIdx(String ekey){
				Trans2Upper(ekey);
				if (!Entries.count(ekey)) return std::shared_ptr<_CEntry>(new _CEntry(this,ekey); //new CEntry(this, ekey);
				return Entries[ekey];
			}

			void _CScenario::UpdateGUI() {
				//if (CurrentProject == nullptr) return;
				//if (CurrentProject.Settings["DIRECTORIES", "SCENARIO"] == "") return;
				// Entries
				//MainWindow.ScenarioEntries.Items.Clear();
				//var SDir = Dirry.AD(CurrentProject.Settings["DIRECTORIES", "SCENARIO"]);
				//if (!Directory.Exists(SDir)) return;
				//var EList = FileList.GetTree(SDir);
				//foreach (var E in EList) if (qstr.ExtractExt(E).ToLower()=="ini") MainWindow.ScenarioEntries.Items.Add(qstr.StripExt(E));
				//MainWindow.Me.AllowCheck();
				Crash("UpdateGUI not yet implemented");
			}

			VecString _CScenario::AllEntries {
				auto SDir { Dirry(CurrentProject->Settings->Valuue("DIRECTORIES", "SCENARIO"))};
				if (!DirectoryExists(SDir)) return nullptr;
				auto got {GetTree(SDir)};
				auto ret {NewVecString()};
				for (autoE :* got) {
					//if (qstr.ExtractExt(E).ToLower() == "ini") ret.Add(qstr.StripExt(E));
					if (Lower(Extractxt(E))=="ini") ret->push_back(E);
				}
				return ret; //.ToArray();
			}

			String ChosenEntryName() { Crash("ChosenEnntryName not yet implemented!"); }


			void _CSenario::UpdateGUIEntry() {
				/*
				if (CurrentProject == null) return;
				if (ChosenEntry == null) return;
				MainWindow.ScenarioTags.Items.Clear();
				var _Tags = ChosenEntry.LTags; _Tags.Sort();
				foreach ( var _Tag in _Tags) {
					MainWindow.ScenarioTags.Items.Add(_Tag);
				}
				MainWindow.Me.AllowCheck();
				*/
				Crash("UpdateGUIEntry not yet implemented");
			}

			void _CSenario::UpdateGUITag() {
				/*
				var old = MainWindow.scenario_allowmodify;
				if (ChosenEntry != null && ChosenEntry.CurrentTag != null) {
					var CPage = ChosenEntry.CurrentTag.CurrentPage;
					MainWindow.scenario_allowmodify = false;
					MainWindow.Me.Scenario_ShD_LB_Entry.Content = ChosenEntryName;
					MainWindow.Me.Scenario_ShD_LB_Tag.Content = ChosenEntry.CurrentTagName;
					MainWindow.Me.Scenario_ShD_LB_Page.Content = $"{ChosenEntry.CurrentTag.CurrentPageNumber + 1} / {ChosenEntry.CurrentTag.PageCount}";
					MainWindow.Me.Scenario_ShD_TB_PicSpecific.Text = CPage.PicSpecific;
					MainWindow.Me.Scenario_ShD_TB_PicDir.Text = CPage.PicDir;
					MainWindow.Me.Scenario_ShD_TB_Audio.Text = CPage.Audio;
					MainWindow.Me.Scenario_ShD_TB_AltFont.Text = CPage.AltFont;
					MainWindow.Me.Scenario_ShD_TB_Namelinking.IsChecked = CPage.NameLinking;
					MainWindow.Me.AllowCheck();
					MainWindow.Me.ScenarioSetLang();
					ChosenEntry.CurrentTag.CurrentPage.LinkUpdate();
					for(var i = 1; i <= 2; i++) {
						var CLang = CPage.ChosenLang(i);
						if (CLang != null) {
							MainWindow.Me.LangField(i).Text = CLang.Content;
						}
					}
				}
				MainWindow.scenario_allowmodify = old;
				*/
				Crash("UpdateGUITag not yet implemented");
			}

			//}
		}
	}
}
