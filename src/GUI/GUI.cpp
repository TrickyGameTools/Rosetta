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
// Version: 25.10.08 II
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

			static double hue{0};

			bool AllowModify = true;
			String Exefile{};
			VecString Args{NewVecString()};

			static void MakeSkyBlue(j19gadget*g,j19action) {
				g->FR=g->FR==0?0:std::max(--g->FR,(Byte)0);
				g->FG=g->FG>180?std::max(--g->FG,(Byte)180):std::min((Byte)180,++g->FG);
				g->FB=g->FB==255?255:std::min((Byte)255,++g->FB);
			}
			MakeCol1(MakeAmber,255,180,0);
			MakeCol2(BoxPurple,180,0,255);

			static void HSV(j19gadget*g,j19action) {
				g->SetForegroundHSV(hue,1,1.0);
				g->SetBackgroundHSV(hue,1,0.1);
			}

			static j19gadget
				* Back{nullptr},
				* ProjectsLabel{nullptr},
				* StringsOrScenario{nullptr},
				* PanStrings{nullptr},
				* ButStrings{nullptr},
				* ButScenario{nullptr},
				* PanScenario{nullptr},
				* Myrah{nullptr},
				* LabCategories{nullptr};
			j19gadget
				* ListCategories{nullptr},
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
				hue+=0.01; if (hue>360) hue-=360;
				BoxPurple(g,a);
				StringsOrScenario->Visible=(g->SelectedItem()>=0);
				//std::cout<<"Panel Strings Visible: "<<boolstring(PanStrings->Visible)<<" <- "<<boolstring(ButStrings->checked)<<"\n";
			}
			static void DrawButtonStrings(j19gadget*g,j19action a) {
				MakeAmber(g,a); PanStrings->Visible=ButStrings->checked;
				//std::cout<<"DBS> Panel Strings Visible: "<<boolstring(PanStrings->Visible)<<" <- "<<boolstring(ButStrings->checked)<<"\n";
			}
			static void DrawButtonScenario(j19gadget*g,j19action a) { MakeAmber(g,a); PanScenario->Visible=ButScenario->checked; }


			String SelectedProject() {
				if (ProjectsList->SelectedItem()<0) return "";
				auto SI{ProjectsList->ItemText()};
				auto SP{IndexOf(SI,':')};
				if (SP<0) {return "";}
				return SI.substr(SP+1);
			}

			static void PJ_Select(j19gadget*g,j19action a) {
				using namespace Slyvina::Rosetta::Class;
				if (ProjectsList->SelectedItem()<0) return;
				QCol->Doing("Userselect",SelectedProject());
				_ProjectData::CurrentProject = Project[SelectedProject()];
				_ProjectData::CurrentProject->UpdateStrings();
			}

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
				ProjectsList->CBAction=PJ_Select;
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
				ButScenario->CBDraw = DrawButtonScenario;
				FntGadget(ButScenario,"Fonts/Ryanna.jfbf");
				Myrah = CreatePicture(SW-120,2,120,118,PanStrings,Pic_FullStretch); //std::cout << "Myrah: ("<<Myrah->X()<<","<<Myrah->Y()<<")("<<Myrah->DrawX()<<","<<Myrah->DrawY()<<") Visible:"<<boolstring(Myrah->Visible)<<" Screen: "<<SW<<"x"<<SH<<std::endl;
				Mascot(Myrah,"Myrah");
				LabCategories = CreateLabel("Categories:",ProjectsList->W()+25,35,100,30,PanStrings);
				FntGadget(LabCategories,"Fonts/Ryanna.jfbf");
				LabCategories->CBDraw=MakeSkyBlue;
				ListCategories = CreateListBox(LabCategories->DrawX(),LabCategories->DrawY()+30,400,(ProjectsList->Y()+ProjectsList->H())-(LabCategories->DrawY()+30),PanStrings);
				ListCategories->CBDraw=HSV;
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
