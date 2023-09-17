using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrickyUnits;

namespace Rosetta.Class {
	internal class CScenario {

		internal class CEntry {
			private GINIE _Data = null;
			internal readonly CScenario Parent = null;
			internal string EntryName = "";
			internal ProjectData Project => Parent.Parent;
			internal SortedDictionary<string,CTag> Tags = new SortedDictionary<string,CTag>();

			internal string EntryFile => Dirry.AD($"{Project.Settings["DIRECTORIES", "SCENARIO"]}/{EntryName}.ini");

			internal GINIE Data {
				get {
					if (_Data == null) {
						Debug.WriteLine($"Loading: {EntryFile}");
						_Data = GINIE.FromFile(EntryFile);
						_Data.NewValue("::SYS::","Created",DateTime.Now.ToString());
						_Data.NewValue("::SYS::", "Tool", "Rosetta");
						_Data.NewValue("::SYS::", "Entry", EntryName);
					}
					return _Data;
				}
			}
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
			internal CEntry(CScenario Parent,string EN) {
				this.Parent = Parent;
				EntryName = EN;
				Parent.Entries[EN] = this;
			}
		}

		internal class CTag {
			readonly CEntry Parent = null;
			readonly string Tag = "";
			internal CTag (CEntry _Parent,string _Tag) {
				Parent = _Parent;
				Tag = _Tag;
				Parent.Tags[Tag] = this;
			}

			internal ProjectData Project => Parent.Parent.Parent;

		}

		

		readonly ProjectData Parent = null;
		readonly SortedDictionary<string,CEntry> Entries = new SortedDictionary<string,CEntry>();

		public CScenario(ProjectData Parent) { this.Parent = Parent; }
	}
}
