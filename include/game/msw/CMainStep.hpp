#ifndef CMAINSTEP_HPP_
#define CMAINSTEP_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

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
    // 0x78), own members from 0x7C.
    class CMainStep : public bcs::CTask, public bcs::CFsm<CMainStep> {
        public:
            CMainStep();
            virtual ~CMainStep();
            virtual void Main();
            virtual void ChangeStateProc();

            // One Ctor/Loop/Dtor triple per state, in state order.
            virtual void CtorStateInitialize();
            virtual void LoopStateInitialize();
            virtual void DtorStateInitialize();
            virtual void CtorStateSaveInit();
            virtual void LoopStateSaveInit();
            virtual void DtorStateSaveInit();
            virtual void CtorStateMenu();
            virtual void LoopStateMenu();
            virtual void DtorStateMenu();
            virtual void CtorStateTournament();
            virtual void LoopStateTournament();
            virtual void DtorStateTournament();
            virtual void CtorStateUnlock();
            virtual void LoopStateUnlock();
            virtual void DtorStateUnlock();
            virtual void CtorStateGameBasket();
            virtual void LoopStateGameBasket();
            virtual void DtorStateGameBasket();
            virtual void CtorStateGameVolley();
            virtual void LoopStateGameVolley();
            virtual void DtorStateGameVolley();
            virtual void CtorStateGameDodge();
            virtual void LoopStateGameDodge();
            virtual void DtorStateGameDodge();
            virtual void CtorStateGameHockey();
            virtual void LoopStateGameHockey();
            virtual void DtorStateGameHockey();
            virtual void CtorStateGameBoss();
            virtual void LoopStateGameBoss();
            virtual void DtorStateGameBoss();
            virtual void CtorStateFinalize();
            virtual void LoopStateFinalize();
            virtual void DtorStateFinalize();

            void DoChangeStep(long);
            void AppLoop();
            static void SetNextStep(long);
            static void ChangeStep(long);

            static CMainStep *g_pInstance;

        protected:
            long m_Unk7C;    // 0x7C, taken from offset 4 of the task a state creates
            long m_nNextStep; // 0x80, written by SetNextStep (name guessed)
            long m_nSubStep; // 0x84, progress within the current state (name guessed)
            long m_Unk88;    // 0x88
            long m_Unk8C;    // 0x8C
    };
}

#endif /* !CMAINSTEP_HPP_ */
