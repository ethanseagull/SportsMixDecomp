#ifndef MSW_CCHRDRVPADBASKET_HPP_
#define MSW_CCHRDRVPADBASKET_HPP_

#include "bcs/CWifiPad.hpp"
#include "game/msw/CChrCoreBasket.hpp"
#include "game/msw/CChrDrvPad.hpp"
#include "game/msw/CChrManBasket.hpp"
#include "game/msw/CGame.hpp"

namespace msw {

    namespace BASKET {

        // Tracks the stick of the basketball pad driver. Size 0x0C. The
        // state bits (names guessed): 1 the stick is pushed past the
        // threshold this frame (first frame), 2 it stays pushed, 4 it was
        // let go this frame.
        class CStickState {
            public:
                CStickState() : m_nState(0), m_fThreshold(0.5f) {}
                virtual ~CStickState();
                virtual void Reset();
                virtual void UpDate(float fDir, float fPower, float fFrame);

                unsigned long GetState() const { return m_nState; }

            protected:
                unsigned long m_nState; // 0x04
                float m_fThreshold;     // 0x08, the power counted as pushed
        };

        // Two quick pushes of the stick in about the same direction (bit
        // 0x20 of the state for one frame). Size 0x18.
        class CStickState2Tap : public CStickState {
            public:
                CStickState2Tap() { m_fThreshold = 0.85f; }
                virtual ~CStickState2Tap();
                virtual void Reset();
                virtual void UpDate(float fDir, float fPower, float fFrame);

            protected:
                void Clear_()
                {
                    m_nStep = 0;
                    m_fTime = 0.0f;
                    m_fDir = -1.0f;
                    m_nState &= ~0x20;
                }

                long m_nStep;  // 0x0C, 0 to 2, 999 while held too long
                float m_fTime; // 0x10, frames in the step
                float m_fDir;  // 0x14, direction of the first push, -1 for none
        };

        // A roll of the stick through more than 150 degrees one way (bit 8
        // of the state for one direction, 0x10 for the other). Size 0x24.
        class CStickStateRoll : public CStickState {
            public:
                CStickStateRoll()
                {
                    m_nStep = 0;
                    m_fTimer = 0.0f;
                    m_fTime = 0.0f;
                    m_fAngle = 0.0f;
                    m_fStartDeg = -1.0f;
                    m_fLastDeg = -1.0f;
                }
                virtual ~CStickStateRoll();
                virtual void Reset();
                virtual void UpDate(float fDir, float fPower, float fFrame);

            protected:
                void Clear_()
                {
                    m_fTimer = 0.0f;
                    m_fTime = 0.0f;
                    m_nStep = 0;
                    m_fStartDeg = -1.0f;
                    m_fLastDeg = -1.0f;
                    m_fAngle = 0.0f;
                }

                // 0x0C: bit 1 started, 2 and 4 the way it turns
                unsigned long m_nStep;
                float m_fTimer;    // 0x10, frames left to keep turning
                float m_fTime;     // 0x14, frames since the start
                float m_fAngle;    // 0x18, radians turned so far
                float m_fStartDeg; // 0x1C, degrees at the start, -1 for none
                float m_fLastDeg;  // 0x20, degrees last frame, -1 for none
        };
    }

    // The pad driver of the basketball game (BasketF). Size 0xF8 (the last
    // member ends there). Member names are guesses.
    class CChrDrvPadBasket : public CChrDrvPad {
        public:
            CChrDrvPadBasket();
            virtual ~CChrDrvPadBasket();
            virtual long OnMsg(MSG_ID id, _CHR_MSG *pMsg);
            virtual void ClearFunc_();

            int CheckTrg(long nCmd) const;
            int CheckStroke(long nCmd, long nStroke) const;
            int CheckStrokeRaw(long nStroke) const;

            // A table of 42 key bits, one per command; nothing in this file
            // sets an entry, the constructor clears them all.
            static long m_nTrgStroke[42];

        protected:
            long OnMsgMain(_CHR_MSG *pMsg);
            long OnMsgMain_(_CHR_MSG *pMsg);
            void UpDateCross2Tap();
            void UpDateMyKeyState(long nKey, int bPress);
            void MainOffense_(_CHR_MSG *pMsg);
            void SubOffDribble_();
            void SubOffNotHaveBall_();
            void SubDefNormal_();
            int CheckCmd_(long nCmd, long nStroke) const;
            int MethdPass();
            int MethdTrickMove();
            int MethdShoot();
            long CheckArtsChrInput(const _CHR_MSG *pMsg);

            // (inline helpers; names and the split into helpers are
            // guesses, the bodies are what every caller repeats)

            CChrCoreBasket *GetChr() const { return static_cast<CChrCoreBasket *>(m_pChr); }

            // The command check of the pad type: nCmd0 and nStroke0 with
            // pad type 0, nCmd1 and nStroke1 with the other.
            int IsCmd_(long nCmd0, long nStroke0, long nCmd1, long nStroke1) const
            {
                if (m_nPadType == 0) {
                    if (CheckCmd_(nCmd0, nStroke0)) {
                        return 1;
                    }
                    return 0;
                }
                return CheckCmd_(nCmd1, nStroke1) != 0;
            }

            // The jump command; with pad type 0 a stroke counts too while
            // bit 3 of the game flags is set.
            int IsJumpStroke_() const
            {
                if (!(CGame::g_GameWork.m_nFlag18 & 8)) {
                    return 0;
                }
                if (m_nPadType != 0) {
                    return 0;
                }
                return CheckCmd_(0, 1) != 0;
            }

            int IsJumpCmd_() const
            {
                if (m_nPadType == 0) {
                    if (IsJumpStroke_() || CheckCmd_(0x23, 0)) {
                        return 1;
                    }
                    return 0;
                }
                return CheckCmd_(0, 1) != 0;
            }

            int IsYokoStroke_() const
            {
                if (m_pChr != 0) {
                    return bcs::CWifiPad::IsYokoStroke(m_pChr->GetPadNo());
                }
                return 0;
            }

            int IsCrossInput_() const { return m_fStickDir2 >= 0.0f; }

            // The cross key as 1 to 4 (by quarter; meanings not checked),
            // 0 when it is not pushed.
            long GetCrossDir_() const
            {
                float fDeg;

                if (!IsCrossInput_()) {
                    return 0;
                }
                fDeg = 57.295776f * m_fStickDir2;
                if (fDeg >= 45.0f && fDeg <= 135.0f) {
                    return 2;
                }
                if (fDeg >= 135.0f && fDeg <= 225.0f) {
                    return 3;
                }
                if (fDeg >= 225.0f && fDeg <= 315.0f) {
                    return 4;
                }
                return 1;
            }

            // Key nKey (a command number up to 16) is held.
            int IsPress_(long nKey) const
            {
                switch (nKey) {
                    case 8:
                        return IsPressKeyID(4);
                    case 1:
                        return IsPressA();
                    case 2:
                        return IsPressB();
                    case 3:
                        return IsPressC();
                    case 4:
                        return IsPressZ();
                    case 5: {
                        int bRet = 0;
                        if (IsPressA() && IsPressB()) {
                            bRet = 1;
                        }
                        return bRet;
                    }
                    case 6: {
                        int bRet = 0;
                        if (IsPressA() && IsPressZ()) {
                            bRet = 1;
                        }
                        return bRet;
                    }
                    case 7: {
                        int bRet = 0;
                        if (IsPressB() && IsPressZ()) {
                            bRet = 1;
                        }
                        return bRet;
                    }
                    case 13:
                        return bcs::CWifiPad::IsDown(0x1000, m_nPadNo);
                    case 14:
                        return bcs::CWifiPad::IsDown(0x10, m_nPadNo);
                    case 15:
                        return bcs::CWifiPad::IsDown(0x200, m_nPadNo);
                    case 16:
                        return bcs::CWifiPad::IsDown(0x100, m_nPadNo);
                }
                return 0;
            }

            // Release commands 0x18 to 0x1D.
            int IsRelease_(long nCmd) const
            {
                switch (nCmd) {
                    case 0x18:
                        return IsReleaseA();
                    case 0x19:
                        return IsReleaseB();
                    case 0x1A:
                        return IsReleaseC();
                    case 0x1B:
                        return IsReleaseZ();
                    case 0x1C:
                        return bcs::CWifiPad::IsRelease(0x200, m_nPadNo);
                    case 0x1D:
                        return bcs::CWifiPad::IsRelease(0x100, m_nPadNo);
                }
                return 0;
            }

            // An angle brought into [0, 2 pi).
            static float RoundRad_(float fRad)
            {
                while (fRad > 6.2831855f) {
                    fRad -= 6.2831855f;
                }
                while (fRad < 0.0f) {
                    fRad += 6.2831855f;
                }
                return fRad;
            }

            // The stick direction turned to the court, or -1 when the
            // stick is idle.
            float GetMoveRotY_() const
            {
                float fRot = -1.0f;
                if (m_fStickDir >= 0.0f) {
                    fRot = RoundRad_(m_fStickDir + m_pChr->m_fCamRotY);
                }
                return fRot;
            }

            // The same, the character's facing when the stick is idle.
            float GetInputRotY_() const
            {
                float fRotY = m_pChr->m_fCamRotY;
                float fRot = m_pChr->GetRotY();
                if (m_fStickDir >= 0.0f) {
                    fRot = RoundRad_(fRotY + m_fStickDir);
                }
                return fRot;
            }

            // The change-character command, while the team may change.
            int IsChgLeaderCmd_() const
            {
                if (!IsCmd_(0x1F, 0, 3, 0)) {
                    return 0;
                }
                return CChrManBasket::IsChgCharacterInputEnable(m_pChr->GetTeamNo()) != 0;
            }

            // Change to another character of the team.
            int ChgLeader_()
            {
                if (IsChgLeaderCmd_() && GetChr()->MethdChgLeader()) {
                    CChrManBasket *pMan = CChrManBasket::GetInstance();
                    pMan->OnChgCharacterInputDiableFrame_(m_pChr->GetTeamNo());
                    m_pChr->PlaySePos(0xBC0, 0, 0, -1, 0.0f, 1.0f, 0.0f);
                    return 1;
                }
                return 0;
            }

            int PlayCombination_()
            {
                if (m_nCmdFlag & 1) {
                    return 0;
                }
                if (CGame::g_GameWork.GetGameType() == 2) {
                    return 0;
                }
                if (!CChrManBasket::GetInstance()->PlayCombination_(m_pChr)) {
                    return 0;
                }
                m_nCmdFlag |= 1;
                return 1;
            }

            BASKET::CStickState2Tap *m_pStick2Tap; // 0x9C
            BASKET::CStickStateRoll *m_pStickRoll; // 0xA0
            int m_bCross2Tap;                      // 0xA4, the cross key was pushed twice the same way
            unsigned long m_nCrossFlag;            // 0xA8, bit 0x40: the cross key is held
            // 0xAC: bit 1 a combination was started, 2 A and B pressed
            // together, 4 a combination pass is waiting
            unsigned long m_nCmdFlag;
            unsigned long m_nKeyHold;              // 0xB0, per key: held for 6 frames (one frame)
            unsigned long m_nKeyHoldDone;          // 0xB4, per key: the 6 frames were reported
            unsigned long m_nKeyLong;              // 0xB8, per key: held for 15 frames
            float m_fCrossOnTime;                  // 0xBC
            float m_fCrossOffTime;                 // 0xC0
            float m_fCrossLastDir;                 // 0xC4, -1 for none
            float m_fKeyHoldTime[6];               // 0xC8
            float m_fKeyLongTime[6];               // 0xE0
    };
}

#endif /* !MSW_CCHRDRVPADBASKET_HPP_ */
