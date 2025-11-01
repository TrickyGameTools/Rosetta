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
// Version: 25.11.01
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
			MakeCol1(MakeYellow,255,255,0);
			MakeCol1(MakeDark,100,100,100);
			MakeCol2(BoxPurple,180,0,255);
			MakeCol2(BoxLightGreen,180,255,0);
			MakeCol2(BoxDark,100,100,100);
			MakeCol2(BoxMint,0,180,255);
			MakeCol2(BoxMagenta2,255,0,180);

			static void HSV(j19gadget*g,j19action) {
				g->SetForegroundHSV(hue,1,1.0);
				g->SetBackgroundHSV(hue,1,0.1);
			}

			static void CounterHSV(j19gadget*g,j19action) {
				g->SetForegroundHSV(360-hue,1,1.0);
				g->SetBackgroundHSV(360-hue,1,0.1);
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

			const int MaxScenGroup{2};
			struct TScenGroup {
				TStrHead Head{};
				j19gadget
					* Caption{nullptr},
					* PicDir{nullptr},
					* PicSpc{nullptr},
					* Font{nullptr},
					* Voice{nullptr},
					* Content{nullptr};
			};
			TScenGroup ScenGroup[MaxScenGroup]{};

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
				* NewStringAction{nullptr},
				* Scyndi{nullptr},
				* Francis{nullptr},
				* LabFiles{nullptr},
				* LabTags{nullptr},
				* ListFiles{nullptr},
				* ListTags{nullptr},
				* GroupScenTags{nullptr},
				* ChkLinkName{nullptr};

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

			static void ScenTagDraw(j19gadget*g,j19action a) {
				CounterHSV(g,a);
				g->Enabled=ListFiles->SelectedItem()>=0;
			}


			String SelectedProject() {
				if (ProjectsList->SelectedItem()<0) return "";
				auto SI{ProjectsList->ItemText()};
				auto SP{IndexOf(SI,':')};
				if (SP<0) {return "";}
				return SI.substr(SP+1);
			}

			static void RenewScenEntries() {
				using namespace Slyvina::Rosetta::Class;
				auto Lst{_CScenario::AllEntries()};
				ListFiles->ClearItems();
				for(auto F:*Lst) ListFiles->AddItem(StripExt(F));
			}

			static void PJ_Select(j19gadget*g,j19action a) {
				using namespace Slyvina::Rosetta::Class;
				if (ProjectsList->SelectedItem()<0) return;
				QCol->Doing("Userselect",SelectedProject());
				_ProjectData::CurrentProject = Project[SelectedProject()];
				_ProjectData::CurrentProject->UpdateStrings();
				RenewScenEntries();
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
				GINIE DStr[CatLangs] {};
				for (int i=0;i<CatLangs;i++) {
						auto lngname{CP->Settings->Value("Strings",TrSPrintF("LANG%d",i+1))};
						StrHead[i].Label->Caption=lngname;
						DStr[i]=CP->GetStrings(lngname);
				 }
				for(auto&g:StringLine) g.second.Group->Visible=false;
				if (ListCategories->SelectedItem()<0) return;
				auto Cat{ListCategories->ItemText()};
				uint64 idx{StrCatStart.count(Cat)?StrCatStart[Cat]:0u};
				for(auto&g:StringLine) {
					String BTag{TrSPrintF( ((String)("KEY_"+Cat+"_%d")).c_str(),idx) };
					if (CP->Settings->Value("Strings",BTag)!="") {
						auto Key{CP->Settings->Value("Strings",BTag)};
						g.second.Group->Visible=true;
						g.second.Label->Caption=Key;
						for(int i=0;i<CatLangs;i++) {
							//std::cout << Cat <<" / "<<Key<<"\n";// Debug
							g.second.Box[i]->Text=DStr[i]->Value(Cat,Key);
						}
					}
					idx++;
				}

			}

			static void RenewStringsPanel(j19gadget*,j19action) { RenewStringsPanel(); }

			static void ActNextLanguage(j19gadget* g,j19action) {
				auto Langs{Class::_ProjectData::CurrentProject->SupportedLanguages()};
				for(int i=0;i<CatLangs;i++) {
					int TimeOut{10000};
					if (StrHead[i].Forward==g) {
						size_t iLang{0};
						auto cLang{StrHead[i].Label->Caption};
						int iCat{-1};
						for(size_t j=0;j<Langs->size();j++) {
							if ((*Langs)[j]==cLang) { iLang=j; iCat=i; }
						}
						if (iCat<0) {QCol->Error("Lang find failure!"); return; }
						iLang=( (++iLang) % Langs->size());
						StrHead[i].Label->Caption = (*Langs)[iLang];
						Class::_ProjectData::CurrentProject->Settings->Value("Strings",TrSPrintF("LANG%d",i+1),(*Langs)[iLang]);
						RenewStringsPanel();
						return;
					}
				}
				QCol->Error("Gadget find failure");
			}
				static void ActPrevLanguage(j19gadget* g,j19action) {
				auto Langs{Class::_ProjectData::CurrentProject->SupportedLanguages()};
				for(int i=0;i<CatLangs;i++) {
					int TimeOut{10000};
					if (StrHead[i].Back==g) {
						size_t iLang{0};
						auto cLang{StrHead[i].Label->Caption};
						int iCat{-1};
						for(size_t j=0;j<Langs->size();j++) {
							if ((*Langs)[j]==cLang) { iLang=j; iCat=i; }
						}
						if (iCat<0) {QCol->Error("Lang find failure!"); return; }
						iLang=iLang==0?Langs->size()-1:(--iLang);
						StrHead[i].Label->Caption = (*Langs)[iLang];
						Class::_ProjectData::CurrentProject->Settings->Value("Strings",TrSPrintF("LANG%d",i+1),(*Langs)[iLang]);
						RenewStringsPanel();
						return;
					}
				}
				QCol->Error("Gadget find failure");
			}

			static void StrFieldAction(j19gadget *g,j19action a) {
				//QCol->Doing("DEBUG",g->Text,"\t"); QCol->Green(TrSPrintF("%d\n",(int)a));
				switch(a) {
					case j19action::Activate: QCol->Doing("Activated","TextField related to Strings"); return;
					case j19action::BackSpace:
					case j19action::Type: {
						j19gadget* ouwe{g->GetParent()};
						if (!ouwe->Visible) { QCol->Error("Action to a field that should not be visible"); return; }
						for(auto& s:StringLine) {
							if (s.second.Group==ouwe) {
								for(int i=0;i<CatLangs;i++) {
									if (s.second.Box[i]==g) {
										auto CP{Slyvina::Rosetta::Class::_ProjectData::CurrentProject};
										auto
											lngname{StrHead[i].Label->Caption},
											Cat{ListCategories->ItemText()},
											Key{s.second.Label->Caption};
										auto LN{CP->GetStrings(lngname)};
										if (Cat=="" || Key=="") { QCol->Error("String Edit Error -> "+lngname+"::"+Cat+"::"+Key); return; }
										LN->Value(Cat,Key,g->Text);
										return;
									}
								}
							}
						}
						QCol->Error("Unable to find the correct string to edit");
					} break;
					default:
						QCol->Error(TrSPrintF("Unknown action '%d' on Strings TextField (Non-Fatal)"));
				}
			}

			static void  AllowStringLines(j19gadget*g,j19action) { g->Enabled=StrHead[0].Label->Caption!=StrHead[1].Label->Caption; }

			static void SelectScenEntry(j19gadget*g, j19action) {
				QCol->Doing("Tag renewal",g->ItemText());
				ListTags->ClearItems();
				if (g->SelectedItem()<0) { return; }
				using namespace Class;
				auto CP{_ProjectData::CurrentProject};
				auto SC{CP->Scenario};
				auto EN{(*SC)[g->ItemText()]};
				auto got{EN->LTags()};
				for(auto&ENI:*got) ListTags->AddItem(ENI);
			}

			static void EnTagSelected(j19gadget*g,j19action ) {
				g->Enabled = ListFiles->SelectedItem()>=0 && ListTags->SelectedItem()>=0;
			}

			static void TagEn(j19gadget*g,j19action a) {
				(
					GroupScenTags->Enabled ?
					MakeYellow :
					MakeDark
				)(g,a);
			}

			void ScLabel(String Caption,int y) {
				auto
					s{CreateLabel(Caption,2,y+2,1,30,GroupScenTags,1)},
					g{CreateLabel(Caption,0,y,1,30,GroupScenTags,1)};
				g->CBDraw=MakeYellow;
				s->SetForeground(0,0,0);
			}
			int ScLabel(String Caption) {
				static int y{ProjectsList->H()+ProjectsList->Y()+90};
				auto ret{y};
				ScLabel(Caption,y);
				y+=30;
				return ret;
			}


			void Init(int argc,char** args) {
				// Start
				QCol->White("Rosetta\n\n");
				QCol->Doing("Coded by","Jeroen P. Broks");
				QCol->Doing("Platform",Platform());
				QCol->Doing("Run file",args[0]); Exefile=args[0];
				for(int i=1;i<argc;i++) { Args->push_back(args[i]);} if (!Args->size()) Args->push_back(CurrentDir());
				JCR6::init_zlib();
				IdentifyAssets();
				TQSG::Graphics(SW,SH,"Rosetta - (c) Jeroen P. Broks");
				// Main Window
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
				// Strings
				PanStrings = CreatePanel(0,0,SW,SH,Back);
				PanStrings->Visible=false;
				PanStrings->BA=0;
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
					CH->Back->CBAction = ActPrevLanguage;
					CH->Forward->CBDraw = SC_FW_Draw;
					CH->Forward->CBAction = ActNextLanguage;
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
					SLS->Group->CBDraw= AllowStringLines;
					SLS->Label=CreateLabel(TrSPrintF("%d::%d",idx,y),0,0,495,20,SLS->Group,1);
					SLS->Label->CBDraw=MakeYellow;
					for(int i=0;i<CatLangs;++i) {
						SLS->Box[i] = CreateTextfield(500+(i*505),0,500,25,SLS->Group);
						SLS->Box[i]->CBDraw = BoxMagenta2;
						SLS->Box[i]->CBAction = StrFieldAction;
					}
				}
				// Scenario
				PanScenario = CreatePanel(0,0,SW,SH,Back);
				PanScenario->Visible=false;
				PanScenario->BA=0;
				Scyndi = CreatePicture(0,PanScenario->H()-474,400,474,PanScenario);
				Mascot(Scyndi,"Seelah Gandra Groot");
				Francis = CreatePicture(PanScenario->W()-94,0,94,120,PanScenario,Pic_FullStretch);
				Mascot(Francis,"Francis");
				LabFiles = CreateLabel("Files:",ProjectsList->W()+ 25,35,100,30,PanScenario);
				LabTags = CreateLabel("Tags:",ProjectsList->W()+525,35,100,30,PanScenario);
				LabFiles->CBDraw = MakeSkyBlue;
				LabTags->CBDraw = MakeSkyBlue;
				FntGadget(LabFiles,"Fonts/Ryanna.jfbf");
				FntGadget(LabTags,"Fonts/Ryanna.jfbf");
				ListFiles = CreateListBox(LabFiles->X(),70,490,ProjectsList->H()-60,PanScenario);
				ListFiles->CBDraw = HSV;
				ListFiles->CBAction = SelectScenEntry;
				ListTags = CreateListBox(LabFiles->X()+500,70,490,ListFiles->H(),PanScenario);
				ListTags->CBDraw = ScenTagDraw;
				GroupScenTags = CreateGroup(ListFiles->X(),0,PanScenario->W()-ListFiles->X(),PanScenario->H(),PanScenario);
				GroupScenTags->CBDraw = EnTagSelected;
				ChkLinkName = CreateCheckBox("Name Link",(LabTags->X()+LabTags->W()+10)-GroupScenTags->X(),40,100,20,GroupScenTags);
				ChkLinkName->CBDraw = TagEn;
				{
					int
						ycap = ScLabel("Caption"),
						ypdr = ScLabel("Pic Dir"),
						ypsc = ScLabel("Pic Spe"),
						yfnt = ScLabel("Font"),
						yvoc = ScLabel("Voice"),
						ycnt = ScLabel("Content");
					for(int i=0;i<MaxScenGroup;++i) {
						auto &SC{ScenGroup[i]};
						int XHue{i*150};
						SC.Caption = CreateTextfield(5+(i*500),ycap,495,GroupScenTags);
						SC.Caption-> SetForegroundHSV(25+XHue,1,1);
						SC.PicDir  = CreateTextfield(5+(i*500),ypdr,495,GroupScenTags);
						SC.PicDir -> SetForegroundHSV(50+XHue,1,1);
						SC.PicSpc  = CreateTextfield(5+(i*500),ypsc,495,GroupScenTags);
						SC.PicSpc -> SetForegroundHSV(75+XHue,1,1);
						SC.Font    = CreateTextfield(5+(i*500),yfnt,495,GroupScenTags);
						SC.Font   -> SetForegroundHSV(100+XHue,1,1);
						SC.Voice   = CreateTextfield(5+(i*500),yvoc,495,GroupScenTags);
						SC.Voice  -> SetForegroundHSV(125+XHue,1,1);
						SC.Content = CreateTextArea(5+(i*500),ycnt,495,GroupScenTags->H()-(ycnt+5),GroupScenTags);
						SC.Content-> SetForegroundHSV(150+XHue,1,1);
						SC.Content->Content("Testing one-two-three\nTest een-twee-drie");
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
