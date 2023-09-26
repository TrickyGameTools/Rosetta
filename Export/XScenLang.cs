// Lic:
// Rosetta
// ScenLang Export
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
using System.Text;
using TrickyUnits;

namespace Rosetta.Export {
	internal class XScenLang:XBase {
		internal override void Export(ProjectData D, string language) {
			var Head = $"[rem]\nFile Generated By Rosetta\n(c) Copyright {DateTime.Now.Year} Jeroen P. Broks\nThese files and their content are property of Jeroen P. Broks and may NOT be removed without prior WRITTEN permission by the owner\n\n";
			var Entries = D.Scenario.AllEntries;
			if (Entries != null) {
				foreach (var E in Entries) {
					var Out = new StringBuilder(Head);
					var Entry = D.Scenario[E];
					var Tags = Entry.LTags;
					Out.Append("[tags]\n");
					foreach (var T in Tags) Out.Append($"{T}\n");
					Out.Append("\n\n\n[scenario]\n\n");
					foreach (var T in Tags) {
						Out.Append($"-- Tag: {T}\n");
						var Tag = D.Scenario[E, T];
						for (var pn = 0; pn < Tag.PageCount; pn++) {
							var Page = D.Scenario[E, T, pn];
							var LPage = D.Scenario[E, T, pn, language];
							Out.Append($"@{T}\n");
							Out.Append($"!{Page.PicDir}\n");
							Out.Append($"*{LPage.Header}\n");
							Out.Append($":{Page.PicSpecific}\n");
							Out.Append($"%{Page.AltFont}\n");
							Out.Append($"${Page.Audio}\n");
							var C = LPage.LContent;
							foreach (var CL in C) Out.Append($"#{CL}\n");
							Out.Append($"\n");
						}
						Out.Append("\n");
					}
					QuickStream.SaveString($"{D.ExportDir}/{language}/{E}", Out, true);
				}
			}
		}

		internal XScenLang() => Reg("ScenLang", this);
	}
}