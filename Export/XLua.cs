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
