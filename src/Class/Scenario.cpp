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
// Version: 25.09.30 IV
// End License

#include <algorithm>

#include "../Rosetta.hpp"

#include <SlyvTime.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvString.hpp>
#include <SlyvSTOI.hpp>

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

			String _Centry::CurrentTagName() {
				Crash("CurrentTagName() not yet implemented");
				return "ERROR";
				//		if (MainWindow.ScenarioTags.SelectedItem == null) return "";
				//		return MainWindow.ScenarioTags.SelectedItem.ToString();
			}

			CTag CurrentTag() {
				auto CTN = CurrentTagName();
				if (CTN == "") return nullptr;
				Trans2Upper(CTN);
				return GT(CTN);

			}
			//}

			//{ Tag
			_CTag::_CTag (CEntry _Parent,string _Tag) {
				Parent = _Parent;
				Tag = _Tag;
				Parent->Tags[Tag] = std::shared_ptr<_CTag>(this);
				auto PC {std::max(1,ToInt(Data()->Value("::PAGES::", Tag));
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
			//}
		}
	}
}
