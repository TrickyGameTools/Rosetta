// License:
// 
// Rosetta
// All classes (forward)
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
// Version: 25.09.29
// End License

#pragma once
#define spnew(cl) std::shared_ptr<cl>(new cl())
#define spnew1(cl,p) std::shared_ptr<cl>(new cl(p))

#define spclass(real,sp) class real; typedef std::shared_ptr<real>sp;

namespace Slyvina {
	namespace Rosetta {
		namespace Class {
			spclass( _ProjectData,ProjectData);
			spclass(_CSenario,CScenario);
		}
	}
