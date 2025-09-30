// License:
// 
// Rosetta
// Scenario Classes (header)
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
// Version: 25.09.30 I
// End License

#pragma once
#include "../Rosetta.hpp"


namespace Slyvina { namespace Rosetta { namespace Class {
	//internal class CScenario { // That doesn't work in C++

		class _CEntry {
		private:
			GINIE _Data { nullptr };
		public:
			CScenario Parent { nullptr };
			String EntryName { "" };
			ProjectData Project();
			std:map<string, CTag> Tags {};

			//internal string EntryFile => Dirry.AD($"{Project.Settings["DIRECTORIES", "SCENARIO"]}/{EntryName}.ini");
			inline String EntryFile() { return Dirry(Project->Settings->Value("DIRECTORIES","SCENARIO")+"/"+EntryNamee+".ini"); }

			bool Modified = false;


			bool TagExists(string Tag);

			void SaveMe() ;

			Units::GINIE Data();

			inline ~CEntry() {
				if (_Data != nullptr && Modified) {
					SaveMe();
				}
			}

			//internal List<string> LTags => Data.List("::GLOB::", "Tags");
			inline VecString LTags() { return Data()->List("::GLOB::","Tags");}

			CTag AddTag(string Tag) {
				Tag = Tag.ToUpper();
				var ret = new CTag(this, Tag);
				Tags[Tag] = ret;
				return ret;
			}

			// MARKER
			internal CTag this[string Tag] {
				get {
					Tag = Tag.ToUpper();
					if (!Tags.ContainsKey(Tag)) return AddTag(Tag);
					return Tags[Tag];
				}
			}
			internal CEntry(CScenario Parent, string EN) {
				this.Parent = Parent;
				EntryName = EN;
				Parent.Entries[EN] = this;
			}

			internal string CurrentTagName {
				get {
					if (MainWindow.ScenarioTags.SelectedItem == null) return "";
					return MainWindow.ScenarioTags.SelectedItem.ToString();
				}
			}

			internal CTag CurrentTag {
				get {
					var CTN = CurrentTagName;
					if (CTN == "") return null;
					CTN = CTN.ToUpper();
					return this[CTN];
				}
			}
		}

		internal class CTag {
			readonly internal CEntry Parent = null;
			internal GINIE Data => Parent.Data;
			readonly internal string Tag = "";
			internal CTag (CEntry _Parent,string _Tag) {
				Parent = _Parent;
				Tag = _Tag;
				Parent.Tags[Tag] = this;
				var PC = Math.Max(1,qstr.ToInt(Data["::PAGES::", Tag]));
				for (int i = 0; i < PC; i++) Page.Add(new CPage(this));
			}
			internal List<CPage> Page = new List<CPage>();
			internal CPage this[int idx] => Page[idx];

			internal ProjectData Project => Parent.Parent.Parent;

			private int __currentpagenumber = 0;
			internal int CurrentPageNumber {
				get => __currentpagenumber;
				set {
					var s = Math.Max(0, value);
					if (s >= Page.Count) {
						if (Page.Count == 0 || Confirm.Yes("Add a new page?")) {
							Page.Add(new CPage(this));
							Data["::PAGES::", Tag] = $"{Page.Count}";
						}
						s = Page.Count - 1;
					}
					__currentpagenumber = s;
				}
			}
			public int PageCount => Page.Count;

			internal CPage CurrentPage {
				get {
					if (Page.Count == 0) {
						__currentpagenumber = 0;
						Page.Add(new CPage(this));
					}
					return Page[CurrentPageNumber];
				}
			}

		}

		internal class CPage {

			internal enum ELU { None, PicDir,Lang1,Lang2 }

			internal void LinkUpdate(ELU FromELU=ELU.None,TextBox From=null) {
				//throw new NotImplementedException();
				var old = MainWindow.scenario_allowmodify;
				MainWindow.scenario_allowmodify = false;
				if (FromELU != ELU.PicDir) MainWindow.ScenarioPicDir = PicDir;
				if (FromELU != ELU.Lang1 && ChosenLang1 != null) MainWindow.LangHead[1] = ChosenLang1.Header;
				if (FromELU != ELU.Lang2 && ChosenLang2 != null) MainWindow.LangHead[2] = ChosenLang2.Header;
				MainWindow.scenario_allowmodify = old;
			}
			internal readonly CTag Parent = null;
			internal CPage(CTag _parent) => Parent = _parent;
			internal GINIE Data => Parent.Parent.Data;

			string CGCat => $"::CENTRAL::{Parent.Tag}::{PageIndex}::";

			bool Modified {
				//get => Parent.Parent.Modified;
				set => Parent.Parent.Modified = value;
			}

			internal int PageIndex {
				get {
					for (int i = 0; i < Parent.Page.Count; ++i) if (Parent.Page[i] == this) return i;
					return -1;
				}
			}
			internal string PicDir {
				get => Data[CGCat, "PicDir"];
				set { Data[CGCat, "PicDir"] = value; Modified=true; }
			}

			internal string PicSpecific {
				get {
					Data.NewValue(CGCat, "PicSpecific", "GENERAL");
					return Data[CGCat, "PicSpecific"];
				}
				set { Data[CGCat, "PicSpecific"] = value; Modified = true; }
			}

			internal string Audio {
				get => Data[CGCat, "Audio"];
				set => Data[CGCat, "Audio"] = value;
			}
			internal string AltFont {
				get => Data[CGCat, "Alternate_Font"];
				set => Data[CGCat, "Alternate_Font"] = value;
			}

			internal bool NameLinking {
				get { Data.NewValue(CGCat, "NameLinking", "True"); return Data[CGCat, "NameLinking"].ToLower() == "true"; }
				set => Data[CGCat, "NameLinking"] = value.ToString();
			}

			internal Dictionary<string, CSLang> _Lang = new Dictionary<string, CSLang>();
			internal CSLang this[string lkey] {
				get {
					if (lkey == "") return null;
					if (!_Lang.ContainsKey(lkey)) {
						return new CSLang(this,lkey);
					}
					return _Lang[lkey];
				}
			}
			internal string ChosenLangName1 => Parent.Parent.Parent.Parent.Settings["::SCENARIO::", "LANG1"];
			internal string ChosenLangName2 => Parent.Parent.Parent.Parent.Settings["::SCENARIO::", "LANG2"];
			internal CSLang ChosenLang1 => this[ChosenLangName1];
			internal CSLang ChosenLang2 => this[ChosenLangName2];
			internal CSLang ChosenLang(int idx) {
				switch(idx) {
					case 1: return ChosenLang1;
						case 2: return ChosenLang2;
					default: throw new Exception($"ChosenLang({idx}): Invalid index");
				}
			}
		}

		internal class CSLang {
			internal readonly CPage Parent = null;
			internal readonly string Lang = "";
			internal GINIE Data => Parent.Parent.Data;
			internal CSLang(CPage _Parent,string _Lang) { Parent= _Parent; Lang = _Lang; Parent._Lang[Lang] = this; }
			internal int PageIndex => Parent.PageIndex;

			internal string CGLCat => $"::LANG::{Lang}::{Parent.Parent.Tag}::{PageIndex}::";


			bool Modified {
				//get => Parent.Parent.Parent.Modified;
				set => Parent.Parent.Parent.Modified = value;
			}

			internal string Header {
				get {
					if (Parent.NameLinking) return Parent.PicDir;
					return Data[CGLCat, "Header"];
				}
				set {
					if (Parent.NameLinking) Parent.PicDir = value;
					else Data[CGLCat, "Header"] = value;
					Modified = true;
				}
			}
			internal string Content {
				get {
					var ret = new StringBuilder();
					var lst = Data.List(CGLCat, "Content");
					for(int i = 0; i < lst.Count; ++i) {
						if (i > 0) ret.Append("\n");
						ret.Append(lst[i]);
					}
					return ret.ToString();
				}
				set {
					var v = value.Replace("\r", "");
					var lst = Data.List(CGLCat, "Content"); lst.Clear();
					foreach(var l in v.Split('\n')) lst.Add(l);
					Modified = true;
				}
			}

			internal List<string> LContent => Data.List(CGLCat, "Content");
		}



		readonly ProjectData Parent = null;
		readonly SortedDictionary<string,CEntry> Entries = new SortedDictionary<string,CEntry>();

		public CScenario(ProjectData Parent) { this.Parent = Parent; }

		public void SaveMe(bool force=false) {
			foreach(CEntry entry in Entries.Values) { if (force || entry.Modified) entry.SaveMe(); }
		}

		public ProjectData CurrentProject => ProjectData.CurrentProject;

		internal CEntry this[string ekey] {
			get {
				ekey = ekey.ToUpper();
				if (!Entries.ContainsKey(ekey)) return new CEntry(this, ekey);
				return Entries[ekey];
			}
		}
		internal CTag this[string ekey,string tkey] => this[ekey][tkey];
		internal CPage this[string ekey,string tkey,int idx] => this[ekey][tkey][idx];
		internal CSLang this[string ekey,string tkey,int idx,string lkey] => this[ekey][tkey][idx][lkey];

		internal string Workspace => Parent.Settings["DIRECTORIES", "SCENARIO"];


		public void UpdateGUI() {
			if (CurrentProject == null) return;
			if (CurrentProject.Settings["DIRECTORIES", "SCENARIO"] == "") return;
			// Entries
			MainWindow.ScenarioEntries.Items.Clear();
			var SDir = Dirry.AD(CurrentProject.Settings["DIRECTORIES", "SCENARIO"]);
			if (!Directory.Exists(SDir)) return;
			var EList = FileList.GetTree(SDir);
			foreach (var E in EList) if (qstr.ExtractExt(E).ToLower()=="ini") MainWindow.ScenarioEntries.Items.Add(qstr.StripExt(E));
			MainWindow.Me.AllowCheck();
		}

		internal string[] AllEntries {
			get {
				var SDir = Dirry.AD(CurrentProject.Settings["DIRECTORIES", "SCENARIO"]);
				if (!Directory.Exists(SDir)) return null;
				var got = FileList.GetTree(SDir);
				var ret = new List<string>();
				foreach (var E in got) {
					if (qstr.ExtractExt(E).ToLower() == "ini") ret.Add(qstr.StripExt(E));
				}
				return ret.ToArray();
			}
		}

		internal string ChosenEntryName {
			get {
				var it = MainWindow.ScenarioEntries.SelectedItem; if (it == null) return "";
				return it.ToString();
			}
		}
		internal CEntry ChosenEntry {
			get {
				var CEN = ChosenEntryName;
				if (CEN == "") return null;
				return this[CEN];
			}
		}


		public void UpdateGUIEntry() {
			if (CurrentProject == null) return;
			if (ChosenEntry == null) return;
			MainWindow.ScenarioTags.Items.Clear();
			var _Tags = ChosenEntry.LTags; _Tags.Sort();
			foreach ( var _Tag in _Tags) {
				MainWindow.ScenarioTags.Items.Add(_Tag);
			}
			MainWindow.Me.AllowCheck();
		}

		public void UpdateGUITag() {
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

		}
	}
}}
