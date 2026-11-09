#ifndef CSELECTMENULAYOUTBASE_HPP_
#define CSELECTMENULAYOUTBASE_HPP_

#include "game/msw/CMenuLinkBase.hpp"
#include "game/msw/CMenuLytObj.hpp"

namespace bcs {
    struct FVEC3;
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace msw {

    // Size 0x5C. Base of the SelectMenu screens. CMenuLinkBase is at 0 (vptr
    // at 0x18), CMenuLytObj at 0x1C (vptr at 0x28), own members from 0x2C.
    // Only the members used so far; names and return types are guesses.
    class CSelectMenuLayoutBase : public CMenuLinkBase, public CMenuLytObj<bcs::G2D::CPaneCtrl, 1> {
        public:
            CSelectMenuLayoutBase(long nId);
            virtual ~CSelectMenuLayoutBase();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();
            virtual void SetVisible(int bVisible);

            virtual void CloseMenu();
            virtual void CallbackButtonAnimIn(long);
            virtual void CallbackButtonAnimOut(long);
            virtual void CallbackButtonAnimPush(long);
            virtual void CallbackButtonAnimCancel(long);
            virtual void CallbackButtonAnimMultiIn(long, long);
            virtual void CallbackButtonAnimMultiOut(long, long);
            virtual void HitCheck(bcs::FVEC3 *);
            virtual void SetCursorMarkerPosition(long, long);
            virtual void ClearCursorMarkerPosition(long, long);
            virtual void SetCursorMarkerVisible(long, int);
            virtual void StickMoveCheck(long, long);
            virtual void SetDefaultMoveCursor(long, long);
            virtual void GetOutAnimFrameSize();
            virtual void PushButton(long, long);
            virtual void IsMarkerOnPointer(long, long);
            virtual void IsButtonKindOfStickMode(long);

        protected:
            unsigned long m_Unk2C; // 0x2C
            unsigned long m_Unk30; // 0x30
            unsigned char m_Unk34[0x28];
    };
}

#endif /* !CSELECTMENULAYOUTBASE_HPP_ */
