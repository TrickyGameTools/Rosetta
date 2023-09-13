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

        static internal string GlobalConfigFile => Dirry.AD("$AppSupport$/Rosetta.Global.ini");

        static internal readonly GINIE _Data = GINIE.FromFile(GlobalConfigFile);
        static internal SortedDictionary<string, PrjD> PRJ = new SortedDictionary<string, PrjD>();

        static internal void ToListBox(ListBox LB) {
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
            ToListBox(LB);
        }



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
