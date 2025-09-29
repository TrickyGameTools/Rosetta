// Lic:
// Rosetta
// Lua Export
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
// Version: 23.09.22
// EndLic
using Rosetta.Class;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrickyUnits;

namespace Rosetta.Export {
	internal class XLua:XBase {

		string GenCode(ProjectData D, string _entry, string language) {
			var Out = new StringBuilder($"-- Generated {DateTime.Now} by Rosetta --\n\nreturn {'{'}\n");
			var Tags = D.Scenario[_entry].LTags;
			var Komma = false;
			foreach( var _tag in Tags ) {
				if (Komma) Out.Append(",\n"); Komma = true;
				var Tag = D.Scenario[_entry, _tag];
				Out.Append($"\t[\"{_tag}\"] = {'{'}\n");
				for (int _page = 0; _page < Tag.PageCount; _page++) {
					if (_page > 0) Out.Append(",\n");
					var Page = D.Scenario[_entry, _tag, _page];
					var Lang = D.Scenario[_entry, _tag, _page, language];
					Out.Append("\t\t{\n");
					Out.Append($"\t\t\tPicDir = \"{Page.PicDir}\",\n");
					Out.Append($"\t\t\tPicSpecific = \"{Page.PicSpecific}\",\n");
					Out.Append($"\t\t\tAltFont = \"{Page.AltFont}\",\n");
					Out.Append($"\t\t\tAudio = \"{Page.PicDir}\",\n");
					Out.Append($"\t\t\tHead = \"{Lang.Header}\",\n");
					Out.Append("\t\t\tContent = {\n");
					var Cont = Lang.LContent;
					var Comma = false;
					foreach (var LC in Cont) {
						if (Comma) Out.Append(",\n"); Comma = true;
						Out.Append($"\t\t\t\t\"{LC}\"");
					}
					Out.Append("\n\t\t\t}\n"); // Content
					Out.Append("\t\t}"); // Page
				}
				Out.Append("\n\t}"); // Tag
			}
			Out.Append("\n}"); // End
			return Out.ToString();
		}
		internal override void Export(ProjectData D, string language) {
			var Entries = D.Scenario.AllEntries;
			foreach (var E in Entries) {
				QuickStream.SaveString($"{D.ExportDir}/{language}/{E}.lua", GenCode(D, E, language));
			}
		}

		internal XLua() => Reg("Lua", this);
	}
}