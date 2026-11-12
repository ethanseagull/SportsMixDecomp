#ifndef MSW_CCHRDRVPADDODGE_HPP_
#define MSW_CCHRDRVPADDODGE_HPP_

#include "game/msw/CChrDrvPad.hpp"

namespace msw {

    // The pad driver of the dodgeball game (DodgeF), size 0x1A8 (what
    // CChrManDodge::CreateDriver_ allocates). The virtual functions are the
    // slots of __vt__Q23msw15CChrDrvPadDodge; member names are guesses.
    //
    // Key ids (CheckTrg, CheckPress): 1 the KeyID 4 button, 2 to 5 A, B, C,
    // Z, 6 to 8 A+B, A+Z, B+Z, 9 and 10 the second stick in a quarter, 11 to
    // 14 a tap of A, B, C, Z (see m_PressInfo), 15 and 16 KeyID 4 with B and
    // the second stick, 17 to 22 plus, minus and the cross key, 23 and 24
    // the buttons 1 and 2, 25 1+2, 26 and 27 a tap of 1 and 2, 28 B+1 and 29
    // B+2.
    class CChrDrvPadDodge : public CChrDrvPad {
        public:
            CChrDrvPadDodge();
            virtual ~CChrDrvPadDodge();                                    // 0x08
            virtual long OnMsg(MSG_ID id, _CHR_MSG *pMsg);                 // 0x18
            virtual void ClearFunc_();                                     // 0x28
            // The calls to the team mate who holds a ball (CPU driven): pass
            // it to this character, shoot (nType 0 a shot, 1 a sub shot, 2
            // an item ball), feint; then a change of the character played
            // and a switch of the mate's target. Not zero when done.
            virtual int MethdPassCall(float);                              // 0x2C
            virtual int MethdShootCall(long nType);                        // 0x30
            virtual int MethdFeintCall();                                  // 0x34
            virtual int MethdChrChange();                                  // 0x38
            virtual int MethdSwitchTargetCall();                           // 0x3C

            int CheckTrg(long nKey) const;
            int CheckPress(long nKey) const;
            // A swing of kind nType (CheckStrokeRaw), or the kind
            // m_nTrgStroke gives for the key ids held other than nExcept.
            int CheckStroke(long nExcept, long nType) const;
            int CheckStrokeRaw(long nType) const;
            // The index into m_PressInfo of key id nKey (2 to 5, 1, 23, 24),
            // -1 for the others.
            long GetPressInfoIdx(long nKey) const;
            // Forgets the taps of key id nKey (and the Remote swing when
            // bStroke is set) once they have been acted on.
            void ClearStrokeCmd(long nKey, long bStroke);
            void UpdatePressInfo();

            // Swing kinds per key id that CheckStroke combines (the
            // constructor clears them, nothing in the module sets them).
            static long m_nTrgStroke[30];

        protected:
            // A call queued for a few frames (UpdateCmdBuf_): 0 not used, 1
            // MethdPassCall, 2 MethdShootCall with m_nParam.
            struct CMD_BUF {
                float m_fTime; // frames left, negative when empty
                long m_nParam;
                unsigned char m_Unk08[0x24 - 0x08];
            };

            // Bits of PRESS_INFO::m_nFlag.
            enum {
                PRESS_TAP     = 0x01, // pressed, not held long yet
                PRESS_LONG    = 0x02, // held long: set for one frame
                PRESS_RELEASE = 0x04  // pressed, waiting for the release
            };

            // The state of one key id of UpdatePressInfo's nKeyTbl.
            struct PRESS_INFO {
                long m_bPress;        // held this frame
                unsigned long m_nFlag; // PRESS_* bits
                long m_nPressFrame;   // frames held
                long m_nReleaseFrame; // frames released
            };

            void MainDefense_(_CHR_MSG *pMsg);
            int UpdateCmdBuf_();
            void UpdatePressInfoSub_(long nIdx, int bPress);

            // Inline in the original (not emitted; names guessed).
            void ClearPressInfo_();
            void ResetInput_();
            void ClearPressFlag_(long nKey);
            CMD_BUF *SetCmdBuf_(long nIdx, float fFrame);
            int IsTap_(long nKey) const;
            int IsReleaseTap_(long nKey) const;
            unsigned long GetPressFlag_(long nKey) const;
            bool IsStroke_(long nType) const;
            bool IsTrgKey_(long nKey) const;
            bool IsPressStroke_(long nKey) const;
            long GetCatchType_() const;
            long GetPassSide_() const;

            // 0x9C: the stick direction the target was last switched at, -1
            // for none (message 0x3B).
            float m_fSwitchDir;
            float m_fUnkA0;               // 0xA0, only set by the constructor
            unsigned char m_UnkA4[0xA8 - 0xA4];
            long m_bCmdBufReady;          // 0xA8, 0 while UpdateCmdBuf_ runs
            CMD_BUF m_CmdBuf[3];          // 0xAC
            PRESS_INFO m_PressInfo[7];    // 0x118
            // 0x188: the stick pushed far (set at 0.8, cleared under 0.3),
            // and set once it was; the frames pushed and in the centre; the
            // last direction it was pushed far in.
            long m_bStickOn;
            long m_bStickOnce;
            long m_nStickOnFrame;
            long m_nStickOffFrame;
            float m_fStickOnDir;
            long m_bCatchReady;           // 0x19C, set each frame by MainDefense_
            long m_nFrame;                // 0x1A0, frames run since ClearFunc_
            long m_bOuterAssigned;        // 0x1A4, the character's IsOuterAssigned
    };
}

#endif /* !MSW_CCHRDRVPADDODGE_HPP_ */
