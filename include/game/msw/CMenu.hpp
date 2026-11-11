#ifndef CMENU_HPP_
#define CMENU_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/CTask.hpp"

namespace msw {

    // Root task of the menus. Lives in module MenuF. Size 0x88, laid out
    // like CMainStep: bcs::CTask at 0x00, bcs::CFsm<CMenu> at 0x34, own
    // members from 0x7C.
    class CMenu : public bcs::CTask, public bcs::CFsm<CMenu> {
        public:
            // The states (names from the state functions).
            enum {
                STATE_INITIALIZE,
                STATE_START_UP,
                STATE_TITLE,
                STATE_SELECT,
                STATE_TEST,
                STATE_END,
                STATE_NUM
            };

            CMenu(bcs::CTask *pParent);
            virtual ~CMenu();
            virtual void Main();
            virtual void Disp();

            // The select menu task, null when it has gone.
            bcs::CTask *GetMainTask();

        private:
            void Initialize();
            void Finalize();
            // Loads the menu's layouts and creates its effect controller and
            // cockpit.
            void LoadDataInitialize();

            void StateInitialize_In();
            void StateInitialize_Loop();
            void StateInitialize_Out();
            void StateStartUp_In();
            void StateStartUp_Loop();
            void StateStartUp_Out();
            void StateTitle_In();
            void StateTitle_Loop();
            void StateTitle_Out();
            void StateSelect_In();
            void StateSelect_Loop();
            void StateSelect_Out();
            void StateTest_In();
            void StateTest_Loop();
            void StateTest_Out();
            void StateEnd_In();
            void StateEnd_Loop();
            void StateEnd_Out();

            unsigned char m_nSeq;       // 0x7C, step inside the current state (name guessed)
            unsigned long m_hMainTask;  // 0x80, id of the select menu task (name guessed)
            long m_nNextStep;           // 0x84, the CMainStep step to go to at the end (name guessed)
    };
}

#endif /* !CMENU_HPP_ */
