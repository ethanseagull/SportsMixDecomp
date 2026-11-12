#ifndef MSW_CCHRTARGETMARKERVOLLEY_HPP_
#define MSW_CCHRTARGETMARKERVOLLEY_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace bcs {
    class CMdlObj;
    class CScnGroup;
    struct FVEC3;
}

namespace msw {

    // The target marker of a volleyball character (VolleyF), size 0xD8
    // (what CChrManVolley::CreateMarker_ allocates): bcs::CTask at 0x00,
    // bcs::CFsm<CChrTargetMarkerVolley> at 0x34 (vptr at 0x78), own members
    // from 0x7C. Its functions are in game/volley/CChrTargetMarkerVolley.cpp;
    // the virtual functions are declared in the order of
    // __vt__Q23msw22CChrTargetMarkerVolley. The state functions are not
    // virtual (the state table points at them directly). Member names are
    // guesses.
    class CChrTargetMarkerVolley : public bcs::CTask, public bcs::CFsm<CChrTargetMarkerVolley> {
        public:
            enum {
                STATE_SLEEP = 0,
                STATE_APPEAR = 1,
                STATE_MAIN = 2,
                STATE_DISAPPEAR = 3,
                STATE_NUM = 4
            };

            // (parameter names guessed) CChrManVolley passes itself, the
            // character's index and whether a player drives it (unused).
            CChrTargetMarkerVolley(bcs::CTask *pParent, long nChr, int bPlayer);
            virtual ~CChrTargetMarkerVolley();
            virtual void Main();
            virtual void Disp();

        protected:
            // 2: show, 3: hide (1 is tested by the states but not returned).
            virtual int CheckAppear_();
            // Where the marker goes: where the character puts it.
            virtual void GetTargetPos_(bcs::FVEC3 &rPos);
            virtual void MoveToTargetPos_(const bcs::FVEC3 &rPos, float fFrameRate);

        public:
            void CtorStateSleep();
            void LoopStateSleep();
            void DtorStateSleep();
            void CtorStateAppear();
            void LoopStateAppear();
            void DtorStateAppear();
            void CtorStateMain();
            void LoopStateMain();
            void DtorStateMain();
            void CtorStateDisappear();
            void LoopStateDisappear();
            void DtorStateDisappear();

        protected:
            void Finalize_();
            // Sets up the state machine (inlined into the constructor).
            void InitState_();
            void Create_();
            // A non-zero bOn takes the sub models out of their parents and
            // hands them to the render manager; zero takes them out only.
            void PushBack_(int bOn);
            void AllDispOff_();
            void UpdateColor_();

            long m_nChr;                     // 0x7C, the character's index
            int m_bPadCtrl;                  // 0x80, a pad drives the character
            int m_bStealth;                  // 0x84, the character's stealth mode
            long m_nFixedType;               // 0x88, the character's marker type
            bcs::CScnGroup *m_pGroup;        // 0x8C, holds the models below
            bcs::CMdlObj *m_pMdlBase;        // 0x90, the ground mark
            bcs::CMdlObj *m_pMdlStar;        // 0x94, the star mark
            bcs::CMdlObj *m_pMdlBaseSub;     // 0x98, attached to m_pMdlBase
            bcs::CMdlObj *m_pMdlStarSub;     // 0x9C, attached to m_pMdlStar
            float m_fAppearRate;             // 0xA0, 0 to 1
            float m_fRingRate;               // 0xA4, 0 to 1: the rings close in
            float m_fPulse;                  // 0xA8, an angle (radians)
            bcs::CMdlObj *m_pMdlRing[4];     // 0xAC
            bcs::CMdlObj *m_pMdlRingSub[4];  // 0xBC, each attached to its ring
            float m_fSubRate;                // 0xCC, 0 to 1
            float m_fWait;                   // 0xD0, frames before m_fSubRate rises
            float m_fWaitInit;               // 0xD4
    };
}

#endif /* !MSW_CCHRTARGETMARKERVOLLEY_HPP_ */
