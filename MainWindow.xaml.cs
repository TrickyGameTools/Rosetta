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

namespace Rosetta {
    using Class;
    using System.Windows.Forms;
    using TrickyUnits;

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {
        public MainWindow() {
            InitializeComponent();

            ProjectList.ToListBox(LB_Projects);
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
    }
}
