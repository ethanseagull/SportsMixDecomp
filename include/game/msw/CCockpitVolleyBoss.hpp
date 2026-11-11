#ifndef CCOCKPITVOLLEYBOSS_HPP_
#define CCOCKPITVOLLEYBOSS_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"

namespace msw {

    // Size 0x68 (CCockpitVolley::CreateObjImpl allocates it, object 50):
    // the boss match's board, the boss's life and the time
    // (game/volley/CCockpitVolleyBoss.cpp). Virtuals from the VolleyF
    // vtable; return types and argument names are guesses. The members
    // are not mapped (0x60 and 0x64 are floats; 0x64 is the life).
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

        private:
            unsigned char m_Unk60[0x68 - 0x60];
    };
}

#endif /* !CCOCKPITVOLLEYBOSS_HPP_ */
