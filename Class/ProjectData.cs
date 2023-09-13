using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrickyUnits;

namespace Rosetta.Class {
    internal class ProjectData {
        GINIE Settings = null;

        internal ProjectData(string FileName) { 
            Settings=GINIE.FromFile(FileName);
            Settings.AutoSaveSource = FileName;
        }
    }
}
