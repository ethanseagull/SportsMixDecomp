#ifndef MSW_CCHRDRVPADBOSS_HPP_
#define MSW_CCHRDRVPADBOSS_HPP_

#include "game/msw/CChrDrvPad.hpp"

namespace msw {

    // The pad driver of the boss game (BossF), size 0xB0 (what
    // CChrManBoss::CreateDriver_ allocates). Member names are guesses.
    class CChrDrvPadBoss : public CChrDrvPad {
        public:
            CChrDrvPadBoss();
            virtual ~CChrDrvPadBoss();
            virtual long OnMsg(MSG_ID id, _CHR_MSG *pMsg);
            virtual void UpdateFunc_();
            virtual void ClearFunc_();

            long OnMsgN(MSG_ID id, _CHR_MSG *pMsg);
            long OnMsgMain(_CHR_MSG *pMsg);

        protected:
            // Inline (name guessed): OnMsg's branch for the Nunchuk.
            long OnMsgW_(MSG_ID id, _CHR_MSG *pMsg);

            // Bits of m_nInputFlag: a button press that is being held or is
            // waiting for its release.
            enum {
                INPUT_SPIKE  = 0x01, // spike start held (AskSpikeStart_)
                INPUT_ITEM   = 0x02, // B (Nunchuk): item shot
                INPUT_JUMP   = 0x04, // 2 / A: jump
                INPUT_PASS   = 0x08, // 1: pass
                INPUT_STANCE = 0x10, // 2 held: shooting stance
                INPUT_CHANGE = 0x20, // B: change of character
                INPUT_2      = 0x40, // 2: receive, spike
                INPUT_ARTS   = 0x80  // 1+2 / A+B: special move
            };

            void MainOnBall_(_CHR_MSG *pMsg);
            void MainOffBall_(_CHR_MSG *pMsg);
            int IsArtsInput_();
            int IsJumpBtnInput_();
            int IsPassBtnInput_();
            int IsChrChangeInput_();
            long AskReceive_();
            long AskSpikeStart_();

            // Inline in the original (not emitted; names guessed): checks
            // shared by MainOnBall_, MainOffBall_, AskReceive_ and OnMsgN.
            int IsBtn1Input_();
            int IsBtn2Input_();
            int IsStanceInput_();
            int IsItemInput_();
            int IsPressAB_() const;
            static void GetStickSide_(long &rSide, float fDir, float fRange);

            unsigned long m_nInputFlag;  // 0x9C, INPUT_* bits
            // 0xA0: frames each button has been held (A, B, 1, 2).
            long m_nHoldA;
            long m_nHoldB;
            long m_nHold1;
            long m_nHold2;
    };
}

#endif /* !MSW_CCHRDRVPADBOSS_HPP_ */
