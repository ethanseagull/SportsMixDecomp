// msw::BALL_VOLLEY::CChrParasiteBanana: (meaning guessed) the attack
// collision a character carries for a moment after a banana item hit it.
//
// These two functions belong to game/volley/CBallVolley.cpp: the class's
// inline constructor is expanded there and its vtable sits in that unit's
// .data. They are kept apart only until CBallVolley.cpp has source.
#include "game/msw/CChrParasite.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CChrCore.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {
    namespace BALL_VOLLEY {
        class CChrParasiteBanana : public CC_CMN::CChrParasite {
            public:
                virtual ~CChrParasiteBanana() {}
                virtual void MainFunc();

            private:
                CAtkCollision m_AtkColl; // 0x40
        };
    }
}

// Waits one frame while the character is down (status bits 0x400 or 1),
// then puts the attack collision on its trans node for one frame and turns
// it off; then asks for deletion.
void msw::BALL_VOLLEY::CChrParasiteBanana::MainFunc()
{
    switch (m_nState) {
        case 0: {
            unsigned long flag = GetChrCore_()->GetStatusFlag();
            if (!(flag & 0x400) && !(flag & 1)) {
                m_nState++;
            }
            break;
        }
        case 1: {
            CChrCore *pChr = GetChrCore_();
            unsigned long flag = pChr->GetStatusFlag();
            if (!(flag & 0x400) && !(flag & 1)) {
                bcs::FVEC3 pos;
                pChr->GetTransNodePos(pos);
                m_AtkColl.SetPosAll(pos);
                m_AtkColl.SetEnableAll();
                m_nState++;
            }
            break;
        }
        case 2:
            m_AtkColl.SetDisableAll();
            m_nState++;
            break;
        default:
            Delete();
            break;
    }
}
