#ifndef CCOCKPITVOLLEYLIFEGAUGE_HPP_
#define CCOCKPITVOLLEYLIFEGAUGE_HPP_

#include "game/msw/CCockpitVolleyLayoutBase.hpp"

namespace msw {

    // Size 0x68 (CCockpitVolley::CreateObjImpl allocates it, object 51):
    // the life gauge of the boss match (game/volley/CCockpitVolleyBoss.cpp).
    // Virtuals from the VolleyF vtable; return types and argument names
    // are guesses. The members are not mapped (0x60 is a float).
    class CCockpitVolleyLifeGauge : public CCockpitVolleyLayoutBase {
        public:
            CCockpitVolleyLifeGauge(long nId, float fSortZ);
            virtual ~CCockpitVolleyLifeGauge();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();
            virtual int ScoreOut();
            virtual int ScoreIn();
            virtual int ScoreOutWait();
            virtual int ScoreInWait();

            // nIdx is a team colour, nNo a character of the team.
            void SetLifeValue(long nIdx, long nNo, long nLife);

        private:
            unsigned char m_Unk60[0x68 - 0x60];
    };
}

#endif /* !CCOCKPITVOLLEYLIFEGAUGE_HPP_ */
