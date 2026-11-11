#ifndef CCOCKPITDODGELIFEGAUGE_HPP_
#define CCOCKPITDODGELIFEGAUGE_HPP_

#include "game/msw/CCockpitDodgeLayoutBase.hpp"

namespace msw {

    // Size 0xC4 (CCockpitDodge::CreateObjImpl allocates it). The dodge
    // game's life gauges, one per character of each team, with their face
    // icons and charge gauges (DodgeF). It overrides the gauge slides of
    // CCockpitDodgeLayoutBase. Only what CCockpitDodge calls so far; the
    // members are not mapped. Return types and argument names are guesses;
    // nIdx is a team colour, nNo the character's number in the team.
    class CCockpitDodgeLifeGauge : public CCockpitDodgeLayoutBase {
        public:
            CCockpitDodgeLifeGauge(long nId);
            virtual ~CCockpitDodgeLifeGauge();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual int Set();
            virtual void Remove();

            void SetLifeDisp(int bDisp);
            void SetLifeValue(long nIdx, long nNo, long nLife, long nMax);
            void SetLifeValue(long nIdx, long nNo, float fRate, float fRate2);
            void SetLifeValueText(long nIdx, long nNo, unsigned short nLife);
            void AddLifeValue(long nIdx, long nNo, long nPrevLife, long nLife, long, long);
            void SetLifeAnim(long nIdx, long nNo, long nAnim, long, long);
            void ChangeChargeGauge(long nIdx, long nNo, float fRate, float fRate2);
            void SetFaceIcon();

        private:
            unsigned char m_Unk60[0xC4 - 0x60];
    };
}

#endif /* !CCOCKPITDODGELIFEGAUGE_HPP_ */
