// Lic:
// Rosetta
// Project Data
// 
// 
// 
// (c) Jeroen P. Broks, 2023
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 23.09.22
// EndLic
using Rosetta.Export;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;
using TrickyUnits;

namespace Rosetta.Class {
	internal class ProjectData {

		internal readonly string ProjectFile;
		internal static ProjectData CurrentProject = null;

		internal readonly GINIE Settings = null;
		internal readonly Dictionary<string,GINIE> Strings = new Dictionary<string,GINIE>();
		internal CScenario Scenario = null; 

		internal string[] SupportedLanguages {
			get {
				var ret = Settings["Support", "Languages"].Split(',');
				for(uint i=0; i<ret.Length; i++) ret[i] = ret[i].Trim();
				return ret;
			}
		}

		internal ProjectData(string FileName) {
			Settings = GINIE.FromFile(FileName);
			ProjectFile = FileName;
			Settings.AutoSaveSource = FileName;
			Settings.NewValue("Support", "Languages", "English, Dutch");
			Settings.NewValue("Support", "Language_Def", "English"); // Used when no suitable translation is found.
			Scenario = new CScenario(this);
		}

		internal GINIE GetStrings(string Language) {
			if (!Strings.ContainsKey(Language)) {
				Strings[Language] = GINIE.FromFile($"{StringsDir}/{Language}.ini");
				Strings[Language].NewValue("^SYS^","CreationDate",$"{DateTime.Now}");
				Strings[Language].NewValue("^SYS^", "CreationTool", "Rosetta");
				Strings[Language].NewValue("^SYS^", "Active", "Yes"); // Setting for my own programs to make it easy to hide languages.
			}
			return Strings[Language];
		}

		internal void UpdateStrings() {
			if (!MainWindow.strings_allowmodify) return;
			var old = MainWindow.strings_allowmodify;
			MainWindow.strings_allowmodify = false;
			var sc = MainWindow.Me.StringCats;
			sc.Items.Clear();
			foreach (var cat in Settings.List("Strings", "^Categories^")) sc.Items.Add(cat);
			MainWindow.strings_allowmodify = old;
		}

		internal void UpdateStringsCats() {
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
		}

		~ProjectData() { SaveMe(); }

		internal string StringsDir => Dirry.AD(Settings["Directories", "Strings"]);
		internal string ExportDir => Dirry.AD(Settings["Export", "Scenario"]);

		internal void SaveMe(bool dontexport=false) {
			try {
				// Strings			
				var StrDir = StringsDir;
				if (StrDir != "") {
					Debug.WriteLine(StrDir);
					if (!Directory.Exists(StrDir)) { Directory.CreateDirectory(StringsDir); }
					foreach (var str in Strings) {
						QuickStream.SaveString($"{StrDir}/{str.Key}.ini", str.Value.ToSource());
					}
				}
				// Scenario
				Scenario.SaveMe();

				// Export
				if (dontexport) return;
				var Langs = SupportedLanguages;
				if (Settings["Export", "Method"] != "") {
					if (!XBase.Register.ContainsKey(Settings["Export", "Method"])) {
						Confirm.Error($"Export method {Settings["Export", "Method"]} does not appear to exist!", "Internal error");
						return;
					}
					foreach (var L in Langs) {
						XBase.Register[Settings["Export", "Method"]].Export(this, L);
					}
				}
			} catch(Exception ex) {
				Confirm.Error($"An error occurred while saving a project!\n\n{ex.Message}");
			}
		}

		internal void RenewSettings() {
			var tallow = MainWindow.config_allowmodify;
			MainWindow.config_allowmodify = false;
			foreach(var item in RegisterSettings.Reg.Values) {
				item.TarTextBox.Text = Settings[item.Category, item.Key];
			}
			Settings.NewValue("Export", "Method", "ScenLang");
			MainWindow.Me.ExportMethod = Settings["Export", "Method"];
			MainWindow.config_allowmodify = tallow;

		}
	}
}