// Lic:
// Rosetta
// Strings Register
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
// Version: 23.09.15
// EndLic
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace Rosetta.Class {
    internal class RegisterStrings {
        readonly internal int Index;
        readonly internal ComboBox Key = null;
        readonly internal TextBox Value1 = null;
        readonly internal TextBox Value2 = null;

        readonly internal static Dictionary<TextBox,RegisterStrings> RegValue = new Dictionary<TextBox,RegisterStrings>();
        readonly internal static Dictionary<ComboBox, RegisterStrings> RegKey = new Dictionary<ComboBox, RegisterStrings>();
        readonly internal static List<RegisterStrings> Lijst = new List<RegisterStrings>();

        public RegisterStrings(int _Index,ComboBox _Key,TextBox _Value1,TextBox _Value2) {
            Lijst.Add(this);
            RegKey.Add(_Key, this);
            RegValue.Add(_Value1, this);
            RegValue.Add(_Value2 , this);
            Index = _Index;
            Key= _Key; 
            Value1 = _Value1; 
            Value2 = _Value2;
            Debug.WriteLine($"Registered Strings Slot #{Index}");
        }

    }
}