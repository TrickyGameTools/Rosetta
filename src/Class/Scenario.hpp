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
// Version: 25.10.06 XVII
// End License

#pragma once
#include <june19.hpp>
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
			_CScenario* Parent { nullptr };
			String EntryName { "" };
			ProjectData Project();
			std::map<String, CTag> Tags {};

			//internal string EntryFile => Dirry.AD($"{Project.Settings["DIRECTORIES", "SCENARIO"]}/{EntryName}.ini");
			inline String EntryFile();

			bool Modified = false;


			bool TagExists(String Tag);

			void SaveMe() ;

			Units::GINIE Data();

			inline ~_CEntry() {
				if (_Data != nullptr && Modified) {
					SaveMe();
				}
			}

			//internal List<string> LTags => Data.List("::GLOB::", "Tags");
			inline std::vector<String>* LTags() { return Data()->List("::GLOB::","Tags");}

			inline CTag AddTag(String Tag);


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
			inline CTag operator[](String Tag){return GT(Tag);}

			inline _CEntry(_CScenario* _Parent, String EN);

			String CurrentTagName();

			CTag CurrentTag();
		};


		class _CTag {
			private:
				int __currentpagenumber { 0 };
			public:
				_CEntry* Parent { nullptr };
				Units::GINIE Data() { return Parent->Data(); }
				String Tag { "" };

				_CTag (_CEntry* _Parent,String _Tag);

				std::vector<CPage> Page{}; //internal List<CPage> Page = new List<CPage>();
				//internal CPage this[int idx] => Page[idx];
				CPage operator[](int idx) { return Page[idx]; }

				//internal ProjectData Project => Parent.Parent.Parent;
				_ProjectData* Project();


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
			inline int CurrentPageNumber() { return __currentpagenumber; }
			void CurrentPageNumber(int value);
			int PageCount() { return Page.size(); }

			//internal CPage CurrentPage {
			//	get {
			inline CPage CurrentPage() ;
		};


		enum class ELU { None, PicDir,Lang1,Lang2 };

		// Property in C++! It *is* possible!
		class CPMC {
			public:
			_CPage* Ouwe{nullptr};
			void operator=(bool value);
			inline CPMC(_CPage* O) {Ouwe=O;}
			inline CPMC() { Ouwe=nullptr; }
		};

		class _CPage {
			public:


			void LinkUpdate(ELU FromELU=ELU::None,June19::j19gadget* From=nullptr);
			CPMC Modified{}; // Property itself

			_CTag* Parent = nullptr;

			inline _CPage(_CTag* _parent) { Parent = _parent; Modified.Ouwe=this; } // Here the property is finally defined! I told ya it was possible.
			inline GINIE Data() { return Parent->Parent->Data(); }


			//bool Modified {
			//	//get => Parent.Parent.Modified;
			//	set => Parent.Parent.Modified = value; }

			inline int PageIndex() {
				for (int i = 0; i < Parent->Page.size(); ++i) if (Parent->Page[i].get() == this) return i;
				return -1;
			}
			inline String CGCat() {return "::CENTRAL::" +Parent->Tag+TrSPrintF("::%d::",PageIndex());};

			//internal string PicDir {
			inline String  PicDir() {return Data()->Value(CGCat(), "PicDir"); }
			inline void PicDir(String value) { Data()->Value(CGCat(), "PicDir",value); Modified=true; }


			inline String PicSpecific() {
				Data()->NewValue(CGCat(), "PicSpecific", "GENERAL");
				return Data()->Value(CGCat(), "PicSpecific");
			}
			inline void PicSpecific(String value) {	Data()->Value(CGCat(), "PicSpecific",value); Modified = true; }


			inline String Audio() { return Data()->Value(CGCat(), "Audio"); }
			inline void Audio(String value) { Data()->Value(CGCat(), "Audio") = value; }

			inline String AltFont() {return Data()->Value(CGCat(), "Alternate_Font"); }
			inline void AltFont(String value) { Data()->Value(CGCat(), "Alternate_Font",value); }

			inline bool NameLinking() {Data()->NewValue(CGCat(), "NameLinking", "True"); return Lower(Data()->Value(CGCat(), "NameLinking")) == "true"; }
			inline void NameLinking(bool value) {Data()->Value(CGCat(), "NameLinking",boolstring(value));}


			//internal Dictionary<string, CSLang> _Lang = new Dictionary<string, CSLang>();
			std::map<String,CSLang> _Lang{};


			//internal CSLang this[string lkey] {
			//	get {
			CSLang* GLang(String lkey);
			inline CSLang* operator[](String lkey) { return GLang(lkey); }

			String ChosenLangName1();// { return Parent->Parent->Parent->Parent->Settings->Value("::SCENARIO::", "LANG1"); }
			String ChosenLangName2();// { return  Parent->Parent->Parent->Parent->Settings->Value("::SCENARIO::", "LANG2"); }
			CSLang* ChosenLang1() { return  (*this)[ChosenLangName1()]; }
			CSLang* ChosenLang2() { return  (*this)[ChosenLangName2()]; }
			CSLang* ChosenLang(int idx) {
				switch(idx) {
					case 1: return ChosenLang1();
					case 2: return ChosenLang2();
					default: throw std::runtime_error(TrSPrintF("ChosenLang(%d): Invalid index",idx));
				}
			}
		};

		class CSLangModified{
		private:
			CSLang* Ouwe{nullptr};
		public:
			inline CSLangModified(){}; // Never use this.
			inline CSLangModified(CSLang*m){Ouwe=m;}
			void operator=(bool value);
		};

		class CSLang {
			public:
			_CPage* Parent { nullptr };
			String Lang { "" };
			GINIE Data => Parent->Parent->Data;
			CSLangModified Modified{this};
			inline CSLang(_CPage* _Parent,String _Lang) { Parent= _Parent; Lang = _Lang; Parent._Lang[Lang] = this;  }
			int PageIndex() {return  Parent->PageIndex;}

			String CGLCat() { reteurn "::LANG::"+Lang+"::"+Parent->Parent->Tag+TrSPrintF("::%d::",PageIndex()); }


			//bool Modified {
			//	//get => Parent.Parent.Parent.Modified;
			//	set => Parent.Parent.Parent.Modified = value;
			//}

			//internal string Header {
			inline String Header() {	//get {
				if (Parent->NameLinking) return Parent->PicDir;
				return Data->Value(CGLCat, "Header");
			}
			inline void Header(String value) { //	set {
					if (Parent->NameLinking) Parent.PicDir = value;
					else Data->Value(CGLCat, "Header", value);
					Modified = true;
			}

			//internal string Content {
			inline String Content() { //	get {
				String ret{""}; //= new StringBuilder();
				auto lst = Data->List(CGLCat, "Content");
				for(int i = 0; i < lst.size(); ++i) {
					if (i > 0) ret+="\n"; //ret.Append("\n");
					ret+=lst[i];//ret.Append(lst[i]);
				}
				return ret; //.ToString();
			}
			inline void Content(String value) {//	set {
				auto v { StReplace( value,"\r", "") };
				auto lst { Data->List(CGLCat, "Content"); } lst->clear();
				//foreach(var l in v.Split('\n')) lst.Add(l);
				auto vsplit{Split(v,'\n');}
				for(auto l:vsplit) lst+=l;
				Modified = true;
			}

			//internal List<string> LContent => Data.List(CGLCat, "Content");
			VecString  LContent() { return Data->List(CGLCat,"Content"); }
		};


		class _CScenario {
			public:
			_ProjectData* Parent { nullptr };
			//readonly SortedDictionary<string,CEntry> Entries = new SortedDictionary<string,CEntry>();
			std::map<String,CEntry> Entries{};
			_CScenario(_ProjectData* _Parent) { Parent = _Parent; }

			inline void SaveMe(bool force=false);

		public ProjectData CurrentProject => ProjectData.CurrentProject;

		//internal CEntry this[string ekey] {
		//	get {
		//		ekey = ekey.ToUpper();
		//		if (!Entries.ContainsKey(ekey)) return new CEntry(this, ekey);
		//		return Entries[ekey];
		//	}
		//}
		CEntry GetByIdx(String key);
		inline GEntry operator[](String key) { return GetByIdx(key);}

		inline CTag GetTag(String ekey,String tkey) { return GetByIdx(ekey)->GLang(tkey); } // internal CTag this[string ekey,string tkey] => this[ekey][tkey];
		inline CPage GetPage(String ekey,String tkey, int idx) { return GetTag(ekey,tkey)->Page[idx]; } //internal CPage this[string ekey,string tkey,int idx] => this[ekey][tkey][idx];
		inline CSLang* GetLang(String ekey,String tkey, int idx, String lkey) { return GetPage(ekey,tkey,idx)->GLang(lkey); } //this[string ekey,string tkey,int idx,string lkey] => this[ekey][tkey][idx][lkey];

		inline String Workspace(){ return Parent->Settings->Value("DIRECTORIES", "SCENARIO"); }


		void UpdateGUI();

		//internal string[] AllEntries {
		VecString AllEntries();

		//internal string ChosenEntryName {
		//	get {
		//		var it = MainWindow.ScenarioEntries.SelectedItem; if (it == null) return "";
		//		return it.ToString();
		//	}
		//}
		String ChosenEntryName();

		inline CEntry ChosenEntry() {
			auto CEN = ChosenEntryName;
			if (CEN == "") return nullptr;
			return GetByIdx(CEN);
		}


		void UpdateGUIEntry();

		public void UpdateGUITag();
	};
}}}
