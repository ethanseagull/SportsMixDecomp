#include "game/msw/CTournamentRouteMan.hpp"

#include <stdio.h>
#include "bcs/BCS_Phys.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CCamera.hpp"
#include "bcs/CCameraMan.hpp"
#include "bcs/CWorldEnv.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/Camera.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/math/triangular.h"
#include "game/msw/CEffectCluster.hpp"
#include "game/msw/CEffectCtrl.hpp"
#include "game/msw/CEvDemoMdlObj.hpp"
#include "game/msw/CEvMoveActTmBigGun.hpp"
#include "game/msw/CEvNodeData.hpp"
#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMoveAction.hpp"
#include "game/msw/CGameSnd.hpp"
#include "game/msw/CRouteBasicJump.hpp"
#include "game/msw/CRouteBasicLine.hpp"
#include "game/msw/CRouteDataMove.hpp"
#include "game/msw/CRouteJumpPoints.hpp"
#include "game/msw/CRouteMove.hpp"
#include "game/msw/CRoutePhysOffsetY.hpp"
#include "game/msw/CRouteWait.hpp"
#include "game/msw/CStageManTournament.hpp"
#include "game/msw/CTournament.hpp"
#include "game/msw/CTournamentChip.hpp"
#include "game/msw/CTournamentMan.hpp"
#include "game/msw/CTournamentRouteMan_Stage31.hpp"
#include "game/msw/CTournamentRouteMan_Stage32.hpp"
#include "game/msw/CTournamentRouteMan_Stage33.hpp"
#include "game/msw/CTrophyObj.hpp"
#include "game/msw/EvDemo.hpp"
#include "game/msw/TournamentData.hpp"
#include "game/msw/TournamentWork.hpp"

namespace msw {

    // A pipe of the map (route type 3): the character walks up to the pipe,
    // jumps in, comes out of the pipe at the other end and jumps down to the
    // end point. 0x40 bytes. (member names guessed)
    class CRouteDokan : public CRouteBase {
        public:
            CRouteDokan(const _PARAM &param, float rate) : CRouteBase(param.vStart, param.vEnd, -1)
            {
                m_fRotY = param.fRotY;
                m_fRate = rate;
                m_nPointIn = 2;
                m_nPointOut = 3;
                m_nUnk20 |= 0x18;
            }

            virtual ~CRouteDokan() {}
            virtual void PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2);
            virtual long GetCameraDirect(bcs::FVEC3 &dir, float &f, long n, float rate);
            virtual void GetResultRot(bcs::FVEC3 &rot, float frame, int b);
            virtual long GetStartRouteId() const { return m_nRouteId + 0x100; }

        private:
            float m_fRotY;   // 0x34
            long m_nPointIn;  // 0x38: map object of the pipe the character goes in
            long m_nPointOut; // 0x3C: and of the one it comes out of
    };

    // What a walking character plays: the walk or run motion by its speed,
    // with a dust effect when it runs. 0x14 bytes.
    class CEvMoveActTmNormal : public CEventMoveAction {
        public:
            CEvMoveActTmNormal(CEventModel *pModel) : CEventMoveAction(pModel) {}

            virtual ~CEvMoveActTmNormal() {}
            virtual void UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame);
    };

    // What a jumping character plays: the jump motion stretched over the
    // jump, and the landing sound. 0x1C bytes. (member names guessed)
    class CEvMoveActTmJump : public CEventMoveAction {
        public:
            CEvMoveActTmJump(CEventModel *pModel) : CEventMoveAction(pModel), m_fSeFrame(0.0f), m_fIdleFrame(0.0f)
            {
            }

            virtual ~CEvMoveActTmJump() {}
            virtual void UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame);

        private:
            float m_fSeFrame;   // 0x14: frame the landing sound plays at, 0 when done
            float m_fIdleFrame; // 0x18: frame the idle pose returns at, 0 when done
    };

    CTournamentRouteMan *CTournamentRouteMan::m_pInstance;

    namespace {

        // The file's tables, in the order of the original's .rodata.
        // (names guessed)

        // The chip of the end point of a drawn route, by the directions the
        // route leaves it in (bits 8, 4, 2, 1: +x, -x, +z, -z).
        const unsigned char s_LineEndChip[16] = {0, 2, 1, 3, 8, 10, 9, 11, 4, 6, 5, 7, 12, 14, 13, 15};

        // The type of the line of the first 31 routes (SetupRoute); 0x40:
        // drawn backwards.
        const long s_RouteLineType[32] = {0, 1,    5, 1, 0x45, 1, 5, 1, 0x45, 1, 5,    1, 0x45, 1, 5, 1,
                                          0x45, 2, 2, 2, 2,    6, 0x46, 6, 0x46, 3, 3, 7, 0x47, 4, 0, 0};

        // The prefix of the line animations of each sport (CRouteBasicLine::
        // AnimPlay).
        const char *s_GameLineName[] = {"co_to_ba_", "co_to_vo_", "co_to_do_", "co_to_ho_", "co_to_bo_", "co_to_mix"};

        // The colours of the light of the effect that draws a route's line,
        // by sport.
        const GXColor s_LineColor[6] = {
            {0xFF, 0x00, 0x00, 0xFF}, {0x00, 0xFF, 0x00, 0xFF}, {0xFF, 0xB4, 0x00, 0xFF},
            {0x00, 0xD2, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF},
        };
        const GXColor s_LineColor2[6] = {
            {0xFF, 0xB4, 0xB4, 0xFF}, {0xDC, 0xFF, 0xD2, 0xFF}, {0xFF, 0xFF, 0xD2, 0xFF},
            {0xD2, 0xF0, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF, 0xFF},
        };

        // The mark of a map point by its kind (POINT_INFO 0x0E), in a
        // single sport and in a mixed tournament.
        const long s_PointMark[7] = {-1, 1, 2, 1, 2, -1, -1};
        const long s_PointMarkMix[7] = {-1, 3, 4, 3, 4, -1, -1};

        // msw::CLoadData entry of the map objects. (name guessed)
        const long LOADDATA_OBJ = 0x5C;

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

        // The direction of a route run forwards or backwards.
        inline float GetRotY(float fRotY, int bReverse)
        {
            if (bReverse) {
                fRotY += 3.1415927f;
                while (fRotY > 6.2831855f) {
                    fRotY -= 6.2831855f;
                }
                while (fRotY < 0.0f) {
                    fRotY += 6.2831855f;
                }
            }
            return fRotY;
        }

        // The offset of a position on the map grid (as POINT_INFO's
        // GetViewOfsX and GetViewOfsZ).
        inline float GetGridOfsX(long nX)
        {
            return 2.0f * (nX - 16);
        }

        inline float GetGridOfsZ(long nZ)
        {
            return 2.0f * -nZ;
        }

        // (name guessed) Stands in for `r1 - r2` as the original most likely
        // had it: the difference goes to a temporary and is copied float by
        // float.
        inline void FVEC3Diff(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
        {
            bcs::FVEC3 diff;

            bcs::FVEC3Sub(&diff, &r1, &r2);
            rOut = diff;
        }

        // The same for `r1 + r2`.
        inline void FVEC3Plus(bcs::FVEC3 &rOut, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
        {
            bcs::FVEC3 sum;

            bcs::FVEC3Add(&sum, &r1, &r2);
            rOut = sum;
        }

        // The distance of the route's positions at two frames.
        inline float GetMoveLen(CRouteBase *pRoute, const bcs::FVEC3 &vBase, float fFrame)
        {
            bcs::FVEC3 vPos;
            bcs::FVEC3 vDiff;

            pRoute->GetResultPos(vPos, fFrame, 0);
            FVEC3Diff(vDiff, vPos, vBase);
            return bcs::FVEC3Len(&vDiff);
        }
    }

    float CRouteBase::GetOffsetFrame(float fDist, float fFrame, float fStep, float fError)
    {
        if (fDist <= 0.0f) {
            return fFrame;
        }

        float fNumFrame = m_fNumFrame;
        float fAdd = fStep * GetRate();
        float fNow = fFrame + fAdd;
        bcs::FVEC3 vBase;
        GetResultPos(vBase, fFrame, 0);
        float fLen = GetMoveLen(this, vBase, fNow);

        // Step until the distance is passed.
        while (fLen < fDist) {
            float fPrev = fNow - fAdd;
            float fNext = fNow + fAdd;
            if (fPrev < 0.0f) {
                fPrev = 0.0f;
            }
            if (fNext > fNumFrame) {
                fNext = fNumFrame;
            }
            float fLenPrev = GetMoveLen(this, vBase, fPrev);
            float fLenNext = GetMoveLen(this, vBase, fNext);
            if (fLenPrev == fLenNext && fLen == fLenNext) {
                fNow = fNext;
                fLen = fLenNext;
                if (fNext >= fNumFrame) {
                    break;
                }
            } else if (fLenPrev < fLenNext) {
                if (fLenNext < fDist) {
                    fNow = fNext;
                    fLen = fLenNext;
                    if (fNext >= fNumFrame) {
                        break;
                    }
                } else if (fLenPrev > fDist) {
                    fNow = fPrev;
                    fLen = fLenPrev;
                    if (fPrev <= 0.0f) {
                        break;
                    }
                } else {
                    break;
                }
            } else if (fLenPrev < fDist) {
                fNow = fPrev;
                fLen = fLenPrev;
                if (fPrev <= 0.0f) {
                    break;
                }
            } else if (fLenNext > fDist) {
                fNow = fNext;
                fLen = fLenNext;
                if (fNext >= fNumFrame) {
                    break;
                }
            } else {
                break;
            }
        }

        // Then halve the step until the error is small enough.
        float fHalf = (fNow - fFrame) * 0.5f;
        while (__fabsf(fHalf) > 0.0f) {
            float fLenPrev = GetMoveLen(this, vBase, fNow - fHalf);
            float fLenNext = GetMoveLen(this, vBase, fNow + fHalf);
            fHalf *= 0.5f;
            float fErrPrev = __fabsf(fLenPrev - fDist);
            float fErrNext = __fabsf(fLenNext - fDist);
            if (fErrPrev > fErrNext) {
                fNow += fHalf;
                if (fErrNext < fError) {
                    break;
                }
            } else {
                fNow -= fHalf;
                if (fErrPrev < fError) {
                    break;
                }
            }
        }

        if (fNow < 0.0f) {
            fNow = 0.0f;
        }
        return fNow;
    }

    void CRouteBase::PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2)
    {
        float fFrame = param.pMove->PushRoute(this, flag, f1, f2);
        if (param.pInfo != 0) {
            float fStart;
            float fEnd;
            param.pMove->GetFrame(fStart, fEnd, this, flag, 0.0f, f2);
            param.pInfo->fFrame = fFrame + (fEnd - fStart);
            CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
            pMan->PushCamera(param.pInfo, param.pMove->GetMoveEventId(m_nRouteId, 0), 0.0f, param.pModel);
        }
    }

    void CRouteBase::_SetParam(_PARAM &param, long nFrom, long nTo, int b)
    {
        bcs::FVEC3 vStart;
        TournamentWork::GetPointViewPos(vStart, nFrom);
        bcs::FVEC3 vEnd;
        TournamentWork::GetPointViewPos(vEnd, nTo);
        _SetParam(param, vStart, vEnd, b);
    }

    void CRouteBase::_SetParam(_PARAM &param, const bcs::FVEC3 &start, const bcs::FVEC3 &end, int b)
    {
        bcs::FVEC3 vDiff;
        FVEC3Diff(vDiff, end, start);
        if (b) {
            vDiff.y = 0.0f;
        }
        param.vStart = start;
        param.vEnd = end;
        param.fLength = bcs::FVEC3Len(&vDiff);
        param.fRotY = WrapRad(nw4r::math::Atan2Rad(vDiff.x, vDiff.z));
    }

    float CRouteBase::GetSelectDir(float frame, int b)
    {
        bcs::FVEC3 rot;
        GetResultRot(rot, frame, b);
        return rot.y;
    }

    void CRouteBase::SetupSelect(const bcs::FVEC3 &pos, CEventModel *pModel, float f1, float f2, float f3, int b)
    {
        float fDir = b ? m_afSelectDir[1] : m_afSelectDir[0];

        // The arrow stands f1 away from the point, tilted by f2.
        bcs::FVEC3 vPos(0.0f, 0.0f, f1);
        bcs::FVEC3 vWork;
        bcs::BCS_Vec_RotY(vWork, vPos, fDir);
        bcs::BCS_Vec_RotX(vPos, vWork, WrapRad(f2));
        bcs::FVEC3Add(&vPos, &vPos, &pos);
        pModel->SetPosition(vPos);

        bcs::FVEC3 vAxis(0.0f, 0.0f, -1.0f);
        bcs::BCS_VecZ_RotY(vWork, 1.0f, fDir);
        float fCos = bcs::FVEC3Dot(&vAxis, &vWork);
        vAxis.Set(-1.0f, 0.0f, 0.0f);
        float fSin = bcs::FVEC3Dot(&vAxis, &vWork);
        bcs::FVEC3 vRot;
        vRot.x = WrapRad(f2 * fCos);
        vRot.y = WrapRad(fDir + 3.1415927f);
        vRot.z = WrapRad(f2 * fSin);
        pModel->SetRotation(vRot);
    }

    void CRouteBase::SetSelect(CEventModel *pModel, int b)
    {
        if (b) {
            pModel->AnimPlay("co_to_yajirusi_select_00", 3, 0.0f, 0.0f, 1);
        } else {
            pModel->AnimPlay("co_to_yajirusi_noselect_00", 3, 0.0f, 0.0f, 1);
        }
    }

    bcs::FVEC3 CRouteBase::GetSelectDir2d(CEventModel *pModel, long n)
    {
        // The arrow's tip and root, projected on the screen.
        bcs::FVEC3 vDir(0.0f, 0.0f, -1.0f);
        bcs::FMTX34 mtx;
        const bcs::FVEC3 &vRot = pModel->GetRot();
        nw4r::math::MTX34RotXYZRad(reinterpret_cast<nw4r::math::MTX34 *>(&mtx), vRot.x, vRot.y, vRot.z);
        bcs::FVEC3 vTip;
        PSMTXMultVec(mtx.m, (const Vec *)&vDir, (Vec *)&vTip);
        bcs::FVEC3 vPos = pModel->GetPos();
        vTip.y = 0.0f;
        bcs::FVEC3Add(&vTip, &vTip, &vPos);

        nw4r::g3d::Camera camera(reinterpret_cast<nw4r::g3d::CameraData *>(bcs::CWorldEnv::GetViewCameraData(n)));
        nw4r::math::VEC3 vScreenPos;
        nw4r::math::VEC3 vScreenTip;
        camera.Project(&vScreenPos, reinterpret_cast<const nw4r::math::VEC3 &>(vPos));
        camera.Project(&vScreenTip, reinterpret_cast<const nw4r::math::VEC3 &>(vTip));
        vScreenPos.z = 0.0f;
        vScreenTip.z = 0.0f;
        bcs::FVEC3Sub(&vDir, reinterpret_cast<const bcs::FVEC3 *>(&vScreenTip),
                      reinterpret_cast<const bcs::FVEC3 *>(&vScreenPos));
        if (bcs::FVEC3SquareMag(&vDir) == 0.0f) {
            vDir.Set(0.0f, 0.0f, 0.0f);
        } else {
            PSVECNormalize((const Vec *)&vDir, (Vec *)&vDir);
        }
        return vDir;
    }

    CRouteDataMove::CRouteDataMove(const _PARAM &param, float rate, unsigned long nFlag, long n)
        : CRouteBase(param.vStart, param.vEnd, n)
    {
        m_fRotY = param.fRotY;
        m_nFlag = nFlag;
        m_fStartFrame = 0.0f;
        m_fEndFrame = 0.0f;
        m_fStartWait = 0.0f;
        m_fEndWait = 0.0f;
        m_fRate = rate;
        m_fNumFrame = 0.0f;
        m_fAnmFrame = 0.0f;
        m_nMirror = 0;
        m_anm = nw4r::g3d::ResAnmChr();
        m_nNodeId = -1;
    }

    void CRouteDataMove::SetNode(const nw4r::g3d::ResFile &file, const char *pNode, const char *pName)
    {
        m_anm = CEvNodeData::_GetResAnm(file, pNode);
        m_fNumFrame = 0.0f;
        m_fAnmFrame = 0.0f;
        m_fStartFrame = 0.0f;
        m_fEndFrame = 0.0f;
        m_fStartWait = 0.0f;
        m_fEndWait = 0.0f;
        m_nNodeId = -1;
        m_nNodeId = CEvNodeData::_GetAnmId(m_anm, pName);
        if (m_nNodeId >= 0) {
            CalcFrame();
        }
    }

    void CRouteDataMove::SetNode(const nw4r::g3d::ResFile &file, const char *pNode, long n)
    {
        m_anm = CEvNodeData::_GetResAnm(file, pNode);
        m_fNumFrame = 0.0f;
        m_fAnmFrame = 0.0f;
        m_fStartFrame = 0.0f;
        m_fEndFrame = 0.0f;
        m_fStartWait = 0.0f;
        m_fEndWait = 0.0f;
        m_nNodeId = n;
        if (m_anm.IsValid() && m_nNodeId >= 0) {
            unsigned long nNum;
            nw4r::g3d::ResDic dic = m_anm.GetChrDic();
            if (dic.IsValid()) {
                nNum = dic.GetNumData();
            } else {
                nNum = 0;
            }
            if (m_nNodeId < nNum) {
                CalcFrame();
            }
        }
    }

    void CRouteDataMove::CalcFrame()
    {
        if (m_anm.IsValid()) {
            m_fAnmFrame = m_anm.GetNumFrame() + (-m_fEndFrame - m_fStartFrame);
            m_fNumFrame = m_fAnmFrame / m_fRate + (m_fStartWait + m_fEndWait);
        } else {
            m_fAnmFrame = 0.0f;
            m_fNumFrame = 0.0f;
            m_nNodeId = -1;
        }
    }

    float CRouteDataMove::_SetResult(nw4r::g3d::ChrAnmResult &result, float fFrame, int b)
    {
        float fRotY;
        float fNow;
        long nMirror;
        if (m_nFlag & 0x40) {
            b = !b;
        }
        if (b) {
            fNow = m_fNumFrame - fFrame - m_fEndWait;
            fRotY = WrapRad(m_fRotY + 3.1415927f);
            nMirror = m_nMirror + 2;
        } else {
            fRotY = m_fRotY;
            fNow = fFrame - m_fStartWait;
            nMirror = m_nMirror;
        }

        float fAnm = fNow * m_fRate;
        if (fAnm < 0.0f) {
            fAnm = 0.0f;
        } else if (fAnm > m_fAnmFrame) {
            fAnm = m_fAnmFrame;
        }
        CEventNodeBase::_GetDataResult(m_anm, result, fAnm + m_fStartFrame, m_nNodeId);
        CEventNodeBase::_GetFlipResult(result, bcs::FVEC3(0.0f, 0.0f, 0.0f), nMirror);
        return fRotY;
    }

    void CRouteDataMove::GetResult(_RESULT &result, float frame, int b)
    {
        nw4r::g3d::ChrAnmResult anm;
        float fRotY = _SetResult(anm, frame, b);

        if (m_nFlag & 1) {
            result.vPos.Set(0.0f, 0.0f, 0.0f);
        } else {
            anm.GetTranslate((nw4r::math::VEC3 *)&result.vPos);
        }
        if (m_nFlag & 8) {
            bcs::FVEC3Add(&result.vPos, &result.vPos, &m_vStart);
        } else if (m_nFlag & 0x10) {
            bcs::FVEC3Add(&result.vPos, &result.vPos, &m_vEnd);
        }

        if (m_nFlag & 2) {
            result.vRot.Set(0.0f, 0.0f, 0.0f);
        } else {
            CEventNodeBase::_GetDataRot(result.vRot, anm);
        }
        if (m_nFlag & 0x20) {
            result.vRot.y = WrapRad(fRotY + result.vRot.y);
        }

        if (m_nFlag & 4) {
            result.vScale.Set(1.0f, 1.0f, 1.0f);
        } else {
            anm.GetScale((nw4r::math::VEC3 *)&result.vScale);
        }
        result.nAction = m_nAction;
    }

    void CRouteDataMove::GetResultPos(bcs::FVEC3 &pos, float frame, int b)
    {
        nw4r::g3d::ChrAnmResult anm;
        _SetResult(anm, frame, b);

        if (m_nFlag & 1) {
            pos.Set(0.0f, 0.0f, 0.0f);
        } else {
            anm.GetTranslate((nw4r::math::VEC3 *)&pos);
        }
        if (m_nFlag & 8) {
            bcs::FVEC3Add(&pos, &pos, &m_vStart);
        } else if (m_nFlag & 0x10) {
            bcs::FVEC3Add(&pos, &pos, &m_vEnd);
        }
    }

    void CRouteDataMove::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        nw4r::g3d::ChrAnmResult anm;
        float fRotY = _SetResult(anm, frame, b);

        if (m_nFlag & 2) {
            rot.Set(0.0f, 0.0f, 0.0f);
        } else {
            CEventNodeBase::_GetDataRot(rot, anm);
        }
        if (m_nFlag & 0x20) {
            rot.y = WrapRad(fRotY + rot.y);
        }
    }

    CRoutePhysOffsetY::CRoutePhysOffsetY(const _PARAM &param, float rate, long n)
        : CRouteBase(param.vStart, param.vEnd, n)
    {
        m_fRotY = param.fRotY;
        m_fLength = param.fLength;
        m_fRate = rate;
        m_fNumFrame = 0.0f;
    }

    void CRoutePhysOffsetY::CalcSpeed(float fSpeed, float fHeight)
    {
        float fVelY;
        float fGravity;

        m_fNumFrame = m_fLength / fSpeed;
        bcs::BCS_Phys_CalcYVG(m_fNumFrame, m_vStart.y, m_vEnd.y, fHeight, fVelY, fGravity);
        float fRate = 1.0f / m_fNumFrame;
        m_vGravity.x = 0.0f;
        bcs::FVEC3 vMove;
        bcs::FVEC3 vDiff;
        bcs::FVEC3Sub(&vDiff, &m_vEnd, &m_vStart);
        vMove.x = vDiff.x;
        vMove.y = 0.0f;
        vMove.z = vDiff.z;
        bcs::FVEC3Scale(&m_vSpeed, &vMove, fRate);
        m_vGravity.y = fGravity;
        m_vGravity.z = 0.0f;
        m_vSpeed.y = fVelY;
    }

    void CRoutePhysOffsetY::GetResult(_RESULT &result, float frame, int b)
    {
        result.nAction = m_nAction;
        result.vScale.Set(1.0f, 1.0f, 1.0f);
        if (frame <= 0.0f) {
            result.vPos = m_vStart;
        } else if (frame >= m_fNumFrame) {
            result.vPos = m_vEnd;
        } else {
            bcs::FVEC3 vSpeed;
            bcs::FVEC3 vGravity;
            bcs::FVEC3Scale(&vSpeed, &m_vSpeed, frame);
            bcs::FVEC3Scale(&vGravity, &m_vGravity, 0.5f * (frame * frame));
            FVEC3Plus(result.vPos, vSpeed, vGravity);
            bcs::FVEC3Add(&result.vPos, &result.vPos, &m_vStart);
        }
        result.vRot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    CRouteBasicJump::CRouteBasicJump(const _PARAM &param, float rate, long n) : CRouteBase(param.vStart, param.vEnd, n)
    {
        m_pOffsetRoute = 0;
        m_fRotY = param.fRotY;
        m_fUp = 0.0f;
        m_fDown = 0.0f;
        m_fUnk44 = 0.0f;
        m_fUnk48 = 0.0f;
        m_fLength = param.fLength;
        m_fUnk50 = 1.0f;
        m_fUnk54 = 1.0f;
        m_bNoJump = 0;
        m_nSe = 0xE29;
        m_fRate = rate;
        m_fNumFrame = 0.0f;
    }

    void CRouteBasicJump::CalcFrame()
    {
        m_fNumFrame = m_fLength / m_fRate + (m_fUp + m_fDown);
    }

    void CRouteBasicJump::_SetTranslate(bcs::FVEC3 &pos, bcs::FVEC3 &scale, float fFrame, int b)
    {
        float fNow;
        if (b) {
            fNow = m_fNumFrame - fFrame - m_fDown;
        } else {
            fNow = fFrame - m_fUp;
        }

        float fScale;
        float fMove = fNow * m_fRate;
        if (fMove <= 0.0f) {
            pos = m_vStart;
            fScale = m_fUnk50;
        } else if (fMove >= m_fLength) {
            pos = m_vEnd;
            fScale = m_fUnk54;
        } else {
            float fRate = fMove / m_fLength;
            bcs::FVEC3Lerp(&pos, &m_vStart, &m_vEnd, fRate);
            fScale = fRate * (m_fUnk54 - m_fUnk50) + m_fUnk50;
        }

        if (fScale > 1.0f) {
            fScale = 1.0f;
        } else if (fScale < 0.0f) {
            fScale = 0.0f;
        }
        scale.Set(fScale, fScale, fScale);

        if (m_pOffsetRoute != 0) {
            bcs::FVEC3 vOffset;
            m_pOffsetRoute->GetResultPos(vOffset, fFrame, b);
            bcs::FVEC3Add(&pos, &pos, &vOffset);
        }
    }

    void CRouteBasicJump::GetResult(_RESULT &result, float frame, int b)
    {
        result.nAction = m_nAction;
        _SetTranslate(result.vPos, result.vScale, frame, b);
        result.vRot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    CRouteBasicLine::CRouteBasicLine(const _PARAM &param, float rate, long n) : CRouteBase(param.vStart, param.vEnd, n)
    {
        m_pLine = 0;
        m_fLength = param.fLength;
        m_fRotY = param.fRotY;
        m_fLineFrame = 0.0f;
        m_fRate = rate;
        m_fNumFrame = m_fLength / m_fRate;
    }

    CRouteBasicLine::~CRouteBasicLine()
    {
        if (m_pLine != 0) {
            delete m_pLine;
        }
        m_pLine = 0;
    }

    void CRouteBasicLine::GetResult(_RESULT &result, float frame, int b)
    {
        result.nAction = m_nAction;
        result.vScale.Set(1.0f, 1.0f, 1.0f);
        if (b) {
            frame = m_fNumFrame - frame;
        }
        float fMove = frame * m_fRate;
        if (fMove <= 0.0f) {
            result.vPos = m_vStart;
        } else if (fMove >= m_fNumFrame) {
            result.vPos = m_vEnd;
        } else {
            bcs::BCS_VecZ_RotY(result.vPos, fMove, m_fRotY);
            bcs::FVEC3Add(&result.vPos, &result.vPos, &m_vStart);
        }
        result.vRot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    void CRouteBasicLine::CalcFrame()
    {
        m_fNumFrame = m_fLength / m_fRate;
    }

    void CRouteBasicLine::MakeLine(long nFrom, long nTo)
    {
        CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
        if (m_pLine != 0) {
            delete m_pLine;
        }
        m_pLine = 0;
        m_nLineType = nFrom;
        m_pLine = pMan->CreateWinRoute(m_fLineFrame, nFrom & 0x3F, nTo);
        if (m_pLine != 0) {
            m_pLine->SetPosition(m_vStart);
            if (nFrom & 0x40) {
                m_pLine->SetRotation(bcs::FVEC3(0.0f, 3.1415927f, 0.0f));
            }
            m_pLine->SetRotation(bcs::FVEC3(0.0f, 0.0f, 0.0f));
        }
    }

    void CRouteBasicLine::SetDisp(int bDisp)
    {
        if (m_pLine != 0) {
            m_pLine->SetDisp(bDisp);
        }
    }

    int CRouteBasicLine::IsDisp() const
    {
        if (m_pLine != 0) {
            return m_pLine->IsDisp();
        }
        return 0;
    }

    void CRouteBasicLine::AnimPlay(long nAnim)
    {
        if (m_pLine == 0) {
            return;
        }

        CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
        switch (nAnim) {
            case 0: {
                // The line of the sport being played.
                const char *pName;
                switch (m_nLineType & 0x3F) {
                    case 1:
                    case 2:
                    case 3:
                        pName = "line_tate_00";
                        break;
                    case 4:
                        pName = "line_tate_01";
                        break;
                    case 5:
                        pName = "line_yoko_00";
                        break;
                    case 6:
                        pName = "line_yoko_01";
                        break;
                    case 7:
                        pName = "line_yoko_02";
                        break;
                    default:
                        pName = 0;
                        break;
                }
                if (pName != 0) {
                    char szName[32];
                    if (CTournament::IsMixTournamentGame()) {
                        sprintf(szName, "co_to_mix%s", pName);
                        m_pLine->AnimPlay(szName, 0x1F, 0.0f, m_pLine->AnimResFrame(szName, 0x1F), 1);
                    }
                    sprintf(szName, "co_to_%s", pName);
                    float fFrame = m_pLine->AnimResFrame(szName, 0x1F);
                    m_pLine->AnimPlay(szName, 8, 0.0f, fFrame, 1);
                    sprintf(szName, "%s%s", s_GameLineName[CTournament::GetGameMode()], pName);
                    m_pLine->AnimPlay(szName, 2, 0.0f, fFrame, 1);
                }
                break;
            }
            case 1:
                pMan->_AddTimer(0x3FB, 0x3FB, pMan->m_fTimer + m_fLineFrame, m_nLineType, 0.0f,
                                reinterpret_cast<unsigned long>(m_pLine));
                break;
            case 2:
                pMan->_AddTimer(0x3FC, 0x3FC, pMan->m_fTimer + 0.0f, m_nLineType, 0.1f,
                                reinterpret_cast<unsigned long>(m_pLine));
                break;
        }
    }
}

namespace msw {

    void CEvMoveActTmNormal::UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame)
    {
        if (!m_pModel->IsDisp()) {
            return;
        }

        float fSpeed = (1.0f / fFrame) * bcs::BCS_Vec_GetLenXZ(pos);
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        if (fSpeed > 0.07f) {
            // Running: dust at the feet.
            if (pMan->GetEffect(m_pModel->GetIndex()) == 0) {
                pMan->AttachEffectModel(m_pModel->GetIndex(), m_pModel, 0x54C, 3, 0, 0, 1, 0, 0);
            }
            AnimPlay(3, 2, 4.0f, 0.0f, 1.0f);
        } else if (fSpeed > 0.02f) {
            AnimPlay(2, 2, 4.0f, 0.0f, 1.0f);
        } else {
            pMan->StopEffect(m_pModel->GetIndex(), m_pModel->GetIndex() + 1);
            TournamentData::SetIdlePause(m_pModel, 0.0f);
        }
    }

    void CEvMoveActTmJump::UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame)
    {
        if (!m_pModel->IsDisp()) {
            return;
        }
        CRouteBasicJump *pRoute = static_cast<CRouteBasicJump *>(m_pRoute);
        if (pRoute == 0) {
            return;
        }

        float fStart = pRoute->m_fUnk44;
        if (pRoute->GetNoJump()) {
            if (m_pPrevRoute != m_pRoute) {
                TournamentData::SetIdlePause(m_pModel, 0.0f);
            }
        } else {
            if (m_pPrevRoute != m_pRoute) {
                // A new jump: the jump motion, the idle pose after landing
                // and the landing sound.
                m_pModel->AnimPlay(-1, 0, 0.0f, 0.0f, 1);
                float fLand = pRoute->m_fUnk48;
                float fEnd = TournamentData::SetJumpPause(m_pModel, m_fFrame + fStart);
                float fIdle = 0.0f;
                if (fLand > 0.0f) {
                    fIdle = fEnd - fLand - fStart;
                }
                m_fIdleFrame = fIdle;
                m_fSeFrame = 11.0f - m_fFrame - fStart;
            } else if (m_fIdleFrame > 0.0f && m_fIdleFrame <= m_fFrame) {
                m_pModel->SetAnimFrame(m_fIdleFrame + fStart, 1);
                m_pModel->SetAnimSpeed(0.0f);
                m_fIdleFrame = 0.0f;
            }
            if (m_fSeFrame != 0.0f && pRoute->m_nSe != -1 && m_fSeFrame <= m_fFrame) {
                m_pModel->StopSe(0.0f, 0, 1);
                m_pModel->SetSndTaskSe(0, CGameSnd::Play(pRoute->m_nSe, (const bcs::FVEC3 *)0, 1, -1, 0.0f, 1.0f, 0.0f));
                m_fSeFrame = 0.0f;
            }
        }
        m_pPrevRoute = m_pRoute;
    }

    void CRouteDokan::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        rot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    void CRouteDokan::PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2)
    {
        // The height the characters stand on a pipe at, by character type.
        static const float s_ChrOfsY[25] = {-0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f,
                                            -0.27f, 0.0f,   -0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f,
                                            -0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f, -0.27f};

        _PARAM route;
        long nIndex;
        // Direct initialisation: the original copy-constructs the file from
        // the returned temporary here (weak ResFile copy constructor).
        nw4r::g3d::ResFile file(EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_dokan", 0));
        CTournamentMan::GetInstance();
        CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
        CTournamentRouteMan::GET_PARAM value;
        pMan->GetParamData(0, value);
        float fOfsY = s_ChrOfsY[param.pModel->GetType()];
        float fUp;
        float fDown;
        float fJumpFrame = TournamentData::GetJumpFrame(param.pModel, fUp, fDown);

        // Where the routes before and after this one leave the character.
        bcs::FVEC3 vRot;
        bcs::FVEC3 vPrev;
        bcs::FVEC3 vNext;
        float fRotOut = m_fRotY;
        if (param.pPrev != 0) {
            CRouteBase *pPrev = param.pPrev->pRoute;
            if (pPrev != 0) {
                pPrev->GetResultPos(vPrev, 0.0f, param.pPrev->nFlag & 1);
                pPrev->GetResultRot(vRot, pPrev->GetNumFrame(), param.pPrev->nFlag & 1);
                f2 = param.pPrev->fUnk04;
            }
        }
        _PUSH_INFO *pNextInfo = param.pNext;
        if (pNextInfo != 0) {
            CRouteBase *pNext = pNextInfo->pRoute;
            if (pNext != 0) {
                pNext->GetResultPos(vNext, pNext->GetNumFrame(), pNextInfo->nFlag & 1);
                pNext->GetResultRot(vRot, 0.0f, param.pNext->nFlag & 1);
                fRotOut = vRot.y;
                f1 = param.pNext->fUnk08;
            }
        }

        long nId = GetStartRouteId();
        int bReverse = flag & 1;
        const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(m_nRouteId);
        unsigned long ulPointIn;
        unsigned long ulPointOut;
        if (bReverse) {
            _SetParam(route, vPrev, m_vEnd, 0);
            ulPointIn = pRouteInfo->nPoint1;
            ulPointOut = pRouteInfo->nPoint2;
        } else {
            _SetParam(route, vPrev, m_vStart, 0);
            ulPointIn = pRouteInfo->nPoint2;
            ulPointOut = pRouteInfo->nPoint1;
        }
        float fStart = param.pMove->GetRouteFrame();
        long nEventId = param.pMove->GetMoveEventId(nId, 0);
        bcs::FVEC3 vPipe(route.vEnd);
        pMan->PushTimer(0x3EE, nEventId, 0.0f, m_nPointIn, 0.0f, ulPointIn);

        // Walk up to 2 before the pipe and wait.
        if (route.fLength > 2.0f) {
            route.fLength -= 2.0f;
            bcs::BCS_VecZ_RotY(route.vEnd, route.fLength, route.fRotY);
            // nw4r::math::VEC3::operator+= (FVEC3 derives from VEC3 in the
            // original; this function calls it out of line).
            reinterpret_cast<nw4r::math::VEC3 &>(route.vEnd) += reinterpret_cast<nw4r::math::VEC3 &>(route.vStart);
            CRouteBasicLine *pLine = new CRouteBasicLine(route, 0.17f, 0);
            pLine->SetRouteId(nId);
            param.pMove->PushRoute(pLine, 2, 0.0f, f2);
            route.vStart = route.vEnd;
            nId += 0x100;
            route.fLength = 2.0f;
        }
        route.vEnd = route.vStart;
        CRouteWait *pWait = new CRouteWait(route, 45.0f, 0);
        pWait->SetRouteId(nId);
        param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
        pMan->PushTimer(0x3E8, nEventId, -20.0f + (param.pMove->GetRouteFrame() - fStart), 0, 1.0f / 30.0f, 0);
        param.pPrev->fFrame = param.pMove->GetRouteFrame() - fStart;
        pMan->PushCamera(param.pPrev, nEventId, 0.0f, param.pModel);

        // Jump onto the pipe.
        route.vEnd = vPipe;
        route.vEnd.y += value.fValue + fOfsY;
        CRouteBasicJump *pJump = new CRouteBasicJump(route, route.fLength / fJumpFrame, 1);
        pJump->SetRouteId(nId + 0x100);
        pJump->SetStartWait(fUp);
        pJump->SetEndWait(fDown);
        pJump->CalcFrame();
        long nJumpEventId = param.pMove->GetMoveEventId(nId + 0x100, 0);
        pMan->PushTimer(0x3FE, nJumpEventId, 0.0f, param.pModel->GetIndex(), 0.0f, 7);
        param.pMove->PushRoute(pJump, 2, 0.0f, 0.0f);

        // Down the pipe.
        route.vStart = vPipe;
        route.vEnd = vPipe;
        route.vStart.y += fOfsY;
        route.vEnd.y += fOfsY;
        CRouteDataMove *pIn = new CRouteDataMove(route, m_fRate, 40, 0);
        pIn->SetRouteId(nId + 0x200);
        pIn->SetNode(file, "dokan_IN", "cha_scal_anim");
        pIn->SetStartWait(5.0f);
        pIn->SetEndWait(35.0f);
        pIn->CalcFrame();
        param.pMove->PushRoute(pIn, 2, 0.0f, 0.0f);
        pMan->PushTimer(0x3F2, param.pMove->GetMoveEventId(nId + 0x200, 0), 20.0f, 0, 0.0f, 0);

        // Through the pipe: the camera moves to the other end.
        float fPipeStart = param.pMove->GetRouteFrame();
        long nPipeEventId = param.pMove->GetMoveEventId(nId + 0x300, 0);
        route.vStart = vPipe;
        route.vEnd = vPipe;
        CRouteWait *pPipe = new CRouteWait(route, 80.0f, -1);
        pPipe->SetRouteId(nId + 0x300);
        param.pMove->PushRoute(pPipe, 2, 0.0f, 0.0f);
        pMan->PushTimer(0x3EE, param.pMove->GetMoveEventId(nId + 0x400, 0), 0.0f, m_nPointOut, 0.0f, ulPointOut);
        CRouteWait *pWait2 = new CRouteWait(route, 70.0f, -1);
        pWait2->SetRouteId(nId + 0x400);
        param.pMove->PushRoute(pWait2, 2, 0.0f, 0.0f);
        param.pInfo->fFrame = param.pMove->GetRouteFrame() - fPipeStart;
        if (param.pInfo->nType == 3) {
            param.pInfo->fFrame -= 70.0f;
        }
        pMan->PushCamera(param.pInfo, nPipeEventId, 0.0f, param.pModel);

        // Out of the pipe at the other end.
        TournamentWork::GetRouteInfo(m_nRouteId);
        if (bReverse) {
            _SetParam(route, m_vStart, vNext, 0);
        } else {
            _SetParam(route, m_vEnd, vNext, 0);
        }
        vPipe = route.vStart;
        route.vStart = vPipe;
        route.vEnd = vPipe;
        route.vStart.y += fOfsY;
        route.vEnd.y += fOfsY;
        CRouteDataMove *pOut = new CRouteDataMove(route, m_fRate, 40, 0);
        pOut->SetRouteId(nId + 0x500);
        pOut->SetNode(file, "dokan_OUT", "cha_scal_anim");
        pOut->SetStartWait(5.0f);
        pOut->SetEndWait(15.0f);
        pOut->CalcFrame();
        param.pMove->PushRoute(pOut, 2, 0.0f, 0.0f);
        pMan->PushTimer(0x3F2, param.pMove->GetMoveEventId(nId + 0x500, 0), 10.0f, 0, 0.0f, 0);

        // Jump down from the pipe.
        float fOutStart = param.pMove->GetRouteFrame();
        long nOutEventId = param.pMove->GetMoveEventId(nId + 0x600, 0);
        float fJump = 2.0f;
        route.fLength -= f1;
        if (route.fLength <= fJump) {
            fJump = route.fLength;
        }
        bcs::BCS_VecZ_RotY(route.vEnd, fJump, fRotOut);
        route.vStart.y += value.fValue;
        reinterpret_cast<nw4r::math::VEC3 &>(route.vEnd) += reinterpret_cast<nw4r::math::VEC3 &>(vPipe);
        route.fLength = fJump;
        CRouteBasicJump *pJump2 = new CRouteBasicJump(route, route.fLength / fJumpFrame, 1);
        pJump2->SetRouteId(nId + 0x600);
        pJump2->SetStartWait(fUp);
        pJump2->SetEndWait(fDown);
        pJump2->CalcFrame();
        param.pMove->PushRoute(pJump2, 2, 0.0f, 0.0f);

        unsigned long ulFlag = 0;
        unsigned char nPointFlag = TournamentWork::GetPointInfo(ulPointOut)->nFlag;
        if (nPointFlag & 0x10) {
            ulFlag = 1;
        }
        if (nPointFlag & 0x08) {
            ulFlag |= 2;
        }
        if (nPointFlag & 0x04) {
            ulFlag |= 4;
        }
        pMan->PushTimer(0x3EA, nEventId, -20.0f + (param.pMove->GetRouteFrame() - fStart), ulFlag,
                        0.0025f + route.vEnd.y, 0);
        pMan->PushTimer(0x3E9, nEventId, -20.0f + (param.pMove->GetRouteFrame() - fStart), ulFlag, 1.0f / 30.0f,
                        0);
        pMan->PushTimer(0x3F0, param.pMove->GetMoveEventId(nId + 0x700, 0), 0.0f, m_nPointIn, 0.0f, 0);
        pMan->PushTimer(0x3F0, param.pMove->GetMoveEventId(nId + 0x700, 0), 0.0f, m_nPointOut, 0.0f, 0);
        CRouteWait *pWait3 = new CRouteWait(route, 30.0f, -1);
        pWait3->SetRouteId(nId + 0x700);
        param.pMove->PushRoute(pWait3, 2, 0.0f, 0.0f);

        // Walk on to where the next route starts.
        _SetParam(route, route.vEnd, vNext, 0);
        if (route.fLength > 0.01f) {
            CRouteBasicLine *pLine2 = new CRouteBasicLine(route, 0.17f, 0);
            pLine2->SetRouteId(nId + 0x800);
            param.pMove->PushRoute(pLine2, 2, f1, 0.0f);
        }
        param.pNext->fFrame = param.pMove->GetRouteFrame() - fOutStart;
        pMan->PushCamera(param.pNext, nOutEventId, 0.0f, param.pModel);
    }

    long CRouteDokan::GetCameraDirect(bcs::FVEC3 &dir, float &f, long n, float rate)
    {
        bcs::FVEC3 vCamera;
        CTournamentMan::GetInstance()->GetCameraPos(vCamera);
        TournamentWork::GetPointViewPos(dir, n);
        bcs::FVEC3Add(&dir, &dir, &vCamera);
        bcs::FVEC3Scale(&dir, &dir, 0.5f);
        f = 70.0f;
        return TournamentWork::GetRouteInfo(m_nRouteId)->nCamera;
    }

    CRouteJumpPoints::CRouteJumpPoints(const _PARAM &param, float rate) : CRouteBase(param.vStart, param.vEnd, -1)
    {
        m_pJumpData = 0;
        m_fRate = rate;
        m_fRotY = param.fRotY;
        m_nPointNum = 0;
    }

    CRouteJumpPoints::~CRouteJumpPoints()
    {
    }

    void CRouteJumpPoints::SetPoints(const unsigned char *pPoints, long nNum)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        m_nPointNum = nNum;
        for (long i = 0; i < nNum; i++) {
            long nPoint = pPoints[i];
            m_anPoint[i] = nPoint;
            m_avPoint[i].Set(0.0f, 0.0f, 0.0f);
            CEventModel *pModel = pMan->GetObjModel(nPoint);
            if (pModel != 0) {
                m_avPoint[i] = pModel->GetPos();
            }
        }
        if (nNum > 0) {
            m_anPoint[0] = -1;
            m_anPoint[nNum - 1] = -1;
        }
    }

    float CRouteJumpPoints::GetSelectDir(float frame, int b)
    {
        bcs::FVEC3 vFrom;
        bcs::FVEC3 vTo;
        if (b) {
            vFrom = m_vEnd;
            if (m_nPointNum > 1) {
                vTo = m_avPoint[m_nPointNum - 2];
            } else {
                vTo = m_vStart;
            }
        } else {
            vFrom = m_vStart;
            if (m_nPointNum > 1) {
                vTo = m_avPoint[1];
            } else {
                vTo = m_vEnd;
            }
        }
        bcs::FVEC3 vDiff;
        FVEC3Diff(vDiff, vTo, vFrom);
        return WrapRad(nw4r::math::Atan2Rad(vDiff.x, vDiff.z));
    }

    namespace {

        // The length of r1 - r2 in the XZ plane.
        inline float GetLenXZ(const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
        {
            bcs::FVEC3 vDiff;
            FVEC3Diff(vDiff, r1, r2);
            return bcs::BCS_Vec_GetLenXZ(vDiff);
        }

        // Shortens a stretch of length fLen by up to fRest: what is cut goes
        // to rCut and comes off fRest.
        inline void CutLength(float &rCut, float &fRest, float fLen)
        {
            if (fLen <= fRest) {
                fRest -= fLen;
                rCut = fLen;
            } else {
                rCut = fRest;
                fRest = 0.0f;
            }
        }

        // The direction of r1 - r2 when it has a length, for the first
        // stretch that has one.
        inline void SetFirstDir(float &rDir, int &bFirst, const bcs::FVEC3 &r1, const bcs::FVEC3 &r2)
        {
            bcs::FVEC3 vDiff;
            FVEC3Diff(vDiff, r1, r2);
            if (bFirst && bcs::BCS_Vec_GetLenXZ(vDiff) > 0.0f) {
                rDir = WrapRad(nw4r::math::Atan2Rad(vDiff.x, vDiff.z));
                bFirst = 0;
            }
        }
    }

    void CRouteJumpPoints::PushRoute(_PUSH_PARAM &param, unsigned long flag, float f1, float f2)
    {
        // The parts of the route: [0] the walk to the first point, [1] the
        // walk from the last point, [2 + n] jump n. What f2 cuts off the
        // start and f1 off the end of the route is taken from them.
        float afStartCut[10];
        float afEndCut[10];

        CTournamentMan::GetInstance();
        CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
        float fUp;
        float fDown;
        float fJumpFrame = TournamentData::GetJumpFrame(param.pModel, fUp, fDown);

        // The timer events of the points jumped on.
        long nPrevEventId = param.pMove->GetMoveFirstEventId();
        if (param.ppPrev != 0 && *param.ppPrev != 0) {
            nPrevEventId = param.pMove->GetMoveEventId((*param.ppPrev)->GetStartRouteId(), 0);
        }
        long nChr = param.pModel->GetIndex();
        for (long i = 0; i < m_nPointNum; i++) {
            if (m_anPoint[i] != -1) {
                if (m_pJumpData->nEvent[0] != -1) {
                    pMan->PushTimer(m_pJumpData->nEvent[0], nPrevEventId, m_pJumpData->fEventTime[0], m_anPoint[i],
                                    0.0f, nChr);
                }
                if (m_pJumpData->nEvent[1] != -1) {
                    pMan->PushTimer(m_pJumpData->nEvent[1], param.pMove->GetMoveFirstEventId(),
                                    m_pJumpData->fEventTime[1], m_anPoint[i], 0.0f, nChr);
                }
                if (m_pJumpData->nEvent[2] != -1) {
                    pMan->PushTimer(m_pJumpData->nEvent[2], param.pMove->GetMoveEndEventId(),
                                    m_pJumpData->fEventTime[2], m_anPoint[i], 0.0f, nChr);
                }
            }
        }

        nw4r::g3d::ResFile offsetRes = GetOffsetRes();
        long nId = GetStartRouteId();
        float fDir = param.pModel->GetRot().y;
        float fStart = param.pMove->GetRouteFrame();
        long nEventId = param.pMove->GetMoveEventId(nId, 0);
        long nLast = m_nPointNum - 1;
        for (long i = 0; i < 10; i++) {
            afStartCut[i] = 0.0f;
            afEndCut[i] = 0.0f;
        }

        _PARAM route;
        long nIndex;
        long nStep;
        if (flag & 1) {
            // Backwards: from m_vEnd over the points to m_vStart.
            if (f2 > 0.0f) {
                float fRest = f2;
                CutLength(afStartCut[0], fRest, GetLenXZ(m_avPoint[nLast], m_vEnd));
                for (long i = 0; i < nLast && fRest > 0.01f; i++) {
                    CutLength(afStartCut[i + 2], fRest, GetLenXZ(m_avPoint[nLast - i - 1], m_avPoint[nLast - i]));
                }
                float fLen = GetLenXZ(m_vStart, m_avPoint[0]);
                afStartCut[1] = fLen <= fRest ? fLen : fRest;
            }
            if (f1 > 0.0f) {
                float fRest = f1;
                CutLength(afEndCut[1], fRest, GetLenXZ(m_vStart, m_avPoint[0]));
                for (long i = 0; i < nLast && fRest > 0.01f; i++) {
                    CutLength(afEndCut[nLast - i + 1], fRest, GetLenXZ(m_avPoint[i + 1], m_avPoint[i]));
                }
                float fLen = GetLenXZ(m_avPoint[nLast], m_vEnd);
                afEndCut[0] = fLen <= fRest ? fLen : fRest;
            }

            int bFirst = 1;
            SetFirstDir(fDir, bFirst, m_avPoint[nLast], m_vEnd);
            for (long i = nLast; i > 0; i--) {
                SetFirstDir(fDir, bFirst, m_avPoint[i - 1], m_avPoint[i]);
            }
            SetFirstDir(fDir, bFirst, m_vStart, m_avPoint[0]);

            nIndex = nLast;
            nStep = -1;
            _SetParam(route, m_vEnd, m_avPoint[nLast], 0);
        } else {
            // Forwards: from m_vStart over the points to m_vEnd.
            if (f2 > 0.0f) {
                float fRest = f2;
                CutLength(afStartCut[0], fRest, GetLenXZ(m_avPoint[0], m_vStart));
                for (long i = 0; i < nLast && fRest > 0.01f; i++) {
                    CutLength(afStartCut[i + 2], fRest, GetLenXZ(m_avPoint[i + 1], m_avPoint[i]));
                }
                float fLen = GetLenXZ(m_vEnd, m_avPoint[nLast]);
                afStartCut[1] = fLen <= fRest ? fLen : fRest;
            }
            if (f1 > 0.0f) {
                float fRest = f1;
                CutLength(afEndCut[1], fRest, GetLenXZ(m_vEnd, m_avPoint[nLast]));
                for (long i = 0; i < nLast && fRest > 0.01f; i++) {
                    CutLength(afEndCut[nLast - i + 1], fRest,
                              GetLenXZ(m_avPoint[nLast - i - 1], m_avPoint[nLast - i]));
                }
                float fLen = GetLenXZ(m_avPoint[0], m_vStart);
                afEndCut[0] = fLen <= fRest ? fLen : fRest;
            }

            int bFirst = 1;
            SetFirstDir(fDir, bFirst, m_avPoint[0], m_vStart);
            for (long i = 0; i < nLast; i++) {
                SetFirstDir(fDir, bFirst, m_avPoint[i + 1], m_avPoint[i]);
            }
            SetFirstDir(fDir, bFirst, m_vEnd, m_avPoint[nLast]);

            nIndex = 0;
            nStep = 1;
            _SetParam(route, m_vStart, m_avPoint[0], 0);
        }

        // The jumps that are not cut off.
        long nJump = 0;
        for (long i = 0; i < nLast; i++) {
            if (afStartCut[i + 2] <= 0.0f && afEndCut[i + 2] <= 0.0f) {
                nJump++;
            }
        }

        // The walk to the first point.
        if (route.fLength > 0.01f && route.fLength > afStartCut[0] + afEndCut[0]) {
            fDir = route.fRotY;
            CRouteBasicLine *pLine = new CRouteBasicLine(route, 0.17f, 0);
            pLine->SetRouteId(nId);
            param.pMove->PushRoute(pLine, 2, afEndCut[0], afStartCut[0]);
            nId += 0x100;
        }

        // The markers fade out (after a wait, when the route starts here).
        float fCameraTime = 0.0f;
        if (nJump > 0 && m_pJumpData->fParam[0] > fCameraTime) {
            float fNow = param.pMove->GetRouteFrame() - fStart;
            if (fNow <= fCameraTime && !(flag & 0x10) && m_pJumpData->fParam[6] > fCameraTime) {
                route.fRotY = fDir;
                CRouteWait *pWait = new CRouteWait(route, m_pJumpData->fParam[6], 0);
                pWait->SetRouteId(nId);
                param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
                nId += 0x100;
                fCameraTime = m_pJumpData->fParam[6];
                fStart = param.pMove->GetRouteFrame() - fCameraTime;
            }
            pMan->PushTimer(0x3E8, nEventId, fNow + m_pJumpData->fParam[2], 0, m_pJumpData->fParam[0], 0);
        }

        // The jumps.
        int bWalkLast = 0;
        float fTime = param.pMove->GetRouteFrame() - fStart;
        for (long i = 0; i < nLast; i++, nIndex += nStep) {
            _SetParam(route, m_avPoint[nIndex], m_avPoint[nIndex + nStep], 0);
            if (route.fLength <= 0.01f) {
                continue;
            }

            fDir = route.fRotY;
            CRouteBasicJump *pJump = new CRouteBasicJump(route, route.fLength / fJumpFrame, 1);
            CRouteDataMove *pOffset =
                static_cast<CRouteDataMove *>(GetOffsetRoute(offsetRes));
            if (pOffset != 0) {
                pJump->m_pOffsetRoute = pOffset;
            }

            // The timing of the jump and of the offset route.
            int bNoJump;
            float fUpWait;
            float fDownWait;
            float fOffsetWait;
            float fOffsetStart;
            float fOffsetEnd;
            if (i == 0) {
                bNoJump = 0;
                fUpWait = fUp;
                fOffsetStart = m_pJumpData->fParam[8];
                fOffsetEnd = m_pJumpData->fParam[9];
                fOffsetWait = 0.0f;
                pJump->m_fUnk44 = fOffsetWait;
            } else {
                fOffsetStart = m_pJumpData->fParam[11];
                fOffsetEnd = m_pJumpData->fParam[12];
                if (m_pJumpData->fParam[13] > 0.0f) {
                    fOffsetWait = m_pJumpData->fParam[13] - fUp;
                } else {
                    fOffsetWait = 0.0f;
                }
                pJump->m_fUnk44 = fUp;
                fUpWait = 0.0f;
                bNoJump = m_pJumpData->nUnk58;
            }
            if (i == m_nPointNum - 2) {
                bNoJump = 0;
                fOffsetEnd = 0.0f;
                fOffsetStart = m_pJumpData->fParam[10];
                fDownWait = fDown;
                fOffsetWait = fOffsetEnd;
                pJump->m_fUnk48 = fOffsetEnd;
            } else {
                fDownWait = m_pJumpData->fParam[5];
                pJump->m_fUnk48 = fDown;
                if (bNoJump) {
                    fUpWait = 0.0f;
                    fDownWait = fUpWait;
                }
            }
            pJump->SetRouteId(nId);
            pJump->m_nSe = m_pJumpData->nSe;
            pJump->SetStartWait(fUpWait);
            pJump->SetEndWait(fDownWait);
            pJump->CalcFrame();
            if (pOffset != 0) {
                pOffset->SetStartWait(fOffsetWait);
                pOffset->SetStartFrame(fOffsetStart);
                pOffset->SetEndFrame(fOffsetEnd);
                pOffset->CalcFrame();
            }

            if (route.fLength > afStartCut[i + 2] + afEndCut[i + 2]) {
                // The events of the point landed on.
                long nPoint = m_anPoint[nIndex + nStep];
                if (i == 0 && m_pJumpData->nUnk5A != -1) {
                    pMan->PushTimer(m_pJumpData->nUnk5A, nEventId, fTime, nPoint, 0.0f, nChr);
                } else if (i == m_nPointNum - 2 && m_pJumpData->anJumpEvent[1] != -1) {
                    pMan->PushTimer(m_pJumpData->anJumpEvent[1], nEventId, fTime, nPoint, 0.0f, nChr);
                } else if (m_pJumpData->anJumpEvent[0] != -1) {
                    pMan->PushTimer(m_pJumpData->anJumpEvent[0], nEventId, fTime, nPoint, 0.0f, nChr);
                }
                if (m_pJumpData->nEvent[3] != -1 && nPoint != -1) {
                    pMan->PushTimer(m_pJumpData->nEvent[3], param.pMove->GetMoveEventId(nId, 0),
                                    fDownWait * m_pJumpData->fUnk48 + (m_pJumpData->fUnk44 + (fUpWait + fJumpFrame)),
                                    nPoint, m_pJumpData->fUnk54, nChr);
                }
                pJump->SetNoJump(bNoJump);
                bWalkLast = 0;
                fTime += pJump->GetNumFrame();
            } else {
                pJump->SetNoJump(1);
                bWalkLast = 1;
            }
            param.pMove->PushRoute(pJump, (flag & 0x10) | 2, afEndCut[i + 2], afStartCut[i + 2]);
            nId += 0x100;
        }

        // The markers fade in at the end point.
        const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(m_nRouteId);
        int bReverse = flag & 1;
        long nEndPoint;
        if (bReverse) {
            nEndPoint = pRouteInfo->nPoint2;
        } else {
            nEndPoint = pRouteInfo->nPoint1;
        }
        if (!bWalkLast) {
            route.vStart = route.vEnd;
        } else {
            route.vEnd = route.vStart;
        }
        route.fLength = 0.0f;
        if (nJump > 0 && !(flag & 0x10) && m_pJumpData->fParam[1] > 0.0f) {
            unsigned long ulFlag = 0;
            unsigned char nPointFlag = TournamentWork::GetPointInfo(nEndPoint)->nFlag;
            if (nPointFlag & 0x10) {
                ulFlag = 1;
            }
            if (nPointFlag & 0x08) {
                ulFlag |= 2;
            }
            if (nPointFlag & 0x04) {
                ulFlag |= 4;
            }
            float fNow = param.pMove->GetRouteFrame() - fStart;
            float fFade;
            if (route.vEnd.y > 0.01f) {
                fFade = fNow + m_pJumpData->fParam[4];
            } else {
                fFade = fNow + m_pJumpData->fParam[3];
            }
            pMan->PushTimer(0x3EA, nEventId, fFade, ulFlag, 0.0025f + route.vEnd.y, 0);
            pMan->PushTimer(0x3E9, nEventId, fFade, ulFlag, m_pJumpData->fParam[1], ulFlag);
        }

        // A wait at the end point, then the walk on.
        if (!bWalkLast && nJump > 0 && m_pJumpData->fParam[7] > 0.0f) {
            route.fRotY = fDir;
            CRouteWait *pWait = new CRouteWait(route, m_pJumpData->fParam[7], 0);
            pWait->SetRouteId(nId);
            param.pMove->PushRoute(pWait, 2, 0.0f, 0.0f);
            nId += 0x100;
        }
        if (!bWalkLast) {
            if (bReverse) {
                _SetParam(route, m_avPoint[0], m_vStart, 0);
            } else {
                _SetParam(route, m_avPoint[nLast], m_vEnd, 0);
            }
        }
        if (route.fLength > 0.01f && route.fLength > afStartCut[1] + afEndCut[1]) {
            CRouteBasicLine *pLine = new CRouteBasicLine(route, 0.17f, 0);
            pLine->SetRouteId(nId);
            param.pMove->PushRoute(pLine, 2, afEndCut[1], afStartCut[1]);
        }

        param.pInfo->fFrame = param.pMove->GetRouteFrame() - fStart;
        pMan->PushCamera(param.pInfo, nEventId, fCameraTime, param.pModel);
    }

    long CRouteJumpPoints::GetStartRouteId() const
    {
        return m_nRouteId + 0x100;
    }

    CRouteBase *CRouteJumpPoints::GetOffsetRoute(nw4r::g3d::ResFile file)
    {
        return 0;
    }

    nw4r::g3d::ResFile CRouteJumpPoints::GetOffsetRes()
    {
        return nw4r::g3d::ResFile(0);
    }

    CTournamentRouteMan *CTournamentRouteMan::GetInstance()
    {
        return m_pInstance;
    }

    void CTournamentRouteMan::DeleteInstance()
    {
        if (m_pInstance != 0) {
            delete m_pInstance;
        }
        m_pInstance = 0;
    }

    CTournamentRouteMan *CTournamentRouteMan::CreateInstance(long nStage, CStageManTournament *pStageMan)
    {
        if (m_pInstance == 0) {
            switch (nStage) {
                case 0x21:
                    m_pInstance = new CTournamentRouteMan_Stage33(pStageMan);
                    break;
                case 0x20:
                    m_pInstance = new CTournamentRouteMan_Stage32(pStageMan);
                    break;
                default:
                    m_pInstance = new CTournamentRouteMan_Stage31(pStageMan);
                    break;
            }
            m_pInstance->_Init();
            m_pInstance->Initialize();
        }
        return m_pInstance;
    }

    void CTournamentRouteMan::CreateInstanceDemo(long nStage)
    {
        switch (nStage) {
            case 0x21:
                CTournamentRouteMan_Stage33::InitializeModel();
                break;
            case 0x20:
                CTournamentRouteMan_Stage32::InitializeModel();
                break;
            case 0x1F:
                CTournamentRouteMan_Stage31::InitializeModel();
                break;
        }
    }

    CTournamentRouteMan::~CTournamentRouteMan()
    {
        if (m_pInstance != 0) {
            m_pInstance->_Destory();
            m_pInstance = 0;
        }
    }

    void CTournamentRouteMan::_Init()
    {
        CEvNodeData node;

        for (long i = 0; i < 0x100; i++) {
            m_apRoute[i] = 0;
        }
        for (long i = 0; i < 20; i++) {
            m_apNowRoute[i] = 0;
        }
        for (long i = 0; i < 128; i++) {
            m_apChip[i] = 0;
        }
        for (long i = 0; i < 128; i++) {
            m_anPointEffect[i] = -1;
            SetupStagePoint(i, 1);
        }
        m_fUnk578 = 0.0f;
        m_fParam0 = 0.0f;
        m_fUnk580 = 0.0f;
        m_nUnk08 = 0;

        // Where the character stands in the pipe.
        long nIndex;
        nw4r::g3d::ResFile file = EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_dokan", 0);
        node.SetResAnm(file, "dokan_IN");
        long nId = node.GetAnmId("cha_scal_anim");
        if (nId != -1) {
            bcs::FVEC3 vRot;
            node.GetResult(*reinterpret_cast<bcs::FVEC3 *>(&m_fUnk578), vRot, 0.0f, nId);
        }
    }

    void CTournamentRouteMan::_Destory()
    {
        for (long i = 0; i < 128; i++) {
            if (m_apChip[i] != 0) {
                delete m_apChip[i];
            }
            m_apChip[i] = 0;
        }
        for (long i = 0; i < 0x100; i++) {
            if (m_apRoute[i] != 0) {
                delete m_apRoute[i];
            }
            m_apRoute[i] = 0;
        }
        _CreateTimer(0);
    }

    void CTournamentRouteMan::_UpdateFrame(float fFrame)
    {
        // Run the timers whose time has come.
        if (m_pTimerList != 0) {
            m_fTimer += fFrame;
            do {
                TIMER *pTimer = m_pTimerList;
                float fOver = pTimer->fTime - m_fTimer;
                if (fOver > 0.0f) {
                    break;
                }
                m_nTimerGroup = pTimer->nGroup;
                m_fTimerOver = fOver;
                TimerEvent(pTimer->nEvent, pTimer->nParam, pTimer->fParam, pTimer->ulParam, 0);
                m_pTimerList = pTimer->pNext;
                pTimer->pNext = m_pTimerFree;
                m_pTimerFree = pTimer;
            } while (m_pTimerList != 0);
        }

        // The chips; one whose fade has ended is deleted.
        for (long i = 0; i < 128; i++) {
            if (m_apChip[i] != 0) {
                m_apChip[i]->UpdateFrame(fFrame);
                if (m_apChip[i]->IsFadeEnd()) {
                    if (m_apChip[i] != 0) {
                        delete m_apChip[i];
                    }
                    m_apChip[i] = 0;
                }
            }
        }
    }

    void CTournamentRouteMan::SetupRoute()
    {
        const TournamentWork::TOURNAMENT_INFO &info = TournamentWork::GetTournamentInfo();
        for (long i = 0; i < 0x100; i++) {
            const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(i);
            if (pRouteInfo->nType >= 0) {
                m_apRoute[i] = CreateRoute(pRouteInfo->nType, pRouteInfo->nPoint2, pRouteInfo->nPoint1);
                m_apRoute[i]->SetRouteId(i);

                // The directions of the selection arrows: steps of two
                // degrees from the table, or the route's own.
                float fDir;
                if (pRouteInfo->m_Unk06[0] != 0) {
                    fDir = 0.017453292f * ((pRouteInfo->m_Unk06[0] - 1) * 2);
                } else {
                    fDir = m_apRoute[i]->GetSelectDir(0.0f, 0);
                }
                m_apRoute[i]->m_afSelectDir[0] = fDir;
                if (pRouteInfo->m_Unk06[1] != 0) {
                    fDir = 0.017453292f * ((pRouteInfo->m_Unk06[1] - 1) * 2);
                } else {
                    fDir = m_apRoute[i]->GetSelectDir(0.0f, 1);
                }
                m_apRoute[i]->m_afSelectDir[1] = fDir;

                if (i < 0x1F) {
                    m_apRoute[i]->MakeLine(s_RouteLineType[i], 1);
                } else {
                    m_apRoute[i]->MakeLine(0, 1);
                }
                m_apRoute[i]->AnimPlay(0);
                int bDisp = 0;
                if ((info.aRouteFlag[i / 8] & (unsigned char)(1 << (i % 8))) && (pRouteInfo->nFlag & 0x10)) {
                    bDisp = 1;
                }
                m_apRoute[i]->SetDisp(bDisp);
            }
        }
        for (long i = 0; i < 0x100; i++) {
            SetRouteWrite(i, 0);
        }
        for (long i = 0; i < 128; i++) {
            SetupStagePoint(i, 1);
        }
    }

    CTournamentChip *CTournamentRouteMan::SetupChipModel(long nType, long nX, long nZ, const bcs::FVEC3 &pos,
                                                         long nChip, long nStageChip, long nStagePoint, int bDisp,
                                                         long nLayer, long nSelector)
    {
        for (long i = 0; i < 128; i++) {
            if (m_apChip[i] == 0) {
                nw4r::g3d::ResMdl mdl;
                if (nChip >= 0) {
                    mdl = m_pStageMan->GetChipMdl(nChip);
                } else {
                    mdl = m_pStageMan->GetGridChipMdl(nX, nZ);
                }
                CTournamentChip *pChip = CTournamentChip::CreateModel(mdl, bDisp, nLayer, nSelector);
                pChip->SetType(nType);
                pChip->m_nStageChipArg1 = nX;
                pChip->m_nStageChipArg2 = nZ;
                pChip->m_nStageChip = nStageChip;
                pChip->m_nStagePoint = nStagePoint;
                bcs::FVEC3 vPos(GetGridOfsX(nX) + pos.x, pos.y, GetGridOfsZ(nZ) + pos.z);
                pChip->SetPosition(vPos);
                m_apChip[i] = pChip;
                return pChip;
            }
        }
        return 0;
    }

    void CTournamentRouteMan::_SetupStagePoint(TournamentWork::POINT_INFO &info, long nPoint, unsigned long ulFlag,
                                               unsigned long ulFlag2, long nType, int b)
    {
        if (!info.HasViewPos()) {
            return;
        }

        long nOldChip = -1;
        if ((info.nFlag & 0x80) && info.m_Unk10 < 0x20) {
            nOldChip = GetChipType(info.m_Unk10);
        }
        info.m_Unk10 |= (unsigned char)ulFlag;
        info.nFlag |= (unsigned char)ulFlag2;
        if (info.m_Unk10 < 0x20) {
            info.m_Unk0F = GetChipType(info.m_Unk10);
        }

        long nChip = -1;
        long nMark = -1;
        if (info.nFlag & 0x80) {
            nChip = info.m_Unk0F;
            nMark = info.m_Unk0E;
        }
        if (b && nOldChip != nChip) {
            // The chip changes: hand it to the chip model of the point, or
            // make one.
            for (long i = 0; i < 128; i++) {
                CTournamentChip *pChip = m_apChip[i];
                if (pChip != 0 && nPoint == pChip->m_nStagePoint) {
                    m_apChip[i]->m_nStageChipArg1 = info.nViewX;
                    m_apChip[i]->m_nStageChipArg2 = info.nViewZ;
                    m_apChip[i]->m_nStageChip = nChip;
                    return;
                }
            }
            bcs::FVEC3 vPos(info.vPos);
            SetupChipModel(nType, info.nViewX, info.nViewZ, vPos, nOldChip, nChip, nPoint, 1, 1, 1);
            return;
        }

        // The point's chip and mark on the grid, and its effect.
        if (nMark != -1) {
            if (CTournament::IsMixTournamentGame()) {
                nMark = s_PointMarkMix[nMark];
            } else {
                nMark = s_PointMark[nMark];
            }
        }
        m_pStageMan->SetGridChip(info.nViewX, info.nViewZ, nChip);
        m_pStageMan->SetGridMark(info.nViewX, info.nViewZ, nMark);
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        pMan->StopEffect(m_anPointEffect[nPoint], m_anPointEffect[nPoint] + 1);
        if (info.nFlag & 0x80) {
            bcs::FVEC3 vPos(info.GetViewOfsX() + info.vPos.x, 0.0f, info.GetViewOfsZ() + info.vPos.z);
            if (info.m_Unk0E == 1 || info.m_Unk0E == 3) {
                m_anPointEffect[nPoint] = pMan->GetEffectSlot(0x59, 0xD9);
                CEffectCluster *pEffect = pMan->SetEffect(m_anPointEffect[nPoint], 0x549, 3, 0, 0);
                if (pEffect != 0) {
                    pEffect->SetTranslate(&vPos);
                }
            }
        }
    }

    void CTournamentRouteMan::ChangePoint(long nPoint, int bOn)
    {
        TournamentWork::POINT_INFO *pInfo = TournamentWork::PointInfo(nPoint);
        if (!pInfo->HasViewPos()) {
            return;
        }

        long nMark = pInfo->m_Unk0E;
        switch (nMark) {
            case 1:
                if (!bOn) {
                    nMark = 2;
                }
                break;
            case 2:
                if (bOn) {
                    nMark = 1;
                }
                break;
            case 3:
                if (!bOn) {
                    nMark = 4;
                }
                break;
            case 4:
                if (bOn) {
                    nMark = 3;
                }
                break;
        }
        if (nMark != pInfo->m_Unk0E) {
            pInfo->m_Unk0E = nMark;
            SetupStagePoint(nPoint, 1);
        }
    }

    void CTournamentRouteMan::SetupStagePoint(long nPoint, int b)
    {
        const TournamentWork::POINT_INFO *pInfo = TournamentWork::GetPointInfo(nPoint);
        if (!pInfo->HasViewPos()) {
            return;
        }

        long nChip = -1;
        long nMark = -1;
        if (b && (pInfo->nFlag & 0x80)) {
            nChip = pInfo->m_Unk0F;
            nMark = pInfo->m_Unk0E;
        }

        // The point's chip and mark on the grid, and its effect.
        if (nMark != -1) {
            if (CTournament::IsMixTournamentGame()) {
                nMark = s_PointMarkMix[nMark];
            } else {
                nMark = s_PointMark[nMark];
            }
        }
        m_pStageMan->SetGridChip(pInfo->nViewX, pInfo->nViewZ, nChip);
        m_pStageMan->SetGridMark(pInfo->nViewX, pInfo->nViewZ, nMark);
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        pMan->StopEffect(m_anPointEffect[nPoint], m_anPointEffect[nPoint] + 1);
        if (pInfo->nFlag & 0x80) {
            bcs::FVEC3 vPos(pInfo->GetViewOfsX() + pInfo->vPos.x, 0.0f, pInfo->GetViewOfsZ() + pInfo->vPos.z);
            if (pInfo->m_Unk0E == 1 || pInfo->m_Unk0E == 3) {
                m_anPointEffect[nPoint] = pMan->GetEffectSlot(0x59, 0xD9);
                CEffectCluster *pEffect = pMan->SetEffect(m_anPointEffect[nPoint], 0x549, 3, 0, 0);
                if (pEffect != 0) {
                    pEffect->SetTranslate(&vPos);
                }
            }
        }
    }

    void CTournamentRouteMan::SetupStageChip(long nChip, long nX, long nZ)
    {
        m_pStageMan->SetGridChip(nX, nZ, nChip);
    }

    void CTournamentRouteMan::SetChipWrite(long nPoint, unsigned long ulFlag, unsigned long ulFlag2, long nChip, int b)
    {
        _SetupStagePoint(*TournamentWork::PointInfo(nPoint), nPoint, ulFlag, ulFlag2, nChip, b);
    }

    namespace {

        // The grid directions of a step from (0, 0) by (nDX, nDZ): bits 8,
        // 4, 2, 1 for +x, -x, +z, -z, leaving (ulOut) and entering (ulIn).
        // (names guessed)
        inline void GetStepDir(long nDX, long nDZ, long &nStepX, long &nStepZ, unsigned char &ulOut,
                               unsigned char &ulIn)
        {
            ulIn = 0;
            ulOut = 0;
            if (nDX > 0) {
                ulOut |= 8;
                ulIn |= 4;
                nStepX = 1;
            } else if (nDX < 0) {
                ulOut |= 4;
                ulIn |= 8;
                nStepX = -1;
            }
            if (nDZ > 0) {
                ulOut |= 1;
                ulIn |= 2;
                nStepZ = 1;
            } else if (nDZ < 0) {
                ulOut |= 2;
                ulIn |= 1;
                nStepZ = -1;
            }
        }
    }

    void CTournamentRouteMan::SetRouteWrite(long nRoute, int bWrite)
    {
        const TournamentWork::ROUTE_INFO *pRouteInfo = TournamentWork::GetRouteInfo(nRoute);
        if (pRouteInfo->nType < 0) {
            return;
        }

        long bChip = (pRouteInfo->nFlag >> 7) & 1;
        long bLine = (pRouteInfo->nFlag >> 6) & 1;
        unsigned long ulFlag2 = bChip ? 0x80 : 0;
        TournamentWork::POINT_INFO *pEnd = TournamentWork::PointInfo(pRouteInfo->nPoint2);
        TournamentWork::POINT_INFO *pStart = TournamentWork::PointInfo(pRouteInfo->nPoint1);
        if (bChip) {
            // The points that belong to the route.
            for (long i = 1; i < 128; i++) {
                TournamentWork::POINT_INFO *pInfo = TournamentWork::PointInfo(i);
                if (pInfo->m_Unk12 == nRoute) {
                    _SetupStagePoint(*pInfo, i, 0, ulFlag2, nRoute, bWrite);
                }
            }
        }

        long nX = pEnd->nViewX;
        long nZ = pEnd->nViewZ;
        long nDX = pStart->nViewX - nX;
        long nDZ = pStart->nViewZ - nZ;
        if (nDX != 0 && nDZ != 0) {
            bLine = 0;
        }
        long nStepX = 0;
        long nStepZ = 0;
        unsigned char ulOut;
        unsigned char ulIn;
        GetStepDir(nDX, nDZ, nStepX, nStepZ, ulOut, ulIn);
        bcs::FVEC3 vPos(0.0f, 0.0f, 0.0f);
        if (!bChip || !bLine) {
            _SetupStagePoint(*pEnd, pRouteInfo->nPoint2, 0, ulFlag2, nRoute, bWrite);
        } else {
            _SetupStagePoint(*pEnd, pRouteInfo->nPoint2, ulOut, ulFlag2, nRoute, bWrite);
        }

        if (bLine) {
            // The chips along the straight route.
            long nChip = GetChipType(ulOut | ulIn);
            long nGridChip = -1;
            if (bChip) {
                nGridChip = nChip;
            }
            nX += nStepX;
            nZ += nStepZ;
            nDX -= nStepX;
            nDZ -= nStepZ;
            while (nDX != 0 || nDZ != 0) {
                if (bWrite) {
                    if (bChip) {
                        SetupChipModel(nRoute, nX, nZ, vPos, -1, nChip, -1, 1, 1, 1);
                    } else {
                        SetupChipModel(nRoute, nX, nZ, vPos, nChip, -1, -1, 1, 1, 1);
                    }
                } else {
                    m_pStageMan->SetGridChip(nX, nZ, nGridChip);
                }
                nX += nStepX;
                nZ += nStepZ;
                nDX -= nStepX;
                nDZ -= nStepZ;
            }

            // A bend to the start point.
            nDX = pStart->nViewX - nX;
            nDZ = pStart->nViewZ - nZ;
            if (nDX != 0 || nDZ != 0) {
                GetStepDir(nDX, nDZ, nStepX, nStepZ, ulOut, ulIn);
                nChip = GetChipType(ulOut);
                nGridChip = -1;
                if (bChip) {
                    nGridChip = nChip;
                }
                nX += nStepX;
                nZ += nStepZ;
                nDX -= nStepX;
                nDZ -= nStepZ;
                while (nDX != 0 || nDZ != 0) {
                    if (bWrite) {
                        if (bChip) {
                            SetupChipModel(nRoute, nX, nZ, vPos, -1, nChip, -1, 1, 1, 1);
                        } else {
                            SetupChipModel(nRoute, nX, nZ, vPos, nChip, -1, -1, 1, 1, 1);
                        }
                    } else {
                        m_pStageMan->SetGridChip(nX, nZ, nGridChip);
                    }
                    nX += nStepX;
                    nZ += nStepZ;
                    nDX -= nStepX;
                    nDZ -= nStepZ;
                    nChip = GetChipType(ulOut | ulIn);
                    nGridChip = -1;
                    if (bChip) {
                        nGridChip = nChip;
                    }
                }
            }
        }

        if (!bChip || !bLine) {
            _SetupStagePoint(*pStart, pRouteInfo->nPoint1, 0, ulFlag2, nRoute, bWrite);
        } else {
            _SetupStagePoint(*pStart, pRouteInfo->nPoint1, s_LineEndChip[ulOut], ulFlag2, nRoute, bWrite);
        }
    }

    void CTournamentRouteMan::SetWinRoute(unsigned short *pRoute, long nNum, int bWin, int bAnim)
    {
        TournamentWork::TOURNAMENT_INFO &info = TournamentWork::TournamentInfo();
        int bSet = bWin != 0;
        for (long i = 0; i < nNum; i++) {
            unsigned short nRoute = pRoute[i];
            if (nRoute >= 0x100) {
                nRoute -= 0x100;
            }
            if (nRoute == 0) {
                continue;
            }

            unsigned char nIdx = nRoute;
            int bWas = (info.aRouteFlag[nIdx / 8] & (unsigned char)(1 << (nIdx % 8))) != 0;
            unsigned char nBit = 1 << (nRoute % 8);
            CRouteBase *pRouteObj = m_apRoute[nRoute];
            if (bWin) {
                info.aRouteFlag[nRoute / 8] |= nBit;
            } else {
                info.aRouteFlag[nRoute / 8] &= ~nBit;
            }
            if (pRouteObj == 0) {
                continue;
            }

            // Show the line when the route is now marked, hide it when not.
            int bShown = 0;
            if (bWas && pRouteObj->IsDisp()) {
                bShown = 1;
            }
            int bShow = bSet;
            if (bSet) {
                bShow = (TournamentWork::GetRouteInfo(nRoute)->nFlag >> 4) & 1;
            }
            if (bAnim) {
                if (bShow && !bShown) {
                    pRouteObj->AnimPlay(1);
                } else if (bShown && !bShow) {
                    pRouteObj->AnimPlay(2);
                }
            } else {
                pRouteObj->AnimPlay(0);
                if (bShow && !bShown) {
                    pRouteObj->SetDisp(1);
                } else if (bShown && !bShow) {
                    pRouteObj->SetDisp(0);
                }
            }
        }
    }

    void CTournamentRouteMan::ResetNowRoute(long nChr)
    {
        m_apNowRoute[nChr] = 0;
    }

    int CTournamentRouteMan::IsTimerEvent()
    {
        if (m_pInstance != 0 && m_pInstance->m_pTimerList != 0) {
            return 1;
        }
        return 0;
    }

    void CTournamentRouteMan::ResetTimer()
    {
        TIMER *pTimer = m_pTimerList;
        if (pTimer == 0) {
            return;
        }

        // Run every queued timer now.
        do {
            m_nTimerGroup = pTimer->nGroup;
            m_fTimerOver = 0.0f;
            TimerEvent(pTimer->nEvent, pTimer->nParam, pTimer->fParam, pTimer->ulParam, 1);
            m_pTimerList = pTimer->pNext;
            pTimer->pNext = m_pTimerFree;
            TIMER *pNext = m_pTimerList;
            m_pTimerFree = pTimer;
            pTimer = pNext;
        } while (pTimer != 0);
        m_pTimerList = 0;
        m_fTimer = 0.0f;
        m_nTimerGroup = -1;
    }

    void CTournamentRouteMan::CleanPushTimer()
    {
        TIMER *pTimer = m_pTimerPush;
        m_pTimerPush = 0;
        while (pTimer != 0) {
            TIMER *pFree = pTimer;
            pTimer = pTimer->pNext;
            pFree->pNext = m_pTimerFree;
            m_pTimerFree = pFree;
        }
    }

    void CTournamentRouteMan::_PopTimer(long nId, float fTime)
    {
        TIMER *pTimer = m_pTimerPush;
        m_pTimerPush = 0;
        while (pTimer != 0) {
            TIMER *pFree = pTimer;
            if (nId == -1 || pTimer->nGroup == nId) {
                _AddTimer(pTimer->nEvent, pTimer->nGroup, fTime + pTimer->fTime, pTimer->nParam, pTimer->fParam,
                          pTimer->ulParam);
            } else {
                PushTimer(pTimer->nEvent, pTimer->nGroup, pTimer->fTime, pTimer->nParam, pTimer->fParam,
                          pTimer->ulParam);
            }
            pTimer = pTimer->pNext;
            pFree->pNext = m_pTimerFree;
            m_pTimerFree = pFree;
        }
    }

    void CTournamentRouteMan::_AddTimer(long nEvent, long nGroup, float fTime, long nParam, float fParam,
                                        unsigned long ulParam)
    {
        TIMER *pTimer = m_pTimerFree;
        TIMER *pPrev = 0;
        pTimer->fTime = fTime;
        pTimer->nEvent = nEvent;
        pTimer->nGroup = nGroup;
        pTimer->nParam = nParam;
        pTimer->fParam = fParam;
        pTimer->ulParam = ulParam;
        m_pTimerFree = pTimer->pNext;

        // In order of time, after the timers of the same time.
        TIMER *p = m_pTimerList;
        while (p != 0) {
            if (p->fTime > fTime) {
                break;
            }
            pPrev = p;
            p = p->pNext;
        }
        if (pPrev != 0) {
            pTimer->pNext = pPrev->pNext;
            pPrev->pNext = pTimer;
        } else {
            pTimer->pNext = m_pTimerList;
            m_pTimerList = pTimer;
        }
    }

    void CTournamentRouteMan::_PushTimer(long nEvent, long nId, float fTime, long nParam, float fParam,
                                         unsigned long ulParam)
    {
        PushTimer(nEvent, nId, fTime, nParam, fParam, ulParam);
    }

    void CTournamentRouteMan::_RemoveTimer(long nEvent, long nParam)
    {
        TIMER *pPrev = 0;
        for (TIMER *p = m_pTimerList; p != 0; p = p->pNext) {
            if (p->nEvent == nEvent && p->nParam == nParam) {
                if (pPrev != 0) {
                    pPrev->pNext = p->pNext;
                } else {
                    m_pTimerList = p->pNext;
                }
                p->pNext = m_pTimerFree;
                m_pTimerFree = p;
            }
            pPrev = p;
        }
    }

    void CTournamentRouteMan::_CreateTimer(long nNum)
    {
        if (m_pTimerPool != 0) {
            delete[] m_pTimerPool;
        }
        m_pTimerPool = 0;
        m_pTimerList = 0;
        m_pTimerPush = 0;
        m_pTimerFree = 0;
        m_fTimer = 0.0f;
        m_nTimerGroup = -1;
        if (nNum > 0) {
            m_pTimerPool = new TIMER[nNum];
            m_pTimerFree = &m_pTimerPool[nNum - 1];
            for (long i = nNum - 1; i > 0; i--) {
                m_pTimerPool[i].pNext = &m_pTimerPool[i - 1];
            }
            m_pTimerPool[0].pNext = 0;
        }
    }

    void CTournamentRouteMan::_StageAnim(const char *pAnim, long nParam, float fParam, unsigned long ulParam,
                                         long nEvent, int b, float fFrame, unsigned long ulFlag, int bDisp)
    {
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
        if (pModel == 0) {
            return;
        }
        pModel->SetDisp(bDisp);
        pModel->SetUpdateMove(b);
        float fEnd = pModel->AnimPlay(pAnim, ulFlag, 0.0f, fFrame, 1);
        if (fEnd > 0.0f && nEvent >= 0) {
            _AddTimer(nEvent, m_nTimerGroup, m_fTimer + fEnd, nParam, fParam, ulParam);
        }
    }

    void CTournamentRouteMan::_StageAnimEnd(const char *pAnim, long nParam, float fParam, unsigned long ulParam,
                                            unsigned long ulFlag, int bDisp, long nEvent)
    {
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
        if (pModel == 0) {
            return;
        }
        pModel->SetDisp(bDisp);
        pModel->SetUpdateMove(0);
        float fEnd = pModel->AnimResFrame(pAnim, 0x1F);
        pModel->AnimPlay(pAnim, ulFlag, 0.0f, fEnd, 1);
        if (nEvent >= 0) {
            _AddTimer(nEvent, m_nTimerGroup, m_fTimer + fEnd, nParam, fParam, ulParam);
        }
    }

    float CTournamentRouteMan::_StageAnimNode(const char *pAnim, long nParam, float fParam, unsigned long ulParam,
                                              long nEvent, float fFrame, long n, int b)
    {
        float fNum = 0.0f;
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
        if (pModel != 0) {
            nw4r::g3d::ResAnmChr anm = pModel->GetResFile().GetResAnmChr(pAnim);
            if (anm.IsValid()) {
                fNum = anm.GetNumFrame();
                CEvNodeData *pNode = new CEvNodeData();
                pNode->SetResAnm(anm);
                pModel->SetLinkNode(pNode, 1);
                pModel->SetMoveOffset(pModel->GetPos());
                pModel->SetMoveOffsetRot(pModel->GetRot());
                pModel->SetNode(n, fFrame);
                pModel->SetUpdateMove(1);
                pModel->SetDisp(b);
                if (fNum > 0.0f) {
                    _AddTimer(nEvent, m_nTimerGroup, m_fTimer + fNum, nParam, fParam, ulParam);
                }
            }
        }
        return fNum;
    }

    void CTournamentRouteMan::_StageAttachChr(long nParam, unsigned long ulParam, long n, int bAttach)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pObj = pMan->GetObjModel(nParam);
        CEventModel *pChr = pMan->GetChrModel(n);
        if (pObj == 0 || pChr == 0) {
            return;
        }

        if (bAttach && ulParam != 0) {
            // Ride on the object, keeping where the character is now.
            bcs::FMTX34 mtxObj;
            bcs::FMTX34 mtxChr;
            pObj->GetMatrix(mtxObj);
            pChr->GetMatrix(mtxChr);
            PSMTXInverse(mtxObj.m, mtxObj.m);
            pChr->AttachToModel(pObj);
            PSMTXConcat(mtxObj.m, mtxChr.m, mtxChr.m);
            pChr->SetAttachMtx(mtxChr);
            unsigned short nRoute = ulParam;
            pMan->PushMoveRoute(n, &nRoute, 1);
            pChr->SetUpdateMove(0);
            TournamentData::SetIdlePause(pChr, 0.0f);
        } else {
            pChr->AttachToModel(0);
            pMan->CleanMoveRoute(n, 1);
            pMan->InitMoveRouteRot(n);
            pChr->SetUpdateMove(1);
            TournamentData::SetIdlePause(pChr, 0.0f);
        }
    }

    void CTournamentRouteMan::_StageDisp(long nParam, int bDisp)
    {
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
        if (pModel != 0) {
            pModel->SetDisp(bDisp);
            pModel->SetUpdateMove(0);
        }
    }

    void CTournamentRouteMan::SetCrystalColor(long nColor)
    {
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nColor);
        if (pModel == 0) {
            return;
        }
        bcs::CMdlObj *pMdl = static_cast<bcs::CMdlObj *>(pModel->GetScnObj());
        if (pMdl != 0) {
            CTrophyObj::SetCrystalColor(pMdl, pModel->GetMdlFile(), CTournament::GetGameMode(), CTournament::GetCup());
            CTrophyObj::SetCrystalEnvTex(pMdl, "crystal_env", LOADDATA_OBJ, CTournament::GetStageGameMode(),
                                         CTournament::GetCup());
        }
    }

    void CTournamentRouteMan::_StageSet(const bcs::FVEC3 &pos, long nParam, float fParam, const char *pAnim,
                                        float fFrame, unsigned long ulFlag, int bDisp)
    {
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
        if (pModel == 0) {
            return;
        }
        bcs::FVEC3 vRot(0.0f, WrapRad(fParam), 0.0f);
        pModel->SetPosition(pos);
        pModel->SetRotation(vRot);
        pModel->SetDisp(bDisp);
        pModel->SetUpdateMove(0);
        pModel->EraseEffect(0, pModel->GetEffMax());
        if (pAnim != 0) {
            pModel->AnimPlay(pAnim, ulFlag, 0.0f, fFrame, 1);
        }
    }

    void CTournamentRouteMan::_StageEff(long nEff, long nParam, float fParam, unsigned long ulParam, long n,
                                        unsigned long ul1, unsigned long ul2)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pModel = pMan->GetObjModel(nParam);
        if (pModel != 0) {
            CEffectCluster *pEffect = pMan->SetEffect(pMan->GetEffectSlot(0xDA, 0x1DA), nEff, n, ul1, ul2);
            if (pEffect != 0) {
                pEffect->SetTranslate(&pModel->GetPos());
            }
        }
    }

    void CTournamentRouteMan::_StageEffAttach(long nEff, const char *pNode, long nParam, float fParam,
                                              unsigned long ulParam, long n, unsigned long ul1, unsigned long ul2)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CEventModel *pModel = pMan->GetObjModel(nParam);
        if (pModel != 0) {
            unsigned long ulNode = pModel->GetNodeId(pNode);
            pMan->AttachEffectModel(pMan->GetEffectSlot(0xDA, 0x1DA), pModel, nEff, n, ulNode, 0, 0, ul1, ul2);
        }
    }

    void CTournamentRouteMan::_StageEffFadeIn(long nEff, float fFade, long nParam, float fParam,
                                              unsigned long ulParam, long n, unsigned long ul1, unsigned long ul2)
    {
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
        if (pModel == 0) {
            return;
        }
        unsigned long hEffect = CEffectCtrl::CreateEffect(nEff, n, ul1, ul2, 0);
        pModel->SetEffectHandle(hEffect, 0);
        if (hEffect != 0) {
            CEffectCluster *pEffect = CEffectCtrl::GetInstance()->FindList(hEffect);
            if (pEffect != 0) {
                pEffect->SetTranslate(&pModel->GetPos());
                if (fFade > 0.0f) {
                    pEffect->SetFadeinTime(fFade);
                }
            }
        }
    }

    void CTournamentRouteMan::_StageEffFadeOut(float fFade, long nParam, float fParam, unsigned long ulParam)
    {
        CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
        if (pModel == 0) {
            return;
        }
        unsigned long hEffect = pModel->GetEffcetHandle(0);
        if (hEffect != 0) {
            CEffectCluster *pEffect = CEffectCtrl::GetInstance()->FindList(hEffect);
            if (pEffect != 0) {
                pEffect->SetFadeoutTime(fFade);
            }
            pModel->StopEffect(0, 1);
        }
    }

    void CTournamentRouteMan::_StageChipFade(long nChip, float fFrom, float fSpeed)
    {
        CTournamentChip *pChip = m_apChip[nChip];
        if (pChip != 0) {
            pChip->SetFade(7, fFrom, fSpeed);
        }
    }

    void CTournamentRouteMan::_StageChipEff(long nEff, long nChip, float fParam, unsigned long ulParam, long n,
                                            unsigned long ul1, unsigned long ul2)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        CTournamentChip *pChip = m_apChip[nChip];
        if (pChip != 0) {
            CEffectCluster *pEffect = pMan->SetEffect(pMan->GetEffectSlot(0xDA, 0x1DA), nEff, n, ul1, ul2);
            if (pEffect != 0) {
                pEffect->SetTranslate(&pChip->GetPos());
            }
        }
    }

    void CTournamentRouteMan::_StageSe(long nSe, long nParam, float fParam, unsigned long ulParam, long nSlot)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        if (pMan->GetObjModel(nParam) != 0) {
            if (nSlot == -1) {
                nSlot = pMan->GetSeSlot(0x19, 0x119);
            }
            if (nSlot != -1) {
                pMan->PlaySe(nSe, (const bcs::FVEC3 *)0, nSlot, 0.0f, 1.0f, 0.0f, 1);
            }
        }
    }

    void CTournamentRouteMan::_StageQuake(const bcs::FVEC3 &power, float fFrame)
    {
        for (long i = 0; i < bcs::CWorldEnv::GetViewSplitNum(); i++) {
            bcs::CCamera *pCamera = bcs::CCameraMan::GetActiveCamera(i);
            if (pCamera != 0) {
                pCamera->StartQuakeDirect(0x80, power, fFrame, 0, 0);
            }
        }
    }

    CRouteBase *CTournamentRouteMan::_CreateRoute(CRouteBase::_PARAM &param, long nType, long nFrom, long nTo)
    {
        if (nType == 3) {
            return new CRouteDokan(param, 1.0f);
        }
        return new CRouteBasicLine(param, 0.17f, 0);
    }

    namespace {

        // (name guessed) Fades map object nParam: at once to fFrom, then to
        // fTo at fSpeed per frame.
        inline void StageFade(long nParam, float fAlpha, float fSpeed)
        {
            CEventModel *pModel = CTournamentMan::GetInstance()->GetObjModel(nParam);
            if (pModel != 0) {
                pModel->SetFade(7, fAlpha, fSpeed);
            }
        }
    }

    void CTournamentRouteMan::_DefTimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();

        switch (nEvent) {
            case 0x3FD:
                m_nUnk08 = nParam;
                break;
            case 0x3F9: // a map object fades out
                StageFade(nParam, 1.0f, 0.0f);
                StageFade(nParam, 0.0f, fParam);
                break;
            case 0x3FA: // ... and in
                StageFade(nParam, 0.0f, 0.0f);
                StageFade(nParam, 1.0f, fParam);
                break;
            case 0x3F8:
                _StageDisp(nParam, 1);
                break;
            case 0x3F7:
                _StageDisp(nParam, 0);
                break;
            case 0x3EC: { // the camera of a route (ulParam)
                CRouteBase *pRoute = reinterpret_cast<CRouteBase *>(ulParam);
                bcs::FVEC3 vDir;
                float fDelay = 0.0f;
                long nCamera = pRoute->GetCameraDirect(vDir, fDelay, nParam, fParam);
                pMan->SetCameraActive(0);
                pMan->SetCameraDirect(vDir, nCamera);
                pMan->SetCameraRateFrame(fParam, -0.001f, 0.99316406f, 0.0007f);
                pMan->SetCameraTarget(0);
                _AddTimer(0x3ED, m_nTimerGroup, fParam + (m_fTimer + fDelay), nParam, fParam, ulParam);
                break;
            }
            case 0x3ED: // the camera of map point nParam
                pMan->SetCameraActive(0);
                pMan->SetCameraPoint(nParam, 0);
                pMan->SetCameraRateFrame(fParam, -0.001f, 0.99316406f, 0.0007f);
                pMan->SetCameraTarget(0);
                break;
            case 0x3EB: // ... following a position (ulParam)
                pMan->SetCameraActive(0);
                pMan->SetCameraPoint(nParam, 0);
                if (ulParam != 0) {
                    pMan->SetCameraRateFrame(fParam, -0.001f, 0.75f, 0.0007f);
                } else {
                    pMan->SetCameraRateFrame(fParam, -0.001f, 0.99780273f, 0.0007f);
                }
                pMan->SetCameraTarget(reinterpret_cast<const bcs::FVEC3 *>(ulParam));
                break;
            case 0x3EE: { // a pipe at map point ulParam opens
                bcs::FVEC3 vPos;
                TournamentWork::GetPointViewPos(vPos, ulParam);
                _StageSet(vPos, nParam, fParam, 0, 0.0f, 1, 0);
                _StageEff(0x54F, nParam, fParam, ulParam, 5, 0, 0);
                _StageEff(0x550, nParam, fParam, ulParam, 3, 0, 0);
                _AddTimer(0x3EF, m_nTimerGroup, 2.0f + m_fTimer, nParam, fParam, ulParam);
                _AddTimer(0x3FA, m_nTimerGroup, 2.0f + m_fTimer, nParam, 0.1f, ulParam);
                _StageSe(0xE27, nParam, fParam, ulParam, -1);
                break;
            }
            case 0x3EF:
                _StageAnim("dokan_st", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                break;
            case 0x3F0: // ... and closes
                _StageAnim("dokan_ed", nParam, fParam, ulParam, 0x3F7, 0, 0.0f, 1, 1);
                _AddTimer(0x3F1, m_nTimerGroup, 20.0f + m_fTimer, nParam, fParam, ulParam);
                break;
            case 0x3F1:
                _AddTimer(0x3F9, m_nTimerGroup, m_fTimer, nParam, 0.1f, ulParam);
                _StageEff(0x551, nParam, fParam, ulParam, 3, 0, 0);
                _StageSe(0xE27, nParam, fParam, ulParam, -1);
                break;
            case 0x3F2:
                _StageSe(0xE26, nParam, fParam, ulParam, -1);
                break;
            case 0x3FE: { // character nParam's voice ulParam
                CEventModel *pChr = pMan->GetChrModel(nParam);
                long nVoice = pChr->GetVoiceId(ulParam);
                if (nVoice != -1) {
                    pChr->PlaySeNoPos(nVoice, 2, 0.0f, 1.0f, 0.0f);
                }
                break;
            }
            case 0x3FF: { // effect ulParam on character nParam
                CEventModel *pChr = pMan->GetChrModel(nParam);
                if (pChr != 0) {
                    CEffectCluster *pEffect = pMan->SetEffect(pMan->GetEffectSlot(0xDA, 0x1DA), ulParam, 3, 0, 0);
                    if (pEffect != 0) {
                        bcs::FVEC3 vPos(pChr->GetPos());
                        pEffect->SetTranslate(&vPos);
                    }
                }
                break;
            }
            case 0x3E8: { // the player markers fade out
                if (!(nParam & 1)) {
                    pMan->PlayerMarkerFead(0.0f, fParam, 0xC8);
                }
                if (!(nParam & 2)) {
                    pMan->PlayerMarkerFead(0.0f, fParam, 0xC9);
                }
                if (!(nParam & 4)) {
                    pMan->PlayerShadowFead(0.0f, fParam, 0);
                }
                TournamentWork::TournamentInfo().nMarkerFlag &= ~ulParam;
                float fTime = 0.0f;
                if (fParam > fTime) {
                    fTime = 1.0f / fParam;
                }
                _AddTimer(0x400, m_nTimerGroup, m_fTimer + fTime, nParam, fParam, ulParam);
                break;
            }
            case 0x400:
                if (!(nParam & 1)) {
                    pMan->PlayerMarkerDisp(0, 0xC8, 0);
                }
                if (!(nParam & 2)) {
                    pMan->PlayerMarkerDisp(0, 0xC9, 0);
                }
                if (!(nParam & 4)) {
                    pMan->PlayerMarkerDisp(0, 0xC8, 0);
                }
                break;
            case 0x3E9: // ... and in
                if (nParam & 1) {
                    pMan->PlayerMarkerDisp(1, 0xC8, 1);
                    pMan->PlayerMarkerFead(1.0f, fParam, 0xC8);
                }
                if (nParam & 2) {
                    pMan->PlayerMarkerDisp(1, 0xC9, 1);
                    pMan->PlayerMarkerFead(1.0f, fParam, 0xC9);
                }
                if (nParam & 4) {
                    pMan->PlayerShadowDisp(1, 0, 1);
                    pMan->PlayerShadowFead(1.0f, fParam, 0);
                }
                TournamentWork::TournamentInfo().nMarkerFlag |= ulParam;
                break;
            case 0x3EA: // their height
                if (nParam & 1) {
                    pMan->PlayerMarkerSetPosY(fParam, 0xC8);
                }
                if (nParam & 2) {
                    pMan->PlayerMarkerSetPosY(fParam, 0xC9);
                }
                TournamentWork::TournamentInfo().fMarkerY = fParam;
                break;
            case 0x3FB: { // the line of a route (the model ulParam) is drawn
                CEventModel *pLine = reinterpret_cast<CEventModel *>(ulParam);
                const char *pName;
                long nEff;
                switch (nParam & 0x3F) {
                    case 1:
                    case 2:
                    case 3:
                        nEff = 0x553;
                        if (CTournament::IsMixTournamentGame()) {
                            nEff = 0x55D;
                        }
                        pName = "line_tate_00";
                        break;
                    case 4:
                        nEff = 0x554;
                        if (CTournament::IsMixTournamentGame()) {
                            nEff = 0x55E;
                        }
                        pName = "line_tate_01";
                        break;
                    case 5:
                        nEff = 0x555;
                        if (CTournament::IsMixTournamentGame()) {
                            nEff = 0x55F;
                        }
                        pName = "line_yoko_00";
                        break;
                    case 6:
                        nEff = 0x556;
                        if (CTournament::IsMixTournamentGame()) {
                            nEff = 0x560;
                        }
                        pName = "line_yoko_01";
                        break;
                    case 7:
                        nEff = 0x557;
                        if (CTournament::IsMixTournamentGame()) {
                            nEff = 0x561;
                        }
                        pName = "line_yoko_02";
                        break;
                    default:
                        pName = 0;
                        nEff = -1;
                        break;
                }
                if (nEff != -1) {
                    pLine->SetDisp(1);
                    pLine->PlaySe(0xE18, 0, 0.0f, 1.0f, 0.0f);
                    char szName[128];
                    if (CTournament::IsMixTournamentGame()) {
                        sprintf(szName, "co_to_mix%s", pName);
                        pLine->AnimPlay(szName, 0x1F, 0.0f, 0.0f, 1);
                    } else {
                        sprintf(szName, "co_to_%s", pName);
                        pLine->AnimPlay(szName, 8, 0.0f, 0.0f, 1);
                        sprintf(szName, "%s%s", s_GameLineName[CTournament::GetGameMode()], pName);
                        pLine->AnimPlay(szName, 2, 0.0f, 0.0f, 1);
                    }
                    CTournamentMan *pTourMan = CTournamentMan::GetInstance();
                    CEffectCluster *pEffect = pTourMan->SetEffect(pTourMan->GetEffectSlot(0xDA, 0x1DA), nEff, 3, 0, 0);
                    if (pEffect != 0) {
                        bcs::FVEC3 vPos(pLine->GetPos());
                        vPos.y += 0.005f;
                        pEffect->SetTranslate(&vPos);
                        pEffect->SetRotateRad(&pLine->GetRot());
                        if (!CTournament::IsMixTournamentGame()) {
                            pEffect->SetPointLightColor(s_LineColor[CTournament::GetGameMode()],
                                                        s_LineColor2[CTournament::GetGameMode()]);
                        }
                    }
                }
                break;
            }
            case 0x3FC: // ... and hidden
                reinterpret_cast<CEventModel *>(ulParam)->SetDisp(0);
                break;
        }
    }

    CEventModel *CTournamentRouteMan::CreateWinRoute(float &fFrame, long nType, long nLayer)
    {
        CEvDemoMdlObj *pModel = 0;
        CEvDemoMdlObj::_SETUP setup(1);
        setup.nDrawLayer = nLayer;
        fFrame = 0.0f;
        switch (nType) {
            case 2:
            case 3:
                fFrame = 24.0f;
            case 1:
                if (CTournament::IsMixTournamentGame()) {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_mixline_tate_00_rmdl.brres",
                                                              "co_to_mixline_tate_00");
                } else {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_line_tate_00_rmdl.brres",
                                                              "co_to_line_tate_00");
                }
                break;
            case 4:
                fFrame = 16.0f;
                if (CTournament::IsMixTournamentGame()) {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_mixline_tate_01_rmdl.brres",
                                                              "co_to_mixline_tate_01");
                } else {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_line_tate_01_rmdl.brres",
                                                              "co_to_line_tate_01");
                }
                break;
            case 5:
                fFrame = 15.0f;
                if (CTournament::IsMixTournamentGame()) {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_mixline_yoko_00_rmdl.brres",
                                                              "co_to_mixline_yoko_00");
                } else {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_line_yoko_00_rmdl.brres",
                                                              "co_to_line_yoko_00");
                }
                break;
            case 6:
                if (CTournament::IsMixTournamentGame()) {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_mixline_yoko_01_rmdl.brres",
                                                              "co_to_mixline_yoko_01");
                } else {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_line_yoko_01_rmdl.brres",
                                                              "co_to_line_yoko_01");
                }
                break;
            case 7:
                if (CTournament::IsMixTournamentGame()) {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_mixline_yoko_02_rmdl.brres",
                                                              "co_to_mixline_yoko_02");
                } else {
                    pModel = CEvDemoMdlObj::CreateModelEffRes(setup, "co_to_line_yoko_02_rmdl.brres",
                                                              "co_to_line_yoko_02");
                }
                break;
        }
        if (pModel != 0) {
            pModel->SetDrawPriority(1);
        }
        return pModel;
    }

    void CTournamentRouteMan::_DefPushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel)
    {
        unsigned long ulTarget = 0;
        switch (pInfo->nType) {
            case 3: {
                // A camera that follows the route.
                CRouteBase *pRoute = pInfo->pRoute;
                ulTarget = reinterpret_cast<unsigned long>(pRoute);
                bcs::FVEC3 vRot;
                pRoute->GetResultRot(vRot, 0.0f, pInfo->nFlag & 1);
                _PushCameraTimer();
                m_nUnk584 = 0x3EC;
                m_nCameraId = n;
                m_nCameraParam = pInfo->nEnd;
                m_fCameraDir = vRot.y;
                m_fCameraTime = f;
                m_fCameraFrame = 0.5f * pInfo->fFrame;
                m_ulCameraParam = ulTarget;
                break;
            }
            case 2:
                // A camera that follows the model.
                ulTarget = reinterpret_cast<unsigned long>(&pModel->GetPos());
            case 1: {
                bcs::FVEC3 vRot;
                pInfo->pRoute->GetResultRot(vRot, 0.0f, pInfo->nFlag & 1);
                if (m_nUnk584 == 0x3EB && m_ulCameraParam == ulTarget &&
                    __fabsf(vRot.y - m_fCameraDir) < 0.001f) {
                    // The same direction: the camera move goes on.
                    m_nCameraParam = pInfo->nEnd;
                    m_ulCameraParam = ulTarget;
                    m_fCameraFrame += pInfo->fFrame;
                    break;
                }
                _PushCameraTimer();
                m_nUnk584 = 0x3EB;
                m_nCameraId = n;
                m_nCameraParam = pInfo->nEnd;
                m_fCameraDir = vRot.y;
                m_fCameraFrame = pInfo->fFrame;
                m_fCameraTime = f;
                m_ulCameraParam = ulTarget;
                break;
            }
        }
    }

    CRouteMove::CRouteMove(CEventModel *pModel, int b) : CEventMove(pModel, 0, b)
    {
        m_fRouteFrame = 0.0f;
        m_nRouteNum = 0;
        m_apAction[0] = 0;
        m_apAction[1] = 0;
        m_apAction[2] = 0;
        m_apAction[3] = 0;
        m_apAction[0] = new CEvMoveActTmNormal(pModel);
        m_apAction[1] = new CEvMoveActTmJump(pModel);
        m_apAction[3] = new CEvMoveActTmBigGun(pModel);
    }

    CRouteMove::~CRouteMove()
    {
        for (long i = 0; i < 4; i++) {
            if (m_apAction[i] != 0) {
                delete m_apAction[i];
            }
            m_apAction[i] = 0;
        }
        for (long i = 0; i < m_nRouteNum; i++) {
            ROUTE &route = m_aRoute[i];
            if (route.bOwn && route.pRoute != 0) {
                if (route.pRoute != 0) {
                    delete route.pRoute;
                }
                route.pRoute = 0;
                route.bOwn = 0;
            }
        }
        m_nRouteNum = 0;
    }

    void CEventMoveAction::SetFrame(float fFrame)
    {
        m_fFrame = fFrame;
    }

    int CRouteMove::GetFrame(float &fStart, float &fEnd, CRouteBase *pRoute, unsigned long ulFlag, float f1, float f2)
    {
        float fNum = pRoute->GetNumFrame();
        int bReverse = ulFlag & 1;
        if (0.0f == f1 && 0.0f == f2) {
            fStart = 0.0f;
            fEnd = fNum;
        } else if (bReverse) {
            fStart = fNum - pRoute->GetOffsetFrame(f2, fNum, -1.0f, 0.0001f);
            if (ulFlag & 8) {
                fEnd = fNum - pRoute->GetOffsetFrame(f1, fNum, -1.0f, 0.0001f);
            } else {
                fEnd = fNum - pRoute->GetOffsetFrame(f1, 0.0f, 1.0f, 0.0001f);
            }
        } else {
            fStart = pRoute->GetOffsetFrame(f2, 0.0f, 1.0f, 0.0001f);
            if (ulFlag & 8) {
                fEnd = pRoute->GetOffsetFrame(f1, 0.0f, 1.0f, 0.0001f);
            } else {
                fEnd = pRoute->GetOffsetFrame(f1, fNum, -1.0f, 0.0001f);
            }
        }
        return bReverse;
    }

    float CRouteMove::PushRoute(CRouteBase *pRoute, unsigned long ulFlag, float f1, float f2)
    {
        float fStart;
        float fEnd;
        int bReverse = GetFrame(fStart, fEnd, pRoute, ulFlag, f1, f2);
        float fFrame = 0.0f;
        long nNum = m_nRouteNum;
        if (nNum > 0) {
            CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
            ROUTE &last = m_aRoute[nNum - 1];
            if (last.bOpen) {
                if (ulFlag & 4) {
                    // Takes the place of the open route.
                    if (last.bOwn) {
                        if (last.pRoute != 0) {
                            delete last.pRoute;
                        }
                        last.pRoute = 0;
                    }
                    last.bOpen = 0;
                    last.pRoute = pRoute;
                    last.bOwn = (ulFlag >> 1) & 1;
                    last.bReverse = bReverse;
                    last.fStart = fStart;
                    last.fEnd = fEnd;
                    last.fFrame = fEnd - fStart;
                    m_fRouteFrame = last.fFrame;
                    return fFrame;
                }

                // The open route runs to its end (a walk that was a jump
                // starts its timers now).
                if (last.bOpenEnd) {
                    last.bOpen = 0;
                    last.bReverse = !bReverse;
                    float fFrom = last.pRoute->GetNumFrame() - last.fEnd;
                    last.fStart = fFrom;
                    last.fEnd = last.pRoute->GetNumFrame();
                    last.fFrame = last.fEnd - fFrom;
                    if (!(ulFlag & 0x10) && last.fFrame > 0.0f && last.pRoute->GetNoJump()) {
                        long nIndex = m_pModel->GetIndex();
                        long nId = last.pRoute->GetRouteId();
                        pMan->PushTimer(0x3F4, GetMoveEventId(nId, 0), 0.0f, 0, 0.0f, nIndex);
                        last.pRoute->SetNoJump(0);
                    }
                } else {
                    last.bOpen = 0;
                    float fFrom = last.fEnd;
                    last.fStart = fFrom;
                    last.fEnd = last.pRoute->GetNumFrame();
                    last.fFrame = last.fEnd - fFrom;
                    if (!(ulFlag & 0x10) && last.fFrame > 0.0f && last.pRoute->GetNoJump()) {
                        long nIndex = m_pModel->GetIndex();
                        long nId = last.pRoute->GetRouteId();
                        pMan->PushTimer(0x3F4, GetMoveEventId(nId, 0), 0.0f, 0, 0.0f, nIndex);
                        last.pRoute->SetNoJump(0);
                    }
                }
                fFrame = last.fFrame;
                m_fRouteFrame = fFrame;
            }
        }

        ROUTE &route = m_aRoute[m_nRouteNum];
        route.bOpen = 0;
        route.bOpenEnd = (ulFlag >> 3) & 1;
        route.bOwn = (ulFlag >> 1) & 1;
        route.pRoute = pRoute;
        route.bReverse = bReverse;
        route.fStart = fStart;
        route.fEnd = fEnd;
        route.fFrame = fEnd - fStart;
        m_nRouteNum++;
        m_fRouteFrame += route.fFrame;
        return fFrame;
    }

    void CRouteMove::CleanRoute(float fFade)
    {
        long nNum = m_nRouteNum;
        if (nNum > 0) {
            m_nRouteNum = 0;
            ROUTE &last = m_aRoute[nNum - 1];
            CRouteBase *pRoute = last.pRoute;
            if (pRoute != 0) {
                float fEnd = last.fEnd;
                if (__fabsf(fEnd - pRoute->GetNumFrame()) > fFade) {
                    // The last route stays, open, as the first.
                    ROUTE keep;
                    keep.pRoute = pRoute;
                    keep.fStart = fEnd;
                    keep.fEnd = fEnd;
                    keep.fFrame = 0.0f;
                    keep.bReverse = last.bReverse;
                    keep.bOwn = last.bOwn;
                    keep.bOpen = 1;
                    keep.bOpenEnd = last.bOpenEnd;
                    last = m_aRoute[0];
                    m_aRoute[0] = keep;
                    m_nRouteNum++;
                }
            }
            for (long i = m_nRouteNum; i < nNum; i++) {
                ROUTE &route = m_aRoute[i];
                if (route.bOwn && route.pRoute != 0) {
                    if (route.pRoute != 0) {
                        delete route.pRoute;
                    }
                    route.pRoute = 0;
                    route.bOwn = 0;
                }
            }
        }
        m_fRouteFrame = 0.0f;
    }

    float CRouteMove::_SetStartFrame(long &nIndex, float fFrame)
    {
        if (fFrame <= 0.0f || m_nRouteNum <= 0) {
            nIndex = 0;
            return fFrame;
        }
        for (long i = 0; i < m_nRouteNum; i++) {
            ROUTE &route = m_aRoute[i];
            if (fFrame < route.fFrame) {
                nIndex = i;
                return fFrame;
            }
            fFrame -= route.fFrame;
        }
        nIndex = m_nRouteNum - 1;
        ROUTE &last = m_aRoute[nIndex];
        float fLast = last.fFrame;
        return fFrame + fLast;
    }

    float CRouteMove::_GetResult(CRouteBase::_RESULT &result, float fFrame, long nIndex, float fStart)
    {
        result.nAction = -1;
        if (m_nRouteNum <= 0) {
            return fFrame;
        }

        float fNow = fStart + fFrame;
        ROUTE &first = m_aRoute[nIndex];
        if (fNow <= 0.0f) {
            result.pRoute = first.pRoute;
            result.fFrame = first.fStart;
            first.pRoute->GetResult(result, first.fStart, first.bReverse);
            return fFrame;
        }
        float fRouteFrame = first.fStart + fNow;
        if (fRouteFrame > first.fEnd) {
            fRouteFrame = first.fEnd;
        }
        fNow -= first.fFrame;
        if (fNow < 0.0f) {
            result.pRoute = first.pRoute;
            result.fFrame = fRouteFrame;
            first.pRoute->GetResult(result, fRouteFrame, first.bReverse);
            return fFrame;
        }
        for (long i = nIndex + 1; i < m_nRouteNum; i++) {
            ROUTE &route = m_aRoute[i];
            fRouteFrame = route.fStart + fNow;
            if (fRouteFrame > route.fEnd) {
                fRouteFrame = route.fEnd;
            }
            fNow -= route.fFrame;
            if (fNow < 0.0f) {
                result.pRoute = route.pRoute;
                result.fFrame = fRouteFrame;
                route.pRoute->GetResult(result, fRouteFrame, route.bReverse);
                return fFrame;
            }
        }
        ROUTE &last = m_aRoute[m_nRouteNum - 1];
        result.pRoute = last.pRoute;
        result.fFrame = last.fEnd;
        last.pRoute->GetResult(result, last.fEnd, last.bReverse);
        return fFrame;
    }

    void CRouteMove::SetupMove(float fSpeed, int b)
    {
        m_pModel->SetValidMove(1);
        for (long i = 0; i < 4; i++) {
            CEventMoveAction *pAction = m_apAction[i];
            if (pAction != 0) {
                pAction->m_pRoute = 0;
                pAction->m_pPrevRoute = 0;
            }
        }
        m_fSpeed = fSpeed;
        m_fEndFrame = m_fRouteFrame / fSpeed;
        if (b) {
            CTournamentRouteMan::GetInstance()->_PushCameraTimer();
        }
        CTournamentRouteMan::GetInstance()->m_nUnk584 = 0;
    }

    void CRouteMove::SetFrame(float fFrame)
    {
        if (m_pModel->IsValidMove()) {
            if (fFrame < 0.0f) {
                fFrame = m_fEndFrame;
            }
            m_fFrame = fFrame;
            UpdateFrame(0.0f);
        }
    }

    void CRouteMove::UpdateFrame(float fFrame)
    {
        float fSpeed = m_fSpeed;
        CRouteBase::_RESULT result;
        result.vPos = m_pModel->GetPos();
        float fMove = fFrame * fSpeed;
        result.vRot = m_pModel->GetRot();
        result.vScale = m_pModel->GetScale();
        result.fFrame = 0.0f;
        result.pRoute = 0;
        long nIndex;
        float fNow = _GetResult(result, fMove, nIndex, _SetStartFrame(nIndex, m_fFrame * fSpeed));

        // Keep where the model was, and move it.
        m_vPos = m_pModel->GetPos();
        m_vRot = m_pModel->GetRot();
        m_vScale = m_pModel->GetScale();
        m_pModel->SetPosition(result.vPos);
        m_pModel->SetRotation(result.vRot);
        m_pModel->SetScale(result.vScale);
        if (result.nAction != -1) {
            CEventMoveAction *pAction = m_apAction[result.nAction];
            if (pAction->m_pModel->IsDisp()) {
                pAction->m_pRoute = result.pRoute;
            }
            pAction->SetFrame(result.fFrame);
            bcs::FVEC3 vMove;
            bcs::FVEC3 vTurn;
            FVEC3Diff(vMove, result.vPos, m_vPos);
            FVEC3Diff(vTurn, result.vRot, m_vRot);
            pAction->UpdateFrame(vMove, vTurn, fMove);
        }

        float fAdd = fNow / m_fSpeed;
        if (fAdd <= 0.0f) {
            return;
        }
        m_fFrame += fAdd;

        // Run the timers of the route the model is on now.
        long nChr = m_pModel->GetIndex();
        CTournamentRouteMan *pMan = CTournamentRouteMan::GetInstance();
        CRouteBase *pNow = pMan->m_apNowRoute[nChr];
        if (pNow != result.pRoute) {
            if (pNow != 0 && pNow->GetRouteId() != -1) {
                pMan->_PopTimer(GetMoveEventId(pNow->GetRouteId(), 1), pMan->m_fTimer - result.fFrame);
            }
            pMan->m_apNowRoute[nChr] = result.pRoute;
            if (result.pRoute != 0 && result.pRoute->GetRouteId() != -1) {
                pMan->_PopTimer(GetMoveEventId(result.pRoute->GetRouteId(), 0), pMan->m_fTimer - result.fFrame);
            }
        }
    }

    void CEventMoveAction::UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame)
    {
        m_fFrame += fFrame;
    }

    long CRouteMove::GetMoveEventId(long nRouteId, long n) const
    {
        return nRouteId + (m_pModel->GetIndex() << 11) + n * 0xA000 + 0x400;
    }

    long CRouteMove::GetMoveFirstEventId() const
    {
        return m_pModel->GetIndex() + 0x3D8;
    }

    long CRouteMove::GetMoveEndEventId() const
    {
        return m_pModel->GetIndex() + 0x3EC;
    }

    CRouteDataMove::~CRouteDataMove()
    {
    }

    void CRoutePhysOffsetY::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        rot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    void CRoutePhysOffsetY::GetResultPos(bcs::FVEC3 &pos, float frame, int b)
    {
        if (frame <= 0.0f) {
            pos = m_vStart;
        } else if (frame >= m_fNumFrame) {
            pos = m_vEnd;
        } else {
            bcs::FVEC3 vSpeed;
            bcs::FVEC3 vGravity;
            bcs::FVEC3Scale(&vSpeed, &m_vSpeed, frame);
            bcs::FVEC3Scale(&vGravity, &m_vGravity, 0.5f * (frame * frame));
            FVEC3Plus(pos, vSpeed, vGravity);
            bcs::FVEC3Add(&pos, &pos, &m_vStart);
        }
    }

    CRoutePhysOffsetY::~CRoutePhysOffsetY()
    {
    }

    float CRouteBasicJump::GetRate() const
    {
        return m_fNumFrame;
    }

    void CRouteBasicJump::SetNoJump(int bNoJump)
    {
        m_bNoJump = bNoJump;
    }

    int CRouteBasicJump::GetNoJump() const
    {
        return m_bNoJump;
    }

    void CRouteBasicJump::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        rot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    void CRouteBasicJump::GetResultPos(bcs::FVEC3 &pos, float frame, int b)
    {
        bcs::FVEC3 vScale;
        _SetTranslate(pos, vScale, frame, b);
    }

    CRouteBasicJump::~CRouteBasicJump()
    {
        if (m_pOffsetRoute != 0) {
            delete m_pOffsetRoute;
        }
        m_pOffsetRoute = 0;
    }

    void CRouteBasicLine::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        rot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    void CRouteBasicLine::GetResultPos(bcs::FVEC3 &pos, float frame, int b)
    {
        if (b) {
            frame = m_fNumFrame - frame;
        }
        float fMove = frame * m_fRate;
        if (fMove <= 0.0f) {
            pos = m_vStart;
        } else if (fMove >= m_fNumFrame) {
            pos = m_vEnd;
        } else {
            bcs::BCS_VecZ_RotY(pos, fMove, m_fRotY);
            bcs::FVEC3Add(&pos, &pos, &m_vStart);
        }
    }

    void CRouteWait::GetResultRot(bcs::FVEC3 &rot, float frame, int b)
    {
        rot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    void CRouteWait::GetResultPos(bcs::FVEC3 &pos, float frame, int b)
    {
        pos = m_vStart;
    }

    void CRouteWait::GetResult(_RESULT &result, float frame, int b)
    {
        result.vPos = m_vStart;
        result.nAction = m_nAction;
        result.vRot.Set(0.0f, GetRotY(m_fRotY, b), 0.0f);
    }

    CRouteWait::~CRouteWait()
    {
    }
}
