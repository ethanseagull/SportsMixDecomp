#include "game/msw/CDropCoin.hpp"

namespace msw {

    bool CDropCoin::CanbePickup() const
    {
        if ((m_nFlag & 1) == 0) {
            return false;
        }
        if (m_pUnk48 == 0 || IsDelete()) {
            return false;
        }
        return true;
    }
}
