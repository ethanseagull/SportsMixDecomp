#ifndef MSW_CGRDMDLEFFECT_HPP_
#define MSW_CGRDMDLEFFECT_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {
    namespace CC_CMN {

        // (DOL class; meaning guessed from the name and its one user, BossF's
        // CC09_BOSS::CcKinopioReceiver) A task holding an effect with a
        // model, set on the ground at a point. 0x50 bytes.
        //
        // INCOMPLETE: only what the receiver reads; member names guessed.
        class CGrdMdlEffect : public bcs::CUnivTask {
            public:
                // Effect nEff of the effect table at pos, turned by rot.
                CGrdMdlEffect(bcs::CTask *pParent, long nEff, const bcs::FVEC3 &pos, const bcs::FVEC3 &rot);
                virtual ~CGrdMdlEffect();
                virtual void Main();

                // (name guessed) Inline: the receiver's owner reads the word.
                const bcs::FVEC3 &GetPos() const { return m_Pos; }

            protected:
                bcs::FVEC3 m_Pos;          // 0x34
                bcs::FVEC3 m_Rot;          // 0x40
                unsigned long m_hEffect;   // 0x4C, for CEffectCtrl::FindList; 0 for none
        };
    }
}

#endif /* !MSW_CGRDMDLEFFECT_HPP_ */
