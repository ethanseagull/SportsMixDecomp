#include "bcs/CScnGroup.hpp"
#include "bcs/CHandleTable.hpp"
#include "bcs/CScnObjGather.hpp"
#include "bcs/CFrustum.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/New.hpp"
#include "FastCast.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"
#include <algorithm>

namespace bcs {

    // bcs::CG3dObj

    CG3dObj::~CG3dObj()
    {
        DetachFromParent();
    }

    CG3dObj *CG3dObj::DetachFromParent()
    {
        CG3dObj *pParent = m_pParent;

        if (pParent != 0) {
            pParent->ObjProc(0x10001, 0, this);
        }
        return pParent;
    }

    // bcs::CScnObj

    CHandleTable *CScnObj::s_pHandleTable;
    unsigned long *CScnObj::s_pDelayDeleteHandle;
    unsigned long CScnObj::s_nDelayDeleteNum;

    void CScnObj::Initialize()
    {
        s_pHandleTable = new (0x100) CHandleTable;
        s_pHandleTable->Init(0x1000, 0, 0x20);
        s_pDelayDeleteHandle = new (1) unsigned long[0x1000];
        s_nDelayDeleteNum = 0;
    }

    CScnObj *CScnObj::GetScnObj(unsigned long handle)
    {
        return static_cast<CScnObj *>(s_pHandleTable->Get(handle));
    }

    void CScnObj::AppendDelayDeleteObject(CScnObj *pObj)
    {
        if (pObj != 0) {
            pObj->NotifyAppendDelayDelete();
            pObj->DetachFromParent();
            s_pDelayDeleteHandle[s_nDelayDeleteNum] = pObj->m_nHandle;
            s_nDelayDeleteNum++;
        }
    }

    // (name guessed) Deletes pObj if there is one. The original tests the
    // pointer once more than `delete` itself does, so the test is taken to
    // be in a small helper like this one.
    static inline void SafeDelete(CScnObj *pObj)
    {
        if (pObj != 0) {
            delete pObj;
        }
    }

    void CScnObj::ExecDelayDeleteObject(int bAll)
    {
        if (s_pDelayDeleteHandle == 0) {
            return;
        }

        if (bAll) {
            for (unsigned long i = 0; i < s_nDelayDeleteNum; i++) {
                CScnObj *pObj = GetScnObj(s_pDelayDeleteHandle[i]);
                s_pDelayDeleteHandle[i] = 0;
                if (pObj != 0) {
                    SafeDelete(pObj);
                }
            }
            s_nDelayDeleteNum = 0;
        } else {
            unsigned long nKeep = 0;
            for (unsigned long i = 0; i < s_nDelayDeleteNum; i++) {
                CScnObj *pObj = GetScnObj(s_pDelayDeleteHandle[i]);
                s_pDelayDeleteHandle[i] = 0;
                if (pObj != 0) {
                    if (pObj->m_nFlag & 0x300000) {
                        s_pDelayDeleteHandle[nKeep] = pObj->m_nHandle;
                        nKeep++;
                    } else {
                        SafeDelete(pObj);
                    }
                }
            }
            s_nDelayDeleteNum = nKeep;
        }
    }

    CScnObj::CScnObj()
        : m_nAttachHandle(0)
        , m_nAttachNode(0)
        , m_nAttachMode(0)
        , m_nBBoxZSortCalcType(0)
        , m_UnkE8(0)
        , m_UnkEC(0)
        , m_UnkEE(0)
        , m_pJointObj(0)
        , m_UnkF4(0)
        , m_UnkF6(0)
        , m_nHandle(0)
        , m_nFlag(0)
        , m_nDrawPriorityOpa(0x80)
        , m_nDrawPriorityXlu(0x80)
        , m_nAlphaRate(0xFF)
        , m_nAlphaMode(0)
        , m_fSortValue(0.0f)
        , m_pGroupPrev(0)
        , m_pGroupNext(0)
        , m_pGatherOpaNext(0)
        , m_pGatherXluNext(0)
    {
        m_nHandle = s_pHandleTable->Register(this);
        PSMTXIdentity(m_Mtx[SCNOBJMTXTYPE_ROOT].m);
        PSMTXIdentity(m_Mtx[SCNOBJMTXTYPE_1].m);
        PSMTXIdentity(m_Mtx[SCNOBJMTXTYPE_VIEW].m);
        m_vTrans.Set(0.0f, 0.0f, 0.0f);
        SetRot(0.0f, 0.0f, 0.0f);
        m_vTransOffset.Set(0.0f, 0.0f, 0.0f);
        m_vScale.z = 1.0f;
        m_vScale.y = 1.0f;
        m_vScale.x = 1.0f;
        OnFlag(0x20000);
        m_BBox.max.x = 0.0f;
        m_BBox.max.y = 0.0f;
        m_BBox.max.z = 0.0f;
        m_BBox.min.x = 0.0f;
        m_BBox.min.y = 0.0f;
        m_BBox.min.z = 0.0f;
    }

    void CScnObj::SetFlag(unsigned long flag, int bOn)
    {
        if (bOn) {
            OnFlag(flag);
        } else {
            OffFlag(flag);
        }
    }

    CScnObj::~CScnObj()
    {
        s_pHandleTable->Unregister(m_nHandle);
    }

    int CScnObj::ForEach(ForEachFunc pFunc, void *pArg, int)
    {
        return pFunc(this, pArg) == FOREACHRESULT_1;
    }

    void CScnObj::SetMtx(ScnObjMtxType type, const FMTX34 *pMtx)
    {
        if (pMtx != 0) {
            PSMTXCopy(pMtx->m, m_Mtx[type].m);
        } else {
            PSMTXIdentity(m_Mtx[type].m);
        }
    }

    void CScnObj::SetMtx(ScnObjMtxType type, const FMTX34 &rMtx)
    {
        PSMTXCopy(rMtx.m, m_Mtx[type].m);
    }

    void CScnObj::GetMtx(ScnObjMtxType type, FMTX34 *pMtx) const
    {
        PSMTXCopy(m_Mtx[type].m, pMtx->m);
    }

    void CScnObj::GetMtx(ScnObjMtxType type, FMTX34 &rMtx) const
    {
        PSMTXCopy(m_Mtx[type].m, rMtx.m);
    }

    void CScnObj::GetNodeLocalMtx(unsigned long, FMTX34 &rMtx) const
    {
        PSMTXCopy(m_Mtx[SCNOBJMTXTYPE_ROOT].m, rMtx.m);
    }

    const FMTX34 *CScnObj::GetNodeLocalMtxPtr(unsigned long) const
    {
        return &m_Mtx[SCNOBJMTXTYPE_ROOT];
    }

    void CScnObj::GetNodeWorldMtx(unsigned long, FMTX34 &rMtx) const
    {
        PSMTXCopy(m_Mtx[SCNOBJMTXTYPE_1].m, rMtx.m);
    }

    const FMTX34 *CScnObj::GetNodeWorldMtxPtr(unsigned long) const
    {
        return &m_Mtx[SCNOBJMTXTYPE_1];
    }

    void CScnObj::SetNodeLocalMtx(unsigned long, const FMTX34 &rMtx)
    {
        PSMTXCopy(rMtx.m, m_Mtx[SCNOBJMTXTYPE_ROOT].m);
    }

    void CScnObj::SetNodeWorldMtx(unsigned long, const FMTX34 &rMtx)
    {
        PSMTXCopy(rMtx.m, m_Mtx[SCNOBJMTXTYPE_1].m);
    }

    float CScnObj::GetValueForSortOpa() const
    {
        return -m_fSortValue;
    }

    float CScnObj::GetValueForSortXlu() const
    {
        return m_fSortValue;
    }

    // (name guessed) n limited to [0, 255].
    static inline long ClampU8(long n)
    {
        if (n > 0xFF) {
            return 0xFF;
        }
        return n < 0 ? 0 : n;
    }

    void CScnObj::SetDrawPriority(long nOpa, long nXlu)
    {
        if (nOpa >= 0) {
            SetDrawPriorityOpa(nOpa);
        }
        if (nXlu >= 0) {
            SetDrawPriorityXlu(nXlu);
        }
    }

    void CScnObj::SetDrawPriorityOpa(long nOpa)
    {
        m_nDrawPriorityOpa = ClampU8(nOpa);
    }

    void CScnObj::SetDrawPriorityXlu(long nXlu)
    {
        m_nDrawPriorityXlu = ClampU8(nXlu);
    }

    void CScnObj::GetBoundingBox(long, nw4r::math::AABB *pBox)
    {
        if (m_nFlag & 0x20000) {
            CalcBoundingVolume();
            OffFlag(0x20000);
        }
        *pBox = m_BBox;
    }

    void CScnObj::GetBoundingBox(nw4r::math::AABB *pBox)
    {
        if (m_nFlag & 0x20000) {
            CalcBoundingVolume();
            OffFlag(0x20000);
        }
        *pBox = m_BBox;
    }

    void CScnObj::CalcBoundingVolume()
    {
    }

    // (name guessed) fRad brought into [0, 2 pi].
    static inline float WrapRad(float fRad)
    {
        while (fRad > 6.2831855f) {
            fRad -= 6.2831855f;
        }
        while (fRad < 0.0f) {
            fRad += 6.2831855f;
        }
        return fRad;
    }

    int CScnObj::RotateYTo(float fTarget, float fStep)
    {
        float fDiff = WrapRad(fTarget - m_fRotY);

        if (fDiff <= fStep || 6.2831855f - fDiff <= fStep) {
            SetRotY(fTarget);
            return 1;
        }
        SetRotY(fDiff >= 3.1415927f ? m_fRotY - fStep : m_fRotY + fStep);
        return 0;
    }

    void CScnObj::RotateYRate(float fTarget, float fRate)
    {
        float fDiff = WrapRad(fTarget - m_fRotY);

        if (fDiff >= 3.1415927f) {
            fDiff -= 6.2831855f;
        }

        float fStep = fDiff * fRate;
        if (fDiff < 0.0f) {
            fDiff = fStep < fDiff ? fDiff : fStep;
        } else {
            fDiff = fStep > fDiff ? fDiff : fStep;
        }
        SetRotY(m_fRotY + fDiff);
    }

    void CScnObj::CalcRootMtx()
    {
        nw4r::math::MTX34RotXYZRad(reinterpret_cast<nw4r::math::MTX34 *>(&m_Mtx[SCNOBJMTXTYPE_ROOT]), m_fRotX, m_fRotY,
                                   m_fRotZ);
        m_Mtx[SCNOBJMTXTYPE_ROOT].m[0][3] = m_vTrans.x + m_vTransOffset.x;
        m_Mtx[SCNOBJMTXTYPE_ROOT].m[1][3] = m_vTrans.y + m_vTransOffset.y;
        m_Mtx[SCNOBJMTXTYPE_ROOT].m[2][3] = m_vTrans.z + m_vTransOffset.z;
    }

    const FMTX34 *CScnObj::GetAttachMtx()
    {
        if (m_nAttachHandle == 0) {
            return 0;
        }

        CScnObj *pObj = GetScnObj(m_nAttachHandle);
        if (pObj == 0) {
            AttachToObject(0, 0, ATTACHMODE_0);
            return 0;
        }
        return pObj->GetNodeWorldMtxPtr(m_nAttachNode);
    }

    void CScnObj::CalcWorldMtx(const FMTX34 *pParentMtx)
    {
        const FMTX34 *pAttachMtx = GetAttachMtx();

        if (pAttachMtx != 0) {
            switch (m_nAttachMode) {
                case ATTACHMODE_1: {
                    // Follows the position only.
                    FVEC3 vWorld;
                    FVEC3 vPos(m_Mtx[SCNOBJMTXTYPE_ROOT].m[0][3], m_Mtx[SCNOBJMTXTYPE_ROOT].m[1][3],
                               m_Mtx[SCNOBJMTXTYPE_ROOT].m[2][3]);
                    PSMTXMultVec(pAttachMtx->m, reinterpret_cast<Vec *>(&vPos), reinterpret_cast<Vec *>(&vWorld));
                    PSMTXCopy(m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_1].m);
                    m_Mtx[SCNOBJMTXTYPE_1].m[0][3] = vWorld.x;
                    m_Mtx[SCNOBJMTXTYPE_1].m[1][3] = vWorld.y;
                    m_Mtx[SCNOBJMTXTYPE_1].m[2][3] = vWorld.z;
                    break;
                }
                case ATTACHMODE_3: {
                    // Follows the position in X and Z only.
                    FVEC3 vWorld;
                    FVEC3 vPos(m_Mtx[SCNOBJMTXTYPE_ROOT].m[0][3], m_Mtx[SCNOBJMTXTYPE_ROOT].m[1][3],
                               m_Mtx[SCNOBJMTXTYPE_ROOT].m[2][3]);
                    PSMTXMultVec(pAttachMtx->m, reinterpret_cast<Vec *>(&vPos), reinterpret_cast<Vec *>(&vWorld));
                    PSMTXCopy(m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_1].m);
                    m_Mtx[SCNOBJMTXTYPE_1].m[0][3] = vWorld.x;
                    m_Mtx[SCNOBJMTXTYPE_1].m[2][3] = vWorld.z;
                    break;
                }
                case ATTACHMODE_2:
                    // Follows the rotation only.
                    PSMTXConcat(pAttachMtx->m, m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_1].m);
                    m_Mtx[SCNOBJMTXTYPE_1].m[0][3] = m_Mtx[SCNOBJMTXTYPE_ROOT].m[0][3];
                    m_Mtx[SCNOBJMTXTYPE_1].m[1][3] = m_Mtx[SCNOBJMTXTYPE_ROOT].m[1][3];
                    m_Mtx[SCNOBJMTXTYPE_1].m[2][3] = m_Mtx[SCNOBJMTXTYPE_ROOT].m[2][3];
                    break;
                default:
                    PSMTXConcat(pAttachMtx->m, m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_1].m);
                    break;
            }
        } else if (pParentMtx != 0) {
            PSMTXConcat(pParentMtx->m, m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_1].m);
        } else {
            PSMTXCopy(m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_1].m);
        }

        nw4r::math::MTX34Scale(reinterpret_cast<nw4r::math::MTX34 *>(&m_Mtx[SCNOBJMTXTYPE_1]),
                               reinterpret_cast<nw4r::math::MTX34 *>(&m_Mtx[SCNOBJMTXTYPE_1]),
                               reinterpret_cast<nw4r::math::VEC3 *>(&m_vScale));
        if (TestFlag(0x10000)) {
            OnFlag(0x20000);
        }
    }

    void CScnObj::CalcViewMtx(const FMTX34 *pViewMtx)
    {
        PSMTXConcat(pViewMtx->m, m_Mtx[SCNOBJMTXTYPE_1].m, m_Mtx[SCNOBJMTXTYPE_VIEW].m);
        CalcSortValue(pViewMtx);
    }

    void CScnObj::CalcSortValue(const FMTX34 *pViewMtx)
    {
        if (TestFlag(0x80000)) {
            return;
        }

        if (TestFlag(0x40000) && TestFlag(0x10000)) {
            switch (m_nBBoxZSortCalcType) {
                case 1:
                case 2: {
                    const nw4r::math::VEC3 &rMin = m_BBox.min;
                    const nw4r::math::VEC3 &rMax = m_BBox.max;
                    FVEC3 vCorner[8];

                    vCorner[0].Set(rMin.x, rMin.y, rMin.z);
                    vCorner[1].Set(rMax.x, rMin.y, rMin.z);
                    vCorner[2].Set(rMax.x, rMin.y, rMax.z);
                    vCorner[3].Set(rMin.x, rMin.y, rMax.z);
                    vCorner[4].Set(rMin.x, rMax.y, rMin.z);
                    vCorner[5].Set(rMax.x, rMax.y, rMin.z);
                    vCorner[6].Set(rMax.x, rMax.y, rMax.z);
                    vCorner[7].Set(rMin.x, rMax.y, rMax.z);
                    PSMTXMultVecArray(pViewMtx->m, reinterpret_cast<Vec *>(vCorner), reinterpret_cast<Vec *>(vCorner), 8);

                    // The nearest corner for type 1, the farthest for 2.
                    m_fSortValue = vCorner[0].z;
                    switch (m_nBBoxZSortCalcType) {
                        case 1:
                            for (int i = 1; i < 8; i++) {
                                if (m_fSortValue > vCorner[i].z) {
                                    m_fSortValue = vCorner[i].z;
                                }
                            }
                            break;
                        default:
                            for (int i = 1; i < 8; i++) {
                                if (m_fSortValue < vCorner[i].z) {
                                    m_fSortValue = vCorner[i].z;
                                }
                            }
                            break;
                    }
                    break;
                }
                default: {
                    FVEC3 vCenter;
                    FVEC3 vMin(m_BBox.min.x, m_BBox.min.y, m_BBox.min.z);
                    FVEC3 vMax(m_BBox.max.x, m_BBox.max.y, m_BBox.max.z);

                    FVEC3Add(&vCenter, &vMin, &vMax);
                    FVEC3Scale(&vCenter, &vCenter, 0.5f);
                    PSMTXMultVec(pViewMtx->m, reinterpret_cast<Vec *>(&vCenter), reinterpret_cast<Vec *>(&vCenter));
                    m_fSortValue = vCenter.z;
                    break;
                }
            }
        } else {
            m_fSortValue = m_Mtx[SCNOBJMTXTYPE_VIEW].m[2][3];
        }
    }

    void CScnObj::SetBBoxZSortCalcType(long nType)
    {
        m_nBBoxZSortCalcType = nType;
    }

    void CScnObj::SetAlphaRate(unsigned char nRate)
    {
        m_nAlphaRate = nRate;
    }

    void CScnObj::SetAlphaRateF(float fRate)
    {
        short nRate;

        F32ToS16(255.0f * fRate, &nRate);
        m_nAlphaRate = ClampU8(nRate);
    }

    float CScnObj::GetAlphaRateF() const
    {
        unsigned short nRate = m_nAlphaRate;
        float fRate = 0.003921569f * U16ToF32(&nRate);

        if (fRate > 1.0f) {
            return 1.0f;
        }
        if (fRate < 0.0f) {
            return 0.0f;
        }
        return fRate;
    }

    void CScnObj::AttachToObject(const CScnObj *pParent, long nNode, AttachMode mode)
    {
        if (pParent != 0) {
            m_nAttachHandle = pParent->m_nHandle;
            m_nAttachNode = nNode;
            m_nAttachMode = mode;
        } else {
            m_nAttachHandle = 0;
            m_nAttachNode = 0;
            m_nAttachMode = 0;
        }
    }

    void CScnObj::ObjProc(unsigned long, unsigned long, void *)
    {
    }

    void CScnObj::SetJointObj(CScnObj *pObj)
    {
        if (pObj == 0) {
            ClearJointObj();
        } else if (m_pJointObj != 0) {
            m_pJointObj->SetJointObj(pObj);
        } else {
            m_pJointObj = pObj;
        }
    }

    void CScnObj::DeleteJointObj()
    {
        if (m_pJointObj != 0) {
            m_pJointObj->DeleteJointObj();
        }
        if (m_pJointObj != 0) {
            AppendDelayDeleteObject(m_pJointObj);
        }
        m_pJointObj = 0;
    }

    int CScnObj::IsDrawOpaJoint() const
    {
        int bDraw = IsDrawOpa();

        if (m_pJointObj != 0) {
            bDraw |= m_pJointObj->IsDrawOpaJoint();
        }
        return bDraw;
    }

    int CScnObj::IsDrawOpa() const
    {
        return !(m_nFlag & 0x10000000);
    }

    int CScnObj::IsDrawXluJoint() const
    {
        int bDraw = IsDrawXlu();

        if (m_pJointObj != 0) {
            bDraw |= m_pJointObj->IsDrawXluJoint();
        }
        return bDraw;
    }

    int CScnObj::IsDrawXlu() const
    {
        return !(m_nFlag & 0x20000000);
    }

    // bcs::CScnGroup

    CScnGroup::CScnGroup(unsigned long, long)
    {
        m_pHead = 0;
        m_pTail = 0;
        m_nCount = 0;
        OnFlag(0x30000000);
    }

    CScnGroup::~CScnGroup()
    {
        DetachFromParent();
        while (m_nCount != 0) {
            RemoveLastChild();
        }
    }

    int CScnGroup::ForEach(ForEachFunc pFunc, void *pArg, int bChildFirst)
    {
        CScnObj *pChild;

        if (bChildFirst) {
            for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                if (pChild->ForEach(pFunc, pArg, 0) == 1) {
                    return 1;
                }
            }
            return pFunc(this, pArg) == FOREACHRESULT_1;
        }

        ForEachResult result = pFunc(this, pArg);
        if (result == FOREACHRESULT_0) {
            for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                if (pChild->ForEach(pFunc, pArg, 0) == 1) {
                    return 1;
                }
            }
            return 0;
        }
        return result == FOREACHRESULT_1;
    }

    void CScnGroup::AppendChild(CScnObj *pObj)
    {
        if (m_pHead == 0) {
            pObj->m_pGroupPrev = 0;
            pObj->m_pGroupNext = 0;
            m_pTail = pObj;
            m_pHead = pObj;
            pObj->ObjProc(0x10002, 0, this);
            m_nCount++;
        } else {
            pObj->m_pGroupPrev = m_pTail;
            pObj->m_pGroupNext = 0;
            m_pTail->m_pGroupNext = pObj;
            m_pTail = pObj;
            pObj->ObjProc(0x10002, 0, this);
            m_nCount++;
        }
    }

    CScnObj *CScnGroup::RemoveLastChild()
    {
        CScnObj *pObj = m_pTail;

        if (pObj != 0) {
            RemoveChild(pObj);
        }
        return pObj;
    }

    CScnObj *CScnGroup::RemoveChildAt(unsigned long nIndex)
    {
        if (nIndex == 0) {
            return RemoveFirstChild();
        }
        if (nIndex >= m_nCount) {
            return RemoveLastChild();
        }

        return RemoveChild(GetChild(nIndex));
    }

    CScnObj *CScnGroup::GetChild(unsigned long nIndex) const
    {
        CScnObj *pObj;
        unsigned long i;

        if (nIndex < m_nCount / 2) {
            for (pObj = m_pHead, i = 0; pObj != 0; pObj = pObj->m_pGroupNext, i++) {
                if (i == nIndex) {
                    return pObj;
                }
            }
        } else {
            for (pObj = m_pTail, i = m_nCount - 1; pObj != 0; pObj = pObj->m_pGroupPrev, i--) {
                if (i == nIndex) {
                    return pObj;
                }
            }
        }
        return 0;
    }

    CScnObj *CScnGroup::GetChild(unsigned long nIndex)
    {
        CScnObj *pObj;
        unsigned long i;

        if (nIndex < m_nCount / 2) {
            for (pObj = m_pHead, i = 0; pObj != 0; pObj = pObj->m_pGroupNext, i++) {
                if (i == nIndex) {
                    return pObj;
                }
            }
        } else {
            for (pObj = m_pTail, i = m_nCount - 1; pObj != 0; pObj = pObj->m_pGroupPrev, i--) {
                if (i == nIndex) {
                    return pObj;
                }
            }
        }
        return 0;
    }

    void CScnGroup::ObjProc(unsigned long nTask, unsigned long nArg, void *pArg)
    {
        if (!IsProcDisabled(nTask)) {
            ObjProcGroup(nTask, nArg, pArg);
        }
    }

    void CScnGroup::ObjProcGroup(unsigned long nTask, unsigned long nArg, void *pArg)
    {
        CScnObj *pChild;

        switch (nTask) {
            case 5: {
                CScnObjGather *pGather = static_cast<CScnObjGather *>(pArg);
                int result = pGather->Add(this, IsDrawOpa(), IsDrawXlu(), reinterpret_cast<const CFrustum *>(nArg));
                if (result == 0) {
                    for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                        pChild->ObjProc(5, nArg, pArg);
                    }
                } else if (result == 1) {
                    for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                        pChild->ObjProc(5, 0, pArg);
                    }
                }
                break;
            }
            case 1:
                if (m_nFlag & 0x4000) {
                    CalcRootMtx();
                }
                CalcWorldMtx(static_cast<const FMTX34 *>(pArg));
                {
                    // The children are placed in the group's world matrix.
                    FMTX34 *pWorldMtx = &m_Mtx[SCNOBJMTXTYPE_1];
                    for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                        pChild->ObjProc(1, nArg, pWorldMtx);
                    }
                }
                break;
            case 2:
                for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                    pChild->ObjProc(2, nArg, pArg);
                }
                break;
            case 4:
                CalcViewMtx(static_cast<const FMTX34 *>(pArg));
                for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                    pChild->ObjProc(4, nArg, pArg);
                }
                break;
            case 6:
            case 7:
                break;
            case 0x10001:
                RemoveChild(static_cast<CScnObj *>(pArg));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pArg));
                break;
            default:
                for (pChild = m_pHead; pChild != 0; pChild = pChild->m_pGroupNext) {
                    pChild->ObjProc(nTask, nArg, pArg);
                }
                break;
        }
    }

    CScnObjGather::CScnObjGather(CScnObj **ppOpa, CScnObj **ppXlu, unsigned long nMax)
        : m_nCullMask(0)
    {
        Init(ppOpa, ppXlu, nMax);
    }

    CScnObjGather::~CScnObjGather()
    {
        Init(0, 0, 0);
    }

    int CScnObjGather::Add(CScnObj *pObj, int bOpa, int bXlu, const CFrustum *pFrustum)
    {
        int result;

        // Flag 0x400000 (meaning guessed): culled in this gather.
        pObj->OffFlag(0x400000);
        if (pObj->m_UnkEE & m_nCullMask) {
            pObj->OnFlag(0x400000);
            return 2;
        }
        if (pObj->GetAlphaRate() == 0) {
            return 1;
        }

        result = 0;
        if (pFrustum != 0 && (pObj->m_nFlag & 0x10000)) {
            nw4r::math::AABB box;
            pObj->GetBoundingBox(&box);
            int nHit = pFrustum->IntersectAABB_Ex(&box);
            if (nHit == 0) {
                pObj->OnFlag(0x400000);
                return 2;
            }
            if (nHit == 1) {
                result = 1;
            }
        }

        if (bOpa) {
            if (m_nOpaNum < m_nMax) {
                m_ppOpa[m_nOpaNum++] = pObj;
                SetGatherOpaJoint(pObj);
            } else {
                return result;
            }
        }
        if (bXlu) {
            if (m_nXluNum < m_nMax) {
                m_ppXlu[m_nXluNum++] = pObj;
                SetGatherXluJoint(pObj);
            } else {
                return result;
            }
        }
        return result;
    }

    void CScnObjGather::SetGatherOpaJoint(CScnObj *pObj)
    {
        pObj->SetFlag(0x100000, 1);
        if (pObj->m_pJointObj != 0) {
            SetGatherOpaJoint(pObj->m_pJointObj);
        }
    }

    void CScnObjGather::SetGatherXluJoint(CScnObj *pObj)
    {
        pObj->SetFlag(0x200000, 1);
        if (pObj->m_pJointObj != 0) {
            SetGatherXluJoint(pObj->m_pJointObj);
        }
    }

    void CScnObjGather::Sort()
    {
        std::sort(m_ppOpa, m_ppOpa + m_nOpaNum, CompareOpa);
        std::sort(m_ppXlu, m_ppXlu + m_nXluNum, CompareXlu);
        Link();
    }

    int CScnObjGather::CompareOpa(const CScnObj *pA, const CScnObj *pB)
    {
        long nPriorityA = pA->GetDrawPriorityOpa();
        long nPriorityB = pB->GetDrawPriorityOpa();

        if (nPriorityA == nPriorityB) {
            float fDiff = pA->GetValueForSortOpa() - pB->GetValueForSortOpa();
            if (fDiff == 0.0f) {
                return pA < pB;
            }
            return fDiff < 0.0f;
        }
        return nPriorityA < nPriorityB;
    }

    int CScnObjGather::CompareXlu(const CScnObj *pA, const CScnObj *pB)
    {
        long nPriorityA = pA->GetDrawPriorityXlu();
        long nPriorityB = pB->GetDrawPriorityXlu();

        if (nPriorityA == nPriorityB) {
            float fDiff = pA->GetValueForSortXlu() - pB->GetValueForSortXlu();
            if (fDiff == 0.0f) {
                return pA < pB;
            }
            return fDiff < 0.0f;
        }
        return nPriorityA < nPriorityB;
    }

    void CScnObjGather::DrawOpa(void *pArg, unsigned long nArg)
    {
        for (CScnObj *pObj = m_pOpaHead; pObj != 0; pObj = pObj->m_pGatherOpaNext) {
            pObj->ObjProc(6, nArg, pArg);
            ClearGatherOpaJoint(pObj);
        }
    }

    void CScnObjGather::ClearGatherOpaJoint(CScnObj *pObj)
    {
        pObj->SetFlag(0x100000, 0);
        if (pObj->m_pJointObj != 0) {
            ClearGatherOpaJoint(pObj->m_pJointObj);
        }
    }

    void CScnObjGather::DrawXlu(void *pArg, unsigned long nArg)
    {
        for (CScnObj *pObj = m_pXluHead; pObj != 0; pObj = pObj->m_pGatherXluNext) {
            pObj->ObjProc(7, nArg, pArg);
            ClearGatherXluJoint(pObj);
        }
    }

    void CScnObjGather::ClearGatherXluJoint(CScnObj *pObj)
    {
        pObj->SetFlag(0x200000, 0);
        if (pObj->m_pJointObj != 0) {
            ClearGatherXluJoint(pObj->m_pJointObj);
        }
    }

    void CScnObjGather::Link()
    {
        unsigned long i;

        m_pOpaHead = 0;
        m_pXluHead = 0;
        if (m_nOpaNum != 0) {
            m_pOpaHead = m_ppOpa[0];
            for (i = 0; i < m_nOpaNum - 1; i++) {
                m_ppOpa[i]->m_pGatherOpaNext = m_ppOpa[i + 1];
            }
            m_ppOpa[m_nOpaNum - 1]->m_pGatherOpaNext = 0;
        }
        if (m_nXluNum != 0) {
            m_pXluHead = m_ppXlu[0];
            for (i = 0; i < m_nXluNum - 1; i++) {
                m_ppXlu[i]->m_pGatherXluNext = m_ppXlu[i + 1];
            }
            m_ppXlu[m_nXluNum - 1]->m_pGatherXluNext = 0;
        }
    }

    // The original has this as an inline in the class (a weak copy at the
    // very end of the file, emitted with CG3dObj's vtable). Defined here
    // instead so that the units emitting vtables derived from CG3dObj do
    // not each get a weak copy.
    long CG3dObj::GetObjType() const
    {
        return 0;
    }
}
