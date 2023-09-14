// Lic:
// Rosetta
// Project List
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
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Controls;
using TrickyUnits;

namespace Rosetta.Class {
    static internal class ProjectList {

        internal class PrjD {
            readonly string Tag;
            internal string FTag => $"Project::{Tag}";
            internal string ProjectFile {
                get => _Data[FTag, "File"];
                set => _Data[FTag, "File"] = value.Replace('\\', '/');
            }

            internal string ShowName {
                get {
                    if (_Data[FTag, "ShowName"] == "") return Tag;
                    return _Data[FTag, "ShowName"];
                }
            }

            internal PrjD(string _Tag) => Tag = _Tag;

        }

        internal class GetProject {
            private SortedDictionary<string, ProjectData> Reg = new SortedDictionary<string, ProjectData>();
            internal ProjectData this[string key] {
                get {
                    if (Reg.ContainsKey(key)) { return Reg[key]; }
                    Reg[key] = new ProjectData(PRJ[key].ProjectFile);
                    return Reg[key];
                }

            }
        }

        internal static GetProject Project=new GetProject();
        internal static ProjectData CurrentProject => ProjectData.CurrentProject;

        static internal string GlobalConfigFile => Dirry.AD("$AppSupport$/Rosetta.Global.ini");

        static internal readonly GINIE _Data = GINIE.FromFile(GlobalConfigFile);
        static internal SortedDictionary<string, PrjD> PRJ = new SortedDictionary<string, PrjD>();

        static internal void ToListBox(ListBox LB) {
            ProjectData.CurrentProject = null;
            LB.Items.Clear();
            foreach (var K in PRJ.Keys) LB.Items.Add(K);
        }

        static internal void Create(ListBox LB,string FileName) {
            var Create = true;
            var SFN = qstr.StripDir(FileName);
            if (File.Exists(FileName)) {
                switch(Confirm.YNC($"A file named {FileName} already exists!\nUse it?")) {
                    case -1: return;
                    case 1: Create = false; break;
                    case 0: break;
                    default: throw new Exception("Internal error in project creation!");
                }
            }
            if (Create) QuickStream.SaveString(FileName,$"[Creation]\nCreatorApp=Rosetta\nDate={DateTime.Now}\n");
            _Data.ListAdd("Projects", "Projects", SFN);
            PRJ[SFN] = new PrjD(SFN);
            PRJ[SFN].ProjectFile = FileName;
            ToListBox(LB);
        }

        static internal void Select(ProjectData Prj) {
            ProjectData.CurrentProject = Prj;
            MainWindow.ProjectTB = Prj.ProjectFile;
            CurrentProject.RenewSettings();
        }

        static internal void Select(string Prj) => Select(Project[Prj]);
        


        static ProjectList() {
            _Data.AutoSaveSource = GlobalConfigFile;
            PRJ.Clear();
            foreach (var P in _Data.List("Projects", "Projects")) {
                if (PRJ.ContainsKey(P)) {
                    string NP = "";
                    uint c = 0;
                    do { NP = $"{P} ({c++}"; } while (PRJ.ContainsKey(NP));
                    PRJ[NP] = new PrjD(NP);                    
                } else {
                    PRJ[P] = new PrjD(P);
                }
            }
        }
    }
}