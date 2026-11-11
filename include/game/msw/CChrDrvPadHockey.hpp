#ifndef MSW_CCHRDRVPADHOCKEY_HPP_
#define MSW_CCHRDRVPADHOCKEY_HPP_

#include "game/msw/CChrDrvPad.hpp"

namespace msw {

    // The pad driver of the hockey game (HockeyF), size 0x1A8 (what
    // CChrManHockey::CreateDriver_ allocates). Member names are guesses.
    class CChrDrvPadHockey : public CChrDrvPad {
        public:
            CChrDrvPadHockey();
            virtual ~CChrDrvPadHockey();
            virtual long OnMsg(MSG_ID id, _CHR_MSG *pMsg);
            virtual void UpdateFunc_();
            virtual void ClearFunc_();

            long OnMsgMain(_CHR_MSG *pMsg);
            // Follows the stick round a circle (m_nStickRoll gets ROLL_*).
            void CheckStickParam();
            // Key id nKey pressed this frame, or held. The ids are those of
            // the switch in CheckTrg: 1 the KeyID 4 button, 4 to 7 A, B, C,
            // Z, 8 to 10 two of them together, 2/3 and 15 to 18 that button
            // with the second stick in a quarter, 11 to 14 (CheckPress only)
            // the release of a button tracked by m_PressInfo.
            int CheckTrg(long nKey) const;
            int CheckPress(long nKey) const;
            // A swing of kind nType (CheckStrokeRaw), or the kind
            // m_nTrgStroke gives for the key ids held other than nExcept.
            int CheckStroke(long nExcept, long nType) const;
            // 1 the Remote, 2 vertical, 3 horizontal, 4 and 5 the kinds 3
            // and 0 of m_nStrokeType, 6 the Nunchuk, 7 both hands.
            int CheckStrokeRaw(long nType) const;

            // Swing kinds per key id that CheckStroke combines (19 ids; the
            // constructor clears them, nothing in the module sets them).
            static long m_nTrgStroke[19];

        protected:
            // Bits of m_nInputFlag.
            enum {
                INPUT_TACKLE = 0x01, // 2 pressed, waiting for the release (tackle, smash)
                INPUT_ITEM   = 0x02, // B (Nunchuk) pressed for an item shot
                INPUT_ARTS   = 0x04, // 1+2 / A+B: special move
                INPUT_CHANGE = 0x08  // B (Remote) held for a change of character
            };

            // Bits of m_nStickRoll, set by CheckStickParam for one frame.
            enum {
                ROLL_RIGHT = 0x08,
                ROLL_LEFT  = 0x10
            };

            // Bits of m_nStickFlag.
            enum {
                STICK_ON    = 0x01, // the stick left the centre
                STICK_RIGHT = 0x02, // turning one way
                STICK_LEFT  = 0x04, // turning the other way
                STICK_HALF  = 0x08  // past half a turn
            };

            // A count of what UpdateCmdBuf_ steps (0x24 bytes; only the
            // timer is known).
            struct CMD_BUF {
                float m_fTime;
                unsigned char m_Unk04[0x24 - 0x04];
            };

            // The state of one button of nKeyTbl, kept by OnMsgMain.
            struct PRESS_INFO {
                long m_bPress;       // held this frame
                long m_bPressed;     // was pressed once
                long m_nPressFrame;  // frames held
                long m_nReleaseFrame; // frames released
            };

            void MainOffenseOnBall_(_CHR_MSG *pMsg);
            void MainOffense_(_CHR_MSG *pMsg);
            void MainDefense_(_CHR_MSG *pMsg);
            void MainCrashSkate_(_CHR_MSG *pMsg);
            int MethdPassCall_(long);
            int DefenseTackle_();
            int KSAttack_();
            int IsItem_();
            int UpdateCmdBuf_();

            // Inline in the original (not emitted; names guessed).
            void ClearFlick_();
            void ClearStickParam_();
            void UpdatePressInfo();
            void Move_(float fDir);
            int IsChgLeaderInput_();
            int IsArtsInput_();
            int IsKeeperInput_();
            int IsFeintInput_();
            int IsPassInput_();
            int IsStanceInput_();
            int IsStance2Input_();
            int IsItemBallInput_();
            int IsShootInput_();
            int IsShoot2Input_();
            int IsSwingInput_();
            int IsTrgA2_();
            int IsPressA1_();
            int IsPressAZ2_();
            int IsPressRelease_(long nIdx) const;
            bool IsStroke_() const;
            bool IsPressKey_(long nKey) const;
            bool IsTrgKey_(long nKey) const;

            unsigned short m_nUnk9C;      // 0x9C, cleared by the constructor
            // 0xA0: frames the stick has been in the centre, and frames it
            // has been pushed.
            float m_fNeutralFrame;
            float m_fTiltFrame;
            // 0xA8: the direction of a quick push of the stick (pushed and
            // let go within 14 frames), -1 for none; 0xAC the last
            // direction pushed.
            float m_fFlickDir;
            float m_fTiltDir;
            long m_nVibPattern;           // 0xB0, the rumble started by message 0x53, -1 for none
            unsigned long m_nInputFlag;   // 0xB4, INPUT_* bits
            // 0xB8: frames each button has been held (A, B, 1, 2).
            float m_fHoldA;
            float m_fHoldB;
            float m_fHold1;
            float m_fHold2;
            unsigned long m_nStickRoll;   // 0xC8, ROLL_* bits
            unsigned long m_nStickFlag;   // 0xCC, STICK_* bits
            unsigned long m_nUnkD0;       // 0xD0, only cleared
            // 0xD4: the eighths of a turn the stick started in, as bits.
            unsigned long m_nStickArea;
            // 0xD8: frames left to go on turning; frames since the stick
            // left the centre.
            float m_fStickEnableFrame;
            float m_fStickFrame;
            // 0xE0: the stick angle (degrees) where it left the centre, and
            // the last one counted.
            float m_fStickStartDeg;
            float m_fStickLastDeg;
            long m_bCmdBufReady;          // 0xE8, 0 while UpdateCmdBuf_ runs
            CMD_BUF m_CmdBuf[3];          // 0xEC
            PRESS_INFO m_PressInfo[5];    // 0x158
    };
}

#endif /* !MSW_CCHRDRVPADHOCKEY_HPP_ */
