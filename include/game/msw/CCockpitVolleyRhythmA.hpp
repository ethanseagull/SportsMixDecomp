#ifndef CCOCKPITVOLLEYRHYTHMA_HPP_
#define CCOCKPITVOLLEYRHYTHMA_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"

namespace msw {

    // Size 0x88 (CCockpitVolley::CreateObjImpl allocates it, object 52):
    // the rhythm game's score board, combo and ball scores
    // (game/volley/CCockpitVolleyRhythmA.cpp). Virtuals from the VolleyF
    // vtable; return types and argument names are guesses. The members are
    // not mapped (0x60 to 0x83 floats, bytes at 0x84).
    class CCockpitVolleyRhythmA : public CCockpitVolleyLayoutBase {
        public:
            CCockpitVolleyRhythmA(long nId, float fSortZ);
            virtual ~CCockpitVolleyRhythmA();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();
            virtual long CancelMenu();
            virtual int AllOut();
            virtual int AllIn();
            virtual int AllOutWait();
            virtual int AllInWait();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();
            virtual int ComboOut();
            virtual int ComboIn();
            virtual int ComboOutWait();
            virtual int ComboInWait();
            virtual int BallScoreOut(long nBall);
            virtual int BallScoreIn(long nBall);
            virtual int BallScoreOutWait(long nBall);
            virtual int BallScoreInWait(long nBall);

            void SetScore(long nScore);
            void SetTargetScore(long nScore, int bArg);
            void SetHiScore(long nScore, int bArg);
            void SetCombo(long nCombo);
            void SetBonus(long nBonus);
            void SetBonusDisp(float fArg);
            void SetBallScore(long nBall, long nScore);
            // bArg: whether nIcon is not 6 (as CCockpitVolley passes it).
            void SetBallIcon(long nBall, long nIcon, int bArg);
            void SetScorePosition(long nPos);
            void SetComboTelop(long nArg);

        private:
            unsigned char m_Unk60[0x88 - 0x60];
    };
}

#endif /* !CCOCKPITVOLLEYRHYTHMA_HPP_ */
