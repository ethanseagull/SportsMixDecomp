#ifndef CMAINSTEP_HPP_
#define CMAINSTEP_HPP_

#include "bcs/CTask.hpp"
#include "bcs/CFsm.hpp"

namespace bcs {
    class CRsoModule;
}

namespace msw {

    // Module accessors. Defined in the DOL next to CMainStep; which header
    // they belong to is a guess. Pointer versus reference is also a guess.
    bcs::CRsoModule *GetMenuModule();
    bcs::CRsoModule *GetTournamentModule();
    bcs::CRsoModule *GetBasketModule();
    bcs::CRsoModule *GetVolleyModule();
    bcs::CRsoModule *GetDodgeModule();
    bcs::CRsoModule *GetHockeyModule();
    bcs::CRsoModule *GetBossModule();

    // Size 0x90. bcs::CTask at 0x00, bcs::CFsm<CMainStep> at 0x34 (vptr at
    // 0x78), own members from 0x7C. Offsets are from the constructor in the
    // DOL.
    class CMainStep : public bcs::CTask, public bcs::CFsm<CMainStep> {
        public:
            CMainStep();
            virtual ~CMainStep();
            virtual void Main();
            virtual void ChangeStateProc();

            // One Ctor/Loop/Dtor triple per state, in state order. All are
            // empty here (the DOL's vtable for this class points at 33
            // one-instruction functions emitted after the constructor);
            // CMainStepEx in module SystemF supplies the real ones.
            virtual void CtorStateInitialize() {}
            virtual void LoopStateInitialize() {}
            virtual void DtorStateInitialize() {}
            virtual void CtorStateSaveInit() {}
            virtual void LoopStateSaveInit() {}
            virtual void DtorStateSaveInit() {}
            virtual void CtorStateMenu() {}
            virtual void LoopStateMenu() {}
            virtual void DtorStateMenu() {}
            virtual void CtorStateTournament() {}
            virtual void LoopStateTournament() {}
            virtual void DtorStateTournament() {}
            virtual void CtorStateUnlock() {}
            virtual void LoopStateUnlock() {}
            virtual void DtorStateUnlock() {}
            virtual void CtorStateGameBasket() {}
            virtual void LoopStateGameBasket() {}
            virtual void DtorStateGameBasket() {}
            virtual void CtorStateGameVolley() {}
            virtual void LoopStateGameVolley() {}
            virtual void DtorStateGameVolley() {}
            virtual void CtorStateGameDodge() {}
            virtual void LoopStateGameDodge() {}
            virtual void DtorStateGameDodge() {}
            virtual void CtorStateGameHockey() {}
            virtual void LoopStateGameHockey() {}
            virtual void DtorStateGameHockey() {}
            virtual void CtorStateGameBoss() {}
            virtual void LoopStateGameBoss() {}
            virtual void DtorStateGameBoss() {}
            virtual void CtorStateFinalize() {}
            virtual void LoopStateFinalize() {}
            virtual void DtorStateFinalize() {}

            // (added for DOL game/msw/CMainStep.cpp) The state table's hooks:
            // non-virtual wrappers that call the state functions virtually
            // (the DOL emits each one, 16 bytes, before the empty virtual it
            // calls).
            void CtorStateInitialize_() { CtorStateInitialize(); }
            void LoopStateInitialize_() { LoopStateInitialize(); }
            void DtorStateInitialize_() { DtorStateInitialize(); }
            void CtorStateSaveInit_() { CtorStateSaveInit(); }
            void LoopStateSaveInit_() { LoopStateSaveInit(); }
            void DtorStateSaveInit_() { DtorStateSaveInit(); }
            void CtorStateMenu_() { CtorStateMenu(); }
            void LoopStateMenu_() { LoopStateMenu(); }
            void DtorStateMenu_() { DtorStateMenu(); }
            void CtorStateTournament_() { CtorStateTournament(); }
            void LoopStateTournament_() { LoopStateTournament(); }
            void DtorStateTournament_() { DtorStateTournament(); }
            void CtorStateUnlock_() { CtorStateUnlock(); }
            void LoopStateUnlock_() { LoopStateUnlock(); }
            void DtorStateUnlock_() { DtorStateUnlock(); }
            void CtorStateGameBasket_() { CtorStateGameBasket(); }
            void LoopStateGameBasket_() { LoopStateGameBasket(); }
            void DtorStateGameBasket_() { DtorStateGameBasket(); }
            void CtorStateGameVolley_() { CtorStateGameVolley(); }
            void LoopStateGameVolley_() { LoopStateGameVolley(); }
            void DtorStateGameVolley_() { DtorStateGameVolley(); }
            void CtorStateGameDodge_() { CtorStateGameDodge(); }
            void LoopStateGameDodge_() { LoopStateGameDodge(); }
            void DtorStateGameDodge_() { DtorStateGameDodge(); }
            void CtorStateGameHockey_() { CtorStateGameHockey(); }
            void LoopStateGameHockey_() { LoopStateGameHockey(); }
            void DtorStateGameHockey_() { DtorStateGameHockey(); }
            void CtorStateGameBoss_() { CtorStateGameBoss(); }
            void LoopStateGameBoss_() { LoopStateGameBoss(); }
            void DtorStateGameBoss_() { DtorStateGameBoss(); }
            void CtorStateFinalize_() { CtorStateFinalize(); }
            void LoopStateFinalize_() { LoopStateFinalize(); }
            void DtorStateFinalize_() { DtorStateFinalize(); }

            // Asks the state machine for `step`, or for the step set with
            // SetNextStep when `step` is negative.
            void DoChangeStep(long step);
            // Waits for the task a state created to end, then changes step.
            void AppLoop();
            static void SetNextStep(long step);
            static void ChangeStep(long step);

            // (DOL fn_800C45B8; name guessed; added for
            // game/msw/CCockpitMenuBase.cpp) The state of the instance's
            // state machine (2: the menus), or 0 without an instance.
            static long GetStep();

            static CMainStep *g_pInstance;

        private:
            // (DOL fn_800C46BC, name guessed; added for DOL
            // game/msw/CMainStep.cpp) Called by AppLoop when the task of a
            // state has ended: with m_Unk8C set, stops the sound, ends the
            // online session and puts the system settings back to their
            // defaults.
            void ResetSystem();

        protected:
            unsigned long m_nTaskId; // 0x7C, id of the task the current state created (name guessed)
            long m_nNextStep;        // 0x80, written by SetNextStep and DoChangeStep (name guessed)
            long m_nSubStep;         // 0x84, progress within the current state (name guessed)
            long m_nAppStep;         // 0x88, progress of AppLoop (name guessed)
            long m_Unk8C;            // 0x8C
    };
}

#endif /* !CMAINSTEP_HPP_ */
