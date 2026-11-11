#ifndef COCKPIT_C3DCOIN_HPP_
#define COCKPIT_C3DCOIN_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    namespace cockpit {

        // (name guessed; no symbol) The task that draws the spinning coin
        // model. Lives in the DOL (constructor fn_80246E94, object 0xF8 bytes, a
        // bcs::CTask with a second base at 0x78). Only what C3DCoin calls;
        // the function names are guesses.
        class C3DCoinTask : public bcs::CTask {
            public:
                C3DCoinTask(bcs::CTask *pParent, const bcs::FVEC3 &rPos, long nTeam);
                virtual void Main();

                void SetDisp(int bDisp);
                void SetPos(const bcs::FVEC3 &rPos);
                void CalcMtx();
                void ActReset();                        // fn_80247B4C
                void ActPopup();                        // fn_80247ADC
                void ActDrop();                         // fn_80247B24
                void ActUse(long nNum);                 // fn_80247B74

            private:
                unsigned char m_Unk34[0xF8 - 0x34];
        };

        // The spinning 3D coin drawn next to a team's coin counter. Lives in
        // the DOL (game/msw/cockpit/CMissionCount.cpp). Size 0x18 (the
        // element size CCockpitHockeyCoin passes to __construct_array), vptr
        // at 0x14. Return types are guesses.
        class C3DCoin {
            public:
                C3DCoin();

                // Creates the coin of team nTeam under task pParent.
                void Initialize(long nTeam, bcs::CTask *pParent);
                void Finalize();
                // The coin's actions: back to rest, jump up (a coin gained),
                // fall (a coin lost).
                void ActReset();
                void ActPopup();
                void ActDrop();
                // Coins spent (DodgeF CCockpitDodgeCoin); nNum is passed on
                // from CCockpitDodge::SetCoinSpendAnim.
                void ActUse(long nNum);
                void EnableDisp(int bDisp);
                // Follows pPane on screen.
                void AttachPane(nw4r::lyt::Pane *pPane);
                // The coin's task, null once it is gone.
                C3DCoinTask *Get3DCoinTask();

            private:
                unsigned long m_hTask; // 0x00, id of the C3DCoinTask
                long m_nTeam;          // 0x04
                // 0x08, 0x0C: CLytManager's location adjust scales, for
                // panes whose position is adjusted.
                float m_fAdjustScaleW;
                float m_fAdjustScaleH;
                int m_bDisp;           // 0x10

            public:
                // vptr at 0x14: declared after the data.
                virtual ~C3DCoin();
        };
    }
}

#endif /* !COCKPIT_C3DCOIN_HPP_ */
