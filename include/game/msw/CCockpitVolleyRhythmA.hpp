#ifndef CCOCKPITVOLLEYRHYTHMA_HPP_
#define CCOCKPITVOLLEYRHYTHMA_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"

namespace msw {

    // Size 0x88 (CCockpitVolley::CreateObjImpl allocates it, object 52):
    // the rhythm game's score board, combo and ball scores
    // (game/volley/CCockpitVolleyRhythmA.cpp). Virtuals from the VolleyF
    // vtable; return types and argument names are guesses.
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

            // The Set* functions clamp their value and return the clamped
            // one (SetBonusDisp returns 0).
            long SetScore(long nScore);
            // bReset: count the displayed value up from the start again.
            long SetTargetScore(long nScore, int bReset);
            long SetHiScore(long nScore, int bReset);
            long SetCombo(long nCombo);
            long SetBonus(long nBonus);
            long SetBonusDisp(float fSpeed);
            long SetBallScore(long nBall, long nScore);
            // bArg: whether nIcon is not 6 (as CCockpitVolley passes it).
            void SetBallIcon(long nBall, long nIcon, int bArg);
            void SetScorePosition(long nPos);
            // Shows combo telop nTelop (0-2), none for any other value.
            void SetComboTelop(long nTelop);

        private:
            void StateControl();
            // Counts the displayed numbers towards their values.
            void CountUpCheck();
            // Inline: shows min(fNew, fValue) in pane nPane and keeps it in
            // rDisp.
            void SetCount(long nPane, float &rDisp, float fNew, float fValue);
            void SetAnimScore(long nAnim);
            void SetAnimCombo(long nAnim);
            void SetAnimBonus(long nAnim, float fSpeed);
            // Inline (only their static constants have symbols).
            void SetAnimBallScore(long nBall, long nAnim);
            void SetAnimComboString(long nAnim);

            // A number shown on the board: fDisp moves towards fValue.
            struct Counter {
                float fDisp;  // 0x0
                float fValue; // 0x4
            };

            Counter m_Score;         // 0x60
            Counter m_HiScore;       // 0x68
            Counter m_TargetScore;   // 0x70
            Counter m_Combo;         // 0x78
            float m_fBonusSpeed;     // 0x80, speed of the bonus animation
            unsigned char m_nBallFlag; // 0x84, bit per ball: its icon is shown
    };
}

#endif /* !CCOCKPITVOLLEYRHYTHMA_HPP_ */
