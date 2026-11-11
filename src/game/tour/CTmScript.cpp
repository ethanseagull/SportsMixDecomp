// msw::CTmScript: the tournament's scripts (TourF).

#include "game/msw/CTmScript.hpp"
#include "game/msw/CTmScriptHandler.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CUnlockFlags.hpp"
#include "game/msw/TournamentData.hpp"
#include "game/msw/TournamentWork.hpp"
#include "bcs/BCS_Rand.hpp"
#include "bcs/CWorldEnv.hpp"

namespace msw {

    namespace {
        // The level script of each sport's rules and characters, by
        // CGame::GAME_MODE.
        const char *s_aRuleScript[4] = {
            "SetupRuleBasket_Level",
            "SetupRuleVolley_Level",
            "SetupRuleDodge_Level",
            "SetupRuleHockey_Level",
        };

        const char *s_aCharScript[4] = {
            "SetupCharBasket_Level",
            "SetupCharVolley_Level",
            "SetupCharDodge_Level",
            "SetupCharHockey_Level",
        };

        const float DEG_TO_RAD = 0.017453292f;

        enum {
            POINT_MAX = 0x80,
            ROUTE_MAX = 0x100,
            TEAM_MAX = 0x10,
            MATCH_MAX = 0x10
        };

        // A random index below nNum. (name guessed)
        inline long RandomIndex(long nNum)
        {
            if (nNum > 0) {
                return bcs::BCS_RandMax(nNum - 1);
            }
            return 0;
        }

        // The team on map point nPoint, -1 for none. (name guessed)
        inline long FindPointTeam(unsigned char nPoint)
        {
            for (long i = 0; i < TEAM_MAX; i++) {
                unsigned char nTeamPoint = TournamentWork::GetTeamInfo(i).nPoint;
                if (nTeamPoint != 0xFF && nTeamPoint == nPoint) {
                    return i;
                }
            }
            return -1;
        }

        // The character checks of the level scripts (IsUnlockMission,
        // CreateTeamMission) share one shape: a secret character (types
        // 0xE, 0xF and 0x11) whose cup 3 is not cleared, or a colour that
        // is not unlocked, is still hidden; otherwise IsUnlockChar says
        // whether the character is unlocked. (meaning guessed)
    }

    CTmScript::CTmScript(unsigned long nStackSize) : vm::CVMCpu(nStackSize)
    {
        m_pHandler = 0;
        m_pSysCall = 0;
        m_pPointInfo = 0;
        m_nPointNum = 0;
        m_pRouteInfo = 0;
        m_nRouteNum = 0;
        m_pCameraInfo = 0;
        m_nCameraNum = 0;
        m_nStage = -1;
        m_nCharItemNum = 0;
        m_nStarCharItemNum = 0;
        m_nStarCharItemNum2 = 0;
        m_nStageItemNum = 0;
        m_nRuleNum = 0;
        m_nPanelNum = 0;
        m_nItemParamNum = 0;
        m_nExcludeNum = 0;
    }

    CTmScript::~CTmScript()
    {
        if (m_pSysCall != 0) {
            delete[] m_pSysCall;
        }
        m_pSysCall = 0;
        if (m_pPointInfo != 0) {
            delete[] reinterpret_cast<unsigned char *>(m_pPointInfo);
        }
        m_pPointInfo = 0;
        if (m_pRouteInfo != 0) {
            delete[] m_pRouteInfo;
        }
        m_pRouteInfo = 0;
        if (m_pCameraInfo != 0) {
            delete[] m_pCameraInfo;
        }
        m_pCameraInfo = 0;

        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        info.pPointInfo = 0;
        info.pRouteInfo = 0;
        info.pCameraInfo = 0;
    }

    void CTmScript::SetValue(const char *pName, long nValue)
    {
        vm::CVMData::VALUE_TAG *pTag = m_pData->FindValueTag(pName, 1);
        if (pTag != 0) {
            m_pData->Value(pTag->nIndex) = nValue;
        }
    }

    void CTmScript::SetupInitial(CTmScriptHandler *pHandler)
    {
        m_pHandler = pHandler;

        // Cameras.
        m_nCameraNum = _Setup("GetCameraMax", 1, 0, 0, 0, 0);
        if (m_pCameraInfo != 0) {
            delete[] m_pCameraInfo;
        }
        m_pCameraInfo = 0;
        if (m_nCameraNum > 0) {
            m_pCameraInfo = new TournamentData::CAMERA_INFO[m_nCameraNum];
            float fFovy = bcs::CWorldEnv::GetViewFovy(0);
            for (long i = 0; i < m_nCameraNum; i++) {
                TournamentData::CAMERA_INFO &camera = m_pCameraInfo[i];
                camera.ofs.x = 0.0f;
                camera.ofs.y = 0.0f;
                camera.ofs.z = 0.0f;
                camera.rot.y = 0.0f;
                camera.rot.z = 0.0f;
                camera.dist = 15.0f;
                camera.rot.x = 0.0f;
                camera.fovy = fFovy;
            }
        }
        _Setup("SetupCamera", 1, 0, 0, 0, 0);

        // Map points.
        m_nPointNum = _Setup("GetPointMax", 1, 0, 0, 0, 0);
        if (m_pPointInfo != 0) {
            delete[] reinterpret_cast<unsigned char *>(m_pPointInfo);
        }
        m_pPointInfo = 0;
        if (m_nPointNum > 0) {
            m_pPointInfo = reinterpret_cast<TournamentWork::POINT_INFO *>(
                new unsigned char[sizeof(TournamentWork::POINT_INFO) * POINT_MAX]);
            for (long i = 0; i < POINT_MAX; i++) {
                TournamentWork::POINT_INFO &point = m_pPointInfo[i];
                point.vPos.x = 1.0f;
                point.vPos.z = 1.0f;
                point.vPos.y = 0.005f;
                point.nViewX = 0xFF;
                point.nViewZ = 0xFF;
                point.m_Unk0F = -1;
                point.m_Unk10 = 0;
                point.m_Unk0E = -1;
                point.nFlag = 0;
                point.m_Unk12 = 0xFF;
                point.nCamera = 0;
            }
        }
        _Setup("SetupPoint", 1, 0, 0, 0, 0);

        // Routes.
        m_nRouteNum = _Setup("GetRouteMax", 1, 0, 0, 0, 0);
        if (m_pRouteInfo != 0) {
            delete[] m_pRouteInfo;
        }
        m_pRouteInfo = 0;
        if (m_nRouteNum > 0) {
            m_pRouteInfo = new TournamentWork::ROUTE_INFO[ROUTE_MAX];
            for (long i = 0; i < ROUTE_MAX; i++) {
                TournamentWork::ROUTE_INFO &route = m_pRouteInfo[i];
                route.nPoint2 = 0;
                route.nPoint1 = 0;
                route.nFlag = 0;
                route.nType = -1;
                route.nParam = 0;
                route.nCamera = 0;
                route.m_Unk06[0] = 0;
                route.m_Unk06[1] = 0;
            }
        }
        _Setup("SetupRoute", 1, 0, 0, 0, 0);

        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        info.pPointInfo = m_pPointInfo;
        info.pRouteInfo = m_pRouteInfo;
        info.pCameraInfo = m_pCameraInfo;

        // The cup's first-time script runs once per tournament.
        signed char &bFirst = info.aFirstFlag[(CTournament::GetCup() - 1) * 0x20];
        long nFirst = bFirst;
        if (nFirst == 0) {
            _Setup("FlagsFirstSetup", 1, 0, 0, 0, 0);
        }
        bFirst = 1;
        if (nFirst != 1) {
            _Setup("SetupMatch_Level", 1, 2, CTournament::GetGameModeNoMix() + 1, info.nLevel, 0);
        }
        _Setup("FlagsSetup", 1, 0, 0, 0, 0);
        _Setup("PointSetupEx", 1, 0, 0, 0, 0);
        _Setup("RouteSetupEx", 1, 0, 0, 0, 0);

        // The characters each sport's level scripts give. (GetLevel's
        // result is not used.)
        CTournament::GetLevel();
        for (long nGameMode = 0; nGameMode <= 3; nGameMode++) {
            long nNum = _Setup(s_aCharScript[nGameMode], 0, 2, 0, 0, 0);
            unsigned long long nFlag = 0;
            for (long i = 1; i <= nNum; i++) {
                _ClearItem();
                _Setup(s_aCharScript[nGameMode], 0, 2, i, 0, 0);
                if (m_nCharItemNum > 0 && m_aCharItem[0] != -1) {
                    long nType;
                    long nColor;
                    CUnlockFlags::GetCharType(nType, nColor, m_aCharItem[0]);
                    nFlag |= 1ULL << CUnlockFlags::GetCharFlagPos(nType, nColor, 0);
                }
            }
            info.aLevelCharFlag[nGameMode] = nFlag;
        }
        _ClearItem();
    }

    void CTmScript::SetupRule(long nGameMode, long nRule, long nArg, long nLevel, int bReset)
    {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        for (long i = 0; i < 16; i++) {
            info.aMission[i].nType = 0;
            info.aMission[i].nFlags = 0;
            info.aMission[i].nTarget = 0;
            info.aMission[i].nCount = 0;
            info.aMission[i].nPrevCount = 0;
            info.aMission[i].fCountTime = 0.0f;
            info.aMission[i].nUnk08 = 0;
            info.aMission[i].nUnk10 = 0;
        }
        m_nRuleNum = 0;
        if (nRule > 0) {
            _Setup(s_aRuleScript[nGameMode], bReset, 2, nRule, 0, 0);
        }
        if (nArg > 0) {
            _Setup(s_aRuleScript[nGameMode], bReset, 2, nArg, 0, 0);
        }
        (void)nLevel;
    }

    void CTmScript::SetupPanel(long nPanel, long nLevel, int bReset)
    {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        for (long i = 0; i < 16; i++) {
            info.aPanel[i].nItem = 0;
            info.aPanel[i].nValue = -1;
        }
        m_nPanelNum = 0;
        _Setup("SetupPanelItem_Level", bReset, 2, nPanel, 0, 0);
        (void)nLevel;
    }

    void CTmScript::SetupItemParam(long nItem, long nLevel, int bReset)
    {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        for (long i = 0; i < 32; i++) {
            info.aItemParam[i].nItem = -1;
            info.aItemParam[i].nParam = -1;
            info.aItemParam[i].i = 0;
        }
        m_nItemParamNum = 0;
        _Setup("SetupItemParam_Level", bReset, 2, nItem, 0, 0);
        (void)nLevel;
    }

    long CTmScript::SetupExcludeStage(long nStage, long nExclude, long nLevel, int bReset)
    {
        m_nExcludeNum = 0;
        if (nExclude != 0) {
            _Setup("SetupExcludeStage_Level", bReset, 2, nExclude, 0, 0);
        }
        if (m_nStage != -1) {
            m_aPoint[m_nExcludeNum] = m_nStage;
            m_nExcludeNum++;
        }
        if (nStage == -1) {
            CUnlockFlags flags;
            long aStage[16];
            flags.SetPlayerData(-1);
            long nNum = flags.ListupStage(aStage, 16, -1, CGame::g_GameWork.m_nGameMode);
            for (long i = 0; i < m_nExcludeNum; i++) {
                for (long j = 0; j < nNum; j++) {
                    if (m_aPoint[i] == aStage[j]) {
                        long nTmp = aStage[j];
                        nNum--;
                        aStage[j] = aStage[nNum];
                        aStage[nNum] = nTmp;
                        break;
                    }
                }
            }
            if (nNum > 0) {
                nStage = aStage[nNum > 1 ? bcs::BCS_RandMax(nNum - 1) : 0];
            } else {
                nStage = 1;
            }
        }
        (void)nLevel;
        return nStage;
    }

    int CTmScript::_CheckUnlockStage(long nGameMode)
    {
        int bLocked = 0;
        if (m_nStageItemNum > 0) {
            long nStage = m_aStageItem[0];
            if (nStage != -1) {
                long nParty = -1;
                int bParty = 0;
                CUnlockFlags flags;
                flags.SetPlayerData(-1);
                switch (nGameMode) {
                case 0:
                    if (nStage >= 0x46 && nStage <= 0x54) {
                        nParty = nStage - 0x43;
                        bParty = 1;
                    }
                    break;
                case 1:
                    if (nStage >= 0x28 && nStage <= 0x36) {
                        bParty = 1;
                    }
                    break;
                case 2:
                    if (nStage >= 0x37 && nStage <= 0x45) {
                        nParty = nStage - 0x34;
                        bParty = 1;
                    }
                    break;
                case 3:
                    if (nStage >= 0x55 && nStage <= 0x63) {
                        bParty = 1;
                        nParty = nStage - 0x52;
                    }
                    break;
                }
                if (bParty) {
                    if (nParty != -1 && !flags.IsParty(nParty, nGameMode)) {
                        bLocked = 1;
                    }
                } else if (!flags.IsStage(nStage, nGameMode)) {
                    bLocked = 1;
                }
            }
        }
        return bLocked;
    }

    void CTmScript::SetupMatch(long nMatch, long nTeam0, long nTeam1)
    {
        int bSet = 0;
        TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(nMatch);
        if (match.nState == 0) {
            return;
        }
        match.nSideFlag0D = nTeam0;
        match.nSideFlag0E = nTeam1;
        long nLevel = TournamentWork::GetTournamentInfo().nLevel;
        if (CTournament::IsMixTournamentGame()) {
            // A team with a sport of its own plays the match in it.
            signed char nGameMode = TournamentWork::GetTeamInfo(match.nSideFlag0D).nGameMode;
            if (nGameMode != -1) {
                bSet = 1;
                match.nFlags06 = (match.nFlags06 & 0xF8) | static_cast<unsigned char>(nGameMode);
            }
            nGameMode = TournamentWork::GetTeamInfo(match.nSideFlag0E).nGameMode;
            if (!bSet && nGameMode != -1) {
                bSet = 1;
                match.nFlags06 = (match.nFlags06 & 0xF8) | static_cast<unsigned char>(nGameMode);
            }
            // Otherwise a sport whose stage item is not unlocked yet.
            if (!bSet && match.nRule != 0) {
                long aGameMode[8];
                long nNum = 0;
                for (long i = 0; i <= 3; i++) {
                    _ClearItem();
                    _Setup(s_aCharScript[i], 0, 2, match.nRule, 0, 0);
                    if (_CheckUnlockStage(i)) {
                        aGameMode[nNum] = i;
                        nNum++;
                    }
                }
                if (nNum >= 1) {
                    long nGameMode = aGameMode[bcs::BCS_RandMax(nNum - 1)];
                    _ClearItem();
                    _Setup(s_aCharScript[nGameMode], 0, 2, match.nRule, 0, 0);
                    if (m_nStageItemNum > 0 && m_aStageItem[0] != -1) {
                        match.nStage = m_aStageItem[0];
                        bSet = 1;
                        match.nFlags06 = (((match.nFlags06 & 0x3F) | 0x80) & 0xF8) | static_cast<unsigned char>(nGameMode);
                    }
                }
            }
            // Otherwise the next sport of the tournament's order.
            TournamentWork::GetTournamentInfo();
            if (!bSet && !(match.nFlags06 & 0x10) && (match.nSideFlag0D == 0 || match.nSideFlag0E == 0)) {
                TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
                long nNext = info.aGameModeOrder[0];
                info.aGameModeOrder[0] = info.aGameModeOrder[1];
                info.aGameModeOrder[1] = info.aGameModeOrder[2];
                info.aGameModeOrder[2] = info.aGameModeOrder[3];
                info.aGameModeOrder[3] = nNext;
                match.nFlags06 = (match.nFlags06 & 0xF8) | static_cast<unsigned char>(nNext);
                if (match.nFlags06 & 0x20) {
                    long nStage = match.nStage;
                    _Setup("UpdateMatch_Level", 0, 3, nNext + 1, nLevel, nStage);
                    const TournamentWork::MATCH_INFO &from = TournamentWork::GetMatchInfo(nStage);
                    match.nStage = from.nStage;
                    match.nComLevel = from.nComLevel;
                } else {
                    _Setup("UpdateMatch_Level", 0, 3, nNext + 1, nLevel, nMatch);
                }
                match.nFlags06 = (match.nFlags06 & 0xDF) | 0x10;
            }
        } else if (match.nRule != 0) {
            _ClearItem();
            _Setup(s_aCharScript[match.nFlags06 & 7], 0, 2, match.nRule, 0, 0);
            if (m_nStageItemNum > 0 && m_aStageItem[0] != -1) {
                match.nStage = m_aStageItem[0];
                match.nFlags06 = (match.nFlags06 & 0x3F) | 0x80;
            }
        }
    }

    long CTmScript::_Setup(const char *pName, int bReset, long nArg, long nValue0, long nValue1, long nValue2)
    {
        if (bReset) {
            Reset();
        }
        long nSP = m_nSP;
        long nFP = m_nFP;
        short nState = m_nState;
        unsigned char *pPC = m_pPC;
        switch (nArg) {
        case 3:
            m_nSP += 3;
            SetStack(m_nSP - 3, nValue0);
            SetStack(m_nSP - 2, nValue1);
            SetStack(m_nSP - 1, nValue2);
            break;
        case 2:
            m_nSP += 2;
            SetStack(m_nSP - 2, nValue0);
            SetStack(m_nSP - 1, nValue1);
            break;
        case 1:
            m_nSP += 1;
            SetStack(m_nSP - 1, nValue0);
            break;
        }
        SetCall(pName, nArg, 4, 1);
        if (Run() == 2 && m_nArgNum == 0) {
            m_nState = nState;
            m_nFP = nFP;
            m_nSP = nSP;
            m_pPC = pPC;
            return m_nUnk1C;
        }
        return 0;
    }

    int CTmScript::_SetCall(const char *pName, long nArg, long nValue0, long nValue1, long nValue2)
    {
        switch (nArg) {
        case 3:
            m_nSP += 3;
            SetStack(m_nSP - 3, nValue0);
            SetStack(m_nSP - 2, nValue1);
            SetStack(m_nSP - 1, nValue2);
            break;
        case 2:
            m_nSP += 2;
            SetStack(m_nSP - 2, nValue0);
            SetStack(m_nSP - 1, nValue1);
            break;
        case 1:
            m_nSP += 1;
            SetStack(m_nSP - 1, nValue0);
            break;
        }
        return SetCall(pName, nArg, 4, 1);
    }

    long CTmScript::CallEventEntry()
    {
        long nResult = 0;
        long nState = Run();
        switch (nState) {
        case 3:
        case 4:
            break;
        case 1:
            nResult = 2;
            break;
        case 2:
            if (m_nArgNum == 0 && m_nUnk1C != 0) {
                nResult = 1;
            }
            break;
        case 0:
            break;
        default:
            nResult = m_nState;
            break;
        }
        return nResult;
    }

    void CTmScript::_BindSysCall(SYSCALL pFunc, long nName)
    {
        vm::CVMData::FUNCTION_TAG *pTag = m_pData->FindFunctionTag(nName, 3);
        if (pTag != 0) {
            short nNo = pTag->nNo;
            SYSCALL *pSysCall = &m_pSysCall[nNo];
            *pSysCall = pFunc;
        }
    }

    long CTmScript::OpSysCall()
    {
        // The call number follows the opcode, big-endian.
        long nCall = m_pPC[3];
        nCall |= m_pPC[2] << 8;
        nCall |= m_pPC[1] << 16;
        nCall |= m_pPC[0] << 24;
        m_pPC += 4;
        if (nCall < 0 || nCall >= m_nSysCallNum) {
            return 3;
        }
        if (m_pSysCall[nCall] != 0) {
            return (this->*m_pSysCall[nCall])();
        }
        return 3;
    }

    void CTmScript::UpdateBind()
    {
        if (m_pSysCall != 0) {
            delete[] m_pSysCall;
        }
        m_pSysCall = 0;
        m_nSysCallNum = m_pData->GetSysCallNum();
        m_pSysCall = new SYSCALL[m_nSysCallNum];
        for (long i = 0; i < m_nSysCallNum; i++) {
            m_pSysCall[i] = 0;
        }

        // The second argument is the hash of the call's name in the
        // scripts.
        _BindSysCall(&CTmScript::_OpSysCallIsMatchEnd, 0x9F117845);
        _BindSysCall(&CTmScript::_OpSysCallGetVsMatchPoint, 0x6AFCB477);
        _BindSysCall(&CTmScript::_OpSysCallGetWinTeam, 0xB6D87CDF);
        _BindSysCall(&CTmScript::_OpSysCallGetLoseTeam, 0x496F2CBA);
        _BindSysCall(&CTmScript::_OpSysCallGetMissionClear, 0xD8D1F9F8);
        _BindSysCall(&CTmScript::_OpSysCallGetPointMatch, 0x5F50EA39);
        _BindSysCall(&CTmScript::_OpSysCallSetMatchMake, 0x5B2978F2);
        _BindSysCall(&CTmScript::_OpSysCallSetMatchRule, 0x630B6072);
        _BindSysCall(&CTmScript::_OpSysCallSetMatchStage, 0x192B1D01);
        _BindSysCall(&CTmScript::_OpSysCallSetMatchLevel, 0x185CE27C);
        _BindSysCall(&CTmScript::_OpSysCallSetMatchTelop, 0xAD5101D0);
        _BindSysCall(&CTmScript::_OpSysCallSetMatchUnlock, 0x1A0C0C71);
        _BindSysCall(&CTmScript::_OpSysCallCloseMatchMake, 0x852E00B4);
        _BindSysCall(&CTmScript::_OpSysCallSecretMake, 0x7FE5C1DD);
        _BindSysCall(&CTmScript::_OpSysCallMatchMake, 0xA7057853);
        _BindSysCall(&CTmScript::_OpSysCallMatchVsPause, 0x1DF0A6DC);
        _BindSysCall(&CTmScript::_OpSysCallSetPanelItem, 0xD99EEE67);
        _BindSysCall(&CTmScript::_OpSysCallSetCharItem, 0xE04116AE);
        _BindSysCall(&CTmScript::_OpSysCallSetStarCharItem, 0x4015B965);
        _BindSysCall(&CTmScript::_OpSysCallSetStageItem, 0x23FBBB18);
        _BindSysCall(&CTmScript::_OpSysCallSetRuleItem, 0xA5BBB4D5);
        _BindSysCall(&CTmScript::_OpSysCallSetCameraData, 0x7CBFCEFE);
        _BindSysCall(&CTmScript::_OpSysCallSetPointData, 0x918D07B8);
        _BindSysCall(&CTmScript::_OpSysCallSetRouteData, 0x52BD49EA);
        _BindSysCall(&CTmScript::_OpSysCallSetRoutePoints, 0x95CCBE50);
        _BindSysCall(&CTmScript::_OpSysCallSetExcludeStage, 0x06542B0B);
        _BindSysCall(&CTmScript::_OpSysCallSetItemParamF, 0x126EFC34);
        _BindSysCall(&CTmScript::_OpSysCallSetItemParamI, 0x126EFCCB);
        _BindSysCall(&CTmScript::_OpSysCallPointCamera, 0x9C16B12D);
        _BindSysCall(&CTmScript::_OpSysCallFadeIn, 0x5AB3E9BB);
        _BindSysCall(&CTmScript::_OpSysCallIsFadeInReq, 0x1340A2D8);
        _BindSysCall(&CTmScript::_OpSysCallWaitResultMove, 0xF555397A);
        _BindSysCall(&CTmScript::_OpSysCallWaitCamera, 0xABB5A313);
        _BindSysCall(&CTmScript::_OpSysCallWaitAction, 0x967F027C);
        _BindSysCall(&CTmScript::_OpSysCallWaitFade, 0x1D59CCC9);
        _BindSysCall(&CTmScript::_OpSysCallWaitTimer, 0xC13273DE);
        _BindSysCall(&CTmScript::_OpSysCallCreateTeamMission, 0x52A1C686);
        _BindSysCall(&CTmScript::_OpSysCallIsUnlockMission, 0xF70B780F);
        _BindSysCall(&CTmScript::_OpSysCallGetUnlockExchange, 0xCFC0A5F4);
        _BindSysCall(&CTmScript::_OpSysCallEraseTeam, 0xE6C5BD2F);
        _BindSysCall(&CTmScript::_OpSysCallSetTeamPos, 0x38C22F48);
        _BindSysCall(&CTmScript::_OpSysCallGetPointTeam, 0x9778C965);
        _BindSysCall(&CTmScript::_OpSysCallStockRelayPoint, 0x10E1C04E);
        _BindSysCall(&CTmScript::_OpSysCallSetMoveTarget, 0x0F00038C);
        _BindSysCall(&CTmScript::_OpSysCallSetMoveResult, 0xE0949EF7);
        _BindSysCall(&CTmScript::_OpSysCallSetPointFlags, 0x285F4917);
        _BindSysCall(&CTmScript::_OpSysCallSetPointOffset, 0x5C07F84A);
        _BindSysCall(&CTmScript::_OpSysCallSetPointCamera, 0x48092097);
        _BindSysCall(&CTmScript::_OpSysCallGetPointCamera, 0x12452638);
        _BindSysCall(&CTmScript::_OpSysCallSetChipWrite, 0xD931599B);
        _BindSysCall(&CTmScript::_OpSysCallGetChipWrite, 0xF6A2DA4D);
        _BindSysCall(&CTmScript::_OpSysCallSetRouteWrite, 0x38A263DA);
        _BindSysCall(&CTmScript::_OpSysCallGetRouteWrite, 0x6A301A8E);
        _BindSysCall(&CTmScript::_OpSysCallSetMapObj, 0xD54AD5B9);
        _BindSysCall(&CTmScript::_OpSysCallSetMapObjPoint, 0x2EF539F7);
        _BindSysCall(&CTmScript::_OpSysCallRouteEvent, 0xEC31EDA6);
        _BindSysCall(&CTmScript::_OpSysCallRtSelSetup, 0x9D513AA4);
        _BindSysCall(&CTmScript::_OpSysCallRtSelect, 0x01CA9388);
        _BindSysCall(&CTmScript::_OpSysCallTurnMode, 0x66161B7C);
        _BindSysCall(&CTmScript::_OpSysCallGetTurnMode, 0xEC07E51E);
        _BindSysCall(&CTmScript::_OpSysCallGetTmLevel, 0xE2DE9893);
        _BindSysCall(&CTmScript::_OpSysCallGetTmMode, 0xD7C3D034);
        _BindSysCall(&CTmScript::_OpSysCallGetTmCallMode, 0xEC67E9CD);
        _BindSysCall(&CTmScript::_OpSysCallIsFirstAround, 0xAEC9F923);
        _BindSysCall(&CTmScript::_OpSysCallIsWin, 0xD00F367D);
        _BindSysCall(&CTmScript::_OpSysCallIsAllStarChar, 0xAA8AA99F);
        _BindSysCall(&CTmScript::_OpSysCallGetTeamMax, 0x65812EB3);
        _BindSysCall(&CTmScript::_OpSysCallGetRandom, 0x1C001C4E);
        _BindSysCall(&CTmScript::_OpSysCallBackupFlags, 0x8B105AB6);
        _BindSysCall(&CTmScript::_OpSysCallRestoreFlags, 0x35B3CCA9);
        _BindSysCall(&CTmScript::_OpSysCallBackupFirstFlags, 0x933F50AF);
        _BindSysCall(&CTmScript::_OpSysCallRestoreFirstFlags, 0x384377F4);
        _BindSysCall(&CTmScript::_OpSysCallUnlockOpen, 0x26208069);
        _BindSysCall(&CTmScript::_OpSysCallDbSelIndex, 0x6FFB6E51);
        _BindSysCall(&CTmScript::_OpSysCallDbSelSetup, 0xB16CA12D);
        _BindSysCall(&CTmScript::_OpSysCallDbSelItem, 0xF160C4E2);
        _BindSysCall(&CTmScript::_OpSysCallDbSelItemLabel, 0xC7E95ED9);
        _BindSysCall(&CTmScript::_OpSysCallDbSelItemLabelValue, 0xBEB7DD19);
        _BindSysCall(&CTmScript::_OpSysCallDbSelItemLabelStr, 0xDFF7DF0F);
        _BindSysCall(&CTmScript::_OpSysCallDbSelItemLabelStrValue, 0xF3AFE804);
        _BindSysCall(&CTmScript::_OpSysCallDbSelStockLabelStr, 0xC2DDDEF0);
        _BindSysCall(&CTmScript::_OpSysCallDbSelect, 0x71773984);
        _BindSysCall(&CTmScript::_OpSysCallDbFlgSetup, 0xE331AFE7);
        _BindSysCall(&CTmScript::_OpSysCallDbFlgItem, 0xBB5E4684);
        _BindSysCall(&CTmScript::_OpSysCallDbScript, 0x8FD23FD0);
        _BindSysCall(&CTmScript::_OpSysCallDbScriptEnd, 0xC614128C);
        _BindSysCall(&CTmScript::_OpSysCallDbTrace, 0x215B3AD3);
        _BindSysCall(&CTmScript::_OpSysCallDbTrace1, 0x9738D7E9);
    }

    void CTmScript::ResetBind()
    {
    }

    // The system calls. Arguments are on the VM stack below the top word:
    // with n arguments, the first is at m_nSP - n - 1 and the last at
    // m_nSP - 2. A call that returns a value pops n words and leaves the
    // value on top; one that does not pops n + 1. The value returned is the
    // VM state: 0 to go on, 5 and up to wait.

    long CTmScript::_OpSysCallWaitResultMove()
    {
        m_nSP -= 1;
        return 5;
    }

    long CTmScript::_OpSysCallWaitCamera()
    {
        m_nSP -= 1;
        return 8;
    }

    long CTmScript::_OpSysCallWaitAction()
    {
        m_nSP -= 1;
        return 9;
    }

    long CTmScript::_OpSysCallWaitFade()
    {
        m_nSP -= 1;
        return 7;
    }

    long CTmScript::_OpSysCallWaitTimer()
    {
        m_fWaitTimer = GetStack(m_nSP - 2);
        m_nSP -= 2;
        return 6;
    }

    long CTmScript::_OpSysCallSetMapObj()
    {
        int nFlag = GetStack(m_nSP - 2);
        const char *pName = reinterpret_cast<const char *>(GetStack(m_nSP - 3));
        float fRotY = GetStackF(m_nSP - 4);
        float fZ = GetStackF(m_nSP - 5);
        float fY = GetStackF(m_nSP - 6);
        float fX = GetStackF(m_nSP - 7);
        long nObj = GetStack(m_nSP - 8);
        m_nSP -= 8;
        if (m_pHandler != 0) {
            bcs::FVEC3 vPos(fX, fY, fZ);
            m_pHandler->MapObjSet(nObj, vPos, DEG_TO_RAD * fRotY, pName, nFlag);
        }
        return 0;
    }

    long CTmScript::_OpSysCallSetMapObjPoint()
    {
        int nFlag = GetStack(m_nSP - 2);
        const char *pName = reinterpret_cast<const char *>(GetStack(m_nSP - 3));
        float fRotY = GetStackF(m_nSP - 4);
        long nPoint = GetStack(m_nSP - 5);
        long nObj = GetStack(m_nSP - 6);
        m_nSP -= 6;
        if (m_pHandler != 0) {
            bcs::FVEC3 vPos;
            TournamentWork::GetPointViewPos(vPos, nPoint);
            m_pHandler->MapObjSet(nObj, vPos, DEG_TO_RAD * fRotY, pName, nFlag);
        }
        return 0;
    }

    long CTmScript::_OpSysCallRouteEvent()
    {
        long nEnd = GetStack(m_nSP - 2);
        unsigned long nUnk = GetStack(m_nSP - 3);
        long nStart = GetStack(m_nSP - 4);
        long nEvent = GetStack(m_nSP - 5);
        long nRoute = GetStack(m_nSP - 6);
        m_nSP -= 6;
        if (m_pHandler != 0) {
            m_pHandler->SendRouteEvent(nRoute, nEvent, nStart / 100.0f, nUnk, nEnd / 100.0f);
        }
        return 0;
    }

    long CTmScript::_OpSysCallGetRouteWrite()
    {
        long nRoute = GetStack(m_nSP - 2);
        long bWrite = (TournamentWork::GetRouteInfo(nRoute)->nFlag >> 7) & 1;
        m_nSP -= 1;
        SetStack(m_nSP - 1, bWrite);
        return 0;
    }

    long CTmScript::_OpSysCallSetRouteWrite()
    {
        long nWrite = GetStack(m_nSP - 2);
        long nRoute = GetStack(m_nSP - 3);
        TournamentWork::ROUTE_INFO *pRoute = TournamentWork::RouteInfo(nRoute);
        int bOld = (pRoute->nFlag >> 7) & 1;
        pRoute->nFlag = (pRoute->nFlag & 0xFC) | ((nWrite & 3) | 0x80);
        m_nSP -= 3;
        if (m_pHandler != 0) {
            m_pHandler->RouteWrite(nRoute, bOld != ((pRoute->nFlag >> 7) & 1));
        }
        return 0;
    }

    long CTmScript::_OpSysCallSetChipWrite()
    {
        int nUnk4 = GetStack(m_nSP - 2);
        long nUnk3 = GetStack(m_nSP - 3);
        unsigned long nUnk2 = GetStack(m_nSP - 4);
        long nPoint = GetStack(m_nSP - 5);
        m_nSP -= 5;
        if (m_pHandler != 0) {
            m_pHandler->ChipWrite(nPoint, nUnk2, 0, nUnk3, nUnk4);
        }
        return 0;
    }

    long CTmScript::_OpSysCallGetChipWrite()
    {
        long nPoint = GetStack(m_nSP - 2);
        long bWrite = (TournamentWork::GetPointInfo(nPoint)->nFlag >> 7) & 1;
        m_nSP -= 1;
        SetStack(m_nSP - 1, bWrite);
        return 0;
    }

    long CTmScript::_OpSysCallSetPointFlags()
    {
        long nFlag = GetStack(m_nSP - 2);
        long nPoint = GetStack(m_nSP - 3);
        TournamentWork::POINT_INFO *pPoint = TournamentWork::PointInfo(nPoint);
        pPoint->nFlag = (pPoint->nFlag & 0x80) | static_cast<unsigned char>(nFlag);
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallSetPointOffset()
    {
        long nZ = GetStack(m_nSP - 2);
        long nY = GetStack(m_nSP - 3);
        long nX = GetStack(m_nSP - 4);
        long nPoint = GetStack(m_nSP - 5);
        TournamentWork::POINT_INFO *pPoint = TournamentWork::PointInfo(nPoint);
        pPoint->vPos.x = nX / 100.0f;
        pPoint->vPos.z = nZ / 100.0f;
        pPoint->vPos.y = nY / 100.0f + 0.01f;
        m_nSP -= 5;
        return 0;
    }

    long CTmScript::_OpSysCallSetPointCamera()
    {
        long nCamera = GetStack(m_nSP - 2);
        long nPoint = GetStack(m_nSP - 3);
        TournamentWork::PointInfo(nPoint)->nCamera = nCamera;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallGetPointCamera()
    {
        long nPoint = GetStack(m_nSP - 2);
        long nCamera = TournamentWork::GetPointInfo(nPoint)->nCamera;
        m_nSP -= 1;
        SetStack(m_nSP - 1, nCamera);
        return 0;
    }

    long CTmScript::_OpSysCallGetUnlockExchange()
    {
        m_nSP -= 3;
        SetStack(m_nSP - 1, 0);
        return 0;
    }

    long CTmScript::_OpSysCallIsUnlockMission()
    {
        // 2 when the level's character and stage are open, 1 when the
        // character is still hidden, 0 when something is locked.
        long nRule = GetStack(m_nSP - 2);
        m_nSP -= 1;
        CTournament::GetLevel();
        long nGameMode = CTournament::GetGameMode();
        long nType;
        long nColor;
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        long nResult = 2;
        long nLocked = 0;
        if (CTournament::IsMixTournamentGame()) {
            for (long i = 0; i <= 3; i++) {
                _ClearItem();
                _Setup(s_aCharScript[i], 0, 2, nRule, 0, 0);
                if (m_nCharItemNum > 0) {
                    long nChar = m_aCharItem[0];
                    if (nChar != -1) {
                        CUnlockFlags::GetCharType(nType, nColor, nChar);
                        switch (nType) {
                        case 0xE:
                        case 0xF:
                        case 0x11:
                            if (!CTournament::IsCupClear(3, i, 0, -1)) {
                                nResult = 1;
                                break;
                            }
                            // fall through
                        case 0x10:
                            if (nColor != 0 && !flags.IsUnlockChar(nType, 0, i)) {
                                nResult = 1;
                                break;
                            }
                            // fall through
                        default:
                            if (!flags.IsUnlockChar(nType, nColor, i)) {
                                nLocked++;
                            }
                            break;
                        }
                    }
                }
                if (_CheckUnlockStage(i)) {
                    nLocked++;
                }
            }
        } else {
            _ClearItem();
            _Setup(s_aCharScript[nGameMode], 0, 2, nRule, 0, 0);
            if (m_nCharItemNum > 0) {
                long nChar = m_aCharItem[0];
                if (nChar != -1) {
                    CUnlockFlags::GetCharType(nType, nColor, nChar);
                    switch (nType) {
                    case 0xE:
                    case 0xF:
                    case 0x11:
                        if (!CTournament::IsCupClear(3, nGameMode, 0, -1)) {
                            nResult = 1;
                            break;
                        }
                        // fall through
                    case 0x10:
                        if (nColor != 0 && !flags.IsUnlockChar(nType, 0, nGameMode)) {
                            nResult = 1;
                            break;
                        }
                        // fall through
                    default:
                        if (!flags.IsUnlockChar(nType, nColor, nGameMode)) {
                            nLocked = 1;
                        }
                        break;
                    }
                }
            }
            if (_CheckUnlockStage(nGameMode)) {
                nLocked++;
            }
        }
        SetStack(m_nSP - 1, nLocked > 0 ? 0 : nResult);
        return 0;
    }

    long CTmScript::_OpSysCallCreateTeamMission()
    {
        long aGameMode[4];
        long nType;
        long nColor;
        CUnlockFlags flags;
        long nTeam = GetStack(m_nSP - 8);
        long nFlag = GetStack(m_nSP - 2);
        long nScale = GetStack(m_nSP - 3);
        long nRotY = GetStack(m_nSP - 4);
        long nSetting = GetStack(m_nSP - 5);
        long nRule = GetStack(m_nSP - 6);
        long nPoint = GetStack(m_nSP - 7);
        m_nSP -= 7;

        // The last free team when none is given.
        if (nTeam == -1) {
            for (long i = TEAM_MAX - 1; i >= 0; i--) {
                if (TournamentWork::GetTeamInfo(i).nPoint == 0xFF) {
                    nTeam = i;
                    break;
                }
            }
        }

        TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
        team.aCharType[0] = -1;
        team.aCharFlag[0] = 0;
        team.aCharType[1] = -1;
        team.aCharFlag[1] = 0;
        team.aCharType[2] = -1;
        team.aCharFlag[2] = 0;
        team.fUnk00[0] = 0.0f;
        team.fUnk00[1] = 0.0f;
        team.fUnk00[2] = 0.0f;
        team.fUnk00[3] = 0.0f;
        team.nTurn = 0;
        team.m_Unk21 = 0;
        team.nGameMode = -1;
        team.aRoute[0] = 0;
        team.aRoute[1] = 0;
        team.aRoute[2] = 0;
        team.aRoute[3] = 0;
        team.nPoint = nPoint;
        team.nTargetPoint = nPoint;
        team.nFlag22 = nFlag;
        if (nSetting == 0) {
            team.nFlag22 |= 4;
        }

        // (the result is not used)
        TournamentWork::GetUsedChar(-1, 1);
        long nLevel = CTournament::GetLevel();
        long nGameMode = CTournament::GetGameMode();
        flags.SetPlayerData(-1);
        if (CTournament::IsMixTournamentGame()) {
            // The team plays a sport whose level character is not unlocked
            // yet; otherwise the next sport of the tournament's order.
            long nNum = 0;
            if (nRule > 0) {
                for (long i = 0; i <= 3; i++) {
                    _ClearItem();
                    _Setup(s_aCharScript[i], 0, 2, nRule, 0, 0);
                    if (m_nCharItemNum > 0) {
                        long nChar = m_aCharItem[0];
                        if (nChar != -1) {
                            CUnlockFlags::GetCharType(nType, nColor, nChar);
                            switch (nType) {
                            case 0xE:
                            case 0xF:
                            case 0x11:
                                if (!CTournament::IsCupClear(3, i, 0, -1)) {
                                    break;
                                }
                                // fall through
                            case 0x10:
                                if (nColor != 0 && !flags.IsUnlockChar(nType, 0, i)) {
                                    break;
                                }
                                // fall through
                            default:
                                if (!flags.IsUnlockChar(nType, nColor, i)) {
                                    aGameMode[nNum] = i;
                                    nNum++;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            if (nNum > 0) {
                nGameMode = aGameMode[RandomIndex(nNum)];
                team.nGameMode = nGameMode;
            } else {
                nGameMode = TournamentWork::TournamentInfo().aGameModeOrder[0];
            }
        }

        // A hidden level character is not given.
        _ClearItem();
        if (nRule > 0) {
            _Setup(s_aCharScript[nGameMode], 0, 2, nRule, 0, 0);
        }
        if (m_nCharItemNum > 0) {
            long nChar = m_aCharItem[0];
            if (nChar != -1) {
                CUnlockFlags::GetCharType(nType, nColor, nChar);
                switch (nType) {
                case 0xE:
                case 0xF:
                case 0x11:
                    if (!CTournament::IsCupClear(3, nGameMode, 0, -1)) {
                        m_aCharItem[0] = -1;
                        break;
                    }
                    // fall through
                case 0x10:
                    if (nColor != 0 && !flags.IsUnlockChar(nType, 0, nGameMode)) {
                        m_aCharItem[0] = -1;
                        break;
                    }
                    // fall through
                default:
                    flags.IsUnlockChar(nType, nColor, nGameMode);
                    break;
                }
            }
        }

        // The leader: the level script's character, or a random one.
        long nChar = m_nCharItemNum > 0 ? m_aCharItem[0] : -1;
        if (nChar == -1) {
            unsigned long long nExclude = 0;
            if (nSetting != 0) {
                nExclude = TournamentWork::GetSettingChar(nGameMode, nLevel);
            }
            nExclude |= flags.GetUnlockChar(nGameMode, nLevel, 1);
            CUnlockFlags::GetCharType(nType, nColor,
                                      flags.GetCharRandom(nGameMode, -1, nExclude, TournamentWork::GetUsedChar(-1, 1)));
            team.aCharType[0] = nType;
            team.aCharFlag[0] = (team.aCharFlag[0] & 0xF8) | static_cast<unsigned char>(nColor);
        } else {
            CUnlockFlags::GetCharType(nType, nColor, nChar);
            team.aCharType[0] = nType;
            team.aCharFlag[0] = (team.aCharFlag[0] & 0xF8) | static_cast<unsigned char>(nColor);
        }

        if (m_nCharItemNum > 1 && m_aCharItem[1] != -1) {
            long nType1;
            long nColor1;
            CUnlockFlags::GetCharType(nType1, nColor1, m_aCharItem[1]);
            team.aCharType[1] = nType1;
            team.aCharFlag[1] = (team.aCharFlag[1] & 0xF8) | static_cast<unsigned char>(nColor1);
        }
        if (m_nCharItemNum > 2 && m_aCharItem[2] != -1) {
            long nType2;
            long nColor2;
            CUnlockFlags::GetCharType(nType2, nColor2, m_aCharItem[2]);
            team.aCharType[2] = nType2;
            team.aCharFlag[2] = (team.aCharFlag[2] & 0xF8) | static_cast<unsigned char>(nColor2);
        }

        if (m_pHandler != 0) {
            m_pHandler->TeamCreate(nTeam, nSetting, DEG_TO_RAD * nRotY, nScale / 100.0f);
        }
        SetStack(m_nSP - 1, nTeam);
        return 0;
    }

    long CTmScript::_OpSysCallEraseTeam()
    {
        long nTeam = GetStack(m_nSP - 3);
        long nUnk = GetStack(m_nSP - 2);
        if (nTeam != -1) {
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
            if (team.nPoint != 0xFF) {
                team.aCharFlag[0] |= 0x10;
                team.aCharFlag[1] |= 0x10;
                team.aCharFlag[2] |= 0x10;
            }
        }
        if (m_pHandler != 0) {
            m_pHandler->TeamErase(nTeam, nUnk);
        }
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallSetTeamPos()
    {
        long nPoint = GetStack(m_nSP - 2);
        long nTeam = GetStack(m_nSP - 3);
        m_nSP -= 3;
        if (m_pHandler != 0) {
            m_pHandler->TeamPos(nTeam, nPoint, -1);
        }
        return 0;
    }

    long CTmScript::_OpSysCallSetMoveTarget()
    {
        long nRoute = GetStack(m_nSP - 2);
        long nPoint = GetStack(m_nSP - 3);
        long nTeam = GetStack(m_nSP - 4);
        TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
        team.nTargetPoint = nPoint;
        if (m_pHandler != 0) {
            m_pHandler->MoveTarget(nTeam, nPoint, nRoute > 0 ? m_aPoint : 0);
        }
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetMoveResult()
    {
        float fUnk = GetStackF(m_nSP - 2);
        int nResult = GetStack(m_nSP - 3);
        long nTeam = GetStack(m_nSP - 4);
        if (m_pHandler != 0) {
            m_pHandler->MoveResult(nTeam, nResult, fUnk);
        }
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallStockRelayPoint()
    {
        long nNum = GetStack(m_nSP - 2);
        const char *pName = reinterpret_cast<const char *>(GetStack(m_nSP - 3));
        for (long i = 0; i < 0x20; i++) {
            m_aPoint[i] = 0xFF;
        }
        vm::CVMData::VALUE_TAG *pTag = m_pData->FindValueTag(pName, 1);
        if (pTag != 0) {
            for (long i = 0; i < nNum; i++) {
                m_aPoint[i] = m_pData->Value(pTag->nIndex + i);
            }
        }
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallGetMissionClear()
    {
        long nResult = -1;
        long nMatch = GetStack(m_nSP - 2);
        if (nMatch != -1) {
            nResult = (TournamentWork::GetMatchInfo(nMatch).nState >> 3) & 1;
        }
        m_nSP -= 1;
        SetStack(m_nSP - 1, nResult);
        return 0;
    }

    long CTmScript::_OpSysCallGetPointTeam()
    {
        long nResult = -1;
        long bSkipUser = GetStack(m_nSP - 2);
        long nPoint = GetStack(m_nSP - 3);
        TournamentWork::GetTournamentInfo();
        for (long i = 0; i < TEAM_MAX; i++) {
            // With bSkipUser set, team 0 (the user's) does not count.
            if (bSkipUser == 0 || i != 0) {
                const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(i);
                if (team.nPoint != 0xFF && team.nPoint == nPoint) {
                    nResult = i;
                    break;
                }
            }
        }
        m_nSP -= 2;
        SetStack(m_nSP - 1, nResult);
        return 0;
    }

    long CTmScript::_OpSysCallGetWinTeam()
    {
        long nResult = -1;
        long nMatch = GetStack(m_nSP - 2);
        if (nMatch != -1) {
            nResult = TournamentWork::GetMatchInfo(nMatch).nWinTeam;
        }
        m_nSP -= 1;
        SetStack(m_nSP - 1, nResult);
        return 0;
    }

    long CTmScript::_OpSysCallGetLoseTeam()
    {
        long nResult = -1;
        long nMatch = GetStack(m_nSP - 2);
        if (nMatch != -1) {
            const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(nMatch);
            nResult = match.nSideFlag0D != match.nWinTeam ? match.nSideFlag0D : match.nSideFlag0E;
        }
        m_nSP -= 1;
        SetStack(m_nSP - 1, nResult);
        return 0;
    }

    long CTmScript::_OpSysCallSetExcludeStage()
    {
        m_aPoint[m_nExcludeNum] = GetStack(m_nSP - 2);
        m_nExcludeNum++;
        m_nSP -= 2;
        return 0;
    }

    long CTmScript::_OpSysCallSetItemParamF()
    {
        float fValue = GetStackF(m_nSP - 2);
        long nParam = GetStack(m_nSP - 3);
        long nItem = GetStack(m_nSP - 4);
        TournamentWork::ITEM_PARAM &param = TournamentWork::TournamentInfo().aItemParam[m_nItemParamNum++];
        param.nItem = nItem;
        param.nParam = nParam;
        param.f = fValue;
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetItemParamI()
    {
        long nValue = GetStack(m_nSP - 2);
        long nParam = GetStack(m_nSP - 3);
        long nItem = GetStack(m_nSP - 4);
        TournamentWork::ITEM_PARAM &param = TournamentWork::TournamentInfo().aItemParam[m_nItemParamNum++];
        param.nItem = nItem;
        param.nParam = nParam;
        param.i = nValue;
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetPanelItem()
    {
        long nValue = GetStack(m_nSP - 2);
        long nItem = GetStack(m_nSP - 3);
        TournamentWork::PANEL_INFO &panel = TournamentWork::TournamentInfo().aPanel[m_nPanelNum++];
        panel.nItem = nItem;
        panel.nValue = nValue;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallSetCharItem()
    {
        m_aCharItem[m_nCharItemNum] = GetStack(m_nSP - 3);
        m_nCharItemNum++;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallSetStarCharItem()
    {
        long nChar = GetStack(m_nSP - 3);
        if (GetStack(m_nSP - 4) != 0) {
            m_aStarCharItem2[m_nStarCharItemNum2] = nChar;
            m_nStarCharItemNum2++;
        } else {
            m_aStarCharItem[m_nStarCharItemNum] = nChar;
            m_nStarCharItemNum++;
        }
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetStageItem()
    {
        m_aStageItem[m_nStageItemNum] = GetStack(m_nSP - 3);
        m_nStageItemNum++;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallSetRuleItem()
    {
        long nFlags = GetStack(m_nSP - 2);
        long nTarget3 = GetStack(m_nSP - 3);
        long nTarget12 = GetStack(m_nSP - 4);
        long nTarget = GetStack(m_nSP - 5);
        long nType = GetStack(m_nSP - 6);
        TournamentWork::RULE_INFO &rule = TournamentWork::TournamentInfo().aMission[m_nRuleNum++];
        rule.nType = nType;
        rule.nUnk08 = 0;
        rule.nFlags = nFlags;
        // The target by the tournament's level: one value for levels 1 and
        // 2, one for 3, one for the rest.
        switch (CTournament::GetLevel()) {
        case 3:
            rule.nTarget = nTarget3;
            break;
        case 1:
        case 2:
            rule.nTarget = nTarget12;
            break;
        default:
            rule.nTarget = nTarget;
            break;
        }
        m_nSP -= 6;
        return 0;
    }

    long CTmScript::_OpSysCallSetCameraData()
    {
        // Distances in hundredths, angles in hundredths of a degree.
        long nX = GetStack(m_nSP - 5);
        long nY = GetStack(m_nSP - 4);
        long nZ = GetStack(m_nSP - 3);
        long nRotX = GetStack(m_nSP - 7);
        long nRotY = GetStack(m_nSP - 6);
        long nDist = GetStack(m_nSP - 2);
        TournamentData::CAMERA_INFO &camera = m_pCameraInfo[GetStack(m_nSP - 8)];
        camera.ofs.x = nX / 100.0f;
        camera.ofs.y = nY / 100.0f;
        camera.ofs.z = nZ / 100.0f;
        camera.rot.x = DEG_TO_RAD * (nRotX / 100.0f);
        camera.rot.y = DEG_TO_RAD * (nRotY / 100.0f);
        camera.dist = nDist / 100.0f;
        m_nSP -= 8;
        return 0;
    }

    long CTmScript::_OpSysCallSetPointData()
    {
        // Two words of four packed bytes each.
        long nPoint = GetStack(m_nSP - 4);
        vm::STACK_DATA data0;
        data0.i = GetStack(m_nSP - 3);
        vm::STACK_DATA data1;
        data1.i = GetStack(m_nSP - 2);
        TournamentWork::POINT_INFO &point = m_pPointInfo[nPoint];
        point.m_Unk12 = data0.b[0];
        point.nViewX = data0.b[2];
        point.nViewZ = data0.b[1];
        point.nFlag = data0.b[3];
        point.nCamera = data1.b[0];
        point.m_Unk0E = data1.b[1];
        point.m_Unk0F = data1.b[2];
        point.m_Unk10 = data1.b[3];
        point.nFlag |= 0x14;
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetRouteData()
    {
        // Two words of four packed bytes each.
        vm::STACK_DATA data0;
        data0.i = GetStack(m_nSP - 3);
        long nRoute = GetStack(m_nSP - 4);
        vm::STACK_DATA data1;
        data1.i = GetStack(m_nSP - 2);
        TournamentWork::ROUTE_INFO &route = m_pRouteInfo[nRoute];
        route.nPoint2 = data0.b[2];
        route.nPoint1 = data0.b[1];
        route.nType = data0.b[0];
        route.nFlag = data0.b[3];
        route.nParam = data1.b[0];
        route.nCamera = data1.b[3];
        route.m_Unk06[0] = data1.b[1];
        route.m_Unk06[1] = data1.b[2];
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetRoutePoints()
    {
        long nNum = GetStack(m_nSP - 2);
        long nPoint = GetStack(m_nSP - 3);
        if (m_pHandler != 0) {
            m_pHandler->SetRoutePoints(nPoint, m_aPoint, nNum);
        }
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallCloseMatchMake()
    {
        long nMatch = GetStack(m_nSP - 2);
        if (nMatch != -1) {
            TournamentWork::MatchInfo(nMatch).nState |= 0x40;
        }
        m_nSP -= 2;
        return 0;
    }

    long CTmScript::_OpSysCallGetPointMatch()
    {
        long nResult = -1;
        long nOpen = GetStack(m_nSP - 2);
        long nPoint = GetStack(m_nSP - 3);
        for (long i = 1; i < MATCH_MAX; i++) {
            const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(i);
            if (match.nState != 0 && match.nPoint == nPoint && (nOpen == 0 || !(match.nState & 0x80))) {
                nResult = i;
                break;
            }
        }
        m_nSP -= 2;
        SetStack(m_nSP - 1, nResult);
        return 0;
    }

    long CTmScript::_OpSysCallIsMatchEnd()
    {
        long nResult = 0;
        long nMatch = GetStack(m_nSP - 2);
        if (nMatch != -1) {
            unsigned char nState = TournamentWork::GetMatchInfo(nMatch).nState;
            if (nState != 0) {
                nResult = (nState >> 7) & 1;
            }
        }
        m_nSP -= 1;
        SetStack(m_nSP - 1, nResult);
        return 0;
    }

    long CTmScript::_OpSysCallGetVsMatchPoint()
    {
        long aTeam0[16];
        long aTeam1[16];
        long *pTeam0 = aTeam0;
        long *pTeam1 = aTeam1;
        long nNum0 = 0;
        long nNum1 = 0;
        long nUnk5 = GetStack(m_nSP - 2);
        long nUnk4 = GetStack(m_nSP - 3);
        long nUnk3 = GetStack(m_nSP - 4);
        long nFromPoint = GetStack(m_nSP - 5);
        long nMatch = GetStack(m_nSP - 6);
        long nPoint1 = GetStack(m_nSP - 7);
        long nPoint0 = GetStack(m_nSP - 8);
        long nPoint = GetStack(m_nSP - 9);

        // The teams on the two points.
        for (long i = 0; i < TEAM_MAX; i++) {
            unsigned char nTeamPoint = TournamentWork::GetTeamInfo(i).nPoint;
            if (nTeamPoint != 0xFF) {
                if (nTeamPoint == nPoint0) {
                    *pTeam0++ = i;
                    nNum0++;
                } else if (nTeamPoint == nPoint1) {
                    *pTeam1++ = i;
                    nNum1++;
                }
            }
        }

        long nWinTeam = -1;
        TournamentWork::GetTournamentInfo();
        long nResult = -1;
        if (nNum0 == 0 && nNum1 == 0) {
            nResult = 0;
        }
        if (nNum0 == 1 && nNum1 == 0) {
            nWinTeam = aTeam0[0];
            nResult = nWinTeam == 0 ? 1 : 2;
        }
        if (nNum0 == 0 && nNum1 == 1) {
            nWinTeam = aTeam1[0];
            nResult = nWinTeam == 0 ? 1 : 2;
        }
        if (nNum0 == 2 && nNum1 == 0) {
            if (aTeam0[0] != 0 && aTeam0[1] != 0) {
                nResult = 3;
            } else {
                nResult = 5;
            }
        }
        if (nNum0 == 0 && nNum1 == 2) {
            if (aTeam1[0] != 0 && aTeam1[1] != 0) {
                nResult = 3;
            } else {
                nResult = 5;
            }
        }
        if (nNum0 == 1 && nNum1 == 1) {
            nResult = (aTeam0[0] == 0) + 3;
        }
        if (nNum0 == 2 && nNum1 == 1) {
            nResult = 6;
        }
        if (nNum0 == 1 && nNum1 == 2) {
            nResult = 6;
        }

        // The match the teams came from.
        long nFrom = -1;
        for (long i = 1; i < MATCH_MAX; i++) {
            const TournamentWork::MATCH_INFO &match = TournamentWork::GetMatchInfo(i);
            if (match.nState != 0 && match.nPoint == nFromPoint) {
                nFrom = i;
                break;
            }
        }

        if (nMatch != -1 && nFrom != -1) {
            TournamentWork::MATCH_INFO &from = TournamentWork::MatchInfo(nFrom);
            TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(nMatch);
            match.aTeamPoint[0] = 0xFF;
            match.aTeamPoint[1] = 0xFF;
            match.m_Unk03[0] = 0xFF;
            match.m_Unk03[1] = 0xFF;
            match.m_Unk03[2] = 0xFF;
            match.nSideFlag0D = -1;
            match.nSideFlag0E = -1;
            match.nWinTeam = -1;
            match.nState = 0;
            match.nStage = -1;
            match.nComLevel = 0;
            match.nFlags06 = 0;
            match.nRound = 0;
            match.nRule = 0;
            match.nPoint = nPoint;
            match.aTeamPoint[0] = nPoint0 != -1 ? nPoint0 : nPoint1;
            match.aTeamPoint[1] = nPoint1 != -1 ? nPoint1 : nPoint0;
            match.nFlags06 = from.nFlags06;
            match.nRound = from.nRound;
            match.nRule = from.nRule;
            match.nStage = from.nStage;
            match.nComLevel = from.nComLevel;
            if (CTournament::IsMixTournamentGame()) {
                match.nFlags06 = (match.nFlags06 | 0x20) & 0xEF;
                match.nStage = nFrom;
            }
            match.nFlags06 &= 0x3F;
            if (nUnk3 == -1) {
                nUnk3 = from.m_Unk03[0];
            }
            if (nUnk4 == -1) {
                nUnk4 = from.m_Unk03[1];
            }
            if (nUnk5 == -1) {
                nUnk5 = from.m_Unk03[2];
            }
            match.nState = (match.nState & 0xF8) | 7;
            match.m_Unk03[0] = nUnk3;
            match.m_Unk03[1] = nUnk4;
            match.m_Unk03[2] = nUnk5;
            if (nWinTeam != -1) {
                match.nWinTeam = nWinTeam;
            }
        }
        m_nSP -= 7;
        SetStack(m_nSP - 1, nResult);
        return 0;
    }

    long CTmScript::_OpSysCallSetMatchMake()
    {
        long nMatch = GetStack(m_nSP - 9);
        long nUnk5 = GetStack(m_nSP - 2);
        long nUnk4 = GetStack(m_nSP - 3);
        long nUnk3 = GetStack(m_nSP - 4);
        long nState = GetStack(m_nSP - 5);
        long nPoint1 = GetStack(m_nSP - 6);
        long nPoint0 = GetStack(m_nSP - 7);
        long nPoint = GetStack(m_nSP - 8);

        // A free match when none is given.
        if (nMatch == -1) {
            for (long i = 1; i < MATCH_MAX; i++) {
                if (TournamentWork::GetMatchInfo(i).nState == 0) {
                    nMatch = i;
                    break;
                }
            }
        }
        if (nMatch != -1) {
            TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(nMatch);
            match.nWinTeam = -1;
            match.nStage = -1;
            match.nComLevel = 0;
            match.nRound = 0;
            match.nRule = 0;
            match.nPoint = nPoint;
            match.nSideFlag0D = -1;
            match.nSideFlag0E = -1;
            match.aTeamPoint[0] = nPoint0;
            match.aTeamPoint[1] = nPoint1;
            match.nState = nState;
            match.m_Unk03[0] = nUnk3;
            match.m_Unk03[1] = nUnk4;
            match.m_Unk03[2] = nUnk5;
            match.nFlags06 = 0;
            if (CTournament::IsMixTournamentGame()) {
                match.nFlags06 &= 0xF8;
            } else {
                match.nFlags06 = (match.nFlags06 & 0xF8) | static_cast<unsigned char>(CTournament::GetGameMode());
            }
            match.nStage = 1;
            match.nComLevel = 200;
            match.nRound = 4;
        }
        m_nSP -= 8;
        SetStack(m_nSP - 1, nMatch);
        return 0;
    }

    long CTmScript::_OpSysCallSetMatchRule()
    {
        long nRule = GetStack(m_nSP - 3);
        TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(GetStack(m_nSP - 4));
        match.nRule = nRule;
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetMatchLevel()
    {
        long nLevel = GetStack(m_nSP - 2);
        TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(GetStack(m_nSP - 3));
        match.nComLevel = nLevel;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallSetMatchTelop()
    {
        long nRound = GetStack(m_nSP - 2);
        TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(GetStack(m_nSP - 3));
        match.nRound = nRound;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallSetMatchStage()
    {
        long nExclude = GetStack(m_nSP - 2);
        long nStage = GetStack(m_nSP - 3);
        TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(GetStack(m_nSP - 4));
        match.nStage = nStage;
        m_nStage = nExclude;
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallSetMatchUnlock()
    {
        long nUnlock = GetStack(m_nSP - 2);
        TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(GetStack(m_nSP - 3));
        match.nFlags06 = (match.nFlags06 & 0x3F) | static_cast<unsigned char>(nUnlock);
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallMatchVsPause()
    {
        long nMatch = GetStack(m_nSP - 2);
        long nTeam1 = GetStack(m_nSP - 3);
        long nTeam0 = GetStack(m_nSP - 4);
        m_nSP -= 4;
        if (nMatch != -1) {
            SetupMatch(nMatch, nTeam0, nTeam1);
        }
        if (m_pHandler != 0) {
            m_pHandler->VsMatchPause(nTeam0, nTeam1, nMatch);
        }
        return 0;
    }

    long CTmScript::_OpSysCallMatchMake()
    {
        long nMatch = GetStack(m_nSP - 4);
        long nFlag10 = GetStack(m_nSP - 2);
        long nFlag20 = GetStack(m_nSP - 3);
        m_nSP -= 4;
        if (nMatch != -1) {
            TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(nMatch);
            if (match.nState != 0) {
                // The teams on the two points of the match.
                long nTeam0 = FindPointTeam(match.aTeamPoint[0]);
                long nTeam1 = FindPointTeam(match.aTeamPoint[1]);
                if (nTeam0 != -1 && nTeam1 != -1) {
                    SetupMatch(nMatch, nTeam0, nTeam1);
                    TournamentWork::TeamInfo(nTeam0).nTargetPoint = match.nPoint;
                    TournamentWork::TeamInfo(nTeam1).nTargetPoint = match.nPoint;
                    if (nFlag10 != 0) {
                        match.nState |= 0x10;
                    }
                    if (nFlag20 == 0) {
                        match.nState |= 0x20;
                    }
                    if (m_pHandler != 0) {
                        m_pHandler->SetMatchMake(nMatch, nTeam0, nTeam1);
                    }
                }
            }
        }
        return 0xA;
    }

    long CTmScript::_OpSysCallSecretMake()
    {
        long nMatch = GetStack(m_nSP - 6);
        long nFlag10 = GetStack(m_nSP - 2);
        long nFlag20 = GetStack(m_nSP - 3);
        long nTeam1 = GetStack(m_nSP - 4);
        long nTeam0 = GetStack(m_nSP - 5);
        m_nSP -= 6;
        if (nMatch != -1) {
            SetupMatch(nMatch, nTeam0, nTeam1);
            TournamentWork::MATCH_INFO &match = TournamentWork::MatchInfo(nMatch);
            if (nFlag10 != 0) {
                match.nState |= 0x10;
            }
            if (nFlag20 == 0) {
                match.nState |= 0x20;
            }
            if (m_pHandler != 0) {
                m_pHandler->SetMatchMake(nMatch, nTeam0, nTeam1);
            }
        }
        return 0xB;
    }

    long CTmScript::_OpSysCallFadeIn()
    {
        long nUnk = GetStack(m_nSP - 2);
        long nFrame = GetStack(m_nSP - 3);
        m_nSP -= 3;
        if (m_pHandler != 0) {
            m_pHandler->FadeIn(1.0f / nFrame, nUnk);
        }
        return 0;
    }

    long CTmScript::_OpSysCallIsFadeInReq()
    {
        int bReq = 0;
        if (m_pHandler != 0) {
            bReq = m_pHandler->IsFadeInReq();
        }
        SetStack(m_nSP - 1, bReq);
        return 0;
    }

    long CTmScript::_OpSysCallTurnMode()
    {
        long nTeam = GetStack(m_nSP - 3);
        long nMode = GetStack(m_nSP - 4);
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        info.nTurnMode = nMode;
        if (nMode == 1) {
            info.nTurnIndex = 0;
            for (long i = 0; i < 16; i++) {
                info.aTurnTeam[i] = -1;
            }
            if (nTeam >= 0) {
                TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
                if (team.nPoint != 0xFF) {
                    team.nTurn = 1;
                    info.aTurnTeam[0] = nTeam;
                }
            }
        }
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallIsFirstAround()
    {
        long nLevel = CTournament::GetLevel();
        long nGameMode = CTournament::GetGameMode();
        SetStack(m_nSP - 1, !CTournament::IsCupClear(CTournament::GetCup(), nGameMode, nLevel, -1));
        return 0;
    }

    long CTmScript::_OpSysCallIsWin()
    {
        SetStack(m_nSP - 1, TournamentWork::GetTournamentInfo().aFlag[0x3F] != 0);
        return 0;
    }

    long CTmScript::_OpSysCallIsAllStarChar()
    {
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        CUnlockFlags::m_nGameMode = CTournament::GetGameMode();
        SetStack(m_nSP - 1, flags.IsAllStarChar());
        return 0;
    }

    long CTmScript::_OpSysCallGetTeamMax()
    {
        SetStack(m_nSP - 1, CTournament::GetTeamMax());
        return 0;
    }

    long CTmScript::_OpSysCallGetTurnMode()
    {
        SetStack(m_nSP - 1, TournamentWork::GetTournamentInfo().nTurnMode);
        return 0;
    }

    long CTmScript::_OpSysCallGetTmLevel()
    {
        SetStack(m_nSP - 1, TournamentWork::GetTournamentInfo().nLevel);
        return 0;
    }

    long CTmScript::_OpSysCallGetTmMode()
    {
        SetStack(m_nSP - 1, CGame::g_GameWork.m_nGameMode);
        return 0;
    }

    long CTmScript::_OpSysCallGetTmCallMode()
    {
        SetStack(m_nSP - 1, 0);
        return 0;
    }

    long CTmScript::_OpSysCallGetRandom()
    {
        long nRand = bcs::BCS_RandMax(GetStack(m_nSP - 2) - 1);
        m_nSP -= 1;
        SetStack(m_nSP - 1, nRand);
        return 0;
    }

    long CTmScript::_OpSysCallBackupFlags()
    {
        long nValue = GetStack(m_nSP - 2);
        long nFlag = GetStack(m_nSP - 3);
        TournamentWork::TournamentInfo().aFlag[nFlag] = nValue;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallRestoreFlags()
    {
        long nFlag = GetStack(m_nSP - 2);
        long nValue = TournamentWork::GetTournamentInfo().aFlag[nFlag];
        m_nSP -= 1;
        SetStack(m_nSP - 1, nValue);
        return 0;
    }

    long CTmScript::_OpSysCallBackupFirstFlags()
    {
        long nValue = GetStack(m_nSP - 2);
        long nFlag = GetStack(m_nSP - 3);
        TournamentWork::TournamentInfo().aFirstFlag[nFlag] = nValue;
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallRestoreFirstFlags()
    {
        long nFlag = GetStack(m_nSP - 2);
        long nValue = TournamentWork::GetTournamentInfo().aFirstFlag[nFlag];
        m_nSP -= 1;
        SetStack(m_nSP - 1, nValue);
        return 0;
    }

    long CTmScript::_OpSysCallUnlockOpen()
    {
        long nArg1 = GetStack(m_nSP - 2);
        long nArg0 = GetStack(m_nSP - 3);
        long nKind = GetStack(m_nSP - 4);
        CUnlockFlags flags;
        flags.SetPlayerData(-1);
        CUnlockFlags::m_nGameMode = CTournament::GetGameMode();
        long nCup = CTournament::GetCup() - 1;
        long nId = CTournament::GetLevel() + nCup * 4;
        long nType = -1;
        long nColor = 0;
        switch (nKind) {
        case 0:
            CUnlockFlags::GetCharType(nType, nColor, nArg0);
            if (!flags.IsChar(nType, nColor, CTournament::GetGameModeNoMix())) {
                flags.PushChar(nId + 4, 0, 0, nType, nColor, CTournament::GetGameModeNoMix());
            }
            break;
        case 1:
            if (!flags.IsCup(nArg1, nArg0, CTournament::GetGameModeNoMix())) {
                flags.PushCup(nId + 4, nArg1, nArg0, CTournament::GetGameModeNoMix());
            }
            break;
        case 2:
            if (!flags.IsEtc(nArg0, CTournament::GetGameModeNoMix())) {
                flags.PushEtc(nId + 4, nArg0, CTournament::GetGameModeNoMix());
            }
            break;
        case 3:
            TournamentWork::SetLeaderNoUnlock(TournamentWork::GetTournamentInfo().aLeaderTeam[2], nArg0, nArg1);
            break;
        case 4:
            TournamentWork::SetLeaderNoUnlock(TournamentWork::GetTournamentInfo().aLeaderTeam[1], nArg0, nArg1);
            break;
        case 5:
            TournamentWork::SetLeaderNoUnlock(TournamentWork::GetTournamentInfo().aLeaderTeam[0], nArg0, nArg1);
            break;
        }
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallPointCamera()
    {
        long nCamera = GetStack(m_nSP - 2);
        long nPoint = GetStack(m_nSP - 3);
        if (m_pHandler != 0) {
            if (m_pHandler->IsCameraBusy()) {
                _Retry();
                return 1;
            }
            m_pHandler->PointCamera(nPoint, nCamera, -0.0001f, 0.95703125f);
        }
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallRtSelSetup()
    {
        long nUnk3 = GetStack(m_nSP - 2);
        long nUnk2 = GetStack(m_nSP - 3);
        long nUnk1 = GetStack(m_nSP - 4);
        long nPoint = GetStack(m_nSP - 5);
        m_nSP -= 5;
        if (m_pHandler != 0) {
            m_pHandler->RouteSelectSetup(nPoint, m_aPoint, nUnk1, nUnk2, nUnk3);
        }
        return 0;
    }

    long CTmScript::_OpSysCallRtSelect()
    {
        long nSelect = -1;
        if (m_pHandler == 0 || m_pHandler->RouteSelect(nSelect)) {
            SetStack(m_nSP - 1, nSelect);
            return 0;
        }
        _Retry();
        return 1;
    }

    long CTmScript::_OpSysCallDbSelIndex()
    {
        m_nSP -= 2;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelSetup()
    {
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelItem()
    {
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelItemLabel()
    {
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelItemLabelValue()
    {
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelItemLabelStr()
    {
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelItemLabelStrValue()
    {
        m_nSP -= 4;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelStockLabelStr()
    {
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallDbSelect()
    {
        m_nSP -= 1;
        SetStack(m_nSP - 1, -1);
        return 0;
    }

    long CTmScript::_OpSysCallDbFlgSetup()
    {
        m_nSP -= 2;
        return 0;
    }

    long CTmScript::_OpSysCallDbFlgItem()
    {
        m_nSP -= 3;
        return 0;
    }

    long CTmScript::_OpSysCallDbScript()
    {
        m_nSP -= 5;
        return 0;
    }

    long CTmScript::_OpSysCallDbScriptEnd()
    {
        m_nSP -= 2;
        return 0;
    }

    long CTmScript::_OpSysCallDbTrace()
    {
        m_nSP -= 2;
        return 0;
    }

    long CTmScript::_OpSysCallDbTrace1()
    {
        m_nSP -= 3;
        return 0;
    }
}
