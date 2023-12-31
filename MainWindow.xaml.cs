// Lic:
// Rosetta
// Main Window (code)
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

using Rosetta.Class;
using Rosetta.Export;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using TrickyUnits;

namespace Rosetta {

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {

		static public MainWindow Me { get; private set; } = null;

		static public bool strings_allowmodify = true;
		static public bool config_allowmodify = true;
		static public bool scenario_allowmodify = true;


		ProjectData CurrentProject => ProjectData.CurrentProject;

		public static string ProjectTB {
			get => Me.TB_Project.Text;
			set => Me.TB_Project.Text = value;
		}

		readonly List<Grid> NeedScenarioEntry = new List<Grid>();
		readonly List<Grid> NeedScenarioTag = new List<Grid>();

		public MainWindow() {
			InitializeComponent();

			ProjectList.ToListBox(LB_Projects);
			AllowCheck();
			Me = this;

			new RegisterSettings(TB_Languages, "Support", "Languages");
			new RegisterSettings(TB_LanguageDefault, "Support", "Language_Def");
			new RegisterSettings(TB_DirStrings, "Directories", "Strings");
			new RegisterSettings(TB_DirScenario, "Directories", "Scenario");
			new RegisterSettings(TB_DirScenarioExport, "Export", "Scenario");

			new RegisterStrings(1, StrKey1, StrLan1Val1, StrLan2Val1);
			new RegisterStrings(2, StrKey2, StrLan1Val2, StrLan2Val2);
			new RegisterStrings(3, StrKey3, StrLan1Val3, StrLan2Val3);
			new RegisterStrings(4, StrKey4, StrLan1Val4, StrLan2Val4);
			new RegisterStrings(5, StrKey5, StrLan1Val5, StrLan2Val5);
			new RegisterStrings(6, StrKey6, StrLan1Val6, StrLan2Val6);
			new RegisterStrings(7, StrKey7, StrLan1Val7, StrLan2Val7);
			new RegisterStrings(8, StrKey8, StrLan1Val8, StrLan2Val8);
			new RegisterStrings(9, StrKey9, StrLan1Val9, StrLan2Val9);
			new RegisterStrings(10, StrKey10, StrLan1Val10, StrLan2Val10);
			new RegisterStrings(11, StrKey11, StrLan1Val11, StrLan2Val11);
			new RegisterStrings(12, StrKey12, StrLan1Val12, StrLan2Val12);

			Dirry.InitAltDrives();

			// Scenario
			NeedScenarioEntry.Add(Scenario_Grid_Tag);
			NeedScenarioTag.Add(Scenario_Grid_SharedData);
			NeedScenarioTag.Add(Scenario_Bottom);
			

			// AutoSave
			InitAutoSave();

			// Init Export
			XBase.Init();
			CB_ExportMethod.Items.Clear();
			foreach(var X in XBase.Register.Keys) CB_ExportMethod.Items.Add(X);
		}

		internal void ScenarioSetLang() {
			ScenarioSetLang(Scenario_CB_Lang1, CurrentProject.Settings["::SCENARIO::", "LANG1"]);
			ScenarioSetLang(Scenario_CB_Lang2, CurrentProject.Settings["::SCENARIO::", "LANG2"]);
		}

		internal void ScenarioSetLang(ComboBox CLB,string Lang) {
			if (CurrentProject == null) return;
			CLB.Items.Clear();
			var L = CurrentProject.SupportedLanguages;
			var S = -1;
			for(var i=0; i<L.Length;++i) {
				var CL = L[i];
				CLB.Items.Add(CL);
				if (CL == Lang) S = i;
			}
			if (S >= 0) CLB.SelectedIndex = S;
		}

		public Visibility Vis(bool K) { if (K) return Visibility.Visible; else return Visibility.Hidden; }

		public void AllowCheck() {
			Debug.WriteLine("Allow Check Requested");
			MainTabber.Visibility = Vis(CurrentProject != null);
			TabStrings.IsEnabled = CurrentProject != null && CurrentProject.Settings["Directories", "Strings"] != "";
			TabScenario.IsEnabled = CurrentProject != null && CurrentProject.Settings["Directories", "Scenario"] != "";
			Pick_DirScenarioExport.IsEnabled = TabScenario.IsEnabled;
			TB_DirScenarioExport.IsEnabled = TabScenario.IsEnabled;
			CB_ExportMethod.IsEnabled = TabScenario.IsEnabled && TB_DirScenarioExport.Text.Trim() != "";
			String_NewCategoryActivate.IsEnabled = String_NewCategory.Text != "";
			Strings_Grid.Visibility = Vis(StringCat != "");
			String_NewStringButton.IsEnabled = String_NewString.Text != "";


			foreach (var RS in RegisterStrings.Lijst) {
				var HasKey = RS.Key.SelectedItem != null;
				string Lang1 = "", Lang2 = "";
				if (CurrentProject != null) {
					Lang1 = CurrentProject.Settings["Strings", "Lang1"];
					Lang2 = CurrentProject.Settings["Strings", "Lang2"];
				}
				RS.Value1.IsEnabled = HasKey && Lang1 != "" && Lang1 != Lang2;
				RS.Value2.IsEnabled = HasKey && Lang2 != "" && Lang1 != Lang2;
				foreach (var RS2 in RegisterStrings.Lijst) {
					if (RS2.Index != RS.Index && RS.Key.SelectedItem!=null && RS2.Key.SelectedItem!=null && RS.Key.SelectedItem.ToString() == RS2.Key.SelectedItem.ToString()) {
						RS.Value1.IsEnabled = false;
						RS.Value2.IsEnabled = false;
					}
				}
			}

			// Scenario
			BT_Scenario_File_New.IsEnabled = TB_Scenario_NewFile.Text != "";
			BT_Scenario_Tag_New.IsEnabled = TB_Scenario_NewTag.Text != "";

			var HasScenEntry=LB_Scenario_File.SelectedItem!= null;
			var HasScenTag = HasScenEntry && LB_Scenario_Tag.SelectedItem != null;
			foreach (var G in NeedScenarioEntry) G.Visibility = Vis(HasScenEntry);
			foreach (var G in NeedScenarioTag) G.Visibility = Vis(HasScenTag);
		}

		public void AllowCheck(object o, TextChangedEventArgs NotNeededCrap) => AllowCheck();

		private void BT_NewProject_Click(object sender, RoutedEventArgs e) {
			var NewProject = FFS.RequestFile(true);
			if (NewProject == "") return;
			var NPK = qstr.StripDir(NewProject);
			if (ProjectList.PRJ.ContainsKey(NPK)) {
				Confirm.Annoy("Duplicate project name!\nPlease select another", "Error", System.Windows.Forms.MessageBoxIcon.Error);
				return;
			}
			ProjectList.Create(LB_Projects, NewProject);
		}

		private void LB_Projects_SelectionChanged(object sender, SelectionChangedEventArgs e) {
			var item = LB_Projects.SelectedItem;
			if (item == null) {
				ProjectData.CurrentProject = null;
				Debug.WriteLine("No item selected. So no project is there");
			} else {
				var itemname = item.ToString();
				Debug.WriteLine($"Item selected: {itemname}");
				ProjectList.Select(itemname);
			}
			AllowCheck();
		}

		private void TextBoxMainSettingsChanged(object sender, TextChangedEventArgs NotNeededCrap) {
			if (config_allowmodify) {
				var s = (TextBox)sender;
				var e = RegisterSettings.Reg[s];
				CurrentProject.Settings[e.Category, e.Key] = s.Text;
				AllowCheck();
			}
		}

		Dictionary<Button, TextBox> DirButtonReg = null;
		private void DirButtonClick(object sender, RoutedEventArgs e) {
			if (DirButtonReg == null) {
				Debug.WriteLine("Creating Dir Button Register!");
				DirButtonReg = new Dictionary<Button, TextBox>();
				DirButtonReg[Pick_DirStrings] = TB_DirStrings;
				DirButtonReg[Pick_DirScenario] = TB_DirScenario;
				DirButtonReg[Pick_DirScenarioExport] = TB_DirScenarioExport;
			}
			var PDir = FFS.RequestDir();
			if (PDir != "") DirButtonReg[(Button)sender].Text = PDir.Replace('\\', '/');
		}

		public bool ValidName(string name) {
			for (int i = 0; i < name.Length; i++) {
				if (!(
					(name[i] >= 'a' && name[i] <= 'z') ||
					(name[i] >= 'A' && name[i] <= 'Z') ||
					(name[i] >= '0' && name[i] <= '9') ||
					name[i] == '_'
					)) return false;
			}
			return true;
		}

		public ListBox StringCats => String_Categories;
		public string StringCat {
			get {
				if (StringCats.SelectedItem == null) return "";
				return StringCats.SelectedItem.ToString();
			}
		}

		private void CreateNewStringCategory(object sender, RoutedEventArgs e) {
			var SNC = String_NewCategory.Text.ToUpper();
			if (!ValidName(SNC)) {
				Confirm.Annoy($"\"{SNC}\" is not valid name for a string category. Only letters, numbers and underscores are allowed!", "Error", System.Windows.Forms.MessageBoxIcon.Error);
				return;
			}
			if (CurrentProject.Settings.List("Strings", "^Categories^").Contains(SNC)) {
				Confirm.Error($"The string catagory \"{SNC}\" already exists!");
				return;
			}
			CurrentProject.Settings.ListAdd("Strings", "^Categories^", SNC);
			CurrentProject.UpdateStrings();
			String_NewCategory.Text = "";

		}


		public static void RenewLanguages() {
			var SL = Me.CurrentProject.SupportedLanguages;
			if (Me.CurrentProject == null) return;
			Me.Strings_Lang1.Items.Clear();
			Me.Strings_Lang2.Items.Clear();
			foreach (var L in SL) {
				Me.Strings_Lang1.Items.Add(L);
				Me.Strings_Lang2.Items.Add(L);
			}
			var om = config_allowmodify;
			config_allowmodify = false;
			for (int i = 0; i < SL.Length; ++i) {
				if (SL[i] == Me.CurrentProject.Settings["Strings", "Lang1"]) Me.Strings_Lang1.SelectedIndex = i;
				if (SL[i] == Me.CurrentProject.Settings["Strings", "Lang2"]) Me.Strings_Lang2.SelectedIndex = i;
			}
			foreach (var RegS in RegisterStrings.Lijst) {
				RegS.Key.Items.Clear();
				foreach (var C in Me.CurrentProject.Settings.List("Strings", "^Categories^")) {
					foreach (var K in Me.CurrentProject.Settings.List("Strings", $"CAT_{C}")) {
						RegS.Key.Items.Add(K);
					}
				}
			}
			config_allowmodify = om;

		}

		private void StringCatagoryChanged(object sender, SelectionChangedEventArgs e) {			
			if (strings_allowmodify) {
				CurrentProject.UpdateStringsCats();
				AllowCheck();
			}
		}

		private void CreateNewString(object sender, RoutedEventArgs e) {
			var NewKey = String_NewString.Text.ToUpper().Trim();
			if (!ValidName(NewKey)) {
				Confirm.Annoy($"\"{NewKey}\" is not valid name for a string key. Only letters, numbers and underscores are allowed!", "Error", System.Windows.Forms.MessageBoxIcon.Error);
				return;
			}
			if (CurrentProject.Settings.List("Strings", $"CAT_{StringCat}").Contains(NewKey)) {
				Confirm.Error($"Key {NewKey} already exists in category {StringCat}!");
				return;
			}
			CurrentProject.Settings.ListAdd("Strings", $"CAT_{StringCat}", NewKey);
			CurrentProject.UpdateStringsCats();
			String_NewString.Text = "";
		}

		static public string Lang1 {
			get {
				if (Me.Strings_Lang1.SelectedItem == null) return "";
				return Me.Strings_Lang1.SelectedItem.ToString();
			}
		}

		static public string Lang2 {
			get {
				if (Me.Strings_Lang2.SelectedItem == null) return "";
				return Me.Strings_Lang2.SelectedItem.ToString();
			}
		}


		private void Strings_Lang1_SelectionChanged(object sender, SelectionChangedEventArgs e) { if (config_allowmodify) { if (Strings_Lang1.SelectedItem == null) CurrentProject.Settings["Strings", "Lang1"] = ""; else CurrentProject.Settings["Strings", "Lang1"] = Strings_Lang1.SelectedItem.ToString(); AllowCheck(); } }
		private void Strings_Lang2_SelectionChanged(object sender, SelectionChangedEventArgs e) { if (config_allowmodify) { if (Strings_Lang2.SelectedItem == null) CurrentProject.Settings["Strings", "Lang2"] = ""; else CurrentProject.Settings["Strings", "Lang2"] = Strings_Lang2.SelectedItem.ToString(); AllowCheck(); } }

		private void StringKeyChanged(object sender, SelectionChangedEventArgs e) {
			if (!strings_allowmodify) return;
			var KeyBox = (ComboBox)sender;
			AllowCheck();
			var KeyName = "";
			if (KeyBox.SelectedItem != null) KeyName = KeyBox.SelectedItem.ToString();
			var Rg = RegisterStrings.RegKey[KeyBox];
			CurrentProject.Settings["Strings", $"Key_{StringCat}_{Rg.Index}"] = KeyName;
		}

		private void StringsChanged(object sender, TextChangedEventArgs e) {
			if (!strings_allowmodify) return;
			var TB = (TextBox)sender;
			var RS = RegisterStrings.RegValue[TB];
			var Lang = "";
			if (RS.Value1 == TB) Lang = Strings_Lang1.SelectedItem.ToString();
			else if (RS.Value2==TB) Lang = Strings_Lang2.SelectedItem.ToString();
			else {
				Confirm.Error("Language could not be determine!\n\nPlease report", "Internal Error");
				Environment.Exit(255);
			}
			var LG = CurrentProject.GetStrings(Lang);
			LG[StringCat, RS.Key.SelectedItem.ToString()] = TB.Text;
		}

		private void SaveButton(object sender, RoutedEventArgs e) {
			if (CurrentProject == null) return;
			CurrentProject.SaveMe();
		}

		private void ScenCheck(object sender, RoutedEventArgs e) {
			var s = (CheckBox)sender;
			if (s.IsChecked == true) s.Content = "Yes"; else s.Content = "No";
			if (scenario_allowmodify) {
				if (CurrentProject == null) return;
				var CT = CurrentProject.Scenario.ChosenEntry.CurrentTag; if (CT == null) return;
				CT.CurrentPage.NameLinking = (bool)s.IsChecked;
				CT.CurrentPage.LinkUpdate();
			}
		}

		static public ListBox ScenarioEntries => Me.LB_Scenario_File;
		static public ListBox ScenarioTags => Me.LB_Scenario_Tag;
		static public string ScenarioEntryLabel {
			get => (string)Me.Scenario_ShD_LB_Entry.Content;
			set => Me.Scenario_ShD_LB_Entry.Content = value;
		}
		static public string ScenarioTagLabel {
			get => (string)Me.Scenario_ShD_LB_Tag.Content;
			set => Me.Scenario_ShD_LB_Tag.Content = value;
		}
		static public void ScenarioPage(int P,int PM) {
			Me.Scenario_ShD_LB_Page.Content = $"{P + 1}/{PM}";
		}

		static public string ScenarioPicDir {
			get => Me.Scenario_ShD_TB_PicDir.Text;
			set => Me.Scenario_ShD_TB_PicDir.Text = value;
		}


		public class CLangHead {
			public string this[int idx] {
				get {
					switch (idx) {
						case 1: return Me.Scenario_Head_Lang1.Text;
						case 2: return Me.Scenario_Head_Lang2.Text;
						default: throw new Exception($"Index for LangHead must be either 1 or 2, but not {idx}");
					}

				}
				set {
					switch (idx) {
						case 1: Me.Scenario_Head_Lang1.Text = value; return;
						case 2: Me.Scenario_Head_Lang2.Text = value; return;
						default: throw new Exception($"Index for LangHead must be either 1 or 2, but not {idx}");
					}
				}
			}
		}
		static readonly public CLangHead LangHead = new CLangHead();

		private void BT_Scenario_File_New_Click(object sender, RoutedEventArgs e) {
			var N = TB_Scenario_NewFile.Text;
			if (N == "") return;
			var NewFile = $"{CurrentProject.Scenario.Workspace}/{N}.ini";
			if (File.Exists(Dirry.AD(NewFile))) { Confirm.Error($"File {NewFile} already exists!"); return; }
			try {
				var NewFileDir = qstr.ExtractDir(Dirry.AD(NewFile));
				if (!Directory.Exists(NewFileDir)) {
					Debug.WriteLine($"Creating dir: {NewFileDir}");
					Directory.CreateDirectory(NewFileDir);
				}
				Debug.WriteLine($"Saving: {NewFile}");
				QuickStream.SaveString(Dirry.AD(NewFile), "# Alright, move along, there's nothing to see here!");
				CurrentProject.Scenario[N].Modified = true;
				TB_Scenario_NewFile.Text = "";
				CurrentProject.Scenario.UpdateGUI();
				AllowCheck();
			} catch (Exception exc) {
				Confirm.Error($"Creating '{NewFile}' failed!\n\n{exc.Message}\n");
			}

		}

		private void LB_Scenario_File_SelectionChanged(object sender, SelectionChangedEventArgs e) {
			if (CurrentProject==null) return;
			CurrentProject.Scenario.UpdateGUIEntry();
		}

		private void BT_Scenario_Tag_New_Click(object sender, RoutedEventArgs e) {
			if (CurrentProject == null) return;
			var CE = CurrentProject.Scenario.ChosenEntry;  if(CE == null) return;
			var NewTag = TB_Scenario_NewTag.Text.ToUpper().Trim();
			if (NewTag.Length == 0) { Confirm.Error("???", "Internal error"); return; }
			if (CE.TagExists(NewTag)) { Confirm.Error($"Duplicate tag: {NewTag}"); return; }
			if (!ValidName(NewTag)) { Confirm.Error($"\"{NewTag}\" is NOT a valid name for a scenario tag. Only letters numbers and underscores allowed!"); return; }
			CE.LTags.Add(NewTag);
			CE.LTags.Sort();
			CE.Modified = true;
			CurrentProject.Scenario.UpdateGUIEntry();
			CurrentProject.Scenario.SaveMe(true);
			TB_Scenario_NewTag.Text = "";
			AllowCheck();
		}

		private void Scenario_ShD_TB_PicDir_TextChanged(object sender, TextChangedEventArgs e) {
			if (scenario_allowmodify) {
				if (CurrentProject == null) return;
				var CT = CurrentProject.Scenario.ChosenEntry.CurrentTag; if (CT == null) return;
				CT.CurrentPage.PicDir = Scenario_ShD_TB_PicDir.Text;
				CT.CurrentPage.LinkUpdate(CScenario.CPage.ELU.PicDir);
			}
		}

		private void Scenario_ShD_TB_PicSpecific_TextChanged(object sender, TextChangedEventArgs e) {
			if (scenario_allowmodify) {
				if (CurrentProject == null) return;
				var CT = CurrentProject.Scenario.ChosenEntry.CurrentTag; if (CT == null) return;
				CT.CurrentPage.PicSpecific = Scenario_ShD_TB_PicSpecific.Text;                
			}
		}

		private void Scenario_ShD_TB_Audio_TextChanged(object sender, TextChangedEventArgs e) {
			if (scenario_allowmodify) {
				if (CurrentProject == null) return;
				var CT = CurrentProject.Scenario.ChosenEntry.CurrentTag; if (CT == null) return;
				CT.CurrentPage.Audio = Scenario_ShD_TB_Audio.Text;
			}
		}

		private void Scenario_ShD_TB_AltFont_TextChanged(object sender, TextChangedEventArgs e) {
			if (scenario_allowmodify) {
				if (CurrentProject == null) return;
				var CT = CurrentProject.Scenario.ChosenEntry.CurrentTag; if (CT == null) return;
				CT.CurrentPage.AltFont = Scenario_ShD_TB_AltFont.Text;
			}

		}

		private void LB_Scenario_Tag_SelectionChanged(object sender, SelectionChangedEventArgs e) {
			if (CurrentProject == null) return;
			CurrentProject.Scenario.UpdateGUITag();
		}

		private void Scenario_Shd_Page_Home_Click(object sender, RoutedEventArgs e) {
			CurrentProject.Scenario.ChosenEntry.CurrentTag.CurrentPageNumber = 0;
			CurrentProject.Scenario.UpdateGUITag();
		}

		private void Scenario_Shd_Page_Prev_Click(object sender, RoutedEventArgs e) {
			CurrentProject.Scenario.ChosenEntry.CurrentTag.CurrentPageNumber--;
			CurrentProject.Scenario.UpdateGUITag();
		}

		private void Scenario_Shd_Page_Next_Click(object sender, RoutedEventArgs e) {
			CurrentProject.Scenario.ChosenEntry.CurrentTag.CurrentPageNumber++;
			CurrentProject.Scenario.UpdateGUITag();
		}

		private void Scenario_Shd_Page_End_Click(object sender, RoutedEventArgs e) {
			CurrentProject.Scenario.ChosenEntry.CurrentTag.CurrentPageNumber = CurrentProject.Scenario.ChosenEntry.CurrentTag.PageCount - 1;
			CurrentProject.Scenario.UpdateGUITag();
		}

		#region Save every 5 minutes
		System.Windows.Threading.DispatcherTimer dispatcherTimer = null;
		private void InitAutoSave() {
			/*
			dispatcherTimer = new System.Windows.Threading.DispatcherTimer();
			dispatcherTimer.Tick += new EventHandler(AutoSave);
			dispatcherTimer.Interval = TimeSpan.FromMinutes(5);
			dispatcherTimer.Start();
			//*/
		}

		private void AutoSave(object whatever, EventArgs crap) => ProjectList.SaveAll();

		#endregion

		private void ScenSelectLang(ComboBox sender,byte idx) {
			if (CurrentProject == null) return;
			if (scenario_allowmodify) {
				var SI = sender.SelectedItem;
				if (SI != null) {
					var L = sender.SelectedItem.ToString();
					if (L != "") CurrentProject.Settings["::SCENARIO::", $"LANG{idx}"] = L;
				}
				CurrentProject.Scenario.UpdateGUITag();
			}
		}

		private void ScenSelectLang1(object sender, SelectionChangedEventArgs e) => ScenSelectLang((ComboBox)sender, 1);
		private void ScenSelectLang2(object sender, SelectionChangedEventArgs e) => ScenSelectLang((ComboBox)sender, 2);

		private void Scenario_Head(TextBox sender,int index) {
			if (CurrentProject == null) return;
			if (scenario_allowmodify) {
				var LNG = CurrentProject.Scenario.ChosenEntry.CurrentTag.CurrentPage.ChosenLang(index);
				if (LNG==null) return;
				LNG.Header = sender.Text;
				CurrentProject.Scenario.ChosenEntry.CurrentTag.CurrentPage.LinkUpdate((CScenario.CPage.ELU)index+1);
			}
		}

		private void Scenario_Head_Lang1_TextChanged(object sender, TextChangedEventArgs e) => Scenario_Head((TextBox)sender, 1);
		private void Scenario_Head_Lang2_TextChanged(object sender, TextChangedEventArgs e) => Scenario_Head((TextBox)sender, 2);

		private void Scenario_Content(TextBox sender,int index) {
			if (scenario_allowmodify) {
				if (CurrentProject == null) return;
				var LNG = CurrentProject.Scenario.ChosenEntry.CurrentTag.CurrentPage.ChosenLang(index);
				if (LNG == null) return;
				LNG.Content = sender.Text;
			}
		}

		private void Scenario_Content_Lang1_TextChanged(object sender, TextChangedEventArgs e) => Scenario_Content((TextBox)sender, 1);

		private void Scenario_Content_Lang2_TextChanged(object sender, TextChangedEventArgs e) => Scenario_Content((TextBox)sender, 2);

		public TextBox LangField(int i) {
			switch (i) {
				case 1:return Scenario_Content_Lang1;
				case 2: return Scenario_Content_Lang2;
				default: return null; 
			}
		}

		private void SelectExportMethod(object sender, SelectionChangedEventArgs e) {
			if (CurrentProject == null) return;
			Debug.WriteLine($"Set export to {((ComboBox)sender).SelectedItem}. Allow = {config_allowmodify}");
			if (config_allowmodify) {
				CurrentProject.Settings["EXPORT", "METHOD"] = ((ComboBox)sender).SelectedItem.ToString();
			}
		}

		public string ExportMethod {
			get {
				try {
					var i = CB_ExportMethod.SelectedItem;
					if (i == null) return "";
					return i.ToString();
				} catch {
					Debug.WriteLine("An error occurred while trying to get the export method from GUI. Getting it from the Project in stead");
					if (CurrentProject == null) return "";
					return CurrentProject.Settings["EXPORT", "METHOD"];
				}
			}
			set {
				var si = -1;
				for (int i = 0; i < CB_ExportMethod.Items.Count; ++i) {
					var item = CB_ExportMethod.Items[i];
					if (item.ToString() == value) { si = i; break; }
				}
				if (si >= 0) CB_ExportMethod.SelectedIndex = si;
			}
		}
	}
}