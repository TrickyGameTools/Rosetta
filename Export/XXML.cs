// Lic:
// Rosetta
// Export to XML
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

using Rosetta.Class;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrickyUnits;

namespace Rosetta.Export {
	internal class XXML:XBase {

		override internal void Export(ProjectData D, string language) {
			var Entries = D.Scenario.AllEntries;
			foreach(var E in Entries) {
				Debug.WriteLine($"Exporting entry \"{E}\" to the {language} language");
				var Entry = D.Scenario[E];
				var Tags = Entry.LTags;
				var Out = new StringBuilder("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n\n");
				Out.Append($"<!-- Entry {E}; Language {language}; Generated {DateTime.Now} by Rosetta -->\n\n");
				foreach(var T in Tags) {
					Debug.WriteLine($"= Tag {T}");
					var Tag = D.Scenario[E, T];
					Out.Append($"<Tag name=\"{T}\">\n");
					for(var P=0;P<Tag.PageCount;P++) {
						var Page = D.Scenario[E, T, P];
						var LDat = D.Scenario[E, T, P, language];
						Out.Append($"<Page PicDir=\"{Page.PicDir}\" PicSpecific=\"{Page.PicSpecific}\" AltFont=\"{Page.AltFont}\" Audio=\"{Page.Audio}\" Head=\"{LDat.Header}\">{LDat.Content}</Page>\n\n");
					}
					Out.Append("</Tag>\n");
				}
				QuickStream.SaveString($"{D.ExportDir}/{language}/{E}.xml",Out,true);
			}			
		}

		internal XXML() => Reg("XML", this);
	}
}