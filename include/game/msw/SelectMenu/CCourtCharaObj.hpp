#ifndef SELECTMENU_CCOURTCHARAOBJ_HPP_
#define SELECTMENU_CCOURTCHARAOBJ_HPP_

namespace bcs {
    class CMdlObj;
}

namespace msw {
    namespace SelectMenu {

        // A character model shown on the court and cup selection screens
        // (MenuF; created by msw::SelectMenu::CCourtChr and CCupChr). It
        // holds the handle of a msw::CChrMenuObj (registered in
        // msw::CChrMenuCtrl) and fades it in and out. Size 0x18, vptr at
        // 0x14. Member names are guessed.
        class CCourtCharaObj {
            public:
                // The values of m_nState, the argument of ChangeState
                // (names guessed from the functions each one runs).
                enum {
                    STATE_CHECK = 0,      // StateCheck itself
                    STATE_IN = 1,         // StateIn
                    STATE_OUT = 2,        // StateOut
                    STATE_CHARA_CHANGE = 3 // StateCharaChange
                };

                // The parameter is not used (the callers pass 0).
                CCourtCharaObj(unsigned long hMdl);

                // The model of a handle (bcs::CScnObj::GetScnObj).
                bcs::CMdlObj *GetMdlObj(unsigned long hMdl);
                // Runs the current state, then keeps motion 3 of the model
                // playing.
                void StateCheck();
                void StateIn();
                void StateOut();
                void StateCharaChange();
                // The model to show next; the one queued before is deleted
                // unless it is the one shown.
                void SetNextMdlHandle(unsigned long hMdl);
                // Restarts the state machine in `state`; a state that is
                // already running is restarted only when bForce is 1.
                void ChangeState(long state, int bForce);

                // (names guessed) The model shown and the one queued;
                // msw::SelectMenu::CCupChr reads them.
                unsigned long GetMdlHandle() const { return m_hMdl; }
                unsigned long GetNextMdlHandle() const { return m_hNextMdl; }

                // The fade time in frames.
                static float s_FadeInOutTime;

            private:
                // Makes the queued model the one shown.
                void ApplyNextMdl();
                // Hides or shows the model shown.
                void Hide();
                void Show();
                // Stores the fade value and passes it to the model shown.
                void SetAlpha(float alpha);

                long m_nSubState;            // 0x00
                long m_nState;               // 0x04
                unsigned long m_hMdl;        // 0x08
                unsigned long m_hNextMdl;    // 0x0C
                float m_fAlpha;              // 0x10

            public:
                // Declared after the data: the vptr is at 0x14.
                virtual ~CCourtCharaObj();
        };
    }
}

#endif /* !SELECTMENU_CCOURTCHARAOBJ_HPP_ */
