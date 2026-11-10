#ifndef CEVENTNODESIMPLE_HPP_
#define CEVENTNODESIMPLE_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // A node path an event model follows (DOL class; msw::CEventModel::
    // SetLinkNode takes one). Only what the modules need: the base of
    // msw::CEventNodeSimple. Its constructor stores no vtable of its own in
    // the inlined CEventNodeSimple constructors. (layout of the base not
    // known)
    class CEventNodeBase {
        public:
            CEventNodeBase() {}
            virtual ~CEventNodeBase();
    };

    // A node path from one position and rotation to another (DOL class,
    // 0x48 bytes, vptr at 0x00). The modules construct it inline (TourF:
    // msw::CTournamentMan, msw::CTournamentRouteMan_Stage32) and fill the
    // two ends themselves. Names of the members guessed.
    class CEventNodeSimple : public CEventNodeBase {
        public:
            CEventNodeSimple() : m_fRate(0.0f), m_fRateAdd(0.0f), m_fFrame(0.0f), m_fFrameStep(0.5f)
            {
            }

            virtual ~CEventNodeSimple();

            // (names guessed) Inline in the modules: the two ends of the path.
            void SetStart(const bcs::FVEC3 &vPos, const bcs::FVEC3 &vRot)
            {
                m_vStartPos = vPos;
                m_vStartRot = vRot;
            }
            void SetEnd(const bcs::FVEC3 &vPos, const bcs::FVEC3 &vRot)
            {
                m_vEndPos = vPos;
                m_vEndRot = vRot;
            }

            // Sets the rate, its change per frame and the limit; m_fFrame
            // receives the number of frames until the limit is passed.
            void _SetRate(float fRate, float fMul, float fLimit);

            bcs::FVEC3 m_vStartPos; // 0x04
            bcs::FVEC3 m_vStartRot; // 0x10
            bcs::FVEC3 m_vEndPos;   // 0x1C
            bcs::FVEC3 m_vEndRot;   // 0x28
            float m_fRate;          // 0x34
            float m_fRateAdd;       // 0x38
            float m_fLimit;         // 0x3C
            float m_fFrame;         // 0x40
            float m_fFrameStep;     // 0x44
    }; // size 0x48
}

#endif /* !CEVENTNODESIMPLE_HPP_ */
