// License:
// 
// Rosetta
// Project Data
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
// Version: 25.10.08
// End License

#pragma once
#include "../Rosetta.hpp"

#include <SlyvDirry.hpp>
#include <SlyvGINIE.hpp>

namespace Slyvina {
	namespace Rosetta {
	namespace Class {

	class TGetProject{ public: ProjectData operator[](String p); };
	extern TGetProject Project;


	class _ProjectData {
	public:

		String ProjectFile;
		static ProjectData CurrentProject;

		GINIE Settings { nullptr };
		std::map<String,GINIE> Strings{};
		CScenario Scenario = nullptr;

		//internal string[] SupportedLanguages {
		//	get {
		//		var ret = Settings["Support", "Languages"].Split(',');
		//		for(uint i=0; i<ret.Length; i++) ret[i] = ret[i].Trim();
		//		return ret;
		//	}
		//}
		inline VecString SupportedLanguages() { return Split(Settings->Value("Support","Languages"),','); }

		_ProjectData(String FileName); /* {
			Settings = GINIE.FromFile(FileName);
			ProjectFile = FileName;
			Settings.AutoSaveSource = FileName;
			Settings.NewValue("Support", "Languages", "English, Dutch");
			Settings.NewValue("Support", "Language_Def", "English"); // Used when no suitable translation is found.
			Scenario = new CScenario(this);
		} */

		Units::GINIE GetStrings(String Language);

		void UpdateStrings();
		void UpdateStringsCats();


		~_ProjectData();

		inline String StringsDir() { return Units::Dirry(Settings->Value("Directories::"+Platform(), "Strings") ); }
		inline String ExportDir() { return Units::Dirry(Settings->Value("Export::"+Platform(), "Scenario")); }

		void SaveMe(bool dontexport=false);

		void RenewSettings();
	};
}
}
}
