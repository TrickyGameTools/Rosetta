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
#include <SlyvStream.hpp>
#include <SlyvDir.hpp>

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

			void _ProjectData::UpdateCats() {
				/*
				if (MainWindow.Me.StringCat == "") return;
				var old = MainWindow.strings_allowmodify;
				var Lst = Settings.List("Strings", $"CAT_{MainWindow.Me.StringCat}");
				MainWindow.strings_allowmodify = false;
				foreach (var ES in RegisterStrings.Lijst) {
					ES.Key.Items.Clear();
					foreach (var K in Lst) {
						ES.Key.Items.Add(K);
					}
					var CKey = CurrentProject.Settings["Strings", $"Key_{MainWindow.Me.StringCat}_{ES.Index}"];
					for (int i = 0; i < Lst.Count; ++i) {
						if (Lst[i] == CKey) ES.Key.SelectedIndex = i;
					}
					if (CKey=="") {
						ES.Value1.Text = "";
						ES.Value2.Text = "";
					} else {
						if (MainWindow.Lang1 == "")
							ES.Value1.Text = "";
						else {
							var LG=CurrentProject.GetStrings(MainWindow.Lang1);
							ES.Value1.Text = LG[MainWindow.Me.StringCat, CKey];
						}
						if (MainWindow.Lang2 == "")
							ES.Value2.Text = "";
						else {
							var LG = CurrentProject.GetStrings(MainWindow.Lang2);
							ES.Value2.Text = LG[MainWindow.Me.StringCat, CKey];
						}
					}
				}
				MainWindow.strings_allowmodify = old;
				//*/
				Crash("UpdateCats not yet implemented");
			}

			_ProjectData::~_ProjectData  { SaveMe(); }


			void _ProjectData::SaveMe(bool dontexport=false) {
				try {
					// Strings
					auto StrDir = StringsDir();
					if (StrDir != "") {
						//Debug.WriteLine(StrDir);
						if (!DirectoryExists(StrDir)) { MakeDir(StringsDir); }
						for (auto& str : Strings) {
							SaveString(StrDir+"/"+str.first+".ini", str.second->ToSource());
						}
					}
					// Scenario
					Scenario.SaveMe();

					// Export
					if (dontexport) return;
					auto Langs { SupportedLanguages };
					if (Settings->Value("Export", "Method") != "") {
						if (!XBase.Register.count(Settings->Value("Export", "Method"))) {
							QCol->Error("\7Export method "+Settings->("Export", "Method")+" does not appear to exist!\tInternal error");
							return;
						}
						for (auto& L in Langs) {
							XBase::Register[Settings->Value("Export", "Method")].Export(this, L);
						}
					}
				} catch(std::runtime_error ex) {
					QCol->Error("\7An error occurred while saving a project! -> "+ex.what());
				}
			}

			void _ProjectData::RenewSettings() {
				/*
				var tallow = MainWindow.config_allowmodify;
				MainWindow.config_allowmodify = false;
				foreach(var item in RegisterSettings.Reg.Values) {
					item.TarTextBox.Text = Settings[item.Category, item.Key];
				}
				Settings.NewValue("Export", "Method", "ScenLang");
				MainWindow.Me.ExportMethod = Settings["Export", "Method"];
				MainWindow.config_allowmodify = tallow;
				//*/
				Crash("Renew Settings not yet implemented");
			}
		}
	}
}
