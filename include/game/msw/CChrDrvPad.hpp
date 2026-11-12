#ifndef MSW_CCHRDRVPAD_HPP_
#define MSW_CCHRDRVPAD_HPP_

#include "game/msw/CChrDrv.hpp"

namespace msw {

    class CChrCore;

    // The driver of a character played from a Wii Remote. A DOL class; each
    // game module derives its own (CChrDrvPadBasket, CChrDrvPadBoss,
    // CChrDrvPadHockey, CChrDrvPadVolley). Size 0x9C: CChrDrvPadBoss's
    // constructor clears its own members from 0x9C on, CChrDrvPadBasket's
    // members start at 0x9C, and CChrDrvPad::ClearFunc_ writes up to 0x98.
    //
    // INCOMPLETE: the members are mapped only as far as the module pad
    // drivers read them; names are guesses. The virtual functions are the
    // slots of __vt__Q23msw14CChrDrvPadBoss (BossF); the DOL's own vtable
    // (lbl_8046E050) has OnMsg pure (BasketF). Return types of the ones
    // without a body are guesses.
    class CChrDrvPad : public CChrDrv {
        public:
            CChrDrvPad();

            virtual ~CChrDrvPad();                                         // 0x08
            virtual void MsgFunc(CChrCore *pChr, MSG_ID id, _CHR_MSG *pMsg); // 0x0C
            // Both inline; every module emits them as weak functions.
            //
            // (2026-11-10 land: BasketF's "pool-off stand-in", an unused
            // `const float k = 0.5f;` in GetDrvType, was dropped here. It
            // raised BasketF CChrDrvPadBasket 88.9% -> 97.0% but lowered
            // BossF CChrDrvPadBoss 99.9 -> 93.2, HockeyF CChrDrvPadHockey
            // 95.4 -> 88.2 and VolleyF CChrDrvPadVolley 99.8 -> 93.7: those
            // originals keep the float pool, the basketball driver's does
            // not. 2026-11-11: the stand-in now lives in
            // CChrDrvPadBasket.cpp; see notes/toolchain.md "Float literal pool".)
            virtual long GetDrvType() const { return 1; } // 0x10
            virtual long GetPadType() const { return m_nPadType; }         // 0x14
            // The answer to message id (what the character asks the pad).
            // Pure in the DOL's CChrDrvPad vtable (lbl_8046E050).
            virtual long OnMsg(MSG_ID id, _CHR_MSG *pMsg) = 0;             // 0x18
            virtual void UpdateFunc_();                                    // 0x1C
            virtual long GetStrokeResult_();                               // 0x20
            // Forgets the Remote swing for fFrame frames (meaning guessed).
            virtual void ClearStroke_(float fFrame);                       // 0x24
            virtual void ClearFunc_();                                     // 0x28

            // All in the DOL (return types guessed). The button states of
            // the pad: held, pressed this frame, released this frame.
            int IsPress1() const;
            int IsPress2() const;
            int IsPressA() const;
            int IsPressB() const;
            int IsPressC() const;
            int IsPressZ() const;
            int IsTrg1() const;
            int IsTrg2() const;
            int IsTrgA() const;
            int IsTrgB() const;
            int IsTrgC() const;
            int IsTrgZ() const;
            int IsRelease1() const;
            int IsRelease2() const;
            int IsReleaseA() const;
            int IsReleaseB() const;
            int IsReleaseC() const;
            int IsReleaseZ() const;
            // Not zero when the Remote was swung harder than fPower.
            int IsRemoconStroke(float fPower) const;
            // How hard the Remote and the Nunchuk are shaken.
            float GetScratchPower() const;
            float GetNunchukPower() const;
            // (name guessed) Inline; see m_fStickDir.
            float GetStickDir() const { return m_fStickDir; }

            // Added for HockeyF game/hockey/CChrDrvPadHockey.cpp (DOL;
            // names from the DOL, return types guessed). Not zero for the
            // Remote with the Nunchuk (m_nPadType 1).
            int IsPadTypeAnalogStick_() const;
            // Key nKey (an index into a DOL table per pad type) pressed this
            // frame, or held. BasketF: 0 is A, 3 is Z, 4 the cross key.
            int IsTrgKeyID(long nKey) const;
            int IsPressKeyID(long nKey) const;

        protected:
            // DOL. UpdatePadState reads the pad into the members below.
            void ClearWork_();
            void UpdatePadState();
            // Starts vibration pattern nType. (BossF named the first
            // argument nType; BasketF passes 1 there and the pattern, 0 to
            // 0xB, second. Meanings unproven.)
            void VibStart_(long, long nType, long);
            void VibStop_(long nType);

            // (0x04, 0x08 and 0x58 might belong to CChrDrv; CComDrv is not
            // mapped yet.)
            CChrCore *m_pChr;            // 0x04, the character driven
            // 0x08: 0 for the Remote held sideways (buttons 1 and 2), 1 for
            // the Remote with the Nunchuk (A, B, C, Z and swings).
            long m_nPadType;
            // 0x0C (BasketF): the pad read (bcs::CWifiPad number).
            long m_nPadNo;
            // 0x10: the stick direction in radians, negative while the stick
            // is in the centre.
            float m_fStickDir;
            float m_Unk14;
            float m_fStickPower;         // 0x18, how far the stick is pushed (0 to 1)
            // 0x1C (name guessed): a second stick direction in radians,
            // negative while the stick is in the centre. CChrDrvPadHockey
            // cuts it into quarters for its key ids 2, 3 and 15 to 18;
            // BasketF took it for the cross key's direction.
            float m_fStickDir2;
            unsigned char m_Unk20[0x28 - 0x20];
            // 0x28 (name and meaning guessed): the buttons held, as WPAD
            // bits; VolleyF's serve reads 4 and 8 (down and up on the pad).
            unsigned long m_nButton;
            unsigned char m_Unk2C[0x30 - 0x2C];
            // 0x30 (name guessed): set by GetStrokeResult_, the stroke
            // (swing) recognised, -1 for none. CChrDrvPadHockey::CheckStrokeRaw
            // tests it for 3 and 0; CChrDrvPadBasket reads 0 to 3 as four
            // swing directions.
            long m_nStrokeType;
            long m_nUnk34;
            float m_fUnk38;
            long m_nUnk3C[(0x54 - 0x3C) / 4];
            // 0x54 (name guessed): the kind of Remote swing, -1 for none;
            // VolleyF's serve tells 0 and 3 apart (6 is ignored as with
            // m_nStroke).
            long m_nStrokeKind;
            // 0x58: the direction of the last Remote swing, -1 for none.
            long m_nStroke;
            long m_nUnk5C[(0x70 - 0x5C) / 4];
            float m_fUnk70[4];
            long m_nUnk80;
            unsigned long m_nFlag84;     // 0x84 (BasketF)
            long m_nUnk88[3];
            long m_nUnk94;               // 0x94 (BasketF)
            long m_nUnk98;
    };
}

#endif /* !MSW_CCHRDRVPAD_HPP_ */
