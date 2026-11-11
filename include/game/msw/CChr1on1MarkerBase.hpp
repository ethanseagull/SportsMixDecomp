#ifndef MSW_CCHR1ON1MARKERBASE_HPP_
#define MSW_CCHR1ON1MARKERBASE_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CUnivTask.hpp"

namespace bcs {
    struct BCS_Color;
}

namespace msw {

    // A marker over one character (the one-on-one games). A DOL class; the
    // modules derive their own (HockeyF CChr1on1MarkerHockey, BasketF
    // CChrMoveBlockMarkerBasket). The state machine is a second base at
    // 0x34 (its vptr at 0x78, as in CChrMarker). The virtual functions
    // follow __vt__Q23msw20CChr1on1MarkerHockey; return types are guesses
    // unless a body shows them. Own members (0x7C on) are not mapped; the
    // hockey marker ends at 0x9C.
    class CChr1on1MarkerBase : public bcs::CUnivTask, public bcs::CFsm<CChr1on1MarkerBase> {
        public:
            CChr1on1MarkerBase(bcs::CTask *pParent, long nChr);
            virtual ~CChr1on1MarkerBase();
            virtual void Main();
            virtual void Disp();

            virtual void Create();
            // 3 in HockeyF (meaning not known).
            virtual int CheckAppear();
            virtual float GetAppearFrameNum() const;
            virtual float GetDisappearFrameNum() const;
            virtual float GetDispScale() const;
            virtual void GetColor_(bcs::BCS_Color &rColor0, bcs::BCS_Color &rColor1, long nIdx);
            virtual void UpdateColor_();

        protected:
            // 0x7C to 0x88 named from BasketF CChrMoveBlockMarkerBasket::
            // CheckAppear (it places the marker; names guessed).
            float m_fPosX;          // 0x7C
            float m_fPosY;          // 0x80
            float m_fPosZ;          // 0x84
            float m_fRotY;          // 0x88
            void *m_Unk8C;          // 0x8C
            long m_nChr;            // 0x90, passed to CChrMan::GetChrCore
            unsigned char m_Unk94[0x9C - 0x94];
    };
}

#endif /* !MSW_CCHR1ON1MARKERBASE_HPP_ */
