#ifndef MSW_CCHRDRVPADVOLLEY_HPP_
#define MSW_CCHRDRVPADVOLLEY_HPP_

#include "game/msw/CChrDrvPad.hpp"

namespace msw {

    // (name invented) The object CChrDrvPadVolley owns at 0xA0. Nothing in
    // VolleyF sets it; the destructor only deletes it through its virtual
    // destructor, so its class is not known.
    class CChrDrvPadVolleyUnkA0 {
        public:
            virtual ~CChrDrvPadVolleyUnkA0();
    };

    // The pad driver of the volleyball game (VolleyF), size 0xB4 (what
    // CChrManVolley::CreateDriver_ allocates). Member names are guesses.
    class CChrDrvPadVolley : public CChrDrvPad {
        public:
            CChrDrvPadVolley();
            virtual ~CChrDrvPadVolley();
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
                INPUT_SPIKE   = 0x01, // spike start held (AskSpikeStart_)
                INPUT_ITEM    = 0x02, // B (Nunchuk): item shot
                INPUT_JUMP    = 0x04, // 1 / A: jump, until held too long
                INPUT_RECEIVE = 0x08, // 2: receive
                INPUT_CHANGE  = 0x10, // B: change of character
                INPUT_HOLD    = 0x40  // 1 / A held since its press
            };

            void MainNormal_(_CHR_MSG *pMsg);
            void MainServe_(_CHR_MSG *pMsg);
            int IsJumpBtnInput_();
            int IsChrChangeInput_();
            long AskReceive_();
            long AskSpikeStart_(int bTrgA);

            // Inline in the original (not emitted; names guessed).
            int IsArtsInput_();
            int IsItemInput_();
            int IsReceiveInput_();
            static void GetStickSide_(long &rSide, float fDir, float fRange);

            unsigned long m_nInputFlag;   // 0x9C, INPUT_* bits
            CChrDrvPadVolleyUnkA0 *m_pA0; // 0xA0, see CChrDrvPadVolleyUnkA0
            // 0xA4: frames each button has been held (A, B, 1, 2).
            long m_nHoldA;
            long m_nHoldB;
            long m_nHold1;
            long m_nHold2;
    };

    // The pad driver of the rhythm receive party game, size 0xB4
    // (CHR_MAN_VOLLEY::CChrManVolleyRhythmReceive::CreateDriver_): the
    // character only moves and receives.
    class CChrDrvPadVolleyRhythmReceive : public CChrDrvPadVolley {
        public:
            CChrDrvPadVolleyRhythmReceive();
            virtual ~CChrDrvPadVolleyRhythmReceive();
            virtual long OnMsg(MSG_ID id, _CHR_MSG *pMsg);

        protected:
            void MainNormal_(_CHR_MSG *pMsg);
    };
}

#endif /* !MSW_CCHRDRVPADVOLLEY_HPP_ */
