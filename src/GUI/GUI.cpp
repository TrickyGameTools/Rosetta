// License:
// 
// Rosetta
// Graphics User Interface
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

#include "../Rosetta.hpp"
#include <SlyvQCol.hpp>
#include <SlyvDir.hpp>
#include <JCR6_zlib.hpp>
#include <June19.hpp>
#include <TQSE.hpp>
#include <TQSG.hpp>

using namespace Slyvina::Units;
using namespace Slyvina::June19;


#define MakeCol1(fun,rf,gf,bf) \
	static void fun(j19gadget*g,j19action) {\
		g->FR=rf==g->FR?rf:g->FG>rf?std::max(--g->FR,(Byte)rf):std::min((Byte)rf,++g->FR);\
		g->FG=gf==g->FR?gf:g->FG>gf?std::max(--g->FG,(Byte)gf):std::min((Byte)gf,++g->FG);\
		g->FB=bf==g->FR?bf:g->FB>bf?std::max(--g->FB,(Byte)bf):std::min((Byte)bf,++g->FB);\
	}
#define MakeCol2(fun,fr,fg,fb) \
	static void fun(j19gadget*g,j19action) {\
		byte rb{fr/10},gb{fg/10},bb{fb/10};\
		g->FR=fr==g->FR?fr:g->FR>fr?std::max(--g->FR,(Byte)fr):std::min((Byte)fr,++g->FR);\
		g->FG=fg==g->FG?fg:g->FG>fg?std::max(--g->FG,(Byte)fg):std::min((Byte)fg,++g->FG);\
		g->FB=fb==g->FB?fb:g->FB>fb?std::max(--g->FB,(Byte)fb):std::min((Byte)fb,++g->FB);\
		g->BR=rb==g->BR?rb:g->BR>rb?std::max(--g->BR,rb):std::min(rb,++g->BR);\
		g->BG=gb==g->BG?gb:g->BG>gb?std::max(--g->BG,gb):std::min(gb,++g->BG);\
		g->BB=bb==g->BB?bb:g->BB>bb?std::max(--g->BB,bb):std::min(bb,++g->BB);\
	}

namespace Slyvina {
	namespace Rosetta {
		namespace GUI {

			String Exefile{};
			VecString Args{NewVecString()};

			static void MakeSkyBlue(j19gadget*g,j19action) {
				g->FR=g->FR==0?0:std::max(--g->FR,(Byte)0);
				g->FG=g->FG>180?std::max(--g->FG,(Byte)180):std::min((Byte)180,++g->FG);
				g->FB=g->FB==255?255:std::min((Byte)255,++g->FB);
			}
			MakeCol1(MakeAmber,255,180,0);
			MakeCol2(BoxPurple,180,0,255);

			static j19gadget
				* Back{nullptr},
				* ProjectsLabel{nullptr},
				* StringsOrScenario{nullptr},
				* PanStrings{nullptr},
				* ButStrings{nullptr},
				* ButScenario{nullptr},
				* PanScenario{nullptr};
			j19gadget
				* ProjectsList{nullptr},
				* MainWindow{nullptr};

			void RenewProjects(){
				uint32 c{0};
				ProjectsList->ClearItems();
				for(auto&d:*Args) {
					if(IsFile(d)) {
						ProjectsList->AddItem(StripDir(d)+"  :"+d);
						c++;
					} else if (IsDir(d)) {
						auto dt{GetTree(d)};
						for(auto f:*dt) if (Lower(ExtractExt(f))=="ini") {ProjectsList->AddItem(StripDir(f)+"  :"+d+"/"+f); c++;}
					} else {
						String err{"\x07No file nor directory called \""+d+"\" was found."};
						QCol->Error(err);
						TQSE::Notify("Error:\n"+err);
					}
				}
				QCol->Doing("Projects",c);
				              //        1
				if (!c) {    //123456789012345678
					const char wrn[20]{"No projects found!"};
					QCol->Warn(wrn);
					TQSE::Notify((String)"Warning:\n"+wrn);
				}
			}

			void RenewProjects(j19gadget*,j19action) { RenewProjects(); }

			static void PJ_Draw(j19gadget*g,j19action a) {
				BoxPurple(g,a);
				StringsOrScenario->Visible=(g->SelectedItem()>=0);
			}
			static void DrawButtonStrings(j19gadget*g,j19action a) { MakeAmber(g,a); PanStrings->Visible=g->checked; }
			static void DrawButtonScenario(j19gadget*g,j19action a) { MakeAmber(g,a); PanScenario->Visible=g->checked; }

			void Init(int argc,char** args) {
				QCol->White("Rosetta\n\n");
				QCol->Doing("Coded by","Jeroen P. Broks");
				QCol->Doing("Platform",Platform());
				QCol->Doing("Run file",args[0]); Exefile=args[0];
				for(int i=1;i<argc;i++) { Args->push_back(args[i]);} if (!Args->size()) Args->push_back(CurrentDir());
				JCR6::init_zlib();
				IdentifyAssets();
				TQSG::Graphics(SW,SH,"Rosetta - (c) Jeroen P. Broks");
				MainWindow=WorkScreen();
				FntDefault("Fonts/DOSFont.jfbf");
				Back = CreatePicture(0,0,SW,SH,WorkScreen(),Pic_FullStretch);
				PicGadget(Back,"Alg/Background.jpg");
				ProjectsList = CreateListBox(2,25,300,250,Back);
				ProjectsList->CBDraw=PJ_Draw;
				RenewProjects();
				ProjectsLabel = CreateLabel("Projects:",0,0,100,30,Back);
				FntGadget(ProjectsLabel,"Fonts/Ryanna.jfbf");
				ProjectsLabel->CBDraw = MakeSkyBlue;
				StringsOrScenario = CreatePanel(0,0,SW,SH,Back);
				StringsOrScenario->BA=0;
				PanStrings = CreatePanel(0,0,SW,SH,Back);
				PanStrings->Visible=false;
				PanStrings->BA=0;
				PanScenario = CreatePanel(0,0,SW,SH,Back);
				PanScenario->Visible=false;
				PanScenario->BA=0;
				ButStrings = CreateRadioButton("Strings",ProjectsList->W()+25,0,0,0,StringsOrScenario,true);
				ButStrings->CBDraw = DrawButtonStrings;
				FntGadget(ButStrings,"Fonts/Ryanna.jfbf");
				ButScenario = CreateRadioButton("Scenario",ProjectsList->W()+250,0,0,0,StringsOrScenario,false);
				FntGadget(ButScenario,"Fonts/Ryanna.jfbf");
				ButStrings->CBDraw = DrawButtonScenario;
			}

			void Run() {
				bool Going{true};
				do {
					TQSE::Poll(); if (TQSE::AppTerminate()) { QCol->Magenta("Close request done by user!\n"); return; }
					WorkScreen()->Draw();
					TQSG::Flip();
				} while(Going);
			}

			void Done() {
				QCol->Doing("Releasing","June19"); FreeJune19();
			}
		}
	}
}
