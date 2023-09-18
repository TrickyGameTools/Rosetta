// Lic:
// Rosetta
// Base Class Export
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
// Version: 23.09.18
// EndLic
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

		internal static void Reg(string key, XBase value) { Register[key]= value; }

		static public void Init() {
			new XLua();
			new XScenLang();
			new XXML();
		}
	}
}