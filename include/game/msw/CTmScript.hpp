#ifndef CTMSCRIPT_HPP_
#define CTMSCRIPT_HPP_

#include "game/msw/vm/CVMCpu.hpp"

namespace msw {

    class CTmScriptHandler;

    namespace TournamentWork {
        struct POINT_INFO;
        struct ROUTE_INFO;
    }

    namespace TournamentData {
        struct CAMERA_INFO;
    }

    // The tournament's scripts (TourF game/tour/CTmScript.cpp): the map's
    // set-up, the rules, panels and items of a match, the matches the map
    // makes. A script VM (msw::vm::CVMCpu) whose system calls are the
    // _OpSysCall* members below, bound by name hash in UpdateBind. Size
    // 0x11C. Member names guessed unless they are in a symbol.
    class CTmScript : public vm::CVMCpu {
            // (added for TourF game/tour/CTournamentStep.cpp, which runs
            // scripts by name through _Setup)
            friend class CTournamentStep;

        public:
            // A system call: returns the VM state (0 to go on).
            typedef long (CTmScript::*SYSCALL)();

            CTmScript(unsigned long nStackSize);
            virtual long OpSysCall();
            virtual ~CTmScript();
            virtual void UpdateBind();
            virtual void ResetBind();

            // Sets script value pName to nValue.
            void SetValue(const char *pName, long nValue);
            // Runs the map's set-up scripts and builds its points, routes
            // and cameras; pHandler receives the scripts' requests.
            void SetupInitial(CTmScriptHandler *pHandler);
            void SetupRule(long nGameMode, long nRule, long nArg, long nLevel, int bReset);
            // Picks the stage; returns it.
            long SetupExcludeStage(long nStage, long nExclude, long nLevel, int bReset);
            void SetupPanel(long nPanel, long nLevel, int bReset);
            void SetupItemParam(long nItem, long nLevel, int bReset);
            // Fills in match nMatch between teams nTeam0 and nTeam1.
            void SetupMatch(long nMatch, long nTeam0, long nTeam1);
            // Runs the map's event function; returns 0 when it has ended, 1
            // when it returned without arguments, 2 when it is waiting, or
            // the VM state.
            long CallEventEntry();

        private:
            // Whether stage item 0 of the last level script is a stage of
            // sport nGameMode the player has not unlocked yet.
            int _CheckUnlockStage(long nGameMode);
            // Calls script function pName with nArg of the three values,
            // runs it to the end and returns its result.
            long _Setup(const char *pName, int bReset, long nArg, long nValue0, long nValue1, long nValue2);
            int _SetCall(const char *pName, long nArg, long nValue0, long nValue1, long nValue2);
            void _BindSysCall(SYSCALL pFunc, long nName);

            // Goes back to the system call instruction, so that it runs
            // again next time.
            void _Retry()
            {
                if (m_pPC != 0) {
                    m_pPC -= 5;
                }
            }

            // The item lists a level script fills (SetCharItem and so on).
            void _ClearItem()
            {
                m_nCharItemNum = 0;
                m_nStarCharItemNum = 0;
                m_nStarCharItemNum2 = 0;
                m_nStageItemNum = 0;
            }

            long _OpSysCallWaitResultMove();
            long _OpSysCallWaitCamera();
            long _OpSysCallWaitAction();
            long _OpSysCallWaitFade();
            long _OpSysCallWaitTimer();
            long _OpSysCallSetMapObj();
            long _OpSysCallSetMapObjPoint();
            long _OpSysCallRouteEvent();
            long _OpSysCallGetRouteWrite();
            long _OpSysCallSetRouteWrite();
            long _OpSysCallSetChipWrite();
            long _OpSysCallGetChipWrite();
            long _OpSysCallSetPointFlags();
            long _OpSysCallSetPointOffset();
            long _OpSysCallSetPointCamera();
            long _OpSysCallGetPointCamera();
            long _OpSysCallGetUnlockExchange();
            long _OpSysCallIsUnlockMission();
            long _OpSysCallCreateTeamMission();
            long _OpSysCallEraseTeam();
            long _OpSysCallSetTeamPos();
            long _OpSysCallSetMoveTarget();
            long _OpSysCallSetMoveResult();
            long _OpSysCallStockRelayPoint();
            long _OpSysCallGetMissionClear();
            long _OpSysCallGetPointTeam();
            long _OpSysCallGetWinTeam();
            long _OpSysCallGetLoseTeam();
            long _OpSysCallSetExcludeStage();
            long _OpSysCallSetItemParamF();
            long _OpSysCallSetItemParamI();
            long _OpSysCallSetPanelItem();
            long _OpSysCallSetCharItem();
            long _OpSysCallSetStarCharItem();
            long _OpSysCallSetStageItem();
            long _OpSysCallSetRuleItem();
            long _OpSysCallSetCameraData();
            long _OpSysCallSetPointData();
            long _OpSysCallSetRouteData();
            long _OpSysCallSetRoutePoints();
            long _OpSysCallCloseMatchMake();
            long _OpSysCallGetPointMatch();
            long _OpSysCallIsMatchEnd();
            long _OpSysCallGetVsMatchPoint();
            long _OpSysCallSetMatchMake();
            long _OpSysCallSetMatchRule();
            long _OpSysCallSetMatchLevel();
            long _OpSysCallSetMatchTelop();
            long _OpSysCallSetMatchStage();
            long _OpSysCallSetMatchUnlock();
            long _OpSysCallMatchVsPause();
            long _OpSysCallMatchMake();
            long _OpSysCallSecretMake();
            long _OpSysCallFadeIn();
            long _OpSysCallIsFadeInReq();
            long _OpSysCallTurnMode();
            long _OpSysCallIsFirstAround();
            long _OpSysCallIsWin();
            long _OpSysCallIsAllStarChar();
            long _OpSysCallGetTeamMax();
            long _OpSysCallGetTurnMode();
            long _OpSysCallGetTmLevel();
            long _OpSysCallGetTmMode();
            long _OpSysCallGetTmCallMode();
            long _OpSysCallGetRandom();
            long _OpSysCallBackupFlags();
            long _OpSysCallRestoreFlags();
            long _OpSysCallBackupFirstFlags();
            long _OpSysCallRestoreFirstFlags();
            long _OpSysCallUnlockOpen();
            long _OpSysCallPointCamera();
            long _OpSysCallRtSelSetup();
            long _OpSysCallRtSelect();
            // Debug menu calls: empty in this build.
            long _OpSysCallDbSelIndex();
            long _OpSysCallDbSelSetup();
            long _OpSysCallDbSelItem();
            long _OpSysCallDbSelItemLabel();
            long _OpSysCallDbSelItemLabelValue();
            long _OpSysCallDbSelItemLabelStr();
            long _OpSysCallDbSelItemLabelStrValue();
            long _OpSysCallDbSelStockLabelStr();
            long _OpSysCallDbSelect();
            long _OpSysCallDbFlgSetup();
            long _OpSysCallDbFlgItem();
            long _OpSysCallDbScript();
            long _OpSysCallDbScriptEnd();
            long _OpSysCallDbTrace();
            long _OpSysCallDbTrace1();

            // What the level scripts hand back: stage items, the two kinds
            // of star character items, character items, and the list of
            // excluded stages, which also serves as the route list of
            // StockRelayPoint and the route calls.
            signed char m_aStageItem[16];        // 0x28
            signed char m_aStarCharItem[16];     // 0x38
            signed char m_aStarCharItem2[16];    // 0x48
            signed char m_aCharItem[16];         // 0x58
            unsigned char m_aPoint[0x80];        // 0x68
            CTmScriptHandler *m_pHandler;        // 0xE8
            SYSCALL *m_pSysCall;                 // 0xEC
            long m_nSysCallNum;                  // 0xF0
            TournamentWork::POINT_INFO *m_pPointInfo; // 0xF4
            long m_nPointNum;                    // 0xF8
            TournamentWork::ROUTE_INFO *m_pRouteInfo; // 0xFC
            long m_nRouteNum;                    // 0x100
            TournamentData::CAMERA_INFO *m_pCameraInfo; // 0x104
            long m_nCameraNum;                   // 0x108
            long m_nStage;                       // 0x10C: the stage SetMatchStage gave; -1 for none
            signed char m_nCharItemNum;          // 0x110
            signed char m_nStarCharItemNum;      // 0x111
            signed char m_nStarCharItemNum2;     // 0x112
            signed char m_nStageItemNum;         // 0x113
            signed char m_nRuleNum;              // 0x114
            signed char m_nPanelNum;             // 0x115
            signed char m_nItemParamNum;         // 0x116
            signed char m_nExcludeNum;           // 0x117
            float m_fWaitTimer;                  // 0x118
    };
}

#endif /* !CTMSCRIPT_HPP_ */
