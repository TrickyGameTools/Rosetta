using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Rosetta.Class;

namespace Rosetta.Export {
	abstract internal class XBase {
		abstract internal void Export(ProjectData D, string language);

		readonly static internal SortedDictionary<string,XBase> Register = new SortedDictionary<string,XBase>();
	}
}
