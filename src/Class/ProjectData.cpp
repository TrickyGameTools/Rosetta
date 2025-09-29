// License:
// 
// Rosetta
// Project Data
// 
// 
// 
// 	(c) Jeroen P. Broks, 2025
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
// Version: 25.09.29 I
// End License

#include "../Rosetta.hpp"
#include <SlyvGINIE.hpp>
#include <SlyvTime.hpp>

using namespace Slyvina::Units;

namespace Slyvina {
	namespace Rosetta {
		namespace Class {
			ProjectData _ProjectData::CurrentProject = nullptr;


			_ProjectData::_ProjectData(String FileName);  {
				Settings = LoadGINIE(FileName,FileName,"Rosetta project");
				ProjectFile = FileName;
				//Settings.AutoSaveSource = FileName;
				Settings->NewValue("Support", "Languages", "English, Dutch");
				Settings->NewValue("Support", "Language_Def", "English"); // Used when no suitable translation is found.
				Scenario = cpnew(CScenario,this);
			}

			GINIE _ProjectData::GetStrings(String Language) {
				if (!Strings.count(Language)) {
					Strings[Language] = LoadGINIE(StringsDir()+"/"+Language+".ini");
					Strings[Language]->NewValue("^SYS^","CreationDate",Now());
					Strings[Language]->NewValue("^SYS^", "CreationTool", "Rosetta");
					Strings[Language]->NewValue("^SYS^", "Active", "Yes"); // Setting for my own programs to make it easy to hide languages.
				}
				return Strings[Language];
			}

			void _ProjectData::UpdateStrings() {
				/* Original C# code
				if (!MainWindow.strings_allowmodify) return;
				var old = MainWindow.strings_allowmodify;
				MainWindow.strings_allowmodify = false;
				var sc = MainWindow.Me.StringCats;
				sc.Items.Clear();
				foreach (var cat in Settings.List("Strings", "^Categories^")) sc.Items.Add(cat);
				MainWindow.strings_allowmodify = old;
				*/
				Crash("Update Strings Not Yet Implemented");
			}

		}
	}
}
