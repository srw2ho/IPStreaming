#include "pch.h"
#include "IsTenFootTrigger.h"
#include "SystemInformationHelpers.h"

using namespace Platform;
using namespace Windows::Foundation;

namespace IPStreamingCPP {
    namespace Triggers {
        void IsTenFootTrigger::IsTenFoot::set(bool value)
        {
            _isTenFootRequested = value;
            SetActive(SystemInformationHelpers::IsTenFootExperience() == _isTenFootRequested);
        }
    }
}
