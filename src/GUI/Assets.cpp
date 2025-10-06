// License:
// 
// Rosetta
// Assets
// 
// 
// 
// 	(c) Jeroen P. Broks, 2025
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
// Version: 25.10.06
// End License
#pragma once
#include <Slyvina.hpp>
#include <SlyvQCol.hpp>
#include <JCR6_Core.hpp>
#include <SlyvGINIE.hpp>

#include "../Rosetta.hpp"

#define ef else if
namespace Slyvina {
	namespace Rosetta{
		namespace GUI {
			String AssetsFile() {
				static String _AF{""};
				if (_AF=="") {
					std::vector<String> FilesToTry{};
#ifdef SlyvLinux
					FilesToTry.push_back(Exefile);
					FilesToTry.push_back(Exefile+".jcr");
					FilesToTry.push_back("/proc/self/exe");
#endif
#ifdef SlyvWindows
					auto _ExeFile{StripExt(ExeFile)};
					FilesToTry.push_back(_ExeFile);
					FilesToTry.push_back(_ExeFile+".jcr");
#endif // SlyvWindows
					for (auto FTT:FilesToTry) {
						auto REC{JCR6::JCR6_Recognize(FTT)};
						//QCol->Doing("DEBUG",FTT,"\t"+REC+"\n");
						if (REC!="NONE") {
							QCol->Doing("Recognized",FTT,"\t");
							QCol->LMagenta(REC);
							QCol->White("\n");
							_AF=FTT;
							break;
						}
					}
				}
				if (_AF=="") { QCol->Error("No assets files recognized"); exit(1); }
				return _AF;
			}

			JCR6::JT_Dir Assets() {
				static JCR6::JT_Dir _AJ{nullptr};
				if (!_AJ){
					QCol->Doing("Analyzing",AssetsFile());
					_AJ=JCR6::JCR6_Dir(AssetsFile());
					if (JCR6::Last()->ErrorMessage!="") {
						QCol->Error("Loading Assets File Failed!");
						QCol->Doing("Error",JCR6::Last()->ErrorMessage);
						QCol->Doing("Main",JCR6::Last()->MainFile);
						QCol->Doing("Entry",JCR6::Last()->Entry);
						exit(2);
					}
					if (!_AJ) {QCol->Error("Loading Assets  File Failed for Unknown Reasons"); exit(3); }
				}
				return _AJ;
			}


			void IdentifyAssets(bool crash) {
				QCol->Doing("Identifying",AssetsFile());
				auto IDS{Assets()->GetString("ID/ID.ini")};
				auto IDD{Units::ParseGINIE(IDS)};
				if ((IDD->Value("data","app")!="Rosetta") || (IDD->Value("data","fw")!="Slyvina")) {
					QCol->Error("Assets file not identified as for Rosetta!");
					QCol->Doing("- app",IDD->Value("data","app"));
					QCol->Doing("- fw",IDD->Value("data","fw"));
					if (crash) exit(4);
				}
			}
		}
	}
}

