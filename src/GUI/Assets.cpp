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
// Version: 25.10.08 I
// End License
#pragma once
#include <Slyvina.hpp>
#include <SlyvQCol.hpp>
#include <JCR6_Core.hpp>
#include <SlyvGINIE.hpp>
#include <SlyvString.hpp>
#include <TQSG.hpp>
#include <TQSE.hpp>
#include <June19.hpp>

#include "../Rosetta.hpp"


using namespace Slyvina::TQSG;
using namespace Slyvina::TQSE;

#define ef else if
namespace Slyvina {
	namespace Rosetta{
		namespace GUI {

			static void Paniek(String A) {
				Notify("ERROR\n"+A);
				June19::FreeJune19();
				Crash(A);
			}

			static void PaniekJCR(String A) {
				auto L{JCR6::Last()};
				Paniek(A+"\nJCR6: "+L->ErrorMessage+"\nEntry: "+L->Entry+"\nMain:  "+L->MainFile);
			}

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


			void PicGadget(June19::j19gadget* g,String PicFile) {
				QCol->Doing("Loading image",PicFile);
				auto img=LoadImage(Assets(),PicFile);
				if (!img) PaniekJCR("Error loading: "+PicFile);
				if (!img->Frames()) Paniek("Frameless image: "+PicFile);
				g->Image(img);
			}


			static TImageFont GetFont(String FntDir) {
				static std::map<String,TImageFont> Reg{};
				auto UFntDir{Upper(FntDir)};
				if (!Reg.count(UFntDir)) {
					QCol->Doing("Loading font",FntDir);
					auto R{LoadImageFont(Assets(),FntDir)  };
					if (!R) PaniekJCR("Error linking to font: "+FntDir);
					Reg[UFntDir]=R;
				}
				return Reg[UFntDir];
			}
			void FntGadget(June19::j19gadget* g,String FntDir) { g->SetFont(GetFont(FntDir)); }
			void FntDefault(String FntDir) { June19::j19gadget::SetDefaultFont(GetFont(FntDir)); }
		}
	}
}

