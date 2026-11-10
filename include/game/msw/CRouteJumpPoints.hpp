#ifndef CROUTEJUMPPOINTS_HPP_
#define CROUTEJUMPPOINTS_HPP_

#include "game/msw/CRouteBase.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    // A route that jumps along a row of map points (module TourF). 0xC0
    // bytes. Defined in game/tour/CTournamentRouteMan.cpp. Only the members
    // the stage route managers use are named.
    class CRouteJumpPoints : public CRouteBase {
        public:
            // How a jump looks and which timer events it raises. One per
            // kind of jump, kept in .rodata by the stage route managers and
            // set after construction. 0x64 bytes. Only the event part is
            // understood (CRouteJumpPoints::PushRoute queues nEvent[i] at
            // fEventTime[i]); the other names are placeholders.
            struct JUMP_DATA {
                float fParam[14];     // 0x00
                float fEventTime[3];  // 0x38
                float fUnk44;         // 0x44
                float fUnk48;         // 0x48
                short nEvent[4];      // 0x4C: timer events, -1 for none
                float fUnk54;         // 0x54
                short nUnk58;         // 0x58
                short nUnk5A;         // 0x5A
                long nUnk5C;          // 0x5C
                long nSe;             // 0x60: sound number
            };

            CRouteJumpPoints(const _PARAM &param, float rate);
            virtual ~CRouteJumpPoints();
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual float GetSelectDir(float frame, int b);
            virtual void SetPoints(const unsigned char *pPoints, long nNum);
            virtual long GetStartRouteId() const;

            // Extra route played on the way (a model's own animation path);
            // the base class has none (both return 0). Return types guessed.
            virtual CRouteBase *GetOffsetRoute(nw4r::g3d::ResFile file);
            virtual void *GetOffsetRes();

            void SetJumpData(const JUMP_DATA *pData)
            {
                m_pJumpData = pData;
            }

        protected:
            const JUMP_DATA *m_pJumpData; // 0x34
            float m_fRotY;                // 0x38
            long m_nPointNum;             // 0x3C
            char m_Unk40[0x80];           // 0x40
    }; // size 0xC0
}

#endif /* !CROUTEJUMPPOINTS_HPP_ */
