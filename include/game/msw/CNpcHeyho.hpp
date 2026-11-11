#ifndef MSW_CNPCHEYHO_HPP_
#define MSW_CNPCHEYHO_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CNpcBase.hpp"

namespace bcs {
    class CScnObj;
}

namespace msw {

    class CChrMan;

    // (class name and purpose guessed) The "heyho" (Shy Guy) riding a
    // trolley of stage 17 and throwing things from it: an NPC with its own
    // state machine, made by msw::CStgGmk_Trolley_Mdl::CreateHeyho. A DOL
    // class without exported names, size 0x1E8 (CreateHeyho allocates
    // that much): CNpcBase at 0x000, bcs::CFsm at 0x188 (vptr at 0x1CC),
    // own members from 0x1D0. Constructor fn_8026DC64, vtable
    // lbl_8048F0B8.
    //
    // Only what the trolley's model calls is declared; the new virtual
    // functions follow the CFsm part of the vtable (from 0x88), and the
    // names are guesses from their bodies.
    class CNpcHeyho : public CNpcBase, public bcs::CFsm<CNpcHeyho> {
        public:
            CNpcHeyho(CChrMan *pChrMan);
            virtual ~CNpcHeyho();

            // Attaches the model to node of pObj (null: detaches it).
            virtual void AttachTo(const bcs::CScnObj *pObj, long node); // 0x88
            // Enters state 1 from state 0.
            virtual void Appear();                                      // 0x8C
            // Goes back to state 0.
            virtual void Disappear();                                   // 0x90
            // What to throw (bCoin) and how far, the distances sorted.
            virtual void SetThrowParam(int bCoin, float distMin, float distMax); // 0x94

        private:
            unsigned long m_Unk1D0; // 0x1D0
            unsigned long m_Unk1D4; // 0x1D4
            float m_fUnk1D8;        // 0x1D8
            int m_bCoin;            // 0x1DC
            float m_fThrowDistMin;  // 0x1E0
            float m_fThrowDistMax;  // 0x1E4
    };
}

#endif /* !MSW_CNPCHEYHO_HPP_ */
