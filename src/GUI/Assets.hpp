
#pragma once
#include <Slyvina.hpp>
#include <JCR6_Core.hpp>

namespace Slyvina {
	namespace Rosetta{
		namespace GUI {
			String AssetsFile();
			JCR6::JT_Dir Assets();
			void IdentifyAssets(bool crash=true);
		}
	}
}
