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
// Version: 23.09.14
// EndLic
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Diagnostics;
using Rosetta.Class;

//using System.Windows.Forms;
using TrickyUnits;

namespace Rosetta {

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {

		static public MainWindow Me { get; private set; } = null;

		static public bool strings_allowmodify = true;
		static public bool config_allowmodify = true;


		ProjectData CurrentProject => ProjectData.CurrentProject;

		public static string ProjectTB {
			get => Me.TB_Project.Text;
			set => Me.TB_Project.Text = value;
		}

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
		}

		public Visibility Vis(bool K) { if (K) return Visibility.Visible; else return Visibility.Hidden; }

		public void AllowCheck() {
			Debug.WriteLine("Allow Check Requested");
			MainTabber.Visibility = Vis(CurrentProject != null);
			TabStrings.IsEnabled = CurrentProject != null && CurrentProject.Settings["Directories", "Strings"] != "";
			TabScenario.IsEnabled = CurrentProject != null && CurrentProject.Settings["Directories", "Scenario"] != "";
			Pick_DirScenarioExport.IsEnabled = TabScenario.IsEnabled;
			TB_DirScenarioExport.IsEnabled = TabScenario.IsEnabled;
			String_NewCategoryActivate.IsEnabled = String_NewCategory.Text != "";
			Strings_Grid.Visibility = Vis(StringCat != "");
			String_NewStringButton.IsEnabled = String_NewString.Text != "";


			foreach (var RS in RegisterStrings.Lijst) {
				var HasKey = RS.Key.SelectedItem != null;
				var Lang1 = CurrentProject.Settings["Strings", "Lang1"];
				var Lang2 = CurrentProject.Settings["Strings", "Lang2"];
				RS.Value1.IsEnabled = HasKey && Lang1 != "" && Lang1 != Lang2;
				RS.Value2.IsEnabled = HasKey && Lang2 != "" && Lang1 != Lang2;
			}
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
				foreach (var C in Me.CurrentProject.Settings.List("Strings", "^Catagories^")) {
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



		private void Strings_Lang1_SelectionChanged(object sender, SelectionChangedEventArgs e) { if (config_allowmodify) { CurrentProject.Settings["Strings", "Lang1"] = Strings_Lang1.SelectedItem.ToString(); AllowCheck(); } }
		private void Strings_Lang2_SelectionChanged(object sender, SelectionChangedEventArgs e) { if (config_allowmodify) CurrentProject.Settings["Strings", "Lang2"] = Strings_Lang2.SelectedItem.ToString(); AllowCheck(); }

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

		}


	}
}