#ifndef CCOCKPITPAUSE_HPP_
#define CCOCKPITPAUSE_HPP_

#include "bcs/CFsm.hpp"
#include "game/msw/CMenuTask.hpp"

namespace msw {
    class CCockpitPauseResult;
    class CCockpitPauseResultChr;

    // The pause menu of the sports games. Lives in the DOL; each game module
    // derives its own (CCockpitBossPause in BossF and so on). Size 0xAC:
    // CMenuTask at 0x00, bcs::CFsm<CCockpitPause> at 0x44 (vptr at 0x88),
    // own members from 0x8C. Every state is a group of virtual functions,
    // declared in the order of the DOL's vtable (lbl_80480450); the Fsm
    // state numbers do not follow that order (see TabParamInfo).
    class CCockpitPause : public CMenuTask, public bcs::CFsm<CCockpitPause> {
        public:
            // One tab of the menu, 6 bytes. Member names are guesses; only
            // m_nState is read so far (CheckChangeTab changes to it).
            struct TabParamInfo {
                unsigned char m_Unk00;      // 0x00
                unsigned char m_nState;     // 0x01, the Fsm state of the tab
                unsigned short m_nTitleMsg; // 0x02, a message number (name guessed)
                unsigned short m_Unk04;     // 0x04
            };

            CCockpitPause(bcs::CTask *pParent);
            virtual ~CCockpitPause();
            virtual void Main();
            virtual void Disp();

            virtual void Initialize_In();
            virtual void Initialize_Loop();
            virtual void Initialize_Out();
            virtual void Close_In();
            virtual void Close_Loop();
            virtual void Close_Out();
            virtual void BallPlay_In();
            virtual void BallPlay_Loop();
            virtual void BallPlay_Out();
            virtual void NormalPlay_In();
            virtual void NormalPlay_Loop();
            virtual void NormalPlay_Out();
            virtual void ScoreBoard_In();
            virtual void ScoreBoard_Loop();
            virtual void ScoreBoard_Out();
            virtual void Camera_In();
            virtual void Camera_Loop();
            virtual void Camera_Out();
            virtual void GameEnd_In();
            virtual void GameEnd_Loop();
            virtual void GameEnd_Out();
            virtual void GameEnd_Init();
            virtual void GameEnd_TabMenu();
            virtual void GameEnd_Confirm();
            virtual void RuleInfo_In();
            virtual void RuleInfo_Loop();
            virtual void RuleInfo_Out();
            virtual void RuleInfo_Init();
            virtual void GameInfo_In();
            virtual void GameInfo_Loop();
            virtual void GameInfo_Out();
            virtual void GameInfo_Init();
            virtual void PartyEnd_In();
            virtual void PartyEnd_Loop();
            virtual void PartyEnd_Out();
            virtual void PartyEnd_TabMenu();
            virtual void PartyEnd_Confirm();
            virtual void PartyEnd_Retry();

            // The work every derived Main does first.
            void MainLoopCommon();
            // Changes state when the player switches to another of the nNum
            // tabs in pInfo.
            void CheckChangeTab(const TabParamInfo *pInfo, long nNum);
            // Closes the cockpit object nObjId.
            void CloseMenu(long nObjId);
            // Message nMsg of message group nGroup (group meaning guessed).
            static const wchar_t *GetMessageText(long nGroup, long nMsg);
            // Fill the score board pages from the game; pTotal receives the
            // two teams' total points (two words). Return types guessed.
            void SetPauseResultParameter(CCockpitPauseResult *pResult, long *pTotal);
            void SetPauseResultChrParameter(CCockpitPauseResultChr *pResult);

            // Id of the live pause task in the task manager.
            static unsigned long s_TaskHandle;
            // (added for BasketF game/basket/CCockpitBasket.cpp; return type
            // guessed) Removes the pause task, if there is one.
            static void Remove();

        protected:
            union {
                long m_bGameInfoSet;             // 0x8C, set once GameInfo_Init has filled the page (name guessed)
                // 0x8C as the score board pages use it: ScoreBoard_In sets
                // 1, ScoreBoard_Loop waits one frame (1 -> 0), fills the
                // pages and sets 2 (BasketF; name guessed).
                long m_nScoreBoardStep;
            };
            long m_nTab;                         // 0x90, current tab (CheckChangeTab)
            const TabParamInfo *m_pTabParamInfo; // 0x94
            long m_nTabParamNum;                 // 0x98
            unsigned long m_Unk9C;               // 0x9C
            unsigned long m_UnkA0;               // 0xA0, CheckChangeTab does nothing while it is set
            const wchar_t *m_pUnkA4;             // 0xA4, a message text (BasketF sets it)
            const char *m_pLayoutName;           // 0xA8, a .brlyt file name
    };
}

#endif /* !CCOCKPITPAUSE_HPP_ */
