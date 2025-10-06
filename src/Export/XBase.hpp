// License:
// 
// Rosetta
// Export Basis (header)
// 
// 
// 
// 	(c) Jeroen P. Broks, 2023, 2025
// 
// 		This program is free software: you can redistribute it and/or modify
// 		it under the terms of the GNU General Public License as published by
// 		the Free Software Foundation, either version 3 of the License, or
// 		(at your option) any later version.
// 
// 		This program is distributed in the hope that it will be useful,
// 		but WITHOUT ANY WARRANTY; without even the implied warranty of
// 		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// 		GNU General Public License for more details.
// 		You should have received a copy of the GNU General Public License
// 		along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// 	Please note that some references to data like pictures or audio, do not automatically
// 	fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 25.10.06 II
// End License

#pragma once
#include <Slyvina.hpp>

namespace Slyvina {
	namespace Rosetta {
		namespace Export {

			typedef void(*ExportDelegate)(Class::_ProjectData*,String);

			// Just translated from C#
			//abstract internal
			class XBase {
				public:
				//abstract internal void Export(ProjectData D, string language);
				ExportDelegate Export;

				//readonly static internal SortedDictionary<string,XBase> Register = new SortedDictionary<string,XBase>();
				static std::map<String,XBase> Register;

				//static void Reg(String key, XBase value) { Register[key]= value; }
				static void Reg(String Key,ExportDelegate D);

				static void Init() {
					// True code comes later
					//new XLua();
					//new XScenLang();
					//new XXML();
				}
			};
		}
	}
}
