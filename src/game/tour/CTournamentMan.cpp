#include <stdio.h>
#include <string.h>

#include "game/msw/CTournamentMan.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/CCameraAnm.hpp"
#include "bcs/CCameraMan.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CRenderMan.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/EF/CResource.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CCockpitTournament.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEffectCtrlTournament.hpp"
#include "game/msw/CEvDemoMdlObj.hpp"
#include "game/msw/CEvDemoRes.hpp"
#include "game/msw/CEvMdlCreaterTmMdlChr.hpp"
#include "game/msw/CEvNodeData.hpp"
#include "game/msw/CEventCamera.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMove.hpp"
#include "game/msw/CEventNodeSimple.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CGameAccInput.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CLytCtrl.hpp"
#include "game/msw/CRouteBase.hpp"
#include "game/msw/CRouteMove.hpp"
#include "game/msw/CStageBasket.hpp"
#include "game/msw/CStageMan.hpp"
#include "game/msw/CStageManDemo.hpp"
#include "game/msw/CTournamentCamera.hpp"
#include "game/msw/CTournamentRouteMan.hpp"
#include "game/msw/TournamentData.hpp"
#include "game/msw/TournamentWork.hpp"
#include "game/msw/menu/CSndEff.hpp"
#include "game/msw/menu/CVibEff.hpp"
#include "game/msw/vm/CVMData.hpp"
#include "nw4r/math/triangular.h"

namespace msw {

    namespace {

        // An angle brought into [0, 2 pi). (same helper as in
        // game/tour/CTournamentChip.cpp)
        inline float WrapRad(float rad)
        {
            while (rad > 6.2831855f) {
                rad -= 6.2831855f;
            }
            while (rad < 0.0f) {
                rad += 6.2831855f;
            }
            return rad;
        }

        // (name guessed) rOut = r1 - r2 by way of a temporary, the
        // difference counterpart of bcs::FVEC3Sum.
        inline void FVEC3Dif(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
        {
            bcs::FVEC3 dif;

            bcs::FVEC3Sub(&dif, &r1, &r2);
            rOut = dif;
        }

        inline float Lerp(float from, float to, float t)
        {
            return t * (to - from) + from;
        }

        inline float Clamp01(float f)
        {
            return f > 1.0f ? 1.0f : (f < 0.0f ? 0.0f : f);
        }

        // Camera setting nCamera of the stage.
        inline const TournamentData::CAMERA_INFO *GetCameraInfo(long nCamera)
        {
            const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
            if (nCamera >= 0) {
                return &info.pCameraInfo[nCamera];
            }
            return info.pCameraInfo;
        }

        // The draw layer of the player markers, the files of the route
        // arrows and the object numbers they get. (names guessed)
        const long LOADDATA_OBJ = 0x5C;
        const long OBJ_SELECT = 195;
        const long OBJ_SELECT_NUM = 4;
        const long OBJ_MARKER = 200;

        // A queue of the route searches, 0x200 entries long.
        const long QUEUE_MAX = 0x200;
    }

    namespace TournamentWork {

        long MakePointData(short *pHead, _LINK_POINT *pLink, long, long nLink)
        {
            for (long i = 0; i < 0x80; i++) {
                pHead[i] = 0;
            }

            _LINK_POINT *pBack = &pLink[nLink];
            for (long nRoute = 0; nRoute < 0x100; nRoute++) {
                const ROUTE_INFO *pInfo = GetRouteInfo(nRoute);
                if (pInfo->nType < 0) {
                    continue;
                }

                // The route run forwards, from nPoint2 to nPoint1.
                _LINK_POINT *pFore = &pLink[nLink + 1];
                pFore->nNext = pHead[pInfo->nPoint2];
                pFore->nPoint = pInfo->nPoint1;
                pFore->nRoute = nRoute;
                pFore->nFlag = pInfo->nFlag;
                if (!(pInfo->nFlag & 0x20)) {
                    pFore->nFlag |= 0x20;
                }
                pFore->nCost = 1;
                pHead[pInfo->nPoint2] = nLink + 1;

                // And backwards.
                pBack[2].nNext = pHead[pInfo->nPoint1];
                pBack[2].nPoint = pInfo->nPoint2;
                pBack[2].nRoute = nRoute + 0x100;
                pBack[2].nFlag = pInfo->nFlag;
                pBack += 2;
                if (pInfo->nFlag & 0x20) {
                    pBack->nFlag |= 0x20;
                }
                pBack->nCost = 1;
                pHead[pInfo->nPoint1] = nLink + 2;
                nLink += 2;
            }
            return nLink;
        }

        void MakeRouteCost(short *pCost, short *pHead, _LINK_POINT *pLink, unsigned char nStart, unsigned char nMask,
                           short nCost)
        {
            struct QUEUE {
                unsigned short nRoute;
                short nCost;
                long nPoint;
            };
            QUEUE aQueue[QUEUE_MAX];

            for (long i = 0; i < 0x200; i++) {
                pCost[i] = -1;
            }

            long nIn = 0;
            long nOut = 0;
            aQueue[nIn].nPoint = nStart;
            aQueue[nIn].nRoute = 0;
            aQueue[nIn].nCost = nCost;
            nIn = (nIn + 1) % QUEUE_MAX;

            while (nOut != nIn) {
                QUEUE &rEntry = aQueue[nOut];
                nOut = (nOut + 1) % QUEUE_MAX;
                short nNow = rEntry.nCost;
                pCost[rEntry.nRoute] = nNow;
                short nFirst = pHead[static_cast<unsigned char>(rEntry.nPoint)];
                for (_LINK_POINT *p = nFirst > 0 ? &pLink[nFirst] : 0; p != 0;
                     p = p->nNext > 0 ? &pLink[p->nNext] : 0) {
                    if (p->nFlag & nMask) {
                        short nOld = pCost[p->nRoute];
                        short nNew = nNow + p->nCost;
                        if (nOld == -1 || nOld > nNew) {
                            QUEUE &rNew = aQueue[nIn];
                            rNew.nPoint = p->nPoint;
                            nIn = (nIn + 1) % QUEUE_MAX;
                            rNew.nRoute = p->nRoute;
                            rNew.nCost = nNew;
                        }
                    }
                }
            }
        }

        void GetPointFastRoute(unsigned char, unsigned char nStart, unsigned char nMask, short *pCost, short *pHead,
                               _LINK_POINT *pLink, _MOVE_POINT *pMove, long, long nMove)
        {
            short aQueue[QUEUE_MAX];
            short nMin = 0x7FFF;
            long nOut = 0;
            long nIn = 0;

            aQueue[nIn] = nMove;
            nIn = (nIn + 1) % QUEUE_MAX;
            _MOVE_POINT *pLast = &pMove[nMove];
            pLast->nNext = 0;
            pLast->nParent = 0;
            pLast->nChild = 0;
            pLast->m_Unk0C = 0;
            pLast->nPoint = nStart;
            pLast->nRoute = 0;
            pLast->nCost = 0x7FFF;
            pLast->nFlag = 0x20;

            while (nIn != nOut) {
                short nNow = aQueue[nOut];
                nOut = (nOut + 1) % QUEUE_MAX;
                _MOVE_POINT *pNow = &pMove[nNow];
                unsigned char nPoint = pNow->nPoint;
                short nFirst = pHead[nPoint];

                _LINK_POINT *p;
                for (p = nFirst > 0 ? &pLink[nFirst] : 0; p != 0; p = p->nNext > 0 ? &pLink[p->nNext] : 0) {
                    short nCost = pCost[p->nRoute];
                    if ((p->nFlag & nMask) && nCost != -1 && nCost < nMin) {
                        nMin = nCost;
                    }
                }
                for (p = nFirst > 0 ? &pLink[nFirst] : 0; p != 0; p = p->nNext > 0 ? &pLink[p->nNext] : 0) {
                    short nCost = pCost[p->nRoute];
                    if ((p->nFlag & nMask) && nPoint != p->nPoint && nCost != -1 && nCost <= nMin) {
                        pLast[1].nParent = nNow;
                        long nQueue = nIn;
                        nIn = (nIn + 1) % QUEUE_MAX;
                        nMove++;
                        pLast[1].nNext = pNow->nChild;
                        pLast[1].nChild = 0;
                        pLast[1].m_Unk0C = 0;
                        pLast[1].nPoint = p->nPoint;
                        pLast[1].nRoute = p->nRoute;
                        pLast[1].nFlag = p->nFlag;
                        pLast[1].nCost = nMin;
                        pLast++;
                        pNow->nChild = nMove;
                        aQueue[nQueue] = nMove;
                    }
                }
            }
        }

        long SelectFastRouteBasic(unsigned char nGoal, unsigned short *pRoute, long nNum, long nMove,
                                  _MOVE_POINT *pMove, float)
        {
            _MOVE_POINT *apQueue[QUEUE_MAX];
            long nOut = 0;
            long nIn = 0;
            _MOVE_POINT *pFound = 0;

            apQueue[nIn] = &pMove[nMove];
            nIn = (nIn + 1) % QUEUE_MAX;
            while (nIn != nOut) {
                _MOVE_POINT *pNow = apQueue[nOut];
                nOut = (nOut + 1) % QUEUE_MAX;
                for (_MOVE_POINT *p = pNow->nChild > 0 ? &pMove[pNow->nChild] : 0; p != 0;
                     p = p->nNext > 0 ? &pMove[p->nNext] : 0) {
                    if (p->nPoint == nGoal) {
                        if (pFound == 0) {
                            pFound = p;
                        }
                    } else {
                        apQueue[nIn] = p;
                        nIn = (nIn + 1) % QUEUE_MAX;
                    }
                }
            }

            // Back up the tree from the goal, taking the routes that lead
            // there.
            pRoute += nNum;
            while (pFound != 0 && pFound->nRoute != 0) {
                short nChild = pFound->nChild;
                pFound = &pMove[pFound->nParent];
                for (_MOVE_POINT *p = nChild > 0 ? &pMove[nChild] : 0; p != 0;
                     p = p->nNext > 0 ? &pMove[p->nNext] : 0) {
                    if (pFound->nPoint == p->nPoint && (p->nFlag & 0x20)) {
                        nNum++;
                        *pRoute++ = p->nRoute;
                    }
                }
            }
            return nNum;
        }
    }

    CTournamentMan *CTournamentMan::m_pInstance;

    inline bcs::CSndTask *CTournamentMan::GetSe(long nSlot) const
    {
        if (m_aSe[nSlot] == 0) {
            return 0;
        }
        return static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_aSe[nSlot]));
    }

    CTournamentMan *CTournamentMan::GetInstance()
    {
        return m_pInstance;
    }

    CTournamentMan *CTournamentMan::GetPtr()
    {
        return m_pInstance;
    }

    inline CTournamentMan::CTournamentMan(bcs::CTask *pParent)
        : bcs::CTask(pParent, -1), m_pStageMan(0), m_pNodeData(0), m_pCameraAnm(0), m_pCamera(0), m_pEventCamera(0),
          m_SceneFile(0), m_pScript(0)
    {
    }

    CTournamentMan *CTournamentMan::CreateInstance(bcs::CTask *pParent, long nView)
    {
        if (m_pInstance == 0) {
            CTournamentMan *pMan = new CTournamentMan(pParent);
            m_pInstance = pMan;
            pMan->_Init(nView);
        }
        return m_pInstance;
    }

    CTournamentMan::~CTournamentMan()
    {
        if (m_pInstance != 0) {
            m_pInstance->_Destory();
            m_pInstance = 0;
        }
    }

    void CTournamentMan::_Init(long nView)
    {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        info.mapColor.SetWhite();
        m_Color = info.mapColor;
        m_ColorStart = info.mapColor;
        m_fColorRate = 0.0f;
        m_fColorSpeed = 0.0f;
        m_vSceneOffset.Set(0.0f, 0.0f, 0.0f);
        m_vSceneRot.Set(0.0f, 0.0f, 0.0f);

        m_ulMoveStop = 0;
        long i;
        for (i = 0; i < CHR_MAX; i++) {
            m_apChrModel[i] = 0;
            m_apChrMove[i] = 0;
        }
        for (i = 0; i < OBJ_MAX; i++) {
            m_apObjModel[i] = 0;
        }
        for (i = 0; i < EFFECT_MAX; i++) {
            m_aEffect[i] = 0;
        }

        m_fFovy = bcs::CWorldEnv::GetViewFovy(nView);
        m_nCameraMode = -1;
        m_nSceneCamera = -1;
        bcs::CWorldEnv::SetViewFovy(nView, 35.98f);

        m_pNodeData = new CEvNodeData;
        m_pCamera = new CTournamentCamera(nView, 0x80);
        m_pCameraAnm = new bcs::CCameraAnm(nView, 0x80, 0);
        m_pEventCamera = new CEventCameraNull;
        m_pScript = new vm::CVMData[1];
        m_SceneFile = nw4r::g3d::ResFile(0);
        m_pCameraAnm->SetUnk138Bit0();
        bcs::CRenderMan::PushBack(m_pEventCamera, 0);

        bcs::FVEC3 vCenter(1.0f, 0.0f, 0.0f);
        CEvDemoRes *pRes = CEvDemoRes::CreateInstance(GetParent());
        pRes->Setup(0x100);
        pRes->SetStage(info.nStage);
        pRes->SetGameMode(CGame::g_GameWork.m_nGameMode);
        pRes->SetRevCenter(vCenter);
        bcs::FVEC3 vOffset;
        bcs::FVEC3Sum(vOffset, vCenter, vCenter);
        m_pCameraAnm->m_vUnk140 = vOffset;
        pRes->SetCreater(0, new CEvMdlCreaterTmMdlChr);
        pRes->SetCreater(1, new CEvMdlCreaterTmMdlNpc);
        pRes->SetCreater(2, new CEvMdlCreaterTmMdlObj);
        pRes->SetCreater(3, new CEvMdlCreaterTmMdlObj);
    }

    void CTournamentMan::_Destory()
    {
        long i;
        for (i = 0; i < CHR_MAX; i++) {
            if (m_apChrModel[i] != 0) {
                delete m_apChrModel[i];
            }
            m_apChrModel[i] = 0;
            if (m_apChrMove[i] != 0) {
                delete m_apChrMove[i];
            }
            m_apChrMove[i] = 0;
        }
        for (i = 0; i < OBJ_MAX; i++) {
            if (m_apObjModel[i] != 0) {
                delete m_apObjModel[i];
            }
            m_apObjModel[i] = 0;
        }
        CTournamentRouteMan::DeleteInstance();

        if (m_pCamera != 0) {
            bcs::CWorldEnv::SetViewFovy(m_pCamera->GetView(), 35.98f);
        }
        if (m_pScript != 0) {
            delete[] m_pScript;
        }
        m_pScript = 0;
        if (m_pNodeData != 0) {
            delete m_pNodeData;
        }
        m_pNodeData = 0;
        if (m_pCameraAnm != 0) {
            m_pCameraAnm->Delete();
        }
        m_pCameraAnm = 0;
        if (m_pCamera != 0) {
            m_pCamera->Delete();
        }
        m_pCamera = 0;
        if (m_pEventCamera != 0) {
            bcs::CScnObj::AppendDelayDeleteObject(m_pEventCamera);
        }
        m_pEventCamera = 0;
        CCockpit::Remove();
    }

    void CTournamentMan::Disp()
    {
    }

    void CTournamentMan::SetupDataLoadEnd(int)
    {
        CEffectCtrlTournament::CreateInstance(GetParent());
        bcs::EF::CManager *pManager = bcs::EF::CManager::GetInstance();
        if (pManager != 0) {
            pManager->SetUnk95(false);
        }
        CLytCtrl::CreateInstance(GetParent());
        CCockpitTournament::Create(GetParent());
        menu::CSndEff::s_Instance.Initialize();
        menu::CVibEff::s_Instance.Initialize();
        CGameAccInput::Initialize();
    }

    void CTournamentMan::UpdateFrame(float fFrame)
    {
        long i;
        for (i = 0; i < EFFECT_MAX; i++) {
            unsigned long hEffect = m_aEffect[i];
            if (hEffect != 0 && CEffectCtrl::GetInstance()->FindList(hEffect) == 0) {
                m_aEffect[i] = 0;
            }
        }
        for (i = 0; i < SE_MAX; i++) {
            if (m_aSe[i] != 0 && bcs::CTaskMan::GetTask(m_aSe[i]) == 0) {
                m_aSe[i] = 0;
            }
        }

        // Fade the colour of the map.
        if (m_fColorRate > 0.0f) {
            float fSpeed = m_fColorSpeed;
            TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
            m_fColorRate += fSpeed * fFrame;
            m_pStageMan->SetBaseColor(bcs::BCS_Color(0xFF, 0xFF, 0xFF, 0xFF));
            m_fColorRate = Clamp01(m_fColorRate);
            info.mapColor.r = Lerp(m_ColorStart.r, m_Color.r, m_fColorRate);
            info.mapColor.g = Lerp(m_ColorStart.g, m_Color.g, m_fColorRate);
            info.mapColor.b = Lerp(m_ColorStart.b, m_Color.b, m_fColorRate);
            info.mapColor.a = Lerp(m_ColorStart.a, m_Color.a, m_fColorRate);
        }

        for (i = 0; i < CHR_MAX; i++) {
            if (m_apChrModel[i] != 0) {
                m_apChrModel[i]->UpdateFrame(fFrame);
                m_apChrModel[i]->UpdateMove((m_ulMoveStop & (1 << i)) ? 0.0f : fFrame);
            }
        }
        for (i = 0; i < OBJ_MAX; i++) {
            if (m_apObjModel[i] != 0) {
                m_apObjModel[i]->UpdateFrame(fFrame);
                m_apObjModel[i]->UpdateMove(fFrame);
            }
        }

        // The player markers pulse once they are fully shown.
        for (i = 0; i < 2; i++) {
            CEventModel *pMarker = m_apObjModel[OBJ_MARKER + i];
            if (pMarker != 0) {
                float fScale;
                if (pMarker->GetNowAlpha() >= 1.0f) {
                    m_afMarkerPhase[i] = WrapRad(fFrame * 0.17453292f + m_afMarkerPhase[i]);
                    fScale = 0.1f * nw4r::math::SinRad(m_afMarkerPhase[i]) + 0.9f;
                } else {
                    fScale = 0.3f + pMarker->GetNowAlpha();
                    m_afMarkerPhase[i] = 1.5707964f;
                }
                if (fScale >= 1.0f) {
                    fScale = 1.0f;
                }
                pMarker->SetScale(bcs::FVEC3(fScale, 1.0f, fScale));
            }
        }
    }

    void CTournamentMan::ScriptLoad(long nFile, long nScript)
    {
        m_pScript[nScript].Load(nFile);
    }

    void CTournamentMan::ScriptBindData(long nFile, long nScript)
    {
        m_pScript[nScript].BindData(nFile);
    }

    vm::CVMData *CTournamentMan::GetScript(long nScript)
    {
        return &m_pScript[nScript];
    }

    void CTournamentMan::SetupChrModelLeader(int bDisp)
    {
        for (long nTeam = 0; nTeam < 0x10; nTeam++) {
            const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
            if (team.nPoint != 0xFF && team.aCharType[0] != -1) {
                SetupChrModel(nTeam, team.aCharType[0], team.aCharFlag[0] & 3, 0, nTeam, 0, bDisp);
            }
        }
    }

    void CTournamentMan::SetupChrModelSub(long nTeam, long nGroup, int bDisp)
    {
        if (nTeam != -1) {
            const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
            if (team.nPoint != 0xFF) {
                for (long nNo = 1; nNo < 3; nNo++) {
                    if (team.aCharType[nNo] != -1) {
                        long nIndex;
                        if (nGroup != -1 && nNo > 0) {
                            nIndex = nNo + nGroup * 2 + 0xF;
                        } else {
                            nIndex = nTeam;
                        }
                        SetupChrModel(nIndex, team.aCharType[nNo], team.aCharFlag[nNo] & 3, 0, nTeam, nNo, bDisp);
                    }
                }
            }
        }
    }

    CEventModel *CTournamentMan::SetupChrModel(long nIndex, long nChr, long nColor, int bStar, long nTeam, long nNo,
                                               int bDisp)
    {
        if (m_apChrModel[nIndex] != 0) {
            return m_apChrModel[nIndex];
        }

        // A Mii of the user's team (character 0x13 and up) brings its data.
        void *pMii = 0;
        if (nChr >= 0x13) {
            // The original reads the tournament data here and drops it.
            TournamentWork::GetTournamentInfo();
            if (nTeam == 0) {
                pMii = &TournamentWork::TournamentInfo().aPlayer[nNo].miiData;
            }
        }

        CEvDemoRes *pRes = CEvDemoRes::GetInstance();
        CEvDemoRes::_CHR_INFO &rInfo = pRes->FindIndexNew(nIndex, 0);
        pRes->SetInit(rInfo, nIndex);
        rInfo.SetCreate(1);
        rInfo.SetChrInit(nChr, nColor, pMii, 0);
        rInfo.SetTeam(nTeam, nNo);
        rInfo.SetDispInfo(bDisp, 2, -1, 0);
        CEventModel *pModel = pRes->_CreateRes(rInfo);
        m_apChrModel[nIndex] = pModel;
        if (pModel != 0) {
            m_apChrMove[nIndex] = new CRouteMove(pModel, 0);
            pModel->SetStarEffect(bStar);
            if (bDisp) {
                pModel->SetDisp(bDisp);
            }
        }
        return pModel;
    }

    void CTournamentMan::CleanChrModel(long nIndex, int bDestroy)
    {
        if (m_apChrModel[nIndex] != 0) {
            delete m_apChrModel[nIndex];
        }
        m_apChrModel[nIndex] = 0;
        if (m_apChrMove[nIndex] != 0) {
            delete m_apChrMove[nIndex];
        }
        m_apChrMove[nIndex] = 0;
        if (bDestroy) {
            CEvDemoRes::GetInstance()->FindIndexDestory(nIndex, 0);
        }
    }

    CEventModel *CTournamentMan::SetupObjModel(long nIndex, long nSub, char *pName, long nDisp,
                                               const bcs::FVEC3 &vPos, long nLayer, long nGroup, int b,
                                               const char *pAnim, long nPriority)
    {
        if (m_apObjModel[nIndex] != 0) {
            return m_apObjModel[nIndex];
        }

        CEvDemoRes *pRes = CEvDemoRes::GetInstance();
        CEvDemoRes::_CHR_INFO &rInfo = pRes->FindIndexNew(nIndex, 3);
        rInfo.SetCreate(1);
        pRes->SetInit(rInfo, nIndex);
        rInfo.SetObjBgInit(nSub, pName, 0);
        rInfo.SetTeam(-1, 0);
        rInfo.SetDispInfo(b, nGroup, nLayer, nDisp);
        CEventModel *pModel = pRes->_CreateRes(rInfo);
        m_apObjModel[nIndex] = pModel;
        if (pModel != 0) {
            pModel->SetPosition(vPos);
            if (pAnim != 0) {
                pModel->AnimPlay(pAnim, 0x1F, 0.0f, 0.0f, 1);
            }
            if (nPriority != -1) {
                pModel->SetDrawPriority(nPriority);
            }
            new CEventMove(pModel, 0, 1);
        }
        return pModel;
    }

    CEventModel *CTournamentMan::SetupObjModelEx(long nIndex, long nSub, char *pName, long nDisp,
                                                 const bcs::FVEC3 &vPos, long nLayer, long nGroup, int b,
                                                 const char *pAnim, long nPriority)
    {
        if (m_apObjModel[nIndex] != 0) {
            return m_apObjModel[nIndex];
        }

        CEvDemoRes *pRes = CEvDemoRes::GetInstance();
        CEvDemoRes::_CHR_INFO &rInfo = pRes->FindIndexNew(nIndex, 2);
        rInfo.SetCreate(1);
        pRes->SetInit(rInfo, nIndex);
        rInfo.SetObjInit(nSub, pName, 0);
        rInfo.SetTeam(-1, 0);
        rInfo.SetDispInfo(b, nGroup, nLayer, nDisp);
        CEventModel *pModel = pRes->_CreateRes(rInfo);
        m_apObjModel[nIndex] = pModel;
        if (pModel != 0) {
            pModel->SetPosition(vPos);
            if (pAnim != 0) {
                pModel->AnimPlay(pAnim, 0x1F, 0.0f, 0.0f, 1);
            }
            if (nPriority != -1) {
                pModel->SetDrawPriority(nPriority);
            }
            new CEventMove(pModel, 0, 1);
        }
        return pModel;
    }

    void CTournamentMan::CleanObjModel(long nIndex, int bDestroy)
    {
        if (m_apObjModel[nIndex] != 0) {
            delete m_apObjModel[nIndex];
        }
        m_apObjModel[nIndex] = 0;
        if (bDestroy) {
            CEvDemoRes::GetInstance()->FindIndexDestory(nIndex, 1);
        }
    }

    void CTournamentMan::SetupObj(long nIndex, CEventModel *pModel)
    {
        if (m_apObjModel[nIndex] != 0) {
            delete m_apObjModel[nIndex];
        }
        m_apObjModel[nIndex] = 0;
        m_apObjModel[nIndex] = pModel;
    }

    void CTournamentMan::SetupStageDemoModel(long nStage)
    {
        CStageMan *pStageMan = new CStageManDemo(GetParent());
        pStageMan->SetStageNo(nStage);
        pStageMan->Initialize_();
        m_pStageMan = pStageMan;
        CEvDemoRes::GetInstance()->SetStageMan(pStageMan);
        CTournamentRouteMan::CreateInstanceDemo(nStage);
    }

    void CTournamentMan::SetupStageModel(long nStage)
    {
        CStageManTournament *pStageMan = new CStageManTournament(GetParent());
        pStageMan->SetStageNo(nStage);
        pStageMan->Initialize_();
        m_pStageMan = pStageMan;
        CTournamentRouteMan::CreateInstance(nStage, pStageMan);

        // The arrows of the route choice.
        for (long i = 0; i < OBJ_SELECT_NUM; i++) {
            CEvDemoMdlObj::_SETUP setup(0);
            setup.nShadowSize = 0x20;
            setup.nLightSelector = 0;
            m_apObjModel[OBJ_SELECT + i] =
                CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_yajirusi_00_rmdl.brres", "co_to_yajirusi_00");
        }
    }

    void CTournamentMan::SetSceneData(nw4r::g3d::ResFile file)
    {
        m_pCameraAnm->SetData(file);
        m_pCameraAnm->MakeSceneInfo("SceneID", 0);
        m_pNodeData->SetResAnm(nw4r::g3d::ResAnmChr(0));
        m_SceneFile = file;
    }

    void CTournamentMan::SetSceneOffset(const bcs::FVEC3 &vOffset, const bcs::FVEC3 &vRot)
    {
        m_vSceneOffset = vOffset;
        m_vSceneRot = vRot;
    }

    CEventModel *CTournamentMan::SetSceneNode(CEventModel *pModel, const char *pNode, int bDisp, int bMove)
    {
        long nId = m_pNodeData->GetAnmId(pNode);
        if (pModel != 0 && nId != -1) {
            if (bMove) {
                new CEventMove(pModel, m_pNodeData, 1);
                pModel->SetLinkAction(0, 1);
            }
            pModel->SetDisp(bDisp);
            pModel->SetNode(nId, 0.0f);
            pModel->SetMoveOffset(m_vSceneOffset);
            pModel->SetMoveOffsetRot(m_vSceneRot);
            return pModel;
        }
        return 0;
    }

    CEventModel *CTournamentMan::SetSceneNode(long nNo, long nGroup, short nStart, CEventModel *pModel,
                                              const char *pNode, int bDisp, int bMove)
    {
        short nCamera = GetSceneCameraIndex(nGroup, nStart);
        if (nCamera == -1) {
            return 0;
        }

        // The node animation of the camera, numbered nNo in place of the
        // digits its name ends in.
        const char *pName = m_pCameraAnm->GetSceneInfo(nCamera).pName;
        long nLen = strlen(pName);
        while (nLen > 0 && static_cast<unsigned char>(pName[nLen - 1] - '0') <= 9) {
            nLen--;
        }
        char szName[0x40];
        memcpy(szName, pName, nLen);
        sprintf(&szName[nLen], "%02d", nNo);
        m_pNodeData->SetResAnm(m_SceneFile, szName);
        return SetSceneNode(pModel, pNode, bDisp, bMove);
    }

    void CTournamentMan::SetChrDisp(int bDisp, unsigned long ulMask)
    {
        for (long i = 0; i < CHR_MAX; i++) {
            if ((ulMask & (1 << i)) && m_apChrModel[i] != 0) {
                m_apChrModel[i]->SetDisp(bDisp);
            }
        }
    }

    void CTournamentMan::GetChrMoveFrame(float &fMin, float &fMax, unsigned long ulMask)
    {
        fMin = 3.4028235e38f;
        fMax = 1.1754944e-38f;
        for (long i = 0; i < CHR_MAX; i++) {
            if ((ulMask & (1 << i)) && m_apChrModel[i] != 0) {
                float fFrame = m_apChrModel[i]->GetMoveFrame();
                if (fMin > fFrame) {
                    fMin = fFrame;
                }
                if (fMax < fFrame) {
                    fMax = fFrame;
                }
            }
        }
    }

    int CTournamentMan::IsMoveEnd(unsigned long ulMask)
    {
        for (long i = 0; i < CHR_MAX; i++) {
            if ((ulMask & (1 << i)) && m_apChrMove[i] != 0 && !m_apChrMove[i]->IsEnd()) {
                return 0;
            }
        }
        return 1;
    }

    unsigned long CTournamentMan::SetMoveRoute(long nIndex, int bFootSe, unsigned short *pRoute, long nNum,
                                               float fMin, int bLast, float fFrame)
    {
        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
        CEventModel *pModel = m_apChrModel[nIndex];
        CRouteMove *pMove = m_apChrMove[nIndex];

        if (nNum <= 0) {
            // No routes given: run the team's last route again, when the
            // character is far enough from its end.
            unsigned short nLast = TournamentWork::GetTeamInfo(nIndex).GetLastRoute();
            unsigned short nRoute = nLast;
            if (nRoute >= 0x100) {
                nRoute -= 0x100;
            }
            if (nRoute == 0) {
                return 0;
            }
            const TournamentWork::ROUTE_INFO *pInfo = TournamentWork::GetRouteInfo(nRoute);
            bcs::FVEC3 vEnd;
            // Tests the route number with the backwards bit already taken
            // off, so it always takes nPoint1; kept as the original has it.
            if (nRoute >= 0x100) {
                TournamentWork::GetPointViewPos(vEnd, pInfo->nPoint2);
            } else {
                TournamentWork::GetPointViewPos(vEnd, pInfo->nPoint1);
            }
            bcs::FVEC3 vDif;
            FVEC3Dif(vDif, pModel->GetPos(), vEnd);
            float fLen = bcs::FVEC3Len(&vDif);
            if (fLen < 0.0001f) {
                return 0;
            }
            if (fLen < fMin) {
                return 0;
            }
            pRoute[0] = nLast;
            bLast = 1;
            nNum = 1;
        }

        CRouteBase::_PUSH_INFO aInfo[16];
        long i;
        for (i = 0; i < nNum; i++) {
            unsigned short nRoute = pRoute[i];
            if (nRoute >= 0x100) {
                nRoute -= 0x100;
            }
            aInfo[i].pRoute = pRouteMan->GetRoute(nRoute);
            aInfo[i].fUnk04 = 0.0f;
            aInfo[i].fUnk08 = 0.0f;
            const TournamentWork::ROUTE_INFO *pInfo = TournamentWork::GetRouteInfo(nRoute);
            if (pRoute[i] >= 0x100) {
                aInfo[i].nFlag = 1;
                aInfo[i].nStart = pInfo->nPoint1;
                aInfo[i].nEnd = pInfo->nPoint2;
                aInfo[i].nType = pInfo->nParam;
            } else {
                aInfo[i].nFlag = 0;
                aInfo[i].nStart = pInfo->nPoint2;
                aInfo[i].nEnd = pInfo->nPoint1;
                aInfo[i].nType = pInfo->nParam;
            }
        }

        if (bLast) {
            // The first route starts where the character is.
            bcs::FVEC3 vStart;
            TournamentWork::GetPointViewPos(vStart, aInfo[0].nStart);
            aInfo[0].fUnk04 = bcs::FVEC3Dist(&pModel->GetPos(), &vStart);
            aInfo[0].nFlag |= 4;
        }

        CRouteBase::_PUSH_PARAM param;
        param.pMove = pMove;
        aInfo[nNum - 1].fUnk08 = fMin;
        aInfo[nNum].pRoute = 0;
        param.pModel = pModel;
        param.ppPrev = 0;
        param.pInfo = &aInfo[0];
        for (i = 0; i < nNum; i++) {
            param.pNext = &aInfo[i + 1];
            int bPush = 1;
            if (param.ppPrev != 0 && *param.ppPrev != 0 && ((*param.ppPrev)->GetPushFlag() & 0x10)) {
                bPush = 0;
            }
            if (param.pNext != 0 && param.pNext->pRoute != 0 && (param.pNext->pRoute->GetPushFlag() & 0x8)) {
                bPush = 0;
            }
            if (bPush) {
                param.pInfo->pRoute->PushRoute(param, param.pInfo->nFlag, param.pInfo->fUnk08,
                                               param.pInfo->fUnk04);
            }
            param.ppPrev = &param.pInfo->pRoute;
            param.pInfo = param.pNext;
        }

        pModel->SetEnableFootSe(bFootSe);
        pMove->SetUpdate(1);
        pMove->SetupMove(1.0f, 1);
        pMove->SetFrame(fFrame);
        return 1 << nIndex;
    }

    unsigned long CTournamentMan::SetMoveLine(long nIndex, long nPoint, float fLen, float fFrame, float fRate,
                                              float fMul)
    {
        CEventModel *pModel = m_apChrModel[nIndex];
        new CEventMove(pModel, 0, 1);

        bcs::FVEC3 vEnd;
        TournamentWork::GetPointViewPos(vEnd, nPoint);
        bcs::FVEC3 vStart = pModel->GetPos();
        bcs::FVEC3 vRot;
        FVEC3Dif(vRot, vEnd, vStart);
        vRot.y = 0.0f;
        if (bcs::FVEC3Len(&vRot) > 0.0001f) {
            float fRotY = WrapRad(nw4r::math::Atan2Rad(vRot.x, vRot.z));
            vRot.Set(0.0f, fRotY, 0.0f);
        } else {
            vRot = pModel->GetRot();
        }

        // Stop fLen short of the point, coming from the character's side.
        bcs::FVEC3 vOffset;
        bcs::BCS_VecZ_RotY(vOffset, fLen, WrapRad(vRot.y + 3.1415927f));
        bcs::FVEC3Add(&vEnd, &vEnd, &vOffset);

        CEventNodeSimple *pNode = new CEventNodeSimple;
        pNode->SetStart(pModel->GetPos(), vRot);
        pNode->SetEnd(vEnd, vRot);
        pNode->_SetRate(fRate, fMul, 0.00001f);
        pModel->SetLinkNode(pNode, 1);
        pModel->SetNode(0, fFrame);
        pModel->SetEnableFootSe(0);
        return 1 << nIndex;
    }

    unsigned long CTournamentMan::CleanMoveRoute(long nIndex, int bReset)
    {
        CEventModel *pModel = m_apChrModel[nIndex];
        CRouteMove *pMove = m_apChrMove[nIndex];
        if (pModel == 0 || pMove == 0) {
            return 0;
        }

        if (bReset) {
            pMove->SetFrame(-1.0f);
        }
        pMove->CleanRoute(0.0001f);
        pMove->SetUpdate(0);
        pModel->SetEnableFootSe(0);
        pModel->SetLinkMove(pMove, 0);
        StopEffect(nIndex, nIndex + 1);
        CTournamentRouteMan::GetInstance()->ResetNowRoute(nIndex);
        return 1 << nIndex;
    }

    void CTournamentMan::InitMoveRoute(long nTeam)
    {
        if (nTeam != -1) {
            CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
            const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
            CEventModel *pModel = m_apChrModel[nTeam];
            CRouteMove *pMove = m_apChrMove[nTeam];
            unsigned short nRoute = team.GetLastRoute();
            float fFade = 10000.0f;
            int bBack = 0;
            if (nRoute >= 0x100) {
                bBack = 1;
                nRoute -= 0x100;
            }

            // Put the character back at the end of the route it came along.
            if (nRoute != 0) {
                CRouteBase *pRoute = pRouteMan->GetRoute(nRoute);
                const TournamentWork::ROUTE_INFO *pInfo = TournamentWork::GetRouteInfo(nRoute);
                CRouteBase::_PUSH_INFO info;
                if (bBack) {
                    info.nStart = pInfo->nPoint1;
                    info.nEnd = pInfo->nPoint2;
                    info.nType = pInfo->nParam;
                } else {
                    info.nStart = pInfo->nPoint2;
                    info.nEnd = pInfo->nPoint1;
                    info.nType = pInfo->nParam;
                }
                bcs::FVEC3 vEnd;
                TournamentWork::GetPointViewPos(vEnd, info.nEnd);
                bcs::FVEC3 vDif;
                FVEC3Dif(vDif, pModel->GetPos(), vEnd);
                bcs::FVEC3 vFlat(vDif.x, 0.0f, vDif.z);
                float fLen = bcs::FVEC3Len(&vFlat);
                if (fLen > 0.0001f) {
                    CRouteBase::_PUSH_PARAM param;
                    param.pMove = pMove;
                    param.pModel = pModel;
                    param.ppPrev = 0;
                    param.pInfo = &info;
                    param.pNext = 0;
                    info.pRoute = pRoute;
                    info.nFlag = bBack | 0x10;
                    info.fUnk04 = 0.0f;
                    info.fUnk08 = fLen;
                    pRoute->PushRoute(param, bBack, fLen, 0.0f);
                    pMove->SetupMove(1.0f, 1);
                    pMove->SetFrame(-1.0f);
                    fFade = 0.0001f;
                }
            }
            pModel->SetEnableFootSe(0);
            pMove->CleanRoute(fFade);
            pMove->SetUpdate(0);
        }
    }

    void CTournamentMan::InitMoveRouteRot(long nTeam)
    {
        if (nTeam != -1) {
            CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
            const TournamentWork::TEAM_INFO &team = TournamentWork::GetTeamInfo(nTeam);
            CEventModel *pModel = m_apChrModel[nTeam];
            CRouteMove *pMove = m_apChrMove[nTeam];
            unsigned short nRoute = team.GetLastRoute();
            float fFade = 10000.0f;
            unsigned char nFlag = 0;
            if (nRoute >= 0x100) {
                nFlag |= 1;
                nRoute -= 0x100;
            }

            // Turn the character the way the route it came along goes.
            if (nRoute != 0) {
                bcs::FVEC3 vPoint;
                TournamentWork::GetPointViewPos(vPoint, team.nPoint);
                bcs::FVEC3 vDif;
                FVEC3Dif(vDif, pModel->GetPos(), vPoint);
                bcs::FVEC3 vFlat(vDif.x, 0.0f, vDif.z);
                float fLen = bcs::FVEC3Len(&vFlat);
                if (fLen > 0.0001f) {
                    const TournamentWork::ROUTE_INFO *pInfo = TournamentWork::GetRouteInfo(nRoute);
                    CRouteBase *pRoute = pRouteMan->GetRoute(nRoute);
                    CRouteBase::_PUSH_INFO info;
                    if (nFlag) {
                        info.nStart = pInfo->nPoint1;
                        info.nEnd = pInfo->nPoint2;
                        info.nType = pInfo->nParam;
                    } else {
                        info.nStart = pInfo->nPoint2;
                        info.nEnd = pInfo->nPoint1;
                        info.nType = pInfo->nParam;
                    }
                    if (info.nStart == team.nPoint) {
                        nFlag |= 8;
                    }
                    CRouteBase::_PUSH_PARAM param;
                    param.pMove = pMove;
                    param.pModel = pModel;
                    param.ppPrev = 0;
                    param.pInfo = &info;
                    param.pNext = 0;
                    info.pRoute = pRoute;
                    info.nFlag = nFlag | 0x10;
                    info.fUnk04 = 0.0f;
                    info.fUnk08 = fLen;
                    pRoute->PushRoute(param, info.nFlag, info.fUnk08, info.fUnk04);
                    pMove->SetupMove(1.0f, 0);
                    pMove->SetFrame(-1.0f);
                    fFade = 0.0001f;
                }
            }
            pMove->CleanRoute(fFade);
            pMove->SetUpdate(0);
            pModel->SetEnableFootSe(0);
        }
    }

    void CTournamentMan::PushMoveRoute(long nTeam, unsigned short *pRoute, long nNum)
    {
        if (nTeam != -1) {
            TournamentWork::TournamentInfo();
            TournamentWork::TEAM_INFO &team = TournamentWork::TeamInfo(nTeam);
            if (nNum != 0) {
                long nFrom;
                long nTo;
                if (nNum >= 4) {
                    // Only the last four are kept.
                    nFrom = nNum - 4;
                    nTo = 0;
                } else {
                    nFrom = 0;
                    nTo = 4;
                    if (team.aRoute[0] == 0) {
                        nTo = 0;
                    } else if (team.aRoute[1] == 0) {
                        nTo = 1;
                    } else if (team.aRoute[2] == 0) {
                        nTo = 2;
                    } else if (team.aRoute[3] == 0) {
                        nTo = 3;
                    }
                    // Make room by dropping the oldest.
                    long nOver = nTo + nNum - 4;
                    if (nOver > 0) {
                        nTo = 4 - nNum;
                        for (long i = 0; i < nTo; i++) {
                            team.aRoute[i] = team.aRoute[i + nOver];
                        }
                    }
                }
                for (; nFrom < nNum; nFrom++, nTo++) {
                    team.aRoute[nTo] = pRoute[nFrom];
                }
            }
        }
    }

    void CTournamentMan::SetupSelect(CRouteBase **ppRoute, unsigned char nPoint, const unsigned char *pDir, long nNum,
                                     long nRoute, long nChr, float fHeight, float f1, float f2)
    {
        CTournamentRouteMan *pRouteMan = CTournamentRouteMan::GetInstance();
        long i;
        for (i = 0; i < OBJ_SELECT_NUM; i++) {
            if (m_apObjModel[OBJ_SELECT + i] != 0) {
                m_apObjModel[OBJ_SELECT + i]->SetDisp(0);
            }
            ppRoute[i] = 0;
        }

        bcs::FVEC3 vPoint;
        TournamentWork::GetPointViewPos(vPoint, nPoint);
        vPoint.y += fHeight;
        bcs::FVEC3 vChr = vPoint;
        CEventModel *pChr = m_apChrModel[nChr];
        if (pChr != 0) {
            vChr = pChr->GetPos();
            vChr.y += fHeight;
        }

        for (i = 0; i < nNum; i++, ppRoute++, pDir++) {
            short nFirst = m_aPointHead[nPoint];
            for (TournamentWork::_LINK_POINT *p = nFirst > 0 ? &m_aLink[nFirst] : 0; p != 0;
                 p = p->nNext > 0 ? &m_aLink[p->nNext] : 0) {
                unsigned short nId = p->nRoute;
                int bBack = 0;
                if (nId >= 0x100) {
                    bBack = 1;
                    nId -= 0x100;
                }
                if (p->nPoint == *pDir) {
                    *ppRoute = pRouteMan->GetRoute(nId);
                    if (nRoute == nId) {
                        (*ppRoute)->SetupSelect(vChr, m_apObjModel[OBJ_SELECT + i], f1, f2, 0.0f, bBack);
                    } else {
                        (*ppRoute)->SetupSelect(vPoint, m_apObjModel[OBJ_SELECT + i], f1, f2, 0.0f, bBack);
                    }
                    break;
                }
            }
        }
    }

    void CTournamentMan::MakePointData(long nLink)
    {
        TournamentWork::MakePointData(m_aPointHead, m_aLink, LINK_MAX, nLink);
    }

    void CTournamentMan::MakeRouteSelect(unsigned short *pRoute, unsigned char nStart, unsigned char nGoal,
                                         unsigned char nMask, const unsigned char *pVia, float fRand)
    {
        short aCost[0x200];
        long nNum = 0;

        if (pVia != 0) {
            for (; *pVia != 0xFF; pVia++) {
                unsigned char nNext = *pVia;
                TournamentWork::MakeRouteCost(aCost, m_aPointHead, m_aLink, nStart, nMask, 0);
                TournamentWork::GetPointFastRoute(nStart, nNext, nMask, aCost, m_aPointHead, m_aLink, m_aMove,
                                                  MOVE_MAX, 0);
                nNum = TournamentWork::SelectFastRouteBasic(nStart, pRoute, nNum, 0, m_aMove, fRand);
                nStart = nNext;
            }
        }
        TournamentWork::MakeRouteCost(aCost, m_aPointHead, m_aLink, nStart, nMask, 0);
        TournamentWork::GetPointFastRoute(nStart, nGoal, nMask, aCost, m_aPointHead, m_aLink, m_aMove, MOVE_MAX, 0);
        TournamentWork::SelectFastRouteBasic(nStart, pRoute, nNum, 0, m_aMove, fRand);
    }

    long CTournamentMan::GetEffectSlot(long nFirst, long nLast)
    {
        for (long i = nFirst; i < nLast; i++) {
            if (static_cast<unsigned long>(i) <= EFFECT_MAX - 1) {
                unsigned long hEffect = m_aEffect[i];
                if (hEffect == 0) {
                    return i;
                }
                if (CEffectCtrl::GetInstance()->FindList(hEffect) == 0) {
                    m_aEffect[i] = 0;
                    return i;
                }
            }
        }
        return -1;
    }

    CEffectCluster *CTournamentMan::GetEffect(long nSlot)
    {
        unsigned long hEffect = GetEffectHandle(nSlot);
        if (hEffect != 0) {
            return CEffectCtrl::GetInstance()->FindList(hEffect);
        }
        return 0;
    }

    CEffectCluster *CTournamentMan::SetEffectPoint(long nSlot, long nEff, long nPoint, long n, unsigned long ul1,
                                                   unsigned long ul2)
    {
        EraseEffect(nSlot, nSlot + 1);
        CEffectCluster *pCluster = 0;
        unsigned long hEffect = CEffectCtrl::CreateEffect(nEff, n, ul1, ul2, 0);
        if (hEffect != 0) {
            pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
        }
        SetEffectHandle(nSlot, hEffect);
        if (pCluster != 0) {
            bcs::FVEC3 vPos;
            TournamentWork::GetPointViewPos(vPos, nPoint);
            pCluster->SetTranslate(&vPos);
        }
        return pCluster;
    }

    CEffectCluster *CTournamentMan::SetEffect(long nSlot, long nEff, long n, unsigned long ul1, unsigned long ul2)
    {
        EraseEffect(nSlot, nSlot + 1);
        CEffectCluster *pCluster = 0;
        unsigned long hEffect = CEffectCtrl::CreateEffect(nEff, n, ul1, ul2, 0);
        if (hEffect != 0) {
            pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
        }
        SetEffectHandle(nSlot, hEffect);
        return pCluster;
    }

    CEffectCluster *CTournamentMan::AttachEffectModel(long nSlot, CEventModel *pModel, long nEff, long n,
                                                      unsigned long ulNode, const bcs::FVEC3 *pPos,
                                                      unsigned long ulFlag, unsigned long ul1, unsigned long ul2)
    {
        bcs::CScnObj *pObj = pModel->GetScnObj();
        EraseEffect(nSlot, nSlot + 1);
        CEffectCluster *pCluster = 0;
        unsigned long hEffect = CEffectCtrl::CreateAttachEffect(nEff, pObj, pPos, ulNode, ulFlag & 1,
                                                                (ulFlag >> 1) & 1, (ulFlag >> 2) & 1, n, ul1, ul2,
                                                                0);
        if (hEffect != 0) {
            pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
        }
        SetEffectHandle(nSlot, hEffect);
        return pCluster;
    }

    CEffectCluster *CTournamentMan::AttachEffectCamera(long nSlot, long nEff, const bcs::FVEC3 *pPos, long n,
                                                       unsigned long ulFlag, unsigned long ul1, unsigned long ul2)
    {
        CEventCameraNull *pObj = m_pEventCamera;
        EraseEffect(nSlot, nSlot + 1);
        CEffectCluster *pCluster = 0;
        unsigned long hEffect = CEffectCtrl::CreateAttachEffect(nEff, pObj, pPos, 0, ulFlag & 1, (ulFlag >> 1) & 1,
                                                                (ulFlag >> 2) & 1, n, ul1, ul2, 0);
        if (hEffect != 0) {
            pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
        }
        SetEffectHandle(nSlot, hEffect);
        return pCluster;
    }

    CEffectCluster *CTournamentMan::AttachEffectAuto(long nSlot, bcs::CMdlObj *pMdl, long nEff, long n)
    {
        EraseEffect(nSlot, nSlot + 1);
        CEffectCluster *pCluster = 0;
        unsigned long hEffect = CEffectCtrl::CreateAttachEffectAuto(nEff, pMdl, n);
        if (hEffect != 0) {
            pCluster = CEffectCtrl::GetInstance()->FindList(hEffect);
        }
        SetEffectHandle(nSlot, hEffect);
        return pCluster;
    }

    void CTournamentMan::StopEffect(long nFirst, long nLast)
    {
        for (long i = nFirst; i < nLast; i++) {
            if (static_cast<unsigned long>(i) <= EFFECT_MAX - 1 && m_aEffect[i] != 0) {
                CEffectCtrl::StopAttachEffect(m_aEffect[i]);
                m_aEffect[i] = 0;
            }
        }
    }

    void CTournamentMan::EraseEffect(long nFirst, long nLast)
    {
        for (long i = nFirst; i < nLast; i++) {
            if (static_cast<unsigned long>(i) <= EFFECT_MAX - 1 && m_aEffect[i] != 0) {
                CEffectCtrl::EraseAttachEffect(m_aEffect[i]);
                m_aEffect[i] = 0;
            }
        }
    }

    long CTournamentMan::GetSeSlot(long nFirst, long nLast)
    {
        for (long i = nFirst; i < nLast; i++) {
            if (m_aSe[i] == 0) {
                return i;
            }
            if (bcs::CTaskMan::GetTask(m_aSe[i]) == 0) {
                m_aSe[i] = 0;
                return i;
            }
        }
        return -1;
    }

    bcs::CTask *CTournamentMan::PlaySe(long nSe, const bcs::FVEC3 *pPos, long nSlot, float fParam1, float fVolume,
                                       float fParam3, unsigned long ulFlag)
    {
        StopSe(0.0f, nSlot, nSlot + 1);
        m_aSe[nSlot] = CGameSnd::Play(nSe, pPos, ulFlag, -1, fParam1, fVolume, fParam3);
        return GetSe(nSlot);
    }

    bcs::CTask *CTournamentMan::PlaySe(long nSe, const bcs::FVEC3 &vPos, long nSlot, float fParam1, float fVolume,
                                       float fParam3, unsigned long ulFlag)
    {
        StopSe(0.0f, nSlot, nSlot + 1);
        m_aSe[nSlot] = CGameSnd::Play(nSe, vPos, ulFlag, -1, fParam1, fVolume, fParam3);
        return GetSe(nSlot);
    }

    int CTournamentMan::IsPlaySe(long nSlot)
    {
        if (GetSe(nSlot) != 0) {
            return 1;
        }
        return 0;
    }

    void CTournamentMan::StopSe(float fFade, long nFirst, long nLast)
    {
        for (long i = nFirst; i < nLast; i++) {
            bcs::CSndTask *pSe = GetSe(i);
            if (pSe != 0) {
                pSe->Stop(fFade);
            }
            m_aSe[i] = 0;
        }
    }

    short CTournamentMan::GetSceneCameraIndex(long nGroup, short nStart)
    {
        bcs::CCameraAnm *pAnm = m_pCameraAnm;
        short nNum = pAnm->GetSceneNum();
        if (nStart == -1) {
            nStart = m_nSceneCamera != -1 ? m_nSceneCamera : 0;
        }
        for (short i = nStart; i < nNum; i++) {
            const bcs::CCameraAnm::SCENE_INFO &info = pAnm->GetSceneInfo(i);
            if (info.nGroup == nGroup && info.fFrame > 0.0f) {
                return i;
            }
        }
        return -1;
    }

    short CTournamentMan::GetSceneCameraIndex(long nGroup, const char *pName)
    {
        bcs::CCameraAnm *pAnm = m_pCameraAnm;
        short nNum = pAnm->GetSceneNum();
        for (short i = 0; i < nNum; i++) {
            const bcs::CCameraAnm::SCENE_INFO &info = pAnm->GetSceneInfo(i);
            if (info.nGroup == nGroup && info.fFrame > 0.0f && strcmp(info.pName, pName) == 0) {
                return i;
            }
        }
        return -1;
    }

    void CTournamentMan::SetCameraTarget(const bcs::FVEC3 *pTarget)
    {
        m_pCamera->SetTarget(pTarget);
    }

    float CTournamentMan::SetCameraRate(float fAdd, float fMul, float fMin)
    {
        m_pCamera->SetStep(0.5f);
        m_pCamera->SetRate(fAdd, fMul, fMin);
        return m_pCamera->GetTotal();
    }

    float CTournamentMan::SetCameraRateFrame(float fFrame, float fAdd, float fMul, float fMin)
    {
        m_pCamera->SetStep(0.5f);
        m_pCamera->SetRateFrame(fFrame, fAdd, fMul, fMin);
        return m_pCamera->GetTotal();
    }

    void CTournamentMan::SetCameraDirect(const bcs::FVEC3 &vPos, long nCamera)
    {
        // The settings come in pairs: the first for data mode 1 (see
        // CTournamentCamera::GetDataMode), the second for the other.
        TournamentData::CAMERA_INFO info = CTournamentCamera::GetDataMode() == 1 ? *GetCameraInfo(nCamera)
                                                                                  : *GetCameraInfo(nCamera + 1);
        m_pCamera->SetValueCamera(info, vPos);
    }

    void CTournamentMan::SetCameraPoint(long nPoint, long nCamera)
    {
        bcs::FVEC3 vPos;
        TournamentWork::GetPointViewPos(vPos, nPoint);
        long n = nCamera + TournamentWork::GetPointInfo(nPoint)->nCamera;
        TournamentData::CAMERA_INFO info =
            CTournamentCamera::GetDataMode() == 1 ? *GetCameraInfo(n) : *GetCameraInfo(n + 1);
        m_pCamera->SetValueCamera(info, vPos);
    }

    void CTournamentMan::GetCameraPos(bcs::FVEC3 &pos)
    {
        pos = m_pCamera->GetAt();
    }

    void CTournamentMan::LeapDataSceneCamera(long nGroup, short nStart, unsigned long ulFlag)
    {
        bcs::CCameraAnm *pAnm = m_pCameraAnm;
        short nCamera = GetSceneCameraIndex(nGroup, nStart);
        if (nCamera != -1) {
            // Jump to the scene camera's first frame.
            CInterpLockCamera::_CAMERA_INFO_LOOK info;
            pAnm->GetDataFrame(info.dir, info.at, info.roll, info.fovy, nCamera, 0, 0.0f, ulFlag);
            bcs::FVEC3 vDir;
            FVEC3Dif(vDir, info.at, info.dir);
            info.dist = bcs::FVEC3Len(&vDir);
            bcs::BCS_Vec_Normalize(info.dir, vDir);
            bcs::FVEC3Add(&info.at, &info.at, &m_vSceneOffset);
            m_pCamera->SetValueCamera(info);
        }
    }

    int CTournamentMan::SetDataSceneCamera(long nGroup, short nStart, float fFrame, unsigned long ulFlag)
    {
        short nCamera = GetSceneCameraIndex(nGroup, nStart);
        if (nCamera == -1) {
            return 1;
        }
        m_pNodeData->SetResAnm(m_SceneFile, m_pCameraAnm->GetSceneInfo(nCamera).pName);
        m_pCameraAnm->SetData(nCamera, 0);
        fFrame = fFrame >= 0.0f ? fFrame : m_pCameraAnm->GetEndFrame();
        m_pCameraAnm->m_vPosOffset = m_vSceneOffset;
        m_pCameraAnm->m_vAtOffset = m_vSceneOffset;
        m_pCameraAnm->Play(ulFlag, 1.0f, fFrame);
        m_nSceneCamera = nCamera + 1;
        return 0;
    }

    int CTournamentMan::SetCameraActive(short nMode)
    {
        bcs::CCamera *pFrom;
        CTournamentCamera *pTo;
        switch (nMode) {
        case 0:
            pFrom = m_pCamera;
            pTo = m_pCamera;
            break;
        case 1:
            pFrom = m_pCameraAnm;
            pTo = m_pCamera;
            break;
        default:
            pFrom = 0;
            pTo = 0;
            break;
        }
        if (pFrom == 0 || pTo == 0) {
            return 0;
        }

        bcs::CCamera *pActive = bcs::CCameraMan::GetActiveCamera(pFrom->GetView());
        if (pActive == pFrom) {
            return 0;
        }

        // Start from where the active camera is; the first time from the
        // map camera's default setting.
        int bFirst = m_nCameraMode == -1;
        if (pActive != 0) {
            pFrom = pActive;
        }
        CInterpLockCamera::_CAMERA_INFO_LOOK info;
        if (bFirst) {
            long nView = pFrom->GetView();
            info.at.Set(0.0f, 0.0f, 0.0f);
            info.dist = 15.0f;
            info.roll = 0.0f;
            info.SetRot(160.0f * (3.14159265f / 180.0f), 0.0f);
            info.fovy = bcs::CWorldEnv::GetViewFovy(nView);
        } else {
            info.fovy = bcs::CWorldEnv::GetViewFovy(pFrom->GetView());
            const bcs::CCamera::VIEW_DATA &data = pFrom->GetViewData();
            info.at = data.at;
            info.roll = data.roll;
            bcs::FVEC3 vDir;
            FVEC3Dif(vDir, data.at, data.pos);
            info.dist = bcs::FVEC3Len(&vDir);
            bcs::BCS_Vec_Normalize(info.dir, vDir);
        }
        pTo->SetActive(-1);
        m_pEventCamera->SetCamera(pTo);
        pTo->SetValueDirect(info, bFirst);
        m_nCameraMode = nMode;
        m_nSceneCamera = -1;
        return 1;
    }

    int CTournamentMan::IsCameraEnd()
    {
        if (bcs::CCameraMan::IsDbgCamera(m_pCamera->GetView())) {
            return 1;
        }

        switch (m_nCameraMode) {
        case 0:
            if (m_pCamera != 0 && !m_pCamera->IsEnd()) {
                return 0;
            }
            break;
        case 1:
            // The map camera has arrived: hand over to the scene camera.
            if (m_pCamera != 0) {
                if (!m_pCamera->IsEnd()) {
                    return 0;
                }
                m_pCameraAnm->SetActive(-1);
                m_pEventCamera->SetCamera(m_pCameraAnm);
                m_nCameraMode = 3;
                m_nSceneCamera = -1;
            }
            break;
        case 3:
            if (m_pCameraAnm != 0 && !m_pCameraAnm->IsEnd()) {
                return 0;
            }
            break;
        }
        return 1;
    }

    void CTournamentMan::CreatePlayerMarker(const char *pName, long nChr, long nType, int bDisp, long nIndex)
    {
        static bcs::BCS_Color g_sColMarkerTeamCol[4] = {
            bcs::BCS_Color(0x0088FFB7), bcs::BCS_Color(0xFFFFFFFF), bcs::BCS_Color(0xFFFFFFFF),
            bcs::BCS_Color(0xFFFFFFFF)};

        CEvDemoMdlObj::_SETUP setup(bDisp);
        setup.nDrawLayer = 5;
        setup.nLightSelector = 0;
        if (m_apObjModel[nIndex] != 0) {
            delete m_apObjModel[nIndex];
        }
        m_apObjModel[nIndex] = 0;
        CEventModel *pMarker = CEvDemoMdlObj::CreateModel(setup, LOADDATA_OBJ, pName, "m00");
        m_apObjModel[nIndex] = pMarker;
        if (pMarker != 0) {
            pMarker->SetRotation(bcs::FVEC3(0.0f, 0.0f, 0.0f));
            pMarker->SetPosition(bcs::FVEC3(0.0f, TournamentWork::GetTournamentInfo().fMarkerY, 0.0f));
            pMarker->SetMatTevColor("marker_color00", g_sColMarkerTeamCol[0], GX_TEVREG0);
            pMarker->SetDrawPriority(2);
            if (nType == 1) {
                static_cast<bcs::CMdlObj *>(pMarker->GetScnObj())->MatSetZModeAll(1, GX_ALWAYS, 0);
                pMarker->AttachToObject(m_apChrModel[nChr], "root", bcs::CScnObj::ATTACHMODE_1);
            } else {
                static_cast<bcs::CMdlObj *>(pMarker->GetScnObj())->MatSetZModeAll(1, GX_LEQUAL, 1);
                pMarker->AttachToObject(m_apChrModel[nChr], "root", bcs::CScnObj::ATTACHMODE_3);
            }
        }
    }

    void CTournamentMan::PlayerMarkerSetPosY(float fY, long nIndex)
    {
        CEventModel *pMarker = m_apObjModel[nIndex];
        if (pMarker != 0) {
            pMarker->SetPosition(bcs::FVEC3(pMarker->GetPos().x, fY, pMarker->GetPos().z));
        }
    }

    void CTournamentMan::PlayerMarkerDisp(int bDisp, long nIndex, int bInvert)
    {
        CEventModel *pMarker = m_apObjModel[nIndex];
        if (pMarker == 0) {
            return;
        }
        if (bInvert) {
            if (bDisp) {
                pMarker->SetFade(7, 0.0f, 0.0f);
            } else {
                pMarker->SetFade(7, 1.0f, 0.0f);
            }
        } else {
            if (bDisp) {
                pMarker->SetFade(7, 1.0f, 0.0f);
            } else {
                pMarker->SetFade(7, 0.0f, 0.0f);
            }
        }
        pMarker->SetDisp(bDisp);
    }

    void CTournamentMan::PlayerMarkerFead(float fAlpha, float fSpeed, long nIndex)
    {
        CEventModel *pMarker = m_apObjModel[nIndex];
        if (pMarker != 0) {
            pMarker->SetFade(7, fAlpha, fSpeed);
        }
    }

    void CTournamentMan::PlayerShadowDisp(int bDisp, long nIndex, int bInvert)
    {
        CEventModel *pChr = m_apChrModel[nIndex];
        if (pChr == 0) {
            return;
        }
        if (bInvert) {
            if (bDisp) {
                pChr->SetFade(5, 0.0f, 0.0f);
            } else {
                pChr->SetFade(5, 1.0f, 0.0f);
            }
        } else {
            if (bDisp) {
                pChr->SetFade(5, 1.0f, 0.0f);
            } else {
                pChr->SetFade(5, 0.0f, 0.0f);
            }
        }
        pChr->SetShadowDisp(bDisp);
    }

    void CTournamentMan::PlayerShadowFead(float fAlpha, float fSpeed, long nIndex)
    {
        CEventModel *pChr = m_apChrModel[nIndex];
        if (pChr != 0) {
            pChr->SetFade(5, fAlpha, fSpeed);
        }
    }

    void CTournamentMan::Main()
    {
    }
}
