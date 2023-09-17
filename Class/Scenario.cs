using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Diagnostics.Eventing.Reader;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Documents;
using TrickyUnits;

namespace Rosetta.Class {
	internal class CScenario {

		internal class CEntry {
			private GINIE _Data = null;
			internal readonly CScenario Parent = null;
			internal string EntryName = "";
			internal ProjectData Project => Parent.Parent;
			internal SortedDictionary<string, CTag> Tags = new SortedDictionary<string, CTag>();

			internal string EntryFile => Dirry.AD($"{Project.Settings["DIRECTORIES", "SCENARIO"]}/{EntryName}.ini");

			internal bool Modified = false;

			internal bool TagExists(string Tag) {
				Tag = Tag.ToUpper();
				return LTags.Contains(Tag);
			}
			internal void SaveMe() {
				Debug.WriteLine($"Saving: {EntryFile}");
				QuickStream.SaveString(EntryFile,_Data.ToSource());
				Modified = false;
			}

			internal GINIE Data {
				get {
					if (_Data == null) {
						Debug.WriteLine($"Loading: {EntryFile}");
						_Data = GINIE.FromFile(EntryFile);
						_Data.NewValue("::SYS::", "Created", DateTime.Now.ToString());
						_Data.NewValue("::SYS::", "Tool", "Rosetta");
						_Data.NewValue("::SYS::", "Entry", EntryName);
					}
					return _Data;
				}
			}
			~CEntry() {
				if (_Data != null && Modified) {
					SaveMe();
				}
			}

			internal List<string> LTags => Data.List("::GLOB::", "Tags");

			CTag AddTag(string Tag) {
				Tag = Tag.ToUpper();
				var ret = new CTag(this, Tag);
				Tags[Tag] = ret;
				return ret;
			}
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

			internal void LinkUpdate(TextBox From) {
				throw new NotImplementedException();
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
					if (!_Lang.ContainsKey(lkey)) {
						return new CSLang(this,lkey);
					}
					return _Lang[lkey];
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
			var CPage = ChosenEntry.CurrentTag.CurrentPage;
			MainWindow.scenario_allowmodify = false;
			MainWindow.Me.Scenario_ShD_LB_Entry.Content = ChosenEntryName;
			MainWindow.Me.Scenario_ShD_LB_Tag.Content = ChosenEntry.CurrentTagName;
			MainWindow.Me.Scenario_ShD_LB_Page.Content = $"{ChosenEntry.CurrentTag.CurrentPageNumber + 1} / {ChosenEntry.CurrentTag.PageCount}";
			MainWindow.scenario_allowmodify = old;
			MainWindow.Me.Scenario_ShD_TB_PicSpecific.Text = CPage.PicSpecific;
			MainWindow.Me.Scenario_ShD_TB_PicDir.Text = CPage.PicDir;
			MainWindow.Me.Scenario_ShD_TB_Audio.Text = CPage.Audio;
			MainWindow.Me.Scenario_ShD_TB_AltFont.Text = CPage.AltFont;
			MainWindow.Me.Scenario_ShD_TB_Namelinking.IsChecked = CPage.NameLinking;
			MainWindow.Me.AllowCheck();
		}
	}
}
