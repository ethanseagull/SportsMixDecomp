#ifndef MSW_CCHRDRVPAD_HPP_
#define MSW_CCHRDRVPAD_HPP_

#include "game/msw/CChrDrv.hpp"

namespace msw {

    // The driver of a character played with a pad. A DOL class, size 0x9C
    // (its constructor writes up to 0x98; CChrDrvPadBasket's members start
    // at 0x9C). Each sport derives its own (CChrDrvPadBasket, ...).
    //
    // INCOMPLETE: added for BasketF game/basket/CChrDrvPadBasket.cpp. The
    // vtable (lbl_8046E050 in the DOL) has OnMsg pure; GetDrvType and
    // GetPadType are inline (every module emits them weak). Member names
    // and the meanings of the Unk members are guesses.
    class CChrDrvPad : public CChrDrv {
        public:
            CChrDrvPad();
            virtual ~CChrDrvPad();
            virtual void MsgFunc(CChrCore *pChr, MSG_ID id, _CHR_MSG *pMsg);
            // pool-off stand-in (a guess, behaviour-free): the unused float
            // local turns off the float literal pool in every unit that
            // includes this header, as the original's per-literal lis/lfs
            // shows (BasketF CChrDrvPadBasket 88.9% -> 97.0%). The real inline
            // with a float local that did this is not known; see
            // notes/toolchain.md ("static local inside an inline").
            virtual long GetDrvType() const { const float k = 0.5f; return 1; }
            virtual long GetPadType() const { return m_nPadType; }
            virtual long OnMsg(MSG_ID id, _CHR_MSG *pMsg) = 0;          // 0x18
            virtual void UpdateFunc_();                                 // 0x1C
            virtual long GetStrokeResult_();                            // 0x20
            virtual void ClearStroke_(float fFrame);                    // 0x24
            virtual void ClearFunc_();                                  // 0x28

            // DOL. Button state of the pad: A, B, C, Z, 1 and 2 held,
            // pressed this frame, released this frame.
            int IsPressA() const;
            int IsTrgA() const;
            int IsReleaseA() const;
            int IsPressB() const;
            int IsTrgB() const;
            int IsReleaseB() const;
            int IsPressC() const;
            int IsTrgC() const;
            int IsReleaseC() const;
            int IsPressZ() const;
            int IsTrgZ() const;
            int IsReleaseZ() const;
            // The same by key number (0: A, 3: Z, 4: the cross key; others
            // not checked).
            int IsTrgKeyID(long nKey) const;
            int IsPressKeyID(long nKey) const;
            // DOL. A swing of the remote harder than fPower.
            int IsRemoconStroke(float fPower) const;

        protected:
            // DOL. Reads the pad into the members below.
            void UpdatePadState();
            void ClearWork_();
            // Starts vibration pattern nType.
            void VibStart_(long, long nType, long);

            CChrCore *m_pChr;          // 0x04, the character driven
            long m_nPadType;           // 0x08, 0 for one pad type (the classic style?), else another
            long m_nPadNo;             // 0x0C, the pad read (bcs::CWifiPad number)
            float m_fStickDir;         // 0x10, stick direction in radians, negative when the stick is idle
            float m_fUnk14;
            float m_fStickPower;       // 0x18, how far the stick is pushed (0 to 1)
            float m_fCrossDir;         // 0x1C, cross key direction in radians, negative when none
            float m_fUnk20;
            long m_nUnk24;
            long m_nUnk28;
            long m_nUnk2C;
            long m_nStroke;            // 0x30, the stroke (swing) recognised, -1 for none
            long m_nUnk34;
            float m_fUnk38;
            long m_nUnk3C[13];
            float m_fUnk70[4];
            long m_nUnk80;
            unsigned long m_nFlag84;   // 0x84
            long m_nUnk88[3];
            long m_nUnk94;
            long m_nUnk98;
    };
}

#endif /* !MSW_CCHRDRVPAD_HPP_ */
