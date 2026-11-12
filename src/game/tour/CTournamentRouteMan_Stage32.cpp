#include "game/msw/CTournamentRouteMan_Stage32.hpp"
#include "game/msw/CEvMoveActTmBigGun.hpp"

#include "bcs/BCS_Vec.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEvNodeData.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventNodeSimple.hpp"
#include "game/msw/CRouteBasicJump.hpp"
#include "game/msw/CRouteBasicLine.hpp"
#include "game/msw/CRouteDataMove.hpp"
#include "game/msw/CRouteJumpPoints.hpp"
#include "game/msw/CRouteMove.hpp"
#include "game/msw/CRoutePhysOffsetY.hpp"
#include "game/msw/CRouteWait.hpp"
#include "game/msw/CTournamentMan.hpp"
#include "game/msw/EvDemo.hpp"
#include "game/msw/TournamentData.hpp"
#include "game/msw/TournamentWork.hpp"

namespace msw {

    // The cannon of stage 32, first part: the character walks up to the
    // cannon, jumps into it and is loaded (the cannon's "big_gun_01"
    // animation). 0x3C bytes. Route type 4.
    class CRouteBigGunIn : public CRouteBase {
        public:
            CRouteBigGunIn(const _PARAM &param, float rate);

            virtual ~CRouteBigGunIn();
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);
            virtual void SetPoints(const unsigned char *pPoints, long nNum);
            virtual long GetStartRouteId() const;

        private:
            float m_fRotY; // 0x34
            long m_nPoint; // 0x38: map object of the cannon, -1 for the next free one
    };

    // The cannon of stage 32, second part: the shot from the cannon to the
    // end point of the route. 0x48 bytes. Route types 5 to 10: 5 takes the
    // strength of the shot from the length of the route, 6 to 10 are the
    // five strengths.
    class CRouteBigGunOut : public CRouteBase {
        public:
            CRouteBigGunOut(const _PARAM &param, float rate);

            virtual ~CRouteBigGunOut();
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual long GetCameraDirect(bcs::FVEC3 &dir, float &f, long n, float rate);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);
            virtual void SetPoints(const unsigned char *pPoints, long nNum);
            virtual long GetStartRouteId() const;
            virtual float GetCameraHeight() const { return m_fHeight; }

            // (name guessed) The strength of the shot.
            void SetShot(float fSpeed, float fHeight, float fCameraDelay)
            {
                m_fSpeed = fSpeed;
                m_fHeight = fHeight;
                m_fCameraDelay = fCameraDelay;
            }

        private:
            float m_fSpeed;       // 0x34
            float m_fHeight;      // 0x38: height of the arc; the camera looks this much higher
            float m_fCameraDelay; // 0x3C
            float m_fRotY;        // 0x40
            long m_nPoint;        // 0x44: map object of the cannon, -1 for the next free one
    };

    namespace {

        // msw::CLoadData entry of the map objects. (name guessed)
        const long LOADDATA_OBJ = 0x5C;

        // The jumps between map points (route type 2).
        const CRouteJumpPoints::JUMP_DATA s_JumpData = {
            {1.0f / 30.0f, 1.0f / 30.0f, -20.0f, -20.0f, -20.0f, 5.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             0.0f},
            {0.0f, 0.0f, 0.0f},
            -4.0f,
            0.0f,
            {0xC, -1, -1, 0xD},
            0.0f,
            0,
            -1,
            -1,
            0xE29,
        };

        // Where the cannon's smoke starts, relative to the cannon before
        // it is turned.
        bcs::FVEC3 s_SmokeOffset(0.0f, 2.3f, 0.85f);

        // Where the sounds of the shown map play.
        bcs::FVEC3 s_DispSePos1(1.0f, -0.5f, -8.0f);
        bcs::FVEC3 s_DispSePos2(1.0f, -0.5f, -30.0f);

        // An angle brought into [0, 2 pi].
        inline float WrapRad(float fRad)
        {
            while (fRad > 6.2831855f) {
                fRad -= 6.2831855f;
            }
            while (fRad < 0.0f) {
                fRad += 6.2831855f;
            }
            return fRad;
        }
    }

    // The two constructors are out of line, but the original has no copy
    // of them: CreateRoute is their only user, the compiler inlines them
    // there (-ipa file) and the module link strips them. Their literals
    // (0, 1) are the first of the file's pool.
    CRouteBigGunIn::CRouteBigGunIn(const _PARAM &param, float rate) : CRouteBase(param.vStart, param.vEnd, -1)
    {
        m_fRotY = param.fRotY;
        m_nPoint = -1;
        m_fRate = rate;
    }

    CRouteBigGunIn::~CRouteBigGunIn()
    {
    }

    void CRouteBigGunIn::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        float fRotY = m_fRotY;
        if (b) {
            fRotY += 3.1415927f;
            while (fRotY > 6.2831855f) {
                fRotY -= 6.2831855f;
            }
            while (fRotY < 0.0f) {
                fRotY += 6.2831855f;
            }
        }
        rot.x = 0.0f;
        rot.y = fRotY;
        rot.z = 0.0f;
    }

    void CRouteBigGunIn::SetPoints(const unsigned char *pPoints, long nNum)
    {
        if (nNum > 0) {
            m_nPoint = pPoints[0];
        }
    }

    void CRouteBigGunIn::PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2)
    {
        CTournamentRouteMan::GET_PARAM value;
        float fUp;
        float fDown;
        float fJumpFrame = TournamentData::GetJumpFrame(param.pModel, fUp, fDown);
        float fStart = param.pMove->GetRouteFrame();
        long nId = GetStartRouteId();
        long nEventId = param.pMove->GetMoveEventId(nId, 0);
        const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(m_nRouteId);

        _PARAM route;
        unsigned long ulPoint;
        if (flag & 1) {
            _SetParam(route, m_vEnd, m_vStart, 0);
            ulPoint = pRouteInfo->nPoint2;
        } else {
            _SetParam(route, m_vStart, m_vEnd, 0);
            ulPoint = pRouteInfo->nPoint1;
        }

        // The cannon's two map objects.
        long nObj = m_nPoint;
        CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
        pMan->GetParamData(4, value);
        if (nObj == -1) {
            nObj = value.nValue * 2 + 0x25;
        }

        bcs::FVEC3 vCannon = route.vEnd;
        // (fRotY is read into a local in each branch: that is where the
        // original loads it, before the call in the argument list.)
        if (m_nPoint == -1) {
            float fRotY = route.fRotY;
            pMan->_PushTimer(0x13, param.pMove->GetMoveFirstEventId(), 0.0f, nObj, fRotY, ulPoint);
        } else {
            float fRotY = route.fRotY;
            pMan->_PushTimer(0x15, param.pMove->GetMoveFirstEventId(), 0.0f, nObj, fRotY, ulPoint);
        }

        // Walk up to 2.5 before the cannon.
        if (route.fLength > 2.5f) {
            route.fLength -= 2.5f;
            bcs::BCS_VecZ_RotY(route.vEnd, route.fLength, route.fRotY);
            bcs::FVEC3Add(&route.vEnd, &route.vEnd, &route.vStart);

            CRouteBasicLine *pLine = new CRouteBasicLine(route, 0.17f, 0);
            pLine->SetRouteId(nId);
            param.pMove->PushRoute(pLine, 2, 0.0f, f2);
            nId += 0x100;

            route.vStart = route.vEnd;
            route.fLength = 2.5f;
        }

        route.vEnd = route.vStart;
        CRouteWait *pWait = new CRouteWait(route, 10.0f, 0);
        pWait->SetRouteId(nId);
        param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
        float fTime = -20.0f + (param.pMove->GetRouteFrame() - fStart);
        pMan->_PushTimer(0x3E8, param.pMove->GetMoveEventId(nId + 0x100, 0), fTime, 0, 1.0f / 30.0f, 0);

        // Jump into the cannon.
        route.vEnd = vCannon;
        pMan->GetParamData(1, value);
        route.vEnd.y += value.fValue;
        CRouteBasicJump *pJump = new CRouteBasicJump(route, route.fLength / fJumpFrame, 1);
        pJump->SetRouteId(nId + 0x100);
        pJump->SetUpFrame(fUp);
        pJump->SetDownFrame(fDown);
        pJump->CalcFrame();
        param.pMove->PushRoute(pJump, 2, 0.0f, 0.0f);

        param.pInfo->fFrame = param.pMove->GetRouteFrame() - fStart;
        pMan->PushCamera(param.pInfo, nEventId, 0.0f, param.pModel);

        // Loaded: the cannon's animation moves the character.
        long nIndex;
        nw4r::g3d::ResFile file = EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_big_gun", 0);
        pMan->_PushTimer(0x18, param.pMove->GetMoveEventId(nId + 0x200, 0), 11.0f, nObj, 0.0f, 0);
        route.vStart = vCannon;
        route.vEnd = vCannon;
        CRouteDataMove *pData = new CRouteDataMove(route, m_fRate, 40, 0);
        pData->SetRouteId(nId + 0x200);
        pData->SetNode(file, "big_gun_01", "cha_scal_anim");
        pData->SetFrameParam(11.0f, 5.0f);
        pData->CalcFrame();
        param.pMove->PushRoute(pData, 2, 0.0f, 0.0f);

        // Wait in the cannon.
        pMan->GetParamData(2, value);
        route.vStart.y += value.fValue;
        route.vEnd.y += value.fValue;
        pMan->_PushTimer(0x19, param.pMove->GetMoveEventId(nId + 0x300, 0), 0.0f, nObj, 0.0f, 0);
        pMan->GetParamData(3, value);
        CRouteWait *pWait2 = new CRouteWait(route, value.fValue, -1);
        pWait2->SetRouteId(nId + 0x300);
        param.pMove->PushRoute(pWait2, 2, 0.0f, 0.0f);
    }

    long CRouteBigGunIn::GetStartRouteId() const
    {
        return m_nRouteId + 0x100;
    }

    CRouteBigGunOut::CRouteBigGunOut(const _PARAM &param, float rate) : CRouteBase(param.vStart, param.vEnd, -1)
    {
        m_fRotY = param.fRotY;
        m_nPoint = -1;
        m_fRate = rate;
        m_fCameraDelay = 0.0f;
        m_fSpeed = 1.0f;
        m_fHeight = 10.0f;
    }

    CRouteBigGunOut::~CRouteBigGunOut()
    {
    }

    void CRouteBigGunOut::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        float fRotY = m_fRotY;
        if (b) {
            fRotY += 3.1415927f;
            while (fRotY > 6.2831855f) {
                fRotY -= 6.2831855f;
            }
            while (fRotY < 0.0f) {
                fRotY += 6.2831855f;
            }
        }
        rot.x = 0.0f;
        rot.y = fRotY;
        rot.z = 0.0f;
    }

    void CRouteBigGunOut::SetPoints(const unsigned char *pPoints, long nNum)
    {
        if (nNum > 0) {
            m_nPoint = pPoints[0];
        }
    }

    void CRouteBigGunOut::PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2)
    {
        _PARAM route;
        _SetParam(route, m_vStart, m_vEnd, 0);
        long nId = GetStartRouteId();
        long nEventId = param.pMove->GetMoveEventId(nId, 0);
        float fRotY = route.fRotY;

        // The cannon's two map objects; a new cannon takes the next two.
        long nObj = m_nPoint;
        CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
        CTournamentRouteMan::GET_PARAM value;
        pMan->GetParamData(4, value);
        if (nObj == -1) {
            nObj = value.nValue * 2 + 0x25;
            value.nValue++;
            pMan->SetParamData(4, value);
        }

        // Turn the cannon.
        _SetParam(route, m_vStart, m_vStart, 0);
        pMan->GetParamData(2, value);
        route.vStart.y += value.fValue;
        route.vEnd.y += value.fValue;
        CRouteWait *pWait = new CRouteWait(route, 30.0f, -1);
        pWait->SetRouteId(nId);
        param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
        pMan->_PushTimer(0x1A, nEventId, 0.0f, nObj, fRotY, 0);
        pMan->_PushTimer(0x1B, nEventId, 25.0f, nObj, fRotY, 0);
        pMan->_PushTimer(0x3FE, nEventId, 25.0f, param.pModel->GetIndex(), 0.0f, 8);

        // The shot.
        float fStart = param.pMove->GetRouteFrame();
        long nShotEventId = param.pMove->GetMoveEventId(nId + 0x100, 0);
        _SetParam(route, m_vStart, m_vEnd, 1);
        route.vStart.y += value.fValue;

        const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(m_nRouteId);
        long nPoint;
        if (flag & 1) {
            nPoint = pRouteInfo->nPoint2;
        } else {
            nPoint = pRouteInfo->nPoint1;
        }
        unsigned long ulFlag = 0;
        unsigned char nPointFlag = TournamentWork::GetPointInfo(nPoint)->nFlag;
        if (nPointFlag & 0x10) {
            ulFlag |= 1;
        }
        if (nPointFlag & 0x08) {
            ulFlag |= 2;
        }
        if (nPointFlag & 0x04) {
            ulFlag |= 4;
        }
        pMan->_PushTimer(0x3E9, nShotEventId, param.pMove->GetRouteFrame() - fStart, ulFlag, 1.0f / 30.0f, 0);

        CRoutePhysOffsetY *pShot = new CRoutePhysOffsetY(route, 1.0f, 3);
        pShot->SetRouteId(nId + 0x100);
        pShot->CalcSpeed(m_fSpeed, m_fHeight);
        param.pMove->PushRoute(pShot, 2, 0.0f, 0.0f);

        param.pInfo->fFrame = param.pMove->GetRouteFrame() - fStart;
        if (param.pInfo->nType == 3) {
            param.pInfo->fFrame -= m_fCameraDelay;
        }
        pMan->PushCamera(param.pInfo, nShotEventId, 0.0f, param.pModel);

        // Landing.
        _SetParam(route, m_vEnd, m_vEnd, 0);
        route.fRotY = fRotY;
        CRouteWait *pLand = new CRouteWait(route, 60.0f, -1);
        pLand->SetRouteId(nId + 0x200);
        long nLandEventId = param.pMove->GetMoveEventId(nId + 0x200, 0);
        pMan->_PushTimer(0x1D, nLandEventId, 0.0f, nObj, 0.0f, 0);
        pMan->_PushTimer(0x3FF, nLandEventId, 0.0f, param.pModel->GetIndex(), 0.0f, 0x54D);
        param.pMove->PushRoute(pLand, 2, 0.0f, 0.0f);

        if (m_nPoint == -1) {
            pMan->_PushTimer(0x16, param.pMove->GetMoveEndEventId(), 0.0f, nObj, 0.0f, 0);
        }
    }

    long CRouteBigGunOut::GetStartRouteId() const
    {
        return m_nRouteId + 0x100;
    }

    long CRouteBigGunOut::GetCameraDirect(bcs::FVEC3 &dir, float &f, long n, float rate)
    {
        bcs::FVEC3 vCamera;
        CTournamentMan::GetInstance()->GetCameraPos(vCamera);
        TournamentWork::GetPointViewPos(dir, n);
        bcs::FVEC3Add(&dir, &dir, &vCamera);
        bcs::FVEC3Scale(&dir, &dir, 0.5f);
        dir.y += m_fHeight;
        f = m_fCameraDelay;
        return TournamentWork::GetRouteInfo(m_nRouteId)->nCamera;
    }

    void CEvMoveActTmBigGun::UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame)
    {
        if (!m_pModel->IsDisp()) {
            return;
        }
        if (m_pRoute == 0) {
            return;
        }

        if (m_pPrevRoute != m_pRoute) {
            // A new route: stretch the cannon motion over it.
            float fWait = m_pRoute->GetNumFrame() - 10.0f;
            m_pModel->SetAnimSpeed(m_pModel->AnimResFrame(0x12) / fWait);
            m_pModel->AnimPlay(0x12, 0, 8.0f, m_fFrame, 1);
            m_fWait = fWait;
        } else if (m_fWait > 0.0f && m_fWait <= m_fFrame) {
            m_pModel->AnimPlay(-1, 0, 0.0f, 0.0f, 1);
            TournamentData::SetBigGunPause(m_pModel, 0.0f);
            m_fWait = 0.0f;
        }
        m_pPrevRoute = m_pRoute;
    }

    long CTournamentRouteMan_Stage32::GetChipType(long nPoint)
    {
        // The chip shown at each map point, -1 for none.
        static const signed char g_sPointToChipConvert[32] = {
            -1,   0x1B, 0x1C, 0x10, 0x1E, 0x16, 0x15, 0x19, 0x1D, 0x13, 0x14, 0x17, 0x11, 0x1A, 0x18, 0x12,
            -1,   0x01, 0x03, 0x05, 0x04, 0x0B, 0x0A, 0x0E, 0x02, 0x08, 0x09, 0x0C, 0x06, 0x0F, 0x0D, 0x07,
        };
        return g_sPointToChipConvert[nPoint];
    }

    void CTournamentRouteMan_Stage32::Initialize()
    {
        CEvNodeData node;
        _CreateTimer(0x80);

        m_vGunInPos.Set(0.0f, 0.0f, 0.0f);
        m_vGunOutPos.Set(0.0f, 0.0f, 0.0f);
        m_fGunOutFrame = 0.0f;
        m_nGunCount = 0;

        long nIndex;
        nw4r::g3d::ResFile file = EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_big_gun", 0);

        // Where the character sits in the cannon, before and after the
        // cannon turns.
        node.SetResAnm(file, "big_gun_00");
        long nId = node.GetAnmId("cha_scal_anim");
        if (nId != -1) {
            bcs::FVEC3 rot;
            node.GetResult(m_vGunInPos, rot, 0.0f, nId);
        }
        node.SetResAnm(file, "big_gun_01");
        nId = node.GetAnmId("cha_scal_anim");
        if (nId != -1) {
            bcs::FVEC3 rot;
            node.GetResult(m_vGunOutPos, rot, node.GetNumFrame(), nId);
        }
        node.SetResAnm(file, "big_gun_02");
        m_fGunOutFrame = node.GetNumFrame();

        InitializeModel();
    }

    void CTournamentRouteMan_Stage32::InitializeModel()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();

        pMan->SetupObjModelEx(0, 0, "map_cup", 0, bcs::FVEC3(1.0f, 0.0f, -27.0f), 1, 2, 1, 0, -1);
        SetCrystalColor(0);
        pMan->SetupObjModel(2, 0, "map_dokan", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        pMan->SetupObjModel(3, 0, "map_dokan", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        // The cannons: two objects each.
        for (long i = 0x25; i <= 0x30; i += 2) {
            pMan->SetupObjModel(i, 0, "map_big_gun", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
            pMan->SetupObjModel(i + 1, 1, "map_big_gun", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        }
        for (long i = 7; i <= 9; i++) {
            pMan->SetupObjModel(i, 0, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, "", -1);
        }
        pMan->SetupObjModel(10, 2, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, "", -1);
        // The gates.
        for (long i = 4; i <= 6; i++) {
            pMan->SetupObjModel(i, 1, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, "s32_obj03_02", -1);
        }
        for (long i = 11; i <= 17; i++) {
            pMan->SetupObjModel(i, 10, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, "s32_obj04_01", -1);
        }
        for (long i = 18; i <= 24; i++) {
            pMan->SetupObjModel(i, 11, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 1, 0, "s32_obj04_02", -1);
        }
        for (long i = 25; i <= 36; i++) {
            pMan->SetupObjModel(i, 0, "maker", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, -1, 0, "", -1);
        }
    }

    int CTournamentRouteMan_Stage32::GetParamData(long nId, GET_PARAM &param)
    {
        switch (nId) {
            case 0:
                param.fValue = m_fParam0;
                return 1;
            case 1:
                param.fValue = m_vGunInPos.y;
                return 1;
            case 2:
                param.fValue = m_vGunOutPos.y;
                return 1;
            case 3:
                param.fValue = m_fGunOutFrame;
                return 1;
            case 4:
                param.nValue = m_nGunCount;
                return 1;
        }
        return 0;
    }

    int CTournamentRouteMan_Stage32::SetParamData(long nId, const GET_PARAM &param)
    {
        if (nId == 4) {
            m_nGunCount = param.nValue;
            return 1;
        }
        return 0;
    }

    CRouteBase *CTournamentRouteMan_Stage32::CreateRoute(long nType, long nFrom, long nTo)
    {
        CRouteBase::_PARAM param;
        CRouteBase::_SetParam(param, nFrom, nTo, 0);

        switch (nType) {
            case 4:
                return new CRouteBigGunIn(param, 1.0f);
            case 5: {
                CRouteBigGunOut *pRoute = new CRouteBigGunOut(param, 1.0f);
                if (param.fLength < 12.0f) {
                    pRoute->SetShot(0.15f, 10.0f, 3.0f);
                } else if (param.fLength < 24.0f) {
                    pRoute->SetShot(0.21f, 10.5f, 2.0f);
                } else if (param.fLength < 36.0f) {
                    pRoute->SetShot(0.27f, 11.0f, 1.0f);
                } else if (param.fLength < 48.0f) {
                    pRoute->SetShot(0.33f, 11.5f, 0.0f);
                } else {
                    pRoute->SetShot(0.39f, 11.5f, 0.0f);
                }
                return pRoute;
            }
            case 6: {
                CRouteBigGunOut *pRoute = new CRouteBigGunOut(param, 1.0f);
                pRoute->SetShot(0.15f, 10.0f, 3.0f);
                return pRoute;
            }
            case 7: {
                CRouteBigGunOut *pRoute = new CRouteBigGunOut(param, 1.0f);
                pRoute->SetShot(0.21f, 10.5f, 2.0f);
                return pRoute;
            }
            case 8: {
                CRouteBigGunOut *pRoute = new CRouteBigGunOut(param, 1.0f);
                pRoute->SetShot(0.27f, 11.0f, 1.0f);
                return pRoute;
            }
            case 9: {
                CRouteBigGunOut *pRoute = new CRouteBigGunOut(param, 1.0f);
                pRoute->SetShot(0.33f, 11.5f, 0.0f);
                return pRoute;
            }
            case 10: {
                CRouteBigGunOut *pRoute = new CRouteBigGunOut(param, 1.0f);
                pRoute->SetShot(0.39f, 11.5f, 0.0f);
                return pRoute;
            }
            case 2: {
                CRouteJumpPoints *pRoute = new CRouteJumpPoints(param, 1.0f);
                pRoute->SetJumpData(&s_JumpData);
                return pRoute;
            }
        }
        return _CreateRoute(param, nType, nFrom, nTo);
    }

    void CTournamentRouteMan_Stage32::TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();

        switch (nEvent) {
            case 0x13: { // a cannon is set up at its point (objects nParam, nParam + 1)
                bcs::FVEC3 pos;
                TournamentWork::GetPointViewPos(pos, ulParam);
                _StageSet(pos, nParam, fParam, 0, 0.0f, 1, 0);
                _StageSet(pos, nParam + 1, fParam, 0, 0.0f, 1, 0);
                _StageEff(0x54F, nParam, fParam, ulParam, 5, 0, 0);
                _StageEff(0x550, nParam, fParam, ulParam, 3, 0, 0);
                _AddTimer(0x14, m_nTimerGroup, 2.0f + m_fTimer, nParam, fParam, ulParam);
                _AddTimer(0x3FA, m_nTimerGroup, 2.0f + m_fTimer, nParam, 0.1f, ulParam);
                _AddTimer(0x3FA, m_nTimerGroup, 2.0f + m_fTimer, nParam + 1, 0.1f, ulParam);
                _AddTimer(0x3F8, m_nTimerGroup, 2.0f + m_fTimer, nParam + 1, 0.0f, 0);
                break;
            }
            case 0x15:
                _StageAnim("big_gun_05", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                break;
            case 0x14:
                _StageAnim("big_gun_00", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                break;
            case 0x16: // the cannon goes away
                _StageAnim("big_gun_04", nParam, fParam, ulParam, 0x3F7, 0, 0.0f, 1, 1);
                _AddTimer(0x17, m_nTimerGroup, 20.0f + m_fTimer, nParam, fParam, ulParam);
                break;
            case 0x17:
                _AddTimer(0x3F9, m_nTimerGroup, m_fTimer, nParam, 0.1f, ulParam);
                _AddTimer(0x3F9, m_nTimerGroup, m_fTimer, nParam + 1, 0.1f, ulParam);
                _StageEff(0x551, nParam, fParam, ulParam, 3, 0, 0);
                break;
            case 0x18: // the character is loaded
                _StageAnim("big_gun_01", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                _StageSe(0xE3A, nParam, fParam, ulParam, -1);
                break;
            case 0x19:
                _StageAnim("big_gun_02", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                break;
            case 0x1B: // the cannon fires
                _StageAnim("big_gun_03", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                // 5 frames later (the original subtracts -5).
                _AddTimer(0x1C, m_nTimerGroup, m_fTimer - -5.0f, nParam, fParam, ulParam);
                break;
            case 0x1C: { // smoke out of the barrel
                CEventModel *pModel = pMan->GetObjModel(nParam);
                if (pModel != 0) {
                    CEffectCluster *pEffect = pMan->SetEffect(pMan->GetEffectSlot(0xDA, 0x1DA), 0x56E, 3, 0, 0);
                    if (pEffect != 0) {
                        bcs::FVEC3 pos;
                        bcs::BCS_Vec_RotY(pos, s_SmokeOffset, fParam);
                        bcs::FVEC3Add(&pos, &pos, &pModel->GetPos());
                        // 152 degrees, wrapped at run time as the original does.
                        bcs::FVEC3 rot(WrapRad(2.6529005f), fParam, 0.0f);
                        pEffect->SetTranslate(&pos);
                        pEffect->SetRotateRad(&rot);
                    }
                }
                _StageSe(0xE3C, nParam, fParam, ulParam, -1);
                break;
            }
            case 0x1A: { // the cannon turns to direction fParam
                CEventModel *pModel = pMan->GetObjModel(nParam);
                if (pModel != 0) {
                    CEventNodeSimple *pNode = new CEventNodeSimple();
                    bcs::FVEC3 rot = pModel->GetRot();
                    pNode->SetStart(pModel->GetPos(), rot);
                    rot.y = fParam;
                    pNode->SetEnd(pModel->GetPos(), rot);
                    pNode->_SetRate(-0.0005f, 0.953125f, 0.00001f);
                    pModel->SetLinkNode(pNode, 1);
                    pModel->SetNode(0, 0.0f);
                }
                _StageSe(0xE3B, nParam, fParam, ulParam, -1);
                break;
            }
            case 0x1D: { // the shot lands
                bcs::FVEC3 power(0.08f, 0.65f, 0.05f);
                _StageQuake(power, 40.0f);
                _StageSe(0xE3D, nParam, fParam, ulParam, -1);
                break;
            }
            case 0xE: // the bridge comes down
                _StageAnim("s32_obj03_02_CLOSE", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                _StageEffAttach(0x570, "bridge_down", nParam, fParam, ulParam, 3, 0, 0);
                _StageEffAttach(0x575, "bridge_down", nParam, fParam, ulParam, 3, 0, 0);
                _StageEff(0x573, nParam, fParam, ulParam, 3, 0, 0);
                _StageEff(0x576, nParam, fParam, ulParam, 3, 0, 0);
                _StageSe(0xE43, nParam, fParam, ulParam, -1);
                break;
            case 0xF: // a gate of the first kind closes
                _RemoveTimer(0x12, nParam);
                if (nParam != 10) {
                    _StageAnim("s32_obj03_01_CLOSE", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                    _StageSe(0xE3F, nParam, fParam, ulParam, -1);
                } else {
                    _StageAnim("s32_obj03_03_OPEN", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                    _StageSe(0xE3E, nParam, fParam, ulParam, -1);
                }
                break;
            case 0x10: // ... and opens
                _RemoveTimer(0x11, nParam);
                if (nParam != 10) {
                    _StageAnim("s32_obj03_01_OPEN", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                    _StageSe(0xE3F, nParam, fParam, ulParam, -1);
                } else {
                    _StageAnim("s32_obj03_03_CLOSE", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                    _StageSe(0xE3E, nParam, fParam, ulParam, -1);
                }
                break;
            case 0x11:
                if (nParam != 10) {
                    _StageAnim("s32_obj03_01_CLOSE_IDLE", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                } else {
                    _StageAnimEnd("s32_obj03_03_OPEN", nParam, fParam, ulParam, 1, 1, -1);
                }
                break;
            case 0x12:
                if (nParam != 10) {
                    _StageAnim("s32_obj03_01_OPEN_IDLE", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                } else {
                    _StageAnimEnd("s32_obj03_03_CLOSE", nParam, fParam, ulParam, 1, 1, -1);
                }
                break;
            case 0xC: { // a gate of the second kind opens
                _RemoveTimer(0x1F, nParam);
                _StageSe(0xE41, nParam, fParam, ulParam, -1);
                const char *pAnim = nParam >= 0x12 ? "s32_obj04_02_OPEN" : "s32_obj04_01_OPEN";
                _StageAnim(pAnim, nParam, fParam, ulParam, 0x1E, 0, 0.0f, 1, 1);
                break;
            }
            case 0x1E: {
                const char *pAnim = nParam >= 0x12 ? "s32_obj04_02_OPEN_IDLE" : "s32_obj04_01_OPEN_IDLE";
                _StageAnim(pAnim, nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                break;
            }
            case 0xD: { // ... and closes
                _RemoveTimer(0x1E, nParam);
                _StageSe(0xE40, nParam, fParam, ulParam, -1);
                const char *pAnim = nParam >= 0x12 ? "s32_obj04_02_CLOSE" : "s32_obj04_01_CLOSE";
                _StageAnim(pAnim, nParam, fParam, ulParam, 0x1F, 0, 0.0f, 1, 1);
                break;
            }
            case 0x1F: {
                const char *pAnim = nParam >= 0x12 ? "s32_obj04_02_CLOSE_IDLE" : "s32_obj04_01_CLOSE_IDLE";
                _StageAnim(pAnim, nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                break;
            }
            case 0x3F6:
                m_nGunCount = 0;
                m_nUnk584 = 0;
                break;
            case 0xB: // all chips flash
                if (!pMan->IsPlaySe(0x11A)) {
                    _StageSe(0xE1C, nParam, fParam, ulParam, 0x11A);
                }
                for (long i = 0; i < 128; i++) {
                    if (m_apChip[i] != 0) {
                        _StageChipFade(i, 1.0f, 0.0f);
                        _StageChipFade(i, 0.0f, 1.0f / fParam);
                        _StageChipEff(0x56F, i, 1.0f / fParam, ulParam, 3, 0, 0);
                    }
                }
                _AddTimer(0, m_nTimerGroup, m_fTimer + fParam, nParam, fParam, ulParam);
                break;
            default:
                _DefTimerEvent(nEvent, nParam, fParam, ulParam, b);
                break;
        }
    }

    void CTournamentRouteMan_Stage32::PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel)
    {
        _DefPushCamera(pInfo, n, f, pModel);
    }

    void CTournamentRouteMan_Stage32::StageDispSe(int bDisp)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        if (bDisp) {
            pMan->PlaySe(0xE42, s_DispSePos1, pMan->GetSeSlot(0x120, 0x130), 0.0f, 1.0f, 0.0f, 1);
            pMan->PlaySe(0xE44, s_DispSePos2, pMan->GetSeSlot(0x120, 0x130), 0.0f, 1.0f, 0.0f, 1);
        } else {
            pMan->StopSe(0.0f, 0x120, 0x130);
        }
    }
}
