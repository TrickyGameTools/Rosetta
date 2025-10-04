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
// Version: 25.10.01 I
// End License

#pragma once
#include <SlyvGINIE.hpp>
#include "AllClasses.hpp"
#include "../Rosetta.hpp"

using namespace Slyvina::Units;

namespace Slyvina { namespace Rosetta { namespace Class {
	//internal class CScenario { // That doesn't work in C++

		class _CEntry {
		private:
			GINIE _Data { nullptr };
		public:
			CScenario Parent { nullptr };
			String EntryName { "" };
			ProjectData Project();
			std::map<string, CTag> Tags {};

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

			inline CTag AddTag(string Tag) {
				Units::Trans2Upper(Tag); //Tag = Tag.ToUpper();
				auto ret {std::shared_ptr<_CTag>(new CTag(this, Tag))};
				Tags[Tag] = ret;
				return ret;
			}


			//internal CTag this[string Tag] {
			//	get {
			//		Tag = Tag.ToUpper();
			//		if (!Tags.ContainsKey(Tag)) return AddTag(Tag);
			//		return Tags[Tag];
			//	}
			//}
			inline CTag GT(String Tag) {
				Units::Trans2Upper(Tag);
				if (!Tags.count(Tag)) return AddTag(Tag);
				return Tags[Tag];
			}
			inline CTag operator[](String Tag){return GT(Tag)};


			inline CEntry(CScenario Parent, string EN) {
				this->Parent = Parent;
				EntryName = EN;
				Parent->Entries[EN] = this;
			}

			String CurrentTagName();

			CTag CurrentTag();
		}


		class _CTag {
			private:
				int __currentpagenumber { 0 };
			public:
				CEntry Parent { null };
				Units::GINIE Data() => Parent->Data;
				String Tag { "" };

				_CTag (CEntry _Parent,string _Tag);

				std::vector<CPage> Page{}; //internal List<CPage> Page = new List<CPage>();
				//internal CPage this[int idx] => Page[idx];
				CPage operator[](int idx) { return Page[idx]; }

				//internal ProjectData Project => Parent.Parent.Parent;
				ProjectData Project() { return Parent->Parent->Parent; }


			/*internal int CurrentPageNumber {
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
			*/
			inline int CurrentPageNumber() { return _currentpagenumber; }
			void CurrentPageNumber(int value);
			public int PageCount => Page.Count;

			//internal CPage CurrentPage {
			//	get {
			inline CPage CurrentPage() {
					if (Page.size() == 0) {
						__currentpagenumber = 0;
						//Page.Add(new CPage(this));
						Page.push_back(std::shared_ptr<_CPage>(new _CPage(this)));
					}
					return Page[CurrentPageNumber];
				}
			}
		};


		enum class ELU { None, PicDir,Lang1,Lang2 }

		// Property in C++! It *is* possible!
		class CPMC {
			public:
			_CPage* Ouwe{nullptr};
			inline void operator=(bool value) {Ouwe->Parent->Parent=value;}
			inline CPMC(_CPage* O) {Ouwe=O;}
			inline CPMC() { Ouwe=nullptr; }
		};

		class _CPage {
			public:


			void LinkUpdate(ELU FromELU=ELU::None,June19::j19gadget* From=nullptr);
			CPMC Modified{}; // Property itself

			CTag Parent = nullptr;

			inline CPage(CTag _parent) { Parent = _parent; Modifed=CPMC(this); } // Here the property is finally defined! I told ya it was possible.
			inline GINIE Data { return Parent->Parent->Data; }


			//bool Modified {
			//	//get => Parent.Parent.Modified;
			//	set => Parent.Parent.Modified = value; }

			inline int PageIndex() {
					for (int i = 0; i < Parent->Page.size(); ++i) if (Parent->Page[i].get() == this) return i;
					return -1;

			}
			inline string CGCat() {return "::CENTRAL::" +Parent->Tag+TrSPrintF("::%d::",PageIndex());

			//internal string PicDir {
			inline String  PicDir() {return Data->Value(CGCat, "PicDir"); }
			inline PicDir(String value) { Data->Value(CGCat, "PicDir",value); Modified=true; }


			inline String PicSpecific() {
				Data->NewValue(CGCat, "PicSpecific", "GENERAL");
				return Data->Value(CGCat, "PicSpecific");
			}
			inline void PicSpecific(String value) {	Data->Value(CGCat, "PicSpecific",value); Modified = true; }


			inline String Audio() { return Data->Value(CGCat, "Audio"); }
			inline void Audio(String value) { Data->Value(CGCat, "Audio") = value; }

			inline String AltFont() {return Data->Value(CGCat, "Alternate_Font"); }
			inline void AltFont(String value) Data->Value(CGCat, "Alternate_Font",value); }

			inline bool NameLinking() {Data->NewValue(CGCat, "NameLinking", "True"); return Lower(Data->Value(CGCat, "NameLinking")) == "true"; }
			inline void NameLinking(bool value) {Data->Value(CGCat, "NameLinking",boolstring(value));}


			//internal Dictionary<string, CSLang> _Lang = new Dictionary<string, CSLang>();
			std::map<String,CSLang> _Lang{};


			//internal CSLang this[string lkey] {
			//	get {
			CSLang* GLang(string lkey);
			inline CSLang* operator[](string lkey) { return GLang(lkey); }

			string ChosenLangName1() { return Parent->Parent->Parent->Parent->Settings->Value("::SCENARIO::", "LANG1"); }
			string ChosenLangName2() { return  Parent->Parent->Parent->Parent->Settings->Value("::SCENARIO::", "LANG2"); }
			CSLang* ChosenLang1() { return  (*this)[ChosenLangName1]; }
			CSLang* ChosenLang2() { return  (*this)[ChosenLangName2]; }
			CSLang* ChosenLang(int idx) {
				switch(idx) {
					case 1: return ChosenLang1();
					case 2: return ChosenLang2();
					default: throw std::runtime_error(TrSPrintF("ChosenLang(%d): Invalid index",idx);
				}
			}
		}

		// MARKER
		class CSLang {
			internal readonly _CPage* Parent = null;
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

		public _CScenario(ProjectData Parent) { this.Parent = Parent; }

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
