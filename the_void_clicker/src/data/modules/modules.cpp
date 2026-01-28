#include "modules.h"

namespace Feature {
	void CModules::Activated() {

		Left::Clicker::Get().Activate();
		Left::Clicker::Get().AutoBlockHit();

	}
}