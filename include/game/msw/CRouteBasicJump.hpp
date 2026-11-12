#ifndef CROUTEBASICJUMP_HPP_
#define CROUTEBASICJUMP_HPP_

#include "game/msw/CRouteBase.hpp"

namespace msw {

    // A route that jumps from the start to the end point of its _PARAM
    // (module TourF). 0x60 bytes. Defined in
    // game/tour/CTournamentRouteMan.cpp. Only the members the stage route
    // managers set are named; parameter names guessed.
    class CRouteBasicJump : public CRouteBase {
        public:
            CRouteBasicJump(const _PARAM &param, float rate, long n);
            virtual ~CRouteBasicJump();
            virtual void CalcFrame();
            virtual void GetResult(_RESULT &result, float frame, int b);
            virtual void GetResultPos(bcs::FVEC3 &pos, float frame, int b);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);
            virtual void SetNoJump(int bNoJump);
            virtual int GetNoJump() const;
            virtual float GetRate() const;

            // (names guessed) The take-off and landing timing of the jump
            // motion, as msw::TournamentData::GetJumpFrame returns them.
            // Call CalcFrame after them.
            void SetUpFrame(float fUp) { m_fUp = fUp; }
            void SetDownFrame(float fDown) { m_fDown = fDown; }
            // The same two (TourF has weak copies under these names).
            void SetStartWait(float fWait) { m_fUp = fWait; }
            void SetEndWait(float fWait) { m_fDown = fWait; }
            // (name and meaning guessed) Two shape values of the jump that
            // Stage33's airship routes set before CalcFrame (1 and 0.7 onto
            // a ship, 0.1 and 100 off it).
            void SetJumpParam(float f50, float f54)
            {
                m_fUnk50 = f50;
                m_fUnk54 = f54;
            }

        protected:
            // Reads the jump's timing and sound.
            friend class CEvMoveActTmJump;
            // CRouteJumpPoints::PushRoute sets the timing, the sound and the
            // offset route of its jumps.
            friend class CRouteJumpPoints;

            // Position at frame fFrame (run backwards when b is set) and the
            // scale of the character there.
            void _SetTranslate(bcs::FVEC3 &pos, bcs::FVEC3 &scale, float fFrame, int b);

            CRouteBase *m_pOffsetRoute; // 0x34: added to the position, owned (name guessed)
            float m_fRotY;              // 0x38
            float m_fUp;                // 0x3C
            float m_fDown;              // 0x40
            float m_fUnk44;             // 0x44
            float m_fUnk48;             // 0x48
            float m_fLength;            // 0x4C
            // 0x50, 0x54: the scale of the character at the start and at the
            // end of the jump. (meaning guessed)
            float m_fUnk50;
            float m_fUnk54;
            int m_bNoJump;              // 0x58
            long m_nSe;                 // 0x5C: sound of the jump
    }; // size 0x60
}

#endif /* !CROUTEBASICJUMP_HPP_ */
