#ifndef CCOCKPITVOLLEYBOSS_HPP_
#define CCOCKPITVOLLEYBOSS_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"

namespace msw {

    // Size 0x68 (CCockpitVolley::CreateObjImpl allocates it, object 50):
    // the boss match's board, the boss's life and the time
    // (game/volley/CCockpitVolleyBoss.cpp). Virtuals from the VolleyF
    // vtable; return types and argument names are guesses.
    class CCockpitVolleyBoss : public CCockpitVolleyLayoutBase {
        public:
            CCockpitVolleyBoss(long nId, float fSortZ);
            virtual ~CCockpitVolleyBoss();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual int Set();
            virtual void Remove();
            virtual long CancelMenu();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            void SetCoinState(long nState);
            // fRate: the boss's life (ratio guessed).
            void SetBossLife(float fRate);
            void SetTimeValue(float fTime, int nArg, int bForce);
            // Slide animations of the gauge (Ga_00 to _03 on Boss_HP and
            // line_L), the line (HP_line on the four line panes or on
            // line_bar), the coin and the special coin. SetAnimLineBar,
            // SetAnimCoin and SetAnimCoinSpecial are only inlined.
            void SetAnimGauge(long nAnim);
            void SetAnimHPLine(long nAnim);
            void SetAnimLineBar(long nAnim);
            void SetAnimCoin(long nAnim);
            void SetAnimCoinSpecial(long nAnim);

        private:
            float m_fLayoutSortZ; // 0x60, from the constructor (SetSortZ in Set)
            float m_fLifeFrame;   // 0x64, the frame count of the life animation
    };
}

#endif /* !CCOCKPITVOLLEYBOSS_HPP_ */
