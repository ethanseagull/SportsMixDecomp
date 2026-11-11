#ifndef CGAMEGOALDEMO_HPP_
#define CGAMEGOALDEMO_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CCamera;
}

namespace msw {

    // The demo after a goal (DOL, game/msw/CGameExDemoBase.cpp; 0x64 bytes,
    // vtable lbl_8048D580). msw::CGame::StartGoalDemo_ makes it with its
    // own arguments, IsEndGoalDemo_ asks IsEnd and EndGoalDemo_ calls End.
    // The class name and every member name are guesses; no module names
    // it.
    //
    // Type 1 is a short camera on the scoring position with a telop; any
    // other type plays the demo camera "MakeScore" with the sport's goal
    // telop and speed lines, at a slowed game speed.
    class CGameGoalDemo : public bcs::CTask {
        public:
            CGameGoalDemo(bcs::CTask *pParent, long nType, long nTeam, const bcs::FVEC3 &rPos, long nSide,
                          long nTelop, float fSpeed);
            virtual ~CGameGoalDemo();
            virtual void Main();

            int IsEnd() const;
            void End();

        private:
            // Not zero when the segment rFrom-rTo crosses the net plane of
            // the stage (from either side).
            int IsCrossNet_(const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo);

            long m_nType;          // 0x34
            long m_nTeam;          // 0x38: telop of type 1, demo camera argument
            bcs::FVEC3 m_vPos;     // 0x3C: where the goal was scored
            long m_nSide;          // 0x48: not zero turns the camera round
            long m_nStep;          // 0x4C: 99 when done
            float m_fSaveSpeed;    // 0x50: GameWork::m_fGameSpeed before the demo
            long m_nTelop;         // 0x54: the goal telop, negative for none
            float m_fUnk58;        // 0x58: 1.0, not read here
            float m_fTimer;        // 0x5C: frames
            bcs::CCamera *m_pCamera; // 0x60
    };
}

#endif /* !CGAMEGOALDEMO_HPP_ */
