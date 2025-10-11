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
// Version: 25.10.11
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
			MakeCol2(BoxLightGreen,180,255,0);
			MakeCol2(BoxDark,100,100,100);
			MakeCol2(BoxMint,0,180,255);
			MakeCol2(BoxMagenta2,255,0,180);

			static void HSV(j19gadget*g,j19action) {
				g->SetForegroundHSV(hue,1,1.0);
				g->SetBackgroundHSV(hue,1,0.1);
			}

			static int CatItems{0};
			static const int
				CatItemHeight{20},
				CatLangs{2};

			struct TStringsLine {
				j19gadget
					* Group{nullptr},
					* Label{nullptr},
					* Box[CatLangs]{nullptr,nullptr};
				uint32 index{0};
			};
			std::map<uint32,TStringsLine> StringLine;
			struct TStrHead {
				j19gadget
					* Group{nullptr},
					* Label{nullptr},
					* Back{nullptr},
					* Forward{nullptr};
				int index{0};
			};
			TStrHead StrHead[CatLangs];
			std::map<String,uint64> StrCatStart{};

			static j19gadget
				* Back{nullptr},
				* ProjectsLabel{nullptr},
				* StringsOrScenario{nullptr},
				* PanStrings{nullptr},
				* ButStrings{nullptr},
				* ButScenario{nullptr},
				* PanScenario{nullptr},
				* Myrah{nullptr},
				* LabCategories{nullptr},
				* PanStringsEdit{nullptr},
				* Aziella{nullptr},
				* NewString{nullptr},
				* NewStringAction{nullptr};
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

			static void SC_Draw(j19gadget*g,j19action a) { HSV(g,a); PanStringsEdit->Visible=ListCategories->SelectedItem()>=0; }

			static void SC_FW_Draw(j19gadget*g,j19action a) {
				BoxLightGreen(g,a);
				g->X(g->GetParent()->W()-g->W());
			}


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

			static void NS_Draw(j19gadget*,j19action a) {
				bool allow{Trim(NewString->Text).size()>0};
				BoxMint(NewString,a);
				(allow?BoxMint:BoxDark)(NewStringAction,a);
				NewStringAction->Enabled=allow;
			}

			void RenewStringsPanel(){
				using namespace Slyvina::Rosetta::Class;
				ProjectData CP{_ProjectData::CurrentProject};
				for (int i=0;i<CatLangs;i++) { StrHead[i].Label->Caption=CP->Settings->Value("Strings",TrSPrintF("LANG%d",i+1)); }
				for(auto&g:StringLine) g.second.Group->Visible=false;
				if (ListCategories->SelectedItem()<0) return;
				auto Cat{ListCategories->ItemText()};
				uint64 idx{StrCatStart.count(Cat)?StrCatStart[Cat]:0u};
				for(auto&g:StringLine) {
					String BTag{TrSPrintF( ((String)("KEY_"+Cat+"_%d")).c_str(),idx) };
					if (CP->Settings->Value("Strings",BTag)!="") {
						g.second.Group->Visible=true;
						g.second.Label->Caption=CP->Settings->Value("Strings",BTag);
					}
					idx++;
				}

			}

			static void RenewStringsPanel(j19gadget*,j19action) { RenewStringsPanel(); }

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
				ListCategories->CBDraw=SC_Draw;
				ListCategories->CBAction=RenewStringsPanel;
				int
					Y2{ProjectsList->Y()+ProjectsList->H()+2},
					H2{SH-Y2}; //std::cout << Y2<<"/"<<H2<<"\n";
				PanStringsEdit = CreateGroup(0,Y2,SW,H2,PanStrings);
				Aziella = CreatePicture(0,H2-551,334,551,PanStringsEdit,Pic_FullStretch);
				Mascot(Aziella,"Aziella");
				CatItems = H2 / CatItemHeight;
				for(int i=0;i<CatLangs;++i) {
					auto CH{&StrHead[i]};
					CH->index=i;
					CH->Group = CreateGroup(500+(i*505),0,500,30,PanStringsEdit);
					CH->Back = CreateButton("<",0,0,0,0,CH->Group);
					CH->Forward = CreateButton(">",0,0,0,0,CH->Group);
					CH->Back->CBDraw = BoxLightGreen;
					CH->Forward->CBDraw = SC_FW_Draw;
					FntGadget(CH->Back,"Symbols");
					FntGadget(CH->Forward,"Symbols");
					CH->Label = CreateLabel(TrSPrintF("Lang%d",i+1),0,0,500,30,CH->Group,8);
					CH->Label->CBDraw = BoxLightGreen;
					FntGadget(CH->Label,"Fonts/Ryanna.jfbf");
				}
				NewString = CreateTextfield(500,PanStringsEdit->H()-25,500,PanStringsEdit);
				NewStringAction = CreateButton("New String",NewString->X()+NewString->W()+5,NewString->Y(),PanStringsEdit);
				NewString->CBDraw=NS_Draw;
				uint32 idx{0};
				for (int y=40;y<NewString->Y()-50;y+=25) {
					auto SLS{&StringLine[idx]}; SLS->index=idx++;
					SLS->Group=CreatePanel(0,y,SW,25,PanStringsEdit);
					SLS->Group->BA=127;
					SLS->Label=CreateLabel(TrSPrintF("%d::%d",idx,y),0,0,495,20,SLS->Group,1);
					for(int i=0;i<CatLangs;++i) {
						SLS->Box[i] = CreateTextfield(500+(i*505),0,500,25,SLS->Group);
						SLS->Box[i]->CBDraw = BoxMagenta2;
					}
				}
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
