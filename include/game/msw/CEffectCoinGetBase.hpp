#ifndef MSW_CEFFECTCOINGETBASE_HPP_
#define MSW_CEFFECTCOINGETBASE_HPP_

#include "bcs/CMdlObj.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CDropCoinMan.hpp"

namespace msw {

    // A coin model that flies off and fades when a coin is collected. A DOL
    // class, size 0x64 (msw::CGimmickManBoss::EffectCoinGetCreate_ allocates
    // that much for a derived class that adds nothing). Each game module
    // derives the effect itself (GIMMICK_BOSS::CEffectCoinGet, ...), which
    // supplies Main. The members are read from the constructor and
    // destructor in the DOL and from the modules' Main; their names are
    // guesses.
    class CEffectCoinGetBase : public bcs::CUnivTaskEx {
        public:
            CEffectCoinGetBase(bcs::CTask *pParent);
            virtual ~CEffectCoinGetBase();
            virtual void Main() = 0;

        protected:
            // Loads the coin model of `type` at rPos into m_pMdl. What the
            // int selects is not known (the boss game passes 1). (return
            // type guessed)
            void CreateMdl_(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type, int);

            // (name guessed) The coin model. Being const, it makes the
            // modules' constructors read m_pMdl again for CreateAttachEffect
            // after the null check, as the original does.
            bcs::CMdlObj *GetMdl() const { return m_pMdl; }

            bcs::CMdlObj *m_pMdl;   // 0x34
            bcs::FVEC3 m_Vel;       // 0x38, per frame
            float m_fGravity;       // 0x44, added to m_Vel.y per frame
            float m_fWait;          // 0x48, frames of the hover state
            float m_fTopY;          // 0x4C
            float m_fScale;         // 0x50, 0 to 1
            float m_fRotSpeed;      // 0x54, radians per frame about Y
            float m_fLife;          // 0x58, frames; fades out over the last ones
            long m_nState;          // 0x5C
            unsigned long m_hEffect; // 0x60, from CEffectCtrl::CreateAttachEffect
    };
}

#endif /* !MSW_CEFFECTCOINGETBASE_HPP_ */
