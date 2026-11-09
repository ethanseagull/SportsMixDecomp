#ifndef CMAINSTEPEX_HPP_
#define CMAINSTEPEX_HPP_

#include "game/msw/CMainStep.hpp"

namespace msw {

    // Adds no data to CMainStep; it only supplies the state functions.
    // Lives in module SystemF.
    class CMainStepEx : public CMainStep {
        public:
            // Creates the instance and stores it in CMainStep::g_pInstance.
            static void Initialize();

            // Not inline: the original emits it after DtorStateFinalize and
            // before the inline functions the vtable pulls in.
            virtual ~CMainStepEx();
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
    };
}

#endif /* !CMAINSTEPEX_HPP_ */
