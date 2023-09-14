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

//using System.Windows.Forms;
using TrickyUnits;

namespace Rosetta {
	using Class;
	using System.Diagnostics;

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window {

		static public bool strings_allowmodify = true;
		static public bool config_allowmodify = true;

		ProjectData CurrentProject =>ProjectData.CurrentProject;

		public MainWindow() {
			InitializeComponent();

			ProjectList.ToListBox(LB_Projects);
			AllowCheck();
		}

		public Visibility Vis(bool K) { if (K) return Visibility.Visible; else return Visibility.Hidden; }

		public void AllowCheck() {
			Debug.WriteLine("Allow Check Requested");
			MainTabber.Visibility = Vis(CurrentProject != null);
		}

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
			var item=LB_Projects.SelectedItem;
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
	}
}