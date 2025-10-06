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
// Version: 25.10.06
// End License

#include <SlyvQCol.hpp>

using namespace Slyvina::Units;

namespace Slyvina {
	namespace Rosetta {
		namespace GUI {

			void Init(int argc,char** args) {
				QCol->White("Rosetta\n\n");
				QCol->Doing("Coded by","Jeroen P. Broks");
				QCol->Doing("Platform",Platform());
				QCol->Doing("Run file",args[0]);
			}

			void Run() {}
			void Done() {}
		}
	}
}
