// Lic:
// Rosetta
// Settings GUI objects register
// 
// 
// 
// (c) Jeroen P. Broks, 2023
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Please note that some references to data like pictures or audio, do not automatically
// fall under this licenses. Mostly this is noted in the respective files.
// 
// Version: 23.09.14
// EndLic
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace Rosetta.Class {
	internal class RegisterSettings {
		internal readonly string
			Category,
			Key;
		internal readonly TextBox TarTextBox;

		static readonly internal Dictionary<TextBox,RegisterSettings> Reg=new Dictionary<TextBox,RegisterSettings>();

		internal RegisterSettings(TextBox _TB,string _Cat,string _Key) {
			TarTextBox = _TB;
			Category = _Cat;
			Key = _Key;
			Reg[_TB] = this;
		}
	}
}