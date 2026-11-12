// bcs animation (DOL 0x80006F20..0x80011144, one translation unit): CFrameCtrl,
// CAnmObjMotBase, CAnmChrMotion, CAnmMotion, CAnmBlendMotion, CAnmMatTable,
// CAnmTexSrt, CAnmTexPat, CAnmShp, CAnmMatClr and the nw4r::g3d result helpers.

// Include order: class declaration order decides the order of the vtables
// and weak inline virtuals (the original's are in reverse code order).
#include "bcs/CAnmMotion.hpp"
#include "bcs/CAnmBlendMotion.hpp"
#include "bcs/CAnmTexSrt.hpp"
#include "bcs/CAnmTexPat.hpp"
#include "bcs/CAnmShp.hpp"
#include "bcs/CAnmMatClr.hpp"
#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CAnmObjMat.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "math.h"
#include "nw4r/g3d/ResAnmTexPat.hpp"
#include "nw4r/math/arithmetic.h"
#include "nw4r/math/types.h"
#include "nw4r/ut/Color.hpp"
#include "revolution/mtx.h"

namespace bcs {

    namespace {

        // The parts selected by each mask bit, by the number of parts: with
        // one part it takes both bits, with two each takes one.
        const unsigned short sPartMask[3][CAnmBlendMotion::PART_MAX] = {
            {0, 0},
            {3, 3},
            {1, 2},
        };

        // Row i of a result's rotation-translation matrix, as a vector.
        inline nw4r::math::VEC3 *GetAxis(nw4r::g3d::ChrAnmResult *pResult, int i)
        {
            return reinterpret_cast<nw4r::math::VEC3 *>(pResult->rt[i]);
        }

        // The frame of the first trigger of kind nTrg at or after fFrame,
        // -1 if none.
        inline float FindTrgFrame(const CAnmObjMotBase::MOT_TRG_LIST *pList, unsigned long nTrg, float fFrame)
        {
            if (pList == 0) {
                return -1.0f;
            }
            if (pList->m_nNum != 0) {
                const CAnmObjMotBase::MOT_TRG *pTrg = pList->GetTrg();
                for (unsigned long i = 0; i < pList->m_nNum; i++, pTrg++) {
                    if (pTrg->m_fFrame < fFrame) {
                        continue;
                    }
                    if (pTrg->m_nType == nTrg) {
                        return pTrg->m_fFrame;
                    }
                }
            }
            return -1.0f;
        }

        // x clamped to [0, 1].
        inline float Clamp01(float x)
        {
            return x > 1.0f ? 1.0f : (x < 0.0f ? 0.0f : x);
        }

        // v normalised, or zero when it has no length.
        inline void NormalizeOrZero(nw4r::math::VEC3 *pVec)
        {
            if (0.0f == nw4r::math::VEC3LenSq(pVec)) {
                pVec->x = 0.0f;
                pVec->y = 0.0f;
                pVec->z = 0.0f;
            } else {
                PSVECNormalize(reinterpret_cast<const Vec *>(pVec), reinterpret_cast<Vec *>(pVec));
            }
        }

        // pOut = pA x pB.
        inline void VEC3Cross(nw4r::math::VEC3 *pOut, const nw4r::math::VEC3 *pA, const nw4r::math::VEC3 *pB)
        {
            PSVECCrossProduct(reinterpret_cast<const Vec *>(pA), reinterpret_cast<const Vec *>(pB),
                              reinterpret_cast<Vec *>(pOut));
        }
    }

    // The name of the motion extra data in a motion file. (Defined after
    // sPartMask: the original's .rodata has the table first.)
    const char *s_MotExInfoName = "mot_ex_info.bin";

    void CFrameCtrl::SetFrame(float frame)
    {
        unsigned long flag = m_nFlag;
        m_nFlag = flag & ~(FLAG_WRAP | FLAG_END);

        if (flag & FLAG_LOOP) {
            if (m_fEndFrame > 0.0f) {
                if (frame >= m_fEndFrame && m_fRate >= 0.0f) {
                    m_nFlag |= FLAG_LOOPED | FLAG_WRAP;
                } else if (frame < 0.0f && m_fRate < 0.0f) {
                    m_nFlag |= FLAG_LOOPED | FLAG_WRAP;
                }

                if (frame >= 0.0f) {
                    m_fFrame = fmod(frame, m_fEndFrame);
                } else {
                    float f = fmod(frame + m_fEndFrame, m_fEndFrame);
                    m_fFrame = f + nw4r::math::FSelect(f, 0.0f, m_fEndFrame);
                }
            } else {
                m_fFrame = 0.0f;
                m_nFlag |= FLAG_LOOPED | FLAG_WRAP;
            }
        } else {
            m_fFrame = frame;
            float last = m_fEndFrame - 1.0f;
            if (m_fFrame >= last && m_fRate >= 0.0f) {
                m_nFlag |= FLAG_END;
                m_fFrame = last;
            } else if (m_fFrame <= 0.0f && m_fRate < 0.0f) {
                m_fFrame = 0.0f;
                m_nFlag |= FLAG_END;
            }
        }
    }

    void CFrameCtrl::SetLoop(bool bLoop)
    {
        if (bLoop) {
            m_nFlag |= FLAG_LOOP;
        } else {
            m_nFlag &= ~FLAG_LOOP;
        }
    }

    void CAnmObjMotBase::VFunc4C()
    {
        for (unsigned long i = 0; i < m_nNodeNum; i++) {
            m_pNodeFlag[i] = NODE_UNBOUND;
        }
        OnFlag(ANMFLAG_4);
    }

    CAnmChrMotion::CAnmChrMotion()
        : m_pNodeExInfo(0), m_pResultBuf(0), m_nPlayFlag(0), m_nMotID(-1), m_pTrgCallback(0), m_pTrgUser(0),
          m_nTrgIdx(0), m_pTrgList(0)
    {
    }

    CAnmChrMotion::~CAnmChrMotion()
    {
        Release();
    }

    void CAnmChrMotion::Release()
    {
        DetachFromParent();
        if (m_pResultBuf != 0) {
            delete[] m_pResultBuf;
        }
        m_pResultBuf = 0;
        m_pNodeExInfo = 0;
        m_nPlayFlag = 0;
        m_nMotID = -1;
        m_pTrgCallback = 0;
        m_pTrgUser = 0;
        m_nTrgIdx = 0;
        m_pTrgList = 0;
        m_Frame.SetFrame(0.0f);
        m_Frame.m_fEndFrame = 0.0f;
        m_Frame.SetLoop(false);
        m_Frame.m_fRate = 0.0f;
        ReleaseNodeFlag();
    }

    void CAnmChrMotion::SetFrame(float frame)
    {
        m_Frame.SetFrame(frame);
        CalcResult();
        SeekTrg();
    }

    float CAnmChrMotion::GetFrame() const
    {
        return m_Frame.m_fFrame;
    }

    float CAnmChrMotion::VFunc28() const
    {
        return m_Frame.m_fEndFrame;
    }

    void CAnmChrMotion::SetRate(float rate)
    {
        m_Frame.m_fRate = rate;
    }

    float CAnmChrMotion::GetRate() const
    {
        return m_Frame.m_fRate;
    }

    void CAnmChrMotion::UpdateFrame(float fStep)
    {
        if (!(m_Frame.m_nFlag & CFrameCtrl::FLAG_END)) {
            m_Frame.SetFrame(m_Frame.m_fRate * fStep + m_Frame.m_fFrame);
            CalcResult();
            CallTrg();
        }
    }

    void CAnmChrMotion::SetAnmFrame(unsigned long, float fFrame)
    {
        SetFrame(fFrame);
    }

    float CAnmChrMotion::GetAnmFrame(unsigned long) const
    {
        return GetFrame();
    }

    float CAnmChrMotion::VFunc58(long) const
    {
        return VFunc28();
    }

    void CAnmChrMotion::SetAnmRate(unsigned long, float fRate)
    {
        SetRate(fRate);
    }

    float CAnmChrMotion::GetAnmRate(unsigned long) const
    {
        return GetRate();
    }

    void CAnmChrMotion::SetAnm(nw4r::g3d::ResAnmChr res, unsigned long nPlayFlag, long nMotID,
                               const MOT_TRG_LIST *pTrgList, float fRate, float fFrame)
    {
        m_nPlayFlag = nPlayFlag;
        m_ResAnm = res;
        m_nMotID = nMotID;
        m_nMotID2 = nMotID;
        m_pTrgList = pTrgList;
        if (pTrgList != 0 && pTrgList->m_nNum == 0) {
            m_pTrgList = 0;
        }
        VFunc48();
        m_Frame.m_nFlag &= ~(CFrameCtrl::FLAG_LOOPED | CFrameCtrl::FLAG_WRAP | CFrameCtrl::FLAG_END);
        m_Frame.m_fEndFrame = m_ResAnm.GetNumFrame();
        m_Frame.SetLoop((nPlayFlag & PLAY_LOOP) != 0);
        m_Frame.SetFrame(fFrame);
        m_Frame.m_fRate = fRate;
        m_nTrgIdx = 0;
    }

    long CAnmChrMotion::VFunc64(long, unsigned long) const
    {
        return m_nMotID;
    }

    float CAnmChrMotion::GetBlendWeight(unsigned long, unsigned long) const
    {
        return 1.0f;
    }

    long CAnmChrMotion::GetNodeMotID(unsigned long, unsigned long) const
    {
        return m_nMotID;
    }

    const char *CAnmChrMotion::GetAnmName(unsigned long, unsigned long) const
    {
        if (m_pNodeFlag == 0 || !m_ResAnm.IsValid()) {
            return 0;
        }
        return m_ResAnm.GetName();
    }

    const void *CAnmChrMotion::GetMotExtInfo(unsigned long, unsigned long) const
    {
        return 0;
    }

    const char *CAnmChrMotion::GetNodeAnmName(unsigned long, unsigned long) const
    {
        return GetAnmName(3, 0);
    }

    unsigned long CAnmChrMotion::GetPartNum() const
    {
        return 1;
    }

    unsigned long CAnmChrMotion::GetLayerNum() const
    {
        return 1;
    }

    unsigned long CAnmChrMotion::GetResAnmNum() const
    {
        return 1;
    }

    float CAnmChrMotion::VFunc88(long) const
    {
        if (m_pNodeFlag == 0 || !m_ResAnm.IsValid()) {
            return 0.0f;
        }
        return m_ResAnm.GetNumFrame();
    }

    int CAnmChrMotion::VFunc8C(long) const
    {
        if (m_pNodeFlag == 0 || !m_ResAnm.IsValid()) {
            return 0;
        }
        return m_ResAnm.GetAnmPolicy() == nw4r::g3d::ANM_POLICY_LOOP;
    }

    const void *CAnmChrMotion::GetResMotExtInfo(long) const
    {
        return 0;
    }

    const char *CAnmChrMotion::GetResAnmName(long) const
    {
        if (m_ResAnm.IsValid()) {
            return m_ResAnm.GetName();
        }
        return 0;
    }

    int CAnmChrMotion::VFunc98(long, unsigned long) const
    {
        return m_nPlayFlag & PLAY_MIRROR;
    }

    int CAnmChrMotion::VFunc9C(long, unsigned long) const
    {
        return (m_nPlayFlag >> 1) & 1;
    }

    int CAnmChrMotion::VFuncA0(long) const
    {
        return 0;
    }

    float CAnmChrMotion::VFuncA4(long) const
    {
        return 0.0f;
    }

    void CAnmChrMotion::VFuncA8(int bLoop)
    {
        m_Frame.SetLoop(bLoop);
    }

    int CAnmChrMotion::VFunc48()
    {
        if (!m_ResAnm.IsValid()) {
            return 0;
        }

        int bBound = 0;
        ClearNodeFlag();
        for (unsigned long i = 0; i < m_nNodeNum; i++) {
            if (m_pNodeExInfo == 0 || (m_nPartMask & m_pNodeExInfo[i].nFlag)) {
                nw4r::g3d::ResName name = m_ResMdl.GetResNode(i).GetResName();
                long idx = m_ResAnm.GetChrDic().GetIndex(name);
                if (idx != -1) {
                    m_pNodeFlag[i] = idx;
                    bBound = 1;
                }
            }
        }
        OnFlag(ANMFLAG_4);
        return bBound;
    }

    const nw4r::g3d::ChrAnmResult *CAnmChrMotion::GetResult(nw4r::g3d::ChrAnmResult *pResult, unsigned long nNode)
    {
        if ((m_nPlayFlag & PLAY_MIRROR) && m_pNodeExInfo != 0) {
            nNode = m_pNodeExInfo[nNode].nNode;
        }

        unsigned short id = m_pNodeFlag[nNode];
        if (id & (NODE_UNBOUND | NODE_REST)) {
            if (nNode != 0) {
                m_ResMdl.GetResNode(nNode).CalcChrAnmResult(pResult);
                if ((m_nPlayFlag & PLAY_MIRROR) && m_pNodeExInfo != 0) {
                    MirrorResult(pResult, m_pNodeExInfo[nNode].nKind, m_pNodeExInfo[nNode].nUnk04);
                }
            }
            pResult->flags = 0;
            return pResult;
        }

        if (m_pResultBuf != 0) {
            return &m_pResultBuf[nNode];
        }

        m_ResAnm.GetAnmResult(pResult, id, m_Frame.m_fFrame);
        if ((m_nPlayFlag & PLAY_MIRROR) && m_pNodeExInfo != 0) {
            MirrorResult(pResult, m_pNodeExInfo[nNode].nKind, m_pNodeExInfo[nNode].nUnk04);
        }
        return pResult;
    }

    void CAnmChrMotion::MirrorResult(nw4r::g3d::ChrAnmResult *pResult, unsigned char nKind, unsigned short nAxis)
    {
        switch (nKind) {
            case 0:
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    pResult->rt[0][0] = -pResult->rt[0][0];
                    pResult->rt[0][1] = -pResult->rt[0][1];
                    pResult->rt[1][0] = -pResult->rt[1][0];
                    pResult->rt[1][1] = -pResult->rt[1][1];
                    VEC3Cross(GetAxis(pResult, 2), GetAxis(pResult, 1), GetAxis(pResult, 0));
                    NormalizeOrZero(GetAxis(pResult, 2));
                    VEC3Cross(GetAxis(pResult, 1), GetAxis(pResult, 2), GetAxis(pResult, 0));
                    NormalizeOrZero(GetAxis(pResult, 1));
                }
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                    pResult->rt[0][3] = -pResult->rt[0][3];
                }
                break;
            case 1:
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    pResult->rt[0][2] = -pResult->rt[0][2];
                    pResult->rt[1][2] = -pResult->rt[1][2];
                    VEC3Cross(GetAxis(pResult, 2), GetAxis(pResult, 0), GetAxis(pResult, 1));
                    NormalizeOrZero(GetAxis(pResult, 2));
                    VEC3Cross(GetAxis(pResult, 1), GetAxis(pResult, 2), GetAxis(pResult, 0));
                    NormalizeOrZero(GetAxis(pResult, 1));
                }
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                    pResult->rt[2][3] = -pResult->rt[2][3];
                }
                break;
            case 2:
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    pResult->rt[1][0] = -pResult->rt[1][0];
                    pResult->rt[2][0] = -pResult->rt[2][0];
                    VEC3Cross(GetAxis(pResult, 0), GetAxis(pResult, 1), GetAxis(pResult, 2));
                    NormalizeOrZero(GetAxis(pResult, 0));
                    VEC3Cross(GetAxis(pResult, 1), GetAxis(pResult, 2), GetAxis(pResult, 0));
                    NormalizeOrZero(GetAxis(pResult, 1));
                }
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                    pResult->rt[0][3] = -pResult->rt[0][3];
                }
                break;
            case 4:
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    pResult->rt[0][0] = -pResult->rt[0][0];
                    pResult->rt[2][0] = -pResult->rt[2][0];
                    pResult->rt[0][1] = -pResult->rt[0][1];
                    pResult->rt[2][1] = -pResult->rt[2][1];
                    pResult->rt[0][2] = -pResult->rt[0][2];
                    pResult->rt[1][2] = -pResult->rt[1][2];
                    pResult->rt[2][2] = -pResult->rt[2][2];
                }
                break;
            case 5:
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                    pResult->rt[2][3] = -pResult->rt[2][3];
                }
                break;
            case 6:
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    pResult->rt[2][0] = -pResult->rt[2][0];
                    pResult->rt[1][1] = -pResult->rt[1][1];
                    VEC3Cross(GetAxis(pResult, 0), GetAxis(pResult, 1), GetAxis(pResult, 2));
                    NormalizeOrZero(GetAxis(pResult, 0));
                    VEC3Cross(GetAxis(pResult, 2), GetAxis(pResult, 0), GetAxis(pResult, 1));
                    NormalizeOrZero(GetAxis(pResult, 2));
                }
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                    pResult->rt[2][3] = -pResult->rt[2][3];
                }
                break;
            case 7:
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    pResult->rt[1][0] = -pResult->rt[1][0];
                    pResult->rt[2][0] = -pResult->rt[2][0];
                    VEC3Cross(GetAxis(pResult, 0), GetAxis(pResult, 1), GetAxis(pResult, 2));
                    NormalizeOrZero(GetAxis(pResult, 0));
                    VEC3Cross(GetAxis(pResult, 2), GetAxis(pResult, 0), GetAxis(pResult, 1));
                    NormalizeOrZero(GetAxis(pResult, 2));
                }
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                    pResult->rt[1][3] = -pResult->rt[1][3];
                }
                break;
            case 8:
                // Free mirroring: bit (3 * column + row) of nAxis negates
                // one element, bits 12-13 and 14-15 rebuild an axis from the
                // other two, bits 9-11 negate the translation.
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    if (nAxis & 0x001) {
                        pResult->rt[0][0] = -pResult->rt[0][0];
                    }
                    if (nAxis & 0x002) {
                        pResult->rt[1][0] = -pResult->rt[1][0];
                    }
                    if (nAxis & 0x004) {
                        pResult->rt[2][0] = -pResult->rt[2][0];
                    }
                    if (nAxis & 0x008) {
                        pResult->rt[0][1] = -pResult->rt[0][1];
                    }
                    if (nAxis & 0x010) {
                        pResult->rt[1][1] = -pResult->rt[1][1];
                    }
                    if (nAxis & 0x020) {
                        pResult->rt[2][1] = -pResult->rt[2][1];
                    }
                    if (nAxis & 0x040) {
                        pResult->rt[0][2] = -pResult->rt[0][2];
                    }
                    if (nAxis & 0x080) {
                        pResult->rt[1][2] = -pResult->rt[1][2];
                    }
                    if (nAxis & 0x100) {
                        pResult->rt[2][2] = -pResult->rt[2][2];
                    }

                    switch ((nAxis >> 12) & 3) {
                        case 1:
                            VEC3Cross(GetAxis(pResult, 0), GetAxis(pResult, 1), GetAxis(pResult, 2));
                            NormalizeOrZero(GetAxis(pResult, 0));
                            break;
                        case 2:
                            VEC3Cross(GetAxis(pResult, 1), GetAxis(pResult, 2), GetAxis(pResult, 0));
                            NormalizeOrZero(GetAxis(pResult, 1));
                            break;
                        case 3:
                            VEC3Cross(GetAxis(pResult, 2), GetAxis(pResult, 0), GetAxis(pResult, 1));
                            NormalizeOrZero(GetAxis(pResult, 2));
                            break;
                    }

                    switch ((nAxis >> 14) & 3) {
                        case 1:
                            VEC3Cross(GetAxis(pResult, 0), GetAxis(pResult, 1), GetAxis(pResult, 2));
                            NormalizeOrZero(GetAxis(pResult, 0));
                            break;
                        case 2:
                            VEC3Cross(GetAxis(pResult, 1), GetAxis(pResult, 2), GetAxis(pResult, 0));
                            NormalizeOrZero(GetAxis(pResult, 1));
                            break;
                        case 3:
                            VEC3Cross(GetAxis(pResult, 2), GetAxis(pResult, 0), GetAxis(pResult, 1));
                            NormalizeOrZero(GetAxis(pResult, 2));
                            break;
                    }
                }
                if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                    if (nAxis & 0x200) {
                        pResult->rt[0][3] = -pResult->rt[0][3];
                    }
                    if (nAxis & 0x400) {
                        pResult->rt[1][3] = -pResult->rt[1][3];
                    }
                    if (nAxis & 0x800) {
                        pResult->rt[2][3] = -pResult->rt[2][3];
                    }
                }
                break;
        }
    }

    void CAnmChrMotion::CallTrg()
    {
        if (m_pTrgList == 0) {
            return;
        }
        const MOT_TRG *pTrg = m_pTrgList->GetTrg() + m_nTrgIdx;
        unsigned long num = m_pTrgList->m_nNum;
        if (num == 0) {
            return;
        }

        float frame = m_Frame.m_fFrame;
        if (m_Frame.m_fRate >= 0.0f) {
            if (m_nTrgIdx < 0) {
                m_nTrgIdx = 0;
            }
            if (m_Frame.m_nFlag & CFrameCtrl::FLAG_WRAP) {
                // Wrapped: the rest of the triggers, then from the start.
                for (; m_nTrgIdx < num; m_nTrgIdx++) {
                    CallTrg(pTrg);
                    pTrg++;
                }
                m_nTrgIdx = 0;
                pTrg = m_pTrgList->GetTrg();
            }
            for (; pTrg->m_fFrame <= frame && m_nTrgIdx < num; m_nTrgIdx++) {
                CallTrg(pTrg);
                pTrg++;
            }
        } else {
            if (m_nTrgIdx >= num) {
                m_nTrgIdx = num - 1;
            }
            if (m_Frame.m_nFlag & CFrameCtrl::FLAG_WRAP) {
                for (; m_nTrgIdx >= 0; m_nTrgIdx--) {
                    CallTrg(pTrg);
                    pTrg--;
                }
                m_nTrgIdx = num - 1;
                pTrg = m_pTrgList->GetTrg() + m_nTrgIdx;
            }
            for (; pTrg->m_fFrame >= frame && m_nTrgIdx >= 0; m_nTrgIdx--) {
                CallTrg(pTrg);
                pTrg--;
            }
        }
    }

    void CAnmChrMotion::SeekTrg()
    {
        if (m_pTrgList == 0) {
            return;
        }
        const MOT_TRG *pTrg = m_pTrgList->GetTrg() + m_nTrgIdx;
        unsigned long num = m_pTrgList->m_nNum;
        if (num == 0) {
            return;
        }

        if (m_Frame.m_fRate >= 0.0f) {
            if (m_Frame.m_nFlag & CFrameCtrl::FLAG_WRAP) {
                return;
            }
            float frame = m_Frame.m_fFrame;
            if (m_nTrgIdx > 0) {
                while (pTrg[-1].m_fFrame >= frame && m_nTrgIdx > 0) {
                    pTrg--;
                    m_nTrgIdx--;
                }
            }
        } else {
            if (m_Frame.m_nFlag & CFrameCtrl::FLAG_WRAP) {
                return;
            }
            unsigned long last = num - 1;
            float frame = m_Frame.m_fFrame;
            if (m_nTrgIdx < last) {
                while (pTrg[1].m_fFrame <= frame && m_nTrgIdx < last) {
                    pTrg++;
                    m_nTrgIdx++;
                }
            }
        }
    }

    void CAnmChrMotion::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }

    CAnmMotion::CAnmMotion()
        : m_nFlag(0), m_pMdl(0), m_pNodeExInfo(0), m_pExInfo(0), m_nCur(0), m_fBlendFrame(0.0f), m_fBlendStep(0.0f),
          m_fBlendRate(0.0f)
    {
        m_pMotion[0] = m_pMotion[1] = 0;
        m_pResult[0] = m_pResult[1] = 0;
    }

    CAnmMotion::~CAnmMotion()
    {
        Release();
    }

    inline void CAnmMotion::LoadMotExInfo()
    {
        m_pExInfo = static_cast<MOT_EX_INFO *>(m_ResFile.GetExternalData(s_MotExInfoName));
        if (m_pExInfo != 0) {
            m_pExInfo->Relocate();
        }
    }

    void CAnmMotion::Create(const CMdlObj *pMdl, nw4r::g3d::ResFile file, const NODE_EX_INFO *pInfo,
                            unsigned long nPartMask, long nHeap)
    {
        m_pMdl = pMdl;
        m_pNodeExInfo = pInfo;
        m_ResFile = file;
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        LoadMotExInfo();

        CreateNodeFlag(mdl, nHeap);
        m_nPartMask = 3;
        VFunc4C();

        for (int i = 0; i < 2; i++) {
            CAnmChrMotion *pMotion = new (nHeap | 0x100) CAnmChrMotion;
            m_pMotion[i] = pMotion;
            pMotion->Create(mdl, m_pNodeExInfo, nPartMask, nHeap);
        }

        for (int i = 0; i < 2; i++) {
            m_pResult[i] = new (nHeap | 0x100) nw4r::g3d::ChrAnmResult[m_nNodeNum];
            for (unsigned long j = 0; j < m_nNodeNum; j++) {
                mdl.GetResNode(j).CalcChrAnmResult(&m_pResult[i][j]);
            }
        }

        m_nCur = 0;
        m_nFlag = 0;
        m_fBlendFrame = 0.0f;
        m_fBlendStep = 0.0f;
        m_fBlendRate = 0.0f;
    }

    void CAnmMotion::Release()
    {
        DetachFromParent();
        if (m_pMotion[0] != 0) {
            delete m_pMotion[0];
        }
        m_pMotion[0] = 0;
        if (m_pMotion[1] != 0) {
            delete m_pMotion[1];
        }
        m_pMotion[1] = 0;
        if (m_pResult[0] != 0) {
            delete[] m_pResult[0];
        }
        m_pResult[0] = 0;
        if (m_pResult[1] != 0) {
            delete[] m_pResult[1];
        }
        m_pResult[1] = 0;
        m_nCur = 0;
        m_nFlag = 0;
        m_fBlendFrame = 0.0f;
        m_fBlendStep = 0.0f;
        m_fBlendRate = 0.0f;
        m_pMdl = 0;
        m_pNodeExInfo = 0;
        m_pExInfo = 0;
        m_ResFile = nw4r::g3d::ResFile();
        ReleaseNodeFlag();
    }

    int CAnmMotion::VFunc48()
    {
        OnFlag(ANMFLAG_4);
        return 1;
    }

    void CAnmMotion::ChangeResFile(nw4r::g3d::ResFile file)
    {
        Stop();
        m_ResFile = file;
        LoadMotExInfo();
    }

    void CAnmMotion::Play(long idx, unsigned long flag, float rate, float blend, float frame)
    {
        if (idx < 0) {
            flag |= CAnmChrMotion::PLAY_MIRROR;
        }
        const MOT_TRG_LIST *pTrgList = m_pExInfo != 0 ? m_pExInfo->GetTrgList(idx) : 0;
        PlayImpl(GetResAnmChr(idx), flag, idx, pTrgList, rate, blend, frame);
    }

    void CAnmMotion::Play(nw4r::g3d::ResAnmChr res, unsigned long flag, float rate, float blend, float frame)
    {
        PlayImpl(res, flag, 0, 0, rate, blend, frame);
    }

    void CAnmMotion::PlayImpl(nw4r::g3d::ResAnmChr res, unsigned long flag, long nMotID,
                              const MOT_TRG_LIST *pTrgList, float rate, float blend, float frame)
    {
        m_nFlag &= ~(FLAG_BLEND | FLAG_HOLD | FLAG_BLEND_KEEP);
        if (blend > 0.0f) {
            m_nFlag |= FLAG_BLEND;
            m_fBlendFrame = blend;
            m_fBlendStep = 1.0f / blend;
            m_fBlendRate = 1.0f;
            if ((m_nFlag & FLAG_PLAY) && (flag & 4)) {
                // Blend from the blended pose of the motion playing now.
                m_nFlag |= FLAG_BLEND_KEEP;
            } else {
                // Blend from the model's current pose.
                nw4r::g3d::ChrAnmResult *pResult = &m_pResult[1][1];
                nw4r::math::MTX34 *pNodeMtx = m_pMdl->GetNodeMtxArray();
                nw4r::g3d::ResMdl mdl = m_pMdl->GetResMdl();
                for (unsigned long i = 1; i < m_nNodeNum; i++, pResult++) {
                    CalcChrAnmResultFromMtx(mdl.GetResNode(i), pResult, pNodeMtx);
                }
            }
            if (flag & 8) {
                m_nFlag |= FLAG_HOLD;
            }
        } else {
            m_fBlendFrame = m_fBlendStep = m_fBlendRate = 0.0f;
        }

        m_nCur = 1 - m_nCur;
        m_nFlag |= FLAG_PLAY;
        GetCurMotion()->SetAnm(res, flag, nMotID, pTrgList, rate, frame);
    }

    void CAnmMotion::Stop()
    {
        m_nFlag &= ~FLAG_PLAY;
    }

    void CAnmMotion::CalcBlend()
    {
        float fCur = Clamp01(1.0f - m_fBlendRate);
        float fPrev = 1.0f - fCur;

        GetCurMotion()->GetResult(m_pResult[0], 0);
        if (fCur >= 1.0f || !(m_nFlag & (FLAG_BLEND | FLAG_BLEND_KEEP))) {
            nw4r::g3d::ChrAnmResult *pResult = &m_pResult[0][1];
            for (unsigned long i = 1; i < m_nNodeNum; i++) {
                GetCurMotion()->GetResult(pResult, i);
                pResult++;
            }
            return;
        }

        if (fPrev > 0.0f && (m_nFlag & FLAG_BLEND_KEEP)) {
            nw4r::g3d::ChrAnmResult *pPrev = &m_pResult[1][1];
            for (unsigned long i = 1; i < m_nNodeNum; i++) {
                GetPrevMotion()->GetResult(pPrev, i);
                pPrev++;
            }
        }

        nw4r::g3d::ChrAnmResult *pResult = &m_pResult[0][1];
        nw4r::g3d::ChrAnmResult *pPrev = &m_pResult[1][1];
        int bFlag = TestAnmFlag(8) != 0;
        for (unsigned long i = 1; i < m_nNodeNum; i++) {
            nw4r::g3d::ChrAnmResult result;
            BlendChrAnmResult(pResult, GetCurMotion()->GetResult(&result, i), pPrev, bFlag, fCur, fPrev);
            pResult++;
            pPrev++;
        }
    }

    const nw4r::g3d::ChrAnmResult *CAnmMotion::GetResult(nw4r::g3d::ChrAnmResult *, unsigned long nNode)
    {
        return &m_pResult[0][nNode];
    }

    nw4r::g3d::ResAnmChr CAnmMotion::GetResAnmChr() const
    {
        return GetCurMotion()->m_ResAnm;
    }

    const CAnmObjMotBase::NODE_EX_INFO *CAnmMotion::GetNodeExInfo() const
    {
        return m_pNodeExInfo;
    }

    void CAnmMotion::SetFrame(float frame)
    {
        GetCurMotion()->SetFrame(frame);
        CalcBlend();
    }

    float CAnmMotion::GetFrame() const
    {
        return GetCurMotion()->GetFrame();
    }

    float CAnmMotion::VFunc28() const
    {
        return GetCurMotion()->VFunc28();
    }

    void CAnmMotion::SetRate(float rate)
    {
        GetCurMotion()->SetRate(rate);
    }

    float CAnmMotion::GetRate() const
    {
        return GetCurMotion()->GetRate();
    }

    void CAnmMotion::SetAnmFrame(unsigned long, float fFrame)
    {
        SetFrame(fFrame);
    }

    float CAnmMotion::GetAnmFrame(unsigned long) const
    {
        return GetFrame();
    }

    float CAnmMotion::VFunc58(long) const
    {
        return VFunc28();
    }

    void CAnmMotion::SetAnmRate(unsigned long, float fRate)
    {
        SetRate(fRate);
    }

    float CAnmMotion::GetAnmRate(unsigned long) const
    {
        return GetRate();
    }

    void CAnmMotion::UpdateFrame(float fStep)
    {
        if (!(m_nFlag & FLAG_PLAY)) {
            return;
        }

        if (m_nFlag & FLAG_BLEND) {
            float fRate = GetCurMotion()->GetRate();
            m_fBlendRate -= m_fBlendStep * fStep * fabsf(fRate);
            if (m_fBlendRate <= 0.0f) {
                m_fBlendRate = 0.0f;
                m_nFlag &= ~(FLAG_BLEND | FLAG_HOLD | FLAG_BLEND_KEEP);
            }
        }
        if (!(m_nFlag & FLAG_HOLD)) {
            GetCurMotion()->UpdateFrame(fStep);
        }
        if ((m_nFlag & (FLAG_BLEND | FLAG_BLEND_KEEP)) == (FLAG_BLEND | FLAG_BLEND_KEEP)) {
            GetPrevMotion()->UpdateFrame(fStep);
        }
        CalcBlend();
    }

    int CAnmMotion::IsLooped() const
    {
        return GetCurMotion()->IsLooped();
    }

    void CAnmMotion::ResetLoopedFlag(long)
    {
        GetCurMotion()->m_Frame.m_nFlag &= ~CFrameCtrl::FLAG_LOOPED;
    }

    int CAnmMotion::VFunc3C() const
    {
        return GetCurMotion()->VFunc3C();
    }

    int CAnmMotion::IsEnd() const
    {
        if (m_nFlag & FLAG_PLAY) {
            return GetCurMotion()->IsEnd();
        }
        return 1;
    }

    long CAnmMotion::VFunc64(long, unsigned long) const
    {
        return GetCurMotion()->VFunc64(3, 0);
    }

    float CAnmMotion::GetBlendWeight(unsigned long, unsigned long) const
    {
        return 1.0f;
    }

    long CAnmMotion::GetNodeMotID(unsigned long, unsigned long) const
    {
        return VFunc64(3, 0);
    }

    const char *CAnmMotion::GetAnmName(unsigned long nMask, unsigned long nLayer) const
    {
        return GetCurMotion()->GetAnmName(nMask, nLayer);
    }

    const void *CAnmMotion::GetMotExtInfo(unsigned long, unsigned long) const
    {
        if (m_pExInfo == 0) {
            return 0;
        }
        long nMot = GetCurMotion()->VFunc64(3, 0);
        if (GetCurMotion()->VFunc98(3, 0)) {
            nMot = -nMot;
        }
        return m_pExInfo->GetExt(nMot);
    }

    const char *CAnmMotion::GetNodeAnmName(unsigned long, unsigned long) const
    {
        return GetAnmName(3, 0);
    }

    unsigned long CAnmMotion::GetResAnmNum() const
    {
        if (m_pExInfo != 0) {
            return m_pExInfo->m_nAnmNum;
        }
        return m_ResFile.GetResAnmChrNumEntries();
    }

    float CAnmMotion::VFunc88(long nAnim) const
    {
        return GetResAnmChr(nAnim).GetNumFrame();
    }

    int CAnmMotion::VFunc8C(long nAnim) const
    {
        return GetResAnmChr(nAnim).GetAnmPolicy() == nw4r::g3d::ANM_POLICY_LOOP;
    }

    const void *CAnmMotion::GetResMotExtInfo(long nAnim) const
    {
        if (m_pExInfo == 0) {
            return 0;
        }
        return m_pExInfo->GetExt(nAnim);
    }

    const char *CAnmMotion::GetResAnmName(long nAnim) const
    {
        return GetResAnmChr(nAnim).GetName();
    }

    int CAnmMotion::VFunc98(long nMask, unsigned long nLayer) const
    {
        return GetCurMotion()->VFunc98(nMask, nLayer);
    }

    int CAnmMotion::VFunc9C(long nMask, unsigned long nLayer) const
    {
        return GetCurMotion()->VFunc9C(nMask, nLayer);
    }

    int CAnmMotion::VFuncA0(long) const
    {
        if (m_nFlag & FLAG_PLAY) {
            return (m_nFlag & FLAG_BLEND) != 0;
        }
        return 0;
    }

    float CAnmMotion::VFuncA4(long) const
    {
        return m_fBlendFrame;
    }

    void CAnmMotion::VFuncA8(int bLoop)
    {
        GetCurMotion()->VFuncA8(bLoop);
    }

    void CAnmMotion::SetTrgCallback(void (*pFunc)(const MOT_TRG *pTrg, void *pUser), void *pUser)
    {
        m_pMotion[0]->SetTrgCallback(pFunc, pUser);
        m_pMotion[1]->SetTrgCallback(pFunc, pUser);
    }

    float CAnmMotion::SearchTrgFrame(long nAnim, unsigned long nTrg, float fFrame) const
    {
        return FindTrgFrame(m_pExInfo != 0 ? m_pExInfo->GetTrgList(nAnim) : 0, nTrg, fFrame);
    }

    void CAnmMotion::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }

    CAnmBlendMotion::CAnmBlendMotion()
        : m_pMdl(0), m_pNodeExInfo(0), m_pExtInfo(0), m_nPartNum(0), m_nLayerNum(0), m_pMotion(0), m_pNodeFlagAll(0)
    {
        ResetPlay();
        for (int i = 0; i < PART_MAX; i++) {
            for (int j = 0; j < BANK_NUM; j++) {
                for (int k = 0; k < LAYER_MAX; k++) {
                    m_fWeight[i][j][k] = 0.0f;
                    m_WeightLerp[i][j][k].Reset();
                }
                m_RateLerp[i][j].Set(1.0f);
            }
        }
        m_pResult[0] = 0;
        m_pResult[1] = 0;
    }

    CAnmBlendMotion::~CAnmBlendMotion()
    {
        Release();
    }

    int CAnmBlendMotion::Create(const CMdlObj *pMdl, nw4r::g3d::ResFile file, const NODE_EX_INFO *pInfo,
                                unsigned long nPartNum, unsigned long nLayerNum, long nHeap)
    {
        m_pMdl = pMdl;
        m_pNodeExInfo = pInfo;
        m_nPartNum = nPartNum;
        m_nLayerNum = nLayerNum;
        m_File = file;
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        m_pExtInfo = static_cast<MOT_EX_INFO *>(m_File.GetExternalData(s_MotExInfoName));
        if (m_pExtInfo != 0) {
            m_pExtInfo->Relocate();
        }

        // One block of node flags for this object and one per motion, each
        // rounded up to 16 entries.
        unsigned short nNodeNum = mdl.GetResNodeNumEntries();
        unsigned long nStride = (nNodeNum + 15) & ~15;
        m_pNodeFlag = m_pNodeFlagAll =
            new (nHeap | 0x100) unsigned short[nStride * (m_nLayerNum * (m_nPartNum * BANK_NUM) + 1)];
        m_nNodeNum = nNodeNum;
        m_nPartMask = 3;
        VFunc4C();

        m_pMotion = new (nHeap | 0x100) CAnmChrMotion[m_nLayerNum * (m_nPartNum * BANK_NUM)];
        unsigned short *pNodeFlag = m_pNodeFlagAll + nStride;
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            for (unsigned long j = 0; j < BANK_NUM; j++) {
                for (unsigned long k = 0; k < m_nLayerNum; k++) {
                    CAnmChrMotion *pMotion = GetMotion(i, j, k);
                    pMotion->m_nNodeNum = nNodeNum;
                    pMotion->m_pNodeFlag = pNodeFlag;
                    pMotion->m_nPartMask = sPartMask[m_nPartNum][i];
                    pMotion->Init(mdl, pInfo);
                    pNodeFlag += nStride;
                }
                m_fWeight[i][j][0] = 1.0f;
            }
        }

        for (int i = 0; i < 2; i++) {
            m_pResult[i] = new (nHeap | 0x100) nw4r::g3d::ChrAnmResult[m_nNodeNum];
            for (unsigned long j = 0; j < m_nNodeNum; j++) {
                mdl.GetResNode(j).CalcChrAnmResult(&m_pResult[i][j]);
            }
        }

        VFunc48();
        ResetPlay();
        return 1;
    }

    void CAnmBlendMotion::Release()
    {
        DetachFromParent();
        for (int i = 0; i < PART_MAX; i++) {
            for (int j = 0; j < BANK_NUM; j++) {
                for (int k = 0; k < LAYER_MAX; k++) {
                    m_fWeight[i][j][k] = 0.0f;
                }
            }
        }
        if (m_pMotion != 0) {
            delete[] m_pMotion;
        }
        m_pMotion = 0;
        if (m_pResult[0] != 0) {
            delete[] m_pResult[0];
        }
        m_pResult[0] = 0;
        if (m_pResult[1] != 0) {
            delete[] m_pResult[1];
        }
        m_pResult[1] = 0;
        ResetPlay();
        m_pMdl = 0;
        m_pNodeExInfo = 0;
        m_nPartNum = 0;
        m_nLayerNum = 0;
        ReleaseNodeFlag();
        if (m_pNodeFlagAll != 0) {
            delete[] m_pNodeFlagAll;
        }
        m_pNodeFlagAll = 0;
    }

    int CAnmBlendMotion::VFunc48()
    {
        for (unsigned long i = 0; i < m_nNodeNum; i++) {
            m_pNodeFlag[i] = NODE_UNBOUND;
            unsigned char nFlag = m_pNodeExInfo[i].nFlag;
            if (nFlag & 3) {
                m_pNodeFlag[i] = 0;
            }
            if (nFlag & 0x80) {
                m_pNodeFlag[i] |= 0x2000;
            }
        }
        OnFlag(ANMFLAG_4);
        return 1;
    }
}

namespace bcs {


    void CAnmBlendMotion::Play(const ANIM_ARG &rArg, unsigned long nMask)
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (!(nMask & sPartMask[m_nPartNum][i])) {
                continue;
            }

            float fBlendFrame = rArg.fBlendFrame;
            m_nFlag[i] &= ~FLAG_BLEND_MASK;
            if (fBlendFrame > 0.0f) {
                m_nFlag[i] |= FLAG_BLEND;
                m_fBlendFrame[i] = fBlendFrame;
                m_fBlendStep[i] = 1.0f / fBlendFrame;
                m_fBlendRate[i] = 1.0f;
                if ((m_nFlag[i] & FLAG_PLAY) && (rArg.nFlag & ARG_BLEND_LIVE)) {
                    m_nFlag[i] |= FLAG_BLEND_LIVE;
                } else {
                    // Fade out from the model's current pose.
                    nw4r::g3d::ResMdl mdl = m_pMdl->GetResMdl();
                    nw4r::math::MTX34 *pNodeMtx = m_pMdl->GetNodeMtxArray();
                    unsigned short nPart = sPartMask[m_nPartNum][i];
                    nw4r::g3d::ChrAnmResult *pResult = &m_pResult[1][1];
                    for (unsigned long n = 1; n < m_nNodeNum; n++, pResult++) {
                        if (nPart & m_pNodeExInfo[n].nFlag) {
                            CalcChrAnmResultFromMtx(mdl.GetResNode(n), pResult, pNodeMtx);
                        }
                    }
                }
                if (rArg.nFlag & ARG_PAUSE) {
                    m_nFlag[i] |= FLAG_PAUSE;
                }
            } else {
                m_fBlendRate[i] = 0.0f;
                m_fBlendStep[i] = 0.0f;
                m_fBlendFrame[i] = 0.0f;
            }

            m_nCur[i] = 1 - m_nCur[i];
            m_nFlag[i] |= FLAG_PLAY;
            for (unsigned long j = 0; j < LAYER_MAX; j++) {
                m_fWeight[i][m_nCur[i]][j] = 0.0f;
            }

            for (unsigned long j = 0; j < rArg.nNum && j < m_nLayerNum; j++) {
                m_fWeight[i][m_nCur[i]][j] = rArg.fWeight[j];
                unsigned long nIndex = m_pExtInfo != 0 ? m_pExtInfo->GetResAnmIndex(rArg.nMotID[j]) : abs(rArg.nMotID[j]);
                const MOT_TRG_LIST *pTrgList = m_pExtInfo != 0 ? m_pExtInfo->GetTrgList(rArg.nMotID[j]) : 0;
                nw4r::g3d::ResAnmChr res = m_File.GetResAnmChr(nIndex);
                unsigned long nFlag = rArg.nFlag;
                if (rArg.nMotID[j] < 0) {
                    nFlag = (nFlag & CAnmChrMotion::PLAY_MIRROR) ? nFlag & ~CAnmChrMotion::PLAY_MIRROR
                                                                  : nFlag | CAnmChrMotion::PLAY_MIRROR;
                }
                GetMotion(i, m_nCur[i], j)->SetAnm(res, nFlag, abs(rArg.nMotID[j]), pTrgList, rArg.fRate, rArg.fFrame);
            }

            if (m_RateLerp[i][m_nCur[i]].fT > 0.0f) {
                m_RateLerp[i][m_nCur[i]].Set(rArg.fRate);
            }
            for (unsigned long j = 0; j < LAYER_MAX; j++) {
                m_WeightLerp[i][m_nCur[i]][j].Reset();
            }
        }
    }

    void CAnmBlendMotion::Stop(unsigned long nMask)
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                m_nFlag[i] &= ~FLAG_PLAY;
            }
        }
    }

    void CAnmBlendMotion::CalcResult()
    {
        float fRate = Clamp01(1.0f - m_fBlendRate[0]);
        float fRateOld = 1.0f - fRate;

        GetMotion(0, m_nCur[0], 0)->GetResult(m_pResult[0], 0);
        if (fRate >= 1.0f || !(m_nFlag[0] & (FLAG_BLEND | FLAG_BLEND_LIVE))) {
            nw4r::g3d::ChrAnmResult *pResult = &m_pResult[0][1];
            for (unsigned long n = 1; n < m_nNodeNum; n++, pResult++) {
                GetBankResult(0, m_nCur[0], pResult, n);
            }
        } else {
            if (fRateOld > 0.0f && (m_nFlag[0] & FLAG_BLEND_LIVE)) {
                nw4r::g3d::ChrAnmResult *pResult = &m_pResult[1][1];
                for (unsigned long n = 1; n < m_nNodeNum; n++, pResult++) {
                    GetBankResult(0, 1 - m_nCur[0], pResult, n);
                }
            }

            nw4r::g3d::ChrAnmResult result;
            nw4r::g3d::ChrAnmResult *pOut = &m_pResult[0][1];
            nw4r::g3d::ChrAnmResult *pOld = &m_pResult[1][1];
            int bQuat = TestAnmFlag(8) != 0;
            for (unsigned long n = 1; n < m_nNodeNum; n++, pOut++, pOld++) {
                BlendChrAnmResult(pOut, GetBankResult(0, m_nCur[0], &result, n), pOld, bQuat, fRate, fRateOld);
            }
        }
    }

    void CAnmBlendMotion::CalcResultParts()
    {
        float fRate0 = 1.0f - m_fBlendRate[0];
        float fRate1 = 1.0f - m_fBlendRate[1];
        fRate0 = Clamp01(fRate0);
        fRate1 = Clamp01(fRate1);

        GetMotion(0, m_nCur[0], 0)->GetResult(m_pResult[0], 0);

        nw4r::g3d::ChrAnmResult result;
        nw4r::g3d::ChrAnmResult *pOut = &m_pResult[0][1];
        nw4r::g3d::ChrAnmResult *pOld = &m_pResult[1][1];
        int bQuat = TestAnmFlag(8) != 0;
        for (unsigned long n = 1; n < m_nNodeNum; n++, pOut++, pOld++) {
            unsigned long nPart;
            float fRate;
            if ((m_pNodeExInfo[n].nFlag & 3) == 1) {
                fRate = fRate0;
                nPart = 0;
            } else {
                nPart = 1;
                fRate = fRate1;
            }

            if (!(m_nFlag[nPart] & FLAG_PLAY)) {
                continue;
            }

            float fRateOld = 1.0f - fRate;
            if (fRate >= 1.0f || !(m_nFlag[nPart] & (FLAG_BLEND | FLAG_BLEND_LIVE))) {
                GetBankResult(nPart, m_nCur[nPart], pOut, n);
            } else {
                if (fRateOld > 0.0f && (m_nFlag[nPart] & FLAG_BLEND_LIVE)) {
                    GetBankResult(nPart, 1 - m_nCur[nPart], pOld, n);
                }
                BlendChrAnmResult(pOut, GetBankResult(nPart, m_nCur[nPart], &result, n), pOld, bQuat, fRate, fRateOld);
            }
        }
    }

    const nw4r::g3d::ChrAnmResult *CAnmBlendMotion::GetBankResult(unsigned long nPart, unsigned long nBank,
                                                                   nw4r::g3d::ChrAnmResult *pResult, unsigned long nNode)
    {

        // Count the layers that move the node.
        CAnmChrMotion *pFirst = 0;
        int nActive = 0;
        bool bAllZero = true;
        float fWeightSum = 0.0f;
        for (unsigned long j = 0; j < m_nLayerNum; j++) {
            CAnmChrMotion *pMotion = m_pMotion + nPart * 2 * m_nLayerNum + nBank * m_nLayerNum + j;
            float fWeight = m_fWeight[nPart][nBank][j];
            if (pMotion != 0 && fWeight != 0.0f && !(pMotion->GetNodeFlag(nNode) & 0xC000)) {
                bAllZero = false;
                if (fWeight >= 0.001f) {
                    fWeightSum += fWeight;
                    nActive++;
                    if (pFirst == 0) {
                        pFirst = pMotion;
                    }
                    if (m_pNodeFlag[nNode] & 0x2000) {
                        break;
                    }
                }
            }
        }

        if (nActive == 0) {
            if (nNode != 0 && bAllZero) {
                GetMotion(nPart, nBank, 0)->GetResult(pResult, nNode);
            }
            pResult->flags = 0;
            return pResult;
        }
        if (nActive == 1) {
            return pFirst->GetResult(pResult, nNode);
        }

        // Weighted sum of the layers. The scale is blended in log space when
        // bit 4 of the animation flags is set, the rotation as a quaternion
        // when bit 3 is.
        float fInvSum = nw4r::math::FInv(fWeightSum);
        QUATERNION quat;
        quat.x = 0.0f;
        quat.y = 0.0f;
        quat.z = 0.0f;
        quat.w = 1.0f;
        bool bQuat = TestAnmFlag(8) != 0;
        bool bLogScale = TestAnmFlag(0x10) != 0;
        float fLogX = 0.0f;
        float fLogY = 0.0f;
        float fLogZ = 0.0f;
        float fQuatWeight = 0.0f;
        pResult->flags = 0xFFFFFFFF;
        pResult->s.z = 0.0f;
        pResult->s.y = 0.0f;
        pResult->s.x = 0.0f;
        nw4r::math::MTX34Zero(reinterpret_cast<nw4r::math::MTX34 *>(pResult->rt));

        for (unsigned long j = 0; j < m_nLayerNum; j++) {
            float fWeight = m_fWeight[nPart][nBank][j];
            CAnmChrMotion *pMotion = m_pMotion + nPart * 2 * m_nLayerNum + nBank * m_nLayerNum + j;
            if (pMotion == 0 || fWeight < 0.001f || (pMotion->GetNodeFlag(nNode) & 0xC000)) {
                continue;
            }

            nw4r::g3d::ChrAnmResult result;
            const nw4r::g3d::ChrAnmResult *pLayer = pMotion->GetResult(&result, nNode);
            float fRate = fWeight * fInvSum;
            unsigned long nFlag = pLayer->flags;

            if (bLogScale) {
                if (!(nFlag & 8)) {
                    fLogX += fRate * nw4r::math::FLog(pLayer->s.x);
                    fLogY += fRate * nw4r::math::FLog(pLayer->s.y);
                    fLogZ += fRate * nw4r::math::FLog(pLayer->s.z);
                }
            } else if (!(nFlag & 8)) {
                pResult->s.x += pLayer->s.x * fRate;
                pResult->s.y += pLayer->s.y * fRate;
                pResult->s.z += pLayer->s.z * fRate;
            } else {
                pResult->s.x += fRate;
                pResult->s.y += fRate;
                pResult->s.z += fRate;
            }

            if (bQuat) {
                QUATERNION q;
                if (!(nFlag & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                    BCS_Quat_FromMatrix(q, *reinterpret_cast<const FMTX34 *>(pLayer->rt));
                } else {
                    q.z = 0.0f;
                    q.y = 0.0f;
                    q.x = 0.0f;
                    q.w = 1.0f;
                }
                fQuatWeight += fWeight;
                BCS_Quat_Slerp(quat, quat, q, fWeight * nw4r::math::FInv(fQuatWeight));
            } else if (!(nFlag & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                pResult->rt[0][0] += pLayer->rt[0][0] * fRate;
                pResult->rt[0][1] += pLayer->rt[0][1] * fRate;
                pResult->rt[0][2] += pLayer->rt[0][2] * fRate;
                pResult->rt[1][0] += pLayer->rt[1][0] * fRate;
                pResult->rt[1][1] += pLayer->rt[1][1] * fRate;
                pResult->rt[1][2] += pLayer->rt[1][2] * fRate;
            } else {
                pResult->rt[0][0] += fRate;
                pResult->rt[1][1] += fRate;
            }

            if (!(nFlag & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                pResult->rt[0][3] += pLayer->rt[0][3] * fRate;
                pResult->rt[1][3] += pLayer->rt[1][3] * fRate;
                pResult->rt[2][3] += pLayer->rt[2][3] * fRate;
            }
            pResult->flags &= nFlag;
        }

        if (bLogScale) {
            pResult->s.x = nw4r::math::FExp(fLogX);
            pResult->s.y = nw4r::math::FExp(fLogY);
            pResult->s.z = nw4r::math::FExp(fLogZ);
        }

        if (bQuat) {
            nw4r::math::VEC3 trans(pResult->rt[0][3], pResult->rt[1][3], pResult->rt[2][3]);
            PSMTXQuat(pResult->rt, reinterpret_cast<const Quaternion *>(&quat));
            pResult->rt[0][3] = trans.x;
            pResult->rt[1][3] = trans.y;
            pResult->rt[2][3] = trans.z;
        } else {
            // Make the summed rows orthonormal again: z = x * y, normalise
            // x and z, then y = z * x.
            nw4r::math::VEC3 *pX = reinterpret_cast<nw4r::math::VEC3 *>(pResult->rt[0]);
            nw4r::math::VEC3 *pY = reinterpret_cast<nw4r::math::VEC3 *>(pResult->rt[1]);
            nw4r::math::VEC3 *pZ = reinterpret_cast<nw4r::math::VEC3 *>(pResult->rt[2]);
            PSVECCrossProduct(reinterpret_cast<const Vec *>(pX), reinterpret_cast<const Vec *>(pY), reinterpret_cast<Vec *>(pZ));
            NormalizeOrZero(pX);
            NormalizeOrZero(pZ);
            PSVECCrossProduct(reinterpret_cast<const Vec *>(pZ), reinterpret_cast<const Vec *>(pX), reinterpret_cast<Vec *>(pY));
        }

        pResult->flags &= 0x7FFFFFFF;
        return pResult;
    }

    const nw4r::g3d::ChrAnmResult *CAnmBlendMotion::GetResult(nw4r::g3d::ChrAnmResult *, unsigned long nNode)
    {
        return &m_pResult[0][nNode];
    }

    const CAnmObjMotBase::NODE_EX_INFO *CAnmBlendMotion::GetNodeExInfo() const
    {
        return m_pNodeExInfo;
    }

    void CAnmBlendMotion::SetFrame(float frame)
    {
        SetAnmFrame(3, frame);
    }

    void CAnmBlendMotion::SetAnmFrame(unsigned long nMask, float fFrame)
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                for (unsigned long j = 0; j < m_nLayerNum; j++) {
                    (m_pMotion + i * 2 * m_nLayerNum + m_nCur[i] * m_nLayerNum + j)->SetFrame(fFrame);
                }
            }
        }
    }

    float CAnmBlendMotion::GetFrame() const
    {
        return GetAnmFrame(3);
    }

    float CAnmBlendMotion::GetAnmFrame(unsigned long nMask) const
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                return GetMotion(i, m_nCur[i], 0)->GetFrame();
            }
        }
        return 0.0f;
    }

    float CAnmBlendMotion::VFunc28() const
    {
        return VFunc58(3);
    }

    float CAnmBlendMotion::VFunc58(long nMask) const
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                return GetMotion(i, m_nCur[i], 0)->VFunc28();
            }
        }
        return 0.0f;
    }

    void CAnmBlendMotion::SetRate(float rate)
    {
        SetAnmRate(3, rate);
    }

    void CAnmBlendMotion::SetAnmRate(unsigned long nMask, float fRate)
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                for (unsigned long j = 0; j < m_nLayerNum; j++) {
                    (m_pMotion + i * 2 * m_nLayerNum + m_nCur[i] * m_nLayerNum + j)->SetRate(fRate);
                    if (m_RateLerp[i][m_nCur[i]].fT > 0.0f) {
                        m_RateLerp[i][m_nCur[i]].Set(fRate);
                    }
                }
            }
        }
    }

    void CAnmBlendMotion::ChgAnmRate(unsigned long nMask, float fRate, float fFrame)
    {
        if (fFrame <= 0.0f) {
            SetAnmRate(nMask, fRate);
            return;
        }

        float fStep = 1.0f / fFrame;
        for (int i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                float fNow = GetAnmRate(1 << i);
                LERP &rLerp = m_RateLerp[i][m_nCur[i]];
                rLerp.fStart = fNow;
                rLerp.fEnd = fRate;
                rLerp.fT = 1.0f;
                rLerp.fStep = fStep;
            }
        }
    }

    void CAnmBlendMotion::ChgBlend(unsigned long nMask, long nNum, const float *pWeight, float fFrame)
    {
        if (fFrame <= 0.0f) {
            SetBlend(pWeight, nNum, nMask);
            return;
        }

        float fStep = 1.0f / fFrame;
        for (int i = 0; i < m_nPartNum; i++) {
            if (!(nMask & sPartMask[m_nPartNum][i])) {
                continue;
            }
            long j;
            for (j = 0; j < nNum; j++) {
                LERP &rLerp = m_WeightLerp[i][m_nCur[i]][j];
                rLerp.fStart = m_fWeight[i][m_nCur[i]][j];
                rLerp.fEnd = pWeight[j];
                rLerp.fT = 1.0f;
                rLerp.fStep = fStep;
            }
            for (; j < LAYER_MAX; j++) {
                m_WeightLerp[i][m_nCur[i]][j].Reset();
            }
        }
    }

    float CAnmBlendMotion::GetRate() const
    {
        return GetAnmRate(3);
    }

    float CAnmBlendMotion::GetAnmRate(unsigned long nMask) const
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                return GetMotion(i, m_nCur[i], 0)->GetRate();
            }
        }
        return 1.0f;
    }

    void CAnmBlendMotion::UpdateFrame(float fStep)
    {
        int nPlaying = 0;
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (!(m_nFlag[i] & FLAG_PLAY)) {
                continue;
            }

            for (unsigned long k = 0; k < BANK_NUM; k++) {
                if (m_RateLerp[i][k].fT > 0.0f) {
                    float fRate = m_RateLerp[i][k].Update(fStep);
                    CAnmChrMotion *pMotion = GetMotion(i, k, 0);
                    for (int j = 0; j < m_nLayerNum; j++, pMotion++) {
                        pMotion->SetRate(fRate);
                    }
                }
            }

            for (unsigned long k = 0; k < BANK_NUM; k++) {
                for (unsigned long j = 0; j < m_nLayerNum; j++) {
                    if (m_WeightLerp[i][k][j].fT > 0.0f) {
                        float fRate = (m_pMotion + i * 2 * m_nLayerNum + k * m_nLayerNum + j)->GetRate();
                        m_fWeight[i][k][j] = m_WeightLerp[i][k][j].Update(fStep * fRate);
                    }
                }
            }

            if (m_nFlag[i] & FLAG_BLEND) {
                float fRate = GetMotion(i, m_nCur[i], 0)->GetRate();
                m_fBlendRate[i] -= fStep * m_fBlendStep[i] * __fabsf(fRate);
                if (m_fBlendRate[i] <= 0.0f) {
                    m_nFlag[i] &= ~FLAG_BLEND_MASK;
                    m_fBlendRate[i] = 0.0f;
                }
            }

            if (!(m_nFlag[i] & FLAG_PAUSE)) {
                CAnmChrMotion *pMotion = GetMotion(i, m_nCur[i], 0);
                for (int j = 0; j < m_nLayerNum; j++, pMotion++) {
                    pMotion->UpdateFrame(fStep);
                }
            }
            if ((m_nFlag[i] & (FLAG_BLEND | FLAG_BLEND_LIVE)) == (FLAG_BLEND | FLAG_BLEND_LIVE)) {
                CAnmChrMotion *pMotion = GetMotion(i, 1 - m_nCur[i], 0);
                for (int j = 0; j < m_nLayerNum; j++, pMotion++) {
                    pMotion->UpdateFrame(fStep);
                }
            }
            nPlaying++;
        }

        if (nPlaying > 0) {
            if (m_nPartNum <= 1) {
                CalcResult();
            } else {
                CalcResultParts();
            }
        }
    }

    int CAnmBlendMotion::IsLooped() const
    {
        return IsLooped(3);
    }

    int CAnmBlendMotion::IsLooped(unsigned long nMask) const
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && !GetMotion(i, m_nCur[i], 0)->IsLooped()) {
                return 0;
            }
        }
        return 1;
    }

    int CAnmBlendMotion::VFunc3C() const
    {
        return IsLoop(3);
    }

    int CAnmBlendMotion::IsLoop(unsigned long nMask) const
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && !GetMotion(i, m_nCur[i], 0)->VFunc3C()) {
                return 0;
            }
        }
        return 1;
    }

    int CAnmBlendMotion::IsEnd() const
    {
        return IsEnd(3);
    }

    int CAnmBlendMotion::IsEnd(unsigned long nMask) const
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            // The original tests the first part's play flag for every part
            // (m_nFlag[0], not m_nFlag[i]); kept as it is.
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[0] & FLAG_PLAY) &&
                !GetMotion(i, m_nCur[i], 0)->IsEnd()) {
                return 0;
            }
        }
        return 1;
    }

    int CAnmBlendMotion::VFunc98(long nMask, unsigned long nLayer) const
    {
        if (nMask == 0) {
            return 0;
        }
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[i] & FLAG_PLAY) && m_fWeight[i][m_nCur[i]][nLayer] > 0.0f) {
                return GetMotion(i, m_nCur[i], nLayer)->VFunc98(3, 0);
            }
        }
        return 0;
    }

    int CAnmBlendMotion::VFunc9C(long nMask, unsigned long nLayer) const
    {
        if (nMask == 0) {
            return 0;
        }
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[i] & FLAG_PLAY) && m_fWeight[i][m_nCur[i]][nLayer] > 0.0f) {
                return GetMotion(i, m_nCur[i], nLayer)->VFunc9C(3, 0);
            }
        }
        return 0;
    }

    int CAnmBlendMotion::VFuncA0(long nMask) const
    {
        if (nMask == 0) {
            return 0;
        }
        for (int i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[i] & FLAG_PLAY)) {
                return (m_nFlag[i] >> 1) & 1;
            }
        }
        return 0;
    }

    float CAnmBlendMotion::VFuncA4(long nMask) const
    {
        if (nMask == 0) {
            return 0.0f;
        }
        for (int i = 0; i < m_nPartNum; i++) {
            if (nMask & sPartMask[m_nPartNum][i]) {
                return m_fBlendFrame[i];
            }
        }
        return 0.0f;
    }

    void CAnmBlendMotion::VFuncA8(int bLoop)
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if (m_nFlag[i] & FLAG_PLAY) {
                for (int j = 0; j < m_nLayerNum; j++) {
                    (m_pMotion + i * 2 * m_nLayerNum + m_nCur[i] * m_nLayerNum)[j].VFuncA8(bLoop);
                }
            }
        }
    }

    long CAnmBlendMotion::VFunc64(long nMask, unsigned long nLayer) const
    {
        if (nMask == 0) {
            return -1;
        }
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[i] & FLAG_PLAY) && m_fWeight[i][m_nCur[i]][nLayer] > 0.0f) {
                return GetMotion(i, m_nCur[i], nLayer)->VFunc64(3, 0);
            }
        }
        return -1;
    }

    float CAnmBlendMotion::GetBlendWeight(unsigned long nMask, unsigned long nLayer) const
    {
        if (nMask == 0) {
            return 0.0f;
        }
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[i] & FLAG_PLAY)) {
                return m_fWeight[i][m_nCur[i]][nLayer];
            }
        }
        return 0.0f;
    }

    long CAnmBlendMotion::GetNodeMotID(unsigned long nNode, unsigned long nLayer) const
    {
        return VFunc64(m_pNodeExInfo[nNode].nFlag, nLayer);
    }

    const char *CAnmBlendMotion::GetAnmName(unsigned long nMask, unsigned long nLayer) const
    {
        if (nMask == 0) {
            return 0;
        }
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[i] & FLAG_PLAY) && m_fWeight[i][m_nCur[i]][nLayer] > 0.0f) {
                return GetMotion(i, m_nCur[i], nLayer)->GetAnmName(3, 0);
            }
        }
        return 0;
    }

    const void *CAnmBlendMotion::GetMotExtInfo(unsigned long nMask, unsigned long nLayer) const
    {
        if (nMask == 0) {
            return 0;
        }
        if (m_pExtInfo == 0) {
            return 0;
        }
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            if ((nMask & sPartMask[m_nPartNum][i]) && (m_nFlag[i] & FLAG_PLAY) && m_fWeight[i][m_nCur[i]][nLayer] > 0.0f) {
                CAnmChrMotion *pMotion = GetMotion(i, m_nCur[i], nLayer);
                long nMotID = pMotion->VFunc64(3, 0);
                if (pMotion->VFunc98(3, 0)) {
                    nMotID = -nMotID;
                }
                return m_pExtInfo->GetExt(nMotID);
            }
        }
        return 0;
    }

    const char *CAnmBlendMotion::GetNodeAnmName(unsigned long nNode, unsigned long nLayer) const
    {
        return GetAnmName(m_pNodeExInfo[nNode].nFlag, nLayer);
    }

    unsigned long CAnmBlendMotion::GetResAnmNum() const
    {
        if (m_pExtInfo != 0) {
            return m_pExtInfo->m_nAnmNum;
        }
        return m_File.GetResAnmChrNumEntries();
    }

    inline nw4r::g3d::ResAnmChr CAnmBlendMotion::GetResAnmChr(long nAnim) const
    {
        unsigned long nIndex = m_pExtInfo != 0 ? m_pExtInfo->GetResAnmIndex(nAnim) : abs(nAnim);
        // The result is not used: what is left of a range check.
        m_File.GetResAnmChrNumEntries();
        return m_File.GetResAnmChr(nIndex);
    }

    float CAnmBlendMotion::VFunc88(long nAnim) const
    {
        return GetResAnmChr(nAnim).GetNumFrame();
    }

    int CAnmBlendMotion::VFunc8C(long nAnim) const
    {
        return GetResAnmChr(nAnim).GetAnmPolicy() == 1;
    }

    const void *CAnmBlendMotion::GetResMotExtInfo(long nAnim) const
    {
        if (m_pExtInfo == 0) {
            return 0;
        }
        return m_pExtInfo->GetExt(nAnim);
    }

    const char *CAnmBlendMotion::GetResAnmName(long nAnim) const
    {
        return GetResAnmChr(nAnim).GetName();
    }

    void CAnmBlendMotion::SetBlend(const float *pWeight, unsigned long nNum, unsigned long nMask)
    {
        for (int i = 0; i < m_nPartNum; i++) {
            if (!(nMask & sPartMask[m_nPartNum][i])) {
                continue;
            }
            unsigned long j;
            for (j = 0; j < m_nLayerNum && j < nNum; j++) {
                m_fWeight[i][m_nCur[i]][j] = pWeight[j];
            }
            for (; j < m_nLayerNum; j++) {
                m_fWeight[i][m_nCur[i]][j] = 0.0f;
            }
        }
    }

    void CAnmBlendMotion::SetTrgCallback(void (*pFunc)(const MOT_TRG *pTrg, void *pUser), void *pUser)
    {
        for (unsigned long i = 0; i < m_nPartNum; i++) {
            GetMotion(i, 0, 0)->SetTrgCallback(pFunc, pUser);
            GetMotion(i, 1, 0)->SetTrgCallback(pFunc, pUser);
        }
    }

    float CAnmBlendMotion::SearchTrgFrame(long nAnim, unsigned long nTrg, float fFrame) const
    {
        return FindTrgFrame(m_pExtInfo != 0 ? m_pExtInfo->GetTrgList(nAnim) : 0, nTrg, fFrame);
    }

    void CAnmBlendMotion::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }

    int CAnmMatTable::IsBound(unsigned long nMat) const
    {
        return (m_pTable[nMat] & 0xC000) == 0;
    }
}

namespace bcs {

    void CAnmTexSrt::Release()
    {
        UnbindAll();
    }

    CAnmTexSrt::CAnmTexSrt() : m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmTexSrt::~CAnmTexSrt()
    {
        Destroy();
    }

    int CAnmTexSrt::Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexSrt res, int bAllocResult, long nHeap)
    {
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }

        m_Res = res;
        if (bAllocResult != 0 && m_Res.GetNumMaterial() > 0) {
            m_nResultNum = m_Res.GetNumMaterial();
            m_pResult = new (nHeap | 0x100) nw4r::g3d::TexSrtAnmResult[m_nResultNum];
        }

        m_nNum = mdl.GetResMatNumEntries();
        m_pTable = new (nHeap | 0x100) unsigned short[m_nNum];
        UnbindAll();
        InitFrame();
        Bind(mdl);
        return 1;
    }

    void CAnmTexSrt::SetFrame(float frame)
    {
        m_Frame.SetFrame(frame);
        CalcResult();
    }

    void CAnmTexSrt::UpdateFrame(float fStep)
    {
        m_Frame.SetFrame(m_Frame.m_fRate * fStep + m_Frame.m_fFrame);
        CalcResult();
    }

    int CAnmTexSrt::Bind(nw4r::g3d::ResMdl mdl)
    {
        int num = m_Res.GetNumMaterial();
        int bBound = 0;
        for (unsigned short i = 0; i < num; i++) {
            nw4r::g3d::ResMat mat = mdl.GetResMat(m_Res.GetMatAnm(i)->GetName());
            if (mat.IsValid()) {
                m_pTable[mat.GetID()] = i;
                bBound = 1;
            }
        }
        OnFlag(ANMFLAG_4);
        return bBound;
    }

    int CAnmTexSrt::ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexSrt res)
    {
        UnbindAll();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }
        if (m_pResult != 0 && m_nResultNum < res.GetNumMaterial()) {
            return 0;
        }
        if (m_nNum < mdl.GetResMatNumEntries()) {
            return 0;
        }

        m_Res = res;
        InitFrame();
        return Bind(mdl);
    }

    const nw4r::g3d::TexSrtAnmResult *CAnmTexSrt::GetResult(nw4r::g3d::TexSrtAnmResult *pResult, unsigned long idx)
    {
        unsigned short id = m_pTable[idx];
        if (id & 0xC000) {
            pResult->flags = 0;
            return pResult;
        }
        if (m_pResult != 0) {
            return &m_pResult[id];
        }
        m_Res.GetAnmResult(pResult, id, m_Frame.m_fFrame);
        return pResult;
    }

    void CAnmTexSrt::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }
}

namespace bcs {

    void CAnmTexPat::Release()
    {
        UnbindAll();
    }

    CAnmTexPat::CAnmTexPat() : m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmTexPat::CAnmTexPat(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexPat res, int bAllocResult, long nHeap)
        : m_pResult(0), m_nResultNum(0)
    {
        Create(pMdl, res, bAllocResult, nHeap);
    }

    CAnmTexPat::~CAnmTexPat()
    {
        Destroy();
    }

    int CAnmTexPat::Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmTexPat res, int bAllocResult, long nHeap)
    {
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }

        m_Res = res;
        if (bAllocResult != 0 && m_Res.GetNumMaterial() > 0) {
            m_nResultNum = m_Res.GetNumMaterial();
            m_pResult = new (nHeap | 0x100) nw4r::g3d::TexPatAnmResult[m_nResultNum];
        }

        m_nNum = mdl.GetResMatNumEntries();
        m_pTable = new (nHeap | 0x100) unsigned short[m_nNum];
        // UnbindAll() written out: with the helper, Create is short enough
        // for our compiler to inline it into the constructor above, where
        // the original calls it.
        for (unsigned long i = 0; i < m_nNum; i++) {
            m_pTable[i] = 0x8000;
        }
        OffAnmFlag(ANMFLAG_4);
        InitFrame();
        Bind(mdl);
        return 1;
    }

    void CAnmTexPat::SetFrame(float frame)
    {
        m_Frame.SetFrame(frame);
        CalcResult();
    }

    void CAnmTexPat::UpdateFrame(float fStep)
    {
        m_Frame.SetFrame(m_Frame.m_fRate * fStep + m_Frame.m_fFrame);
        CalcResult();
    }

    int CAnmTexPat::Bind(nw4r::g3d::ResMdl mdl)
    {
        int num = m_Res.GetNumMaterial();
        int bBound = 0;
        for (unsigned short i = 0; i < num; i++) {
            nw4r::g3d::ResMat mat = mdl.GetResMat(m_Res.GetMatAnm(i)->GetName());
            if (mat.IsValid()) {
                m_pTable[mat.GetID()] = i;
                bBound = 1;
            }
        }
        OnFlag(ANMFLAG_4);
        return bBound;
    }

    int CAnmTexPat::ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat res)
    {
        UnbindAll();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }
        if (m_pResult != 0 && m_nResultNum < res.GetNumMaterial()) {
            return 0;
        }
        if (m_nNum < mdl.GetResMatNumEntries()) {
            return 0;
        }

        m_Res = res;
        InitFrame();
        return Bind(mdl);
    }

    const nw4r::g3d::TexPatAnmResult *CAnmTexPat::GetResult(nw4r::g3d::TexPatAnmResult *pResult, unsigned long idx)
    {
        unsigned short id = m_pTable[idx];
        if (id & 0xC000) {
            pResult->bTexExist = 0;
            pResult->bPlttExist = 0;
            return pResult;
        }
        if (m_pResult != 0) {
            return &m_pResult[id];
        }
        m_Res.GetAnmResult(pResult, id, m_Frame.m_fFrame);
        return pResult;
    }

    void CAnmTexPat::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }
}

namespace bcs {

    void CAnmShp::Release()
    {
        UnbindAll();
    }

    CAnmShp::CAnmShp() : m_pVtxSet(0), m_nVtxSetNum(0), m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmShp::~CAnmShp()
    {
        Destroy();
    }

    int CAnmShp::Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmShp res, int bAllocResult, long nHeap)
    {
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }

        m_Res = res;
        if (bAllocResult != 0 && m_Res.GetNumShpAnm() > 0) {
            m_nResultNum = m_Res.GetNumShpAnm();
            m_pResult = new (nHeap | 0x100) nw4r::g3d::ShpAnmResult[m_nResultNum];
        }

        m_nNum = mdl.GetResVtxPosNumEntries();
        m_pTable = new (nHeap | 0x100) unsigned short[m_nNum];
        UnbindAll();

        m_nVtxSetNum = m_Res.GetNumVtxName();
        m_pVtxSet = new (nHeap | 0x100) nw4r::g3d::ShpAnmVtxSet[m_nVtxSetNum];
        InitFrame();
        Bind(mdl);
        return 1;
    }

    void CAnmShp::SetFrame(float frame)
    {
        m_Frame.SetFrame(frame);
        CalcResult();
    }

    void CAnmShp::UpdateFrame(float fStep)
    {
        m_Frame.SetFrame(m_Frame.m_fRate * fStep + m_Frame.m_fFrame);
        CalcResult();
    }

    int CAnmShp::Bind(nw4r::g3d::ResMdl mdl)
    {
        int bBound = 0;
        unsigned long num = m_Res.GetNumShpAnm();
        for (unsigned short i = 0; i < num; i++) {
            nw4r::g3d::ResVtxPos pos = mdl.GetResVtxPos(m_Res.GetShpAnm(i)->GetName());
            if (pos.IsValid()) {
                m_pTable[pos.GetID()] = i;
                bBound = 1;
            }
        }

        int nVtxName = m_Res.GetNumVtxName();
        const long *pNames = m_Res.GetVtxNameArray();
        for (int i = 0; i < nVtxName; i++) {
            nw4r::g3d::ResName name(reinterpret_cast<const nw4r::g3d::ResNameData *>(
                reinterpret_cast<const char *>(pNames) + pNames[i] - 4));
            nw4r::g3d::ShpAnmVtxSet &set = m_pVtxSet[i];
            set.resVtxPos = mdl.GetResVtxPos(name);
            set.resVtxNrm = mdl.GetResVtxNrm(name);
            set.resVtxClr = mdl.GetResVtxClr(name);
        }
        OnFlag(ANMFLAG_4);
        return bBound;
    }

    int CAnmShp::ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmShp res)
    {
        UnbindAll();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }
        if (m_pResult != 0 && m_nResultNum < res.GetNumShpAnm()) {
            return 0;
        }
        if (m_nNum < mdl.GetResVtxPosNumEntries()) {
            return 0;
        }
        if (m_nVtxSetNum < res.GetNumVtxName()) {
            return 0;
        }

        m_Res = res;
        InitFrame();
        return Bind(mdl);
    }

    const nw4r::g3d::ShpAnmResult *CAnmShp::GetResult(nw4r::g3d::ShpAnmResult *pResult, unsigned long idx)
    {
        unsigned short id = m_pTable[idx];
        if (id & 0xC000) {
            pResult->flags = 0;
            return pResult;
        }
        if (m_pResult != 0) {
            return &m_pResult[id];
        }
        m_Res.GetAnmResult(pResult, id, m_Frame.m_fFrame, m_pVtxSet);
        return pResult;
    }

    void CAnmShp::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }

    void CAnmMatClr::Release()
    {
        UnbindAll();
    }

    CAnmMatClr::CAnmMatClr() : m_pResult(0), m_nResultNum(0)
    {
    }

    CAnmMatClr::~CAnmMatClr()
    {
        Destroy();
    }

    int CAnmMatClr::Create(const CMdlObj *pMdl, nw4r::g3d::ResAnmClr res, int bAllocResult, long nHeap)
    {
        nw4r::g3d::ResMdl mdl = pMdl->GetResMdl();
        if (!mdl.IsValid()) {
            return 0;
        }

        m_Res = res;
        int num = mdl.GetResMatNumEntries();
        if (bAllocResult != 0 && num > 0) {
            m_nResultNum = num;
            m_pResult = new (nHeap | 0x100) nw4r::g3d::ClrAnmResult[num];
        }

        m_nNum = num;
        m_pTable = new (nHeap | 0x100) unsigned short[num];
        UnbindAll();
        if (m_Res.IsValid()) {
            InitFrame();
            Bind(mdl);
        }
        return 1;
    }

    void CAnmMatClr::SetFrame(float frame)
    {
        m_Frame.SetFrame(frame);
        CalcResult();
    }

    void CAnmMatClr::UpdateFrame(float fStep)
    {
        m_Frame.SetFrame(m_Frame.m_fRate * fStep + m_Frame.m_fFrame);
        CalcResult();
    }

    int CAnmMatClr::Bind(nw4r::g3d::ResMdl mdl)
    {
        int bBound = 0;
        int num = m_Res.GetNumMaterial();
        for (unsigned short i = 0; i < num; i++) {
            nw4r::g3d::ResMat mat = mdl.GetResMat(m_Res.GetMatAnm(i)->GetName());
            if (mat.IsValid()) {
                m_pTable[mat.GetID()] = i;
                bBound = 1;
            }
        }
        OnFlag(ANMFLAG_4);
        return bBound;
    }

    int CAnmMatClr::ChangeRes(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr res)
    {
        UnbindAll();
        ResetFrame();
        if (!res.IsValid() || !mdl.IsValid()) {
            return 0;
        }
        if (m_pResult != 0 && m_nResultNum < res.GetNumMaterial()) {
            return 0;
        }
        if (m_nNum < mdl.GetResMatNumEntries()) {
            return 0;
        }

        m_Res = res;
        InitFrame();
        return Bind(mdl);
    }

    const nw4r::g3d::ClrAnmResult *CAnmMatClr::GetResult(nw4r::g3d::ClrAnmResult *pResult, unsigned long idx)
    {
        unsigned short id = m_pTable[idx];
        if ((id & 0xC000) || !TestAnmFlag(ANMFLAG_4) || !m_Res.IsValid()) {
            pResult->bRgbaExist = 0;
            return pResult;
        }
        if (m_pResult != 0) {
            return &m_pResult[id];
        }
        m_Res.GetAnmResult(pResult, id, m_Frame.m_fFrame);
        return pResult;
    }

    void CAnmMatClr::CalcResult()
    {
        if (m_pResult != 0 && TestAnmFlag(ANMFLAG_4) && m_Res.IsValid()) {
            float frame = m_Frame.m_fFrame;
            for (unsigned long i = 0; i < m_nNum; i++) {
                unsigned short id = m_pTable[i];
                if (!(id & 0x8000)) {
                    m_Res.GetAnmResult(&m_pResult[id & 0x3FFF], id & 0x3FFF, frame);
                }
            }
        }
    }

    void CAnmMatClr::ObjProc(unsigned long nTask, unsigned long, void *pInfo)
    {
        switch (nTask) {
            case 8:
                UpdateFrame(*static_cast<const float *>(pInfo));
                break;
            case 0x10003:
                SetParent(0);
                break;
            case 0x10002:
                SetParent(static_cast<CG3dObj *>(pInfo));
                break;
        }
    }
}

namespace bcs {

    namespace {
        // (name guessed) Takes the scale out of one row of a matrix: rScale
        // gets its length and the row becomes a unit vector (a zero row
        // stays zero).
        inline void ExtractRowScale(FVEC3 &rRow, float &rScale)
        {
            float lenSq = FVEC3SquareMag(&rRow);
            if (lenSq > 0.0f) {
                rScale = nw4r::math::FSqrt(lenSq);
                FVEC3Scale(&rRow, &rRow, 1.0f / rScale);
            } else {
                rScale = 0.0f;
            }
        }

        // Row i of a matrix as a vector.
        inline FVEC3 &MtxRow(float (*pMtx)[4], int i)
        {
            return *reinterpret_cast<FVEC3 *>(pMtx[i]);
        }
    }

    void CalcChrAnmResultFromMtx(nw4r::g3d::ResNode node, nw4r::g3d::ChrAnmResult *pResult,
                                 const nw4r::math::MTX34 *pNodeMtx)
    {
        nw4r::g3d::ResNode parent = node.GetParentNode();
        const nw4r::math::MTX34 &rMtx = pNodeMtx[node.GetMtxID()];
        nw4r::math::MTX34 inv;

        // The node's matrix relative to its parent's.
        PSMTXInverse(pNodeMtx[parent.GetMtxID()], inv);
        float (*pMtx)[4] = pResult->rt;
        PSMTXConcat(inv, rMtx, pMtx);
        pResult->flags = nw4r::g3d::ChrAnmResult::FLAG_ANM_EXISTS;
        if (nw4r::math::MTX34IsIdentity(reinterpret_cast<const nw4r::math::MTX34 *>(pMtx))) {
            pResult->flags |= nw4r::g3d::ChrAnmResult::FLAG_MTX_IDENT | nw4r::g3d::ChrAnmResult::FLAG_ROT_TRANS_ZERO |
                              nw4r::g3d::ChrAnmResult::FLAG_SCALE_ONE | nw4r::g3d::ChrAnmResult::FLAG_SCALE_UNIFORM |
                              nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO | nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO;
            return;
        }

        ExtractRowScale(MtxRow(pMtx, 0), pResult->s.x);
        ExtractRowScale(MtxRow(pMtx, 1), pResult->s.y);
        ExtractRowScale(MtxRow(pMtx, 2), pResult->s.z);
        if (pResult->s.x == pResult->s.y && pResult->s.x == pResult->s.z) {
            pResult->flags |= nw4r::g3d::ChrAnmResult::FLAG_SCALE_UNIFORM;
            if (pResult->s.x == 1.0f) {
                pResult->flags |= nw4r::g3d::ChrAnmResult::FLAG_SCALE_ONE;
            }
        }
        if (pMtx[0][3] == 0.0f && pMtx[0][3] == pMtx[1][3] && pMtx[0][3] == pMtx[2][3]) {
            pResult->flags |= nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO;
        }
    }

    namespace {
        // Adds the scale of pResult, weighted by fWeight, to pOut's.
        inline void AddScale(nw4r::g3d::ChrAnmResult *pOut, const nw4r::g3d::ChrAnmResult *pResult, float fWeight)
        {
            if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_SCALE_ONE)) {
                pOut->s.x += pResult->s.x * fWeight;
                pOut->s.y += pResult->s.y * fWeight;
                pOut->s.z += pResult->s.z * fWeight;
            } else {
                pOut->s.x += fWeight;
                pOut->s.y += fWeight;
                pOut->s.z += fWeight;
            }
        }

        // The rotation of a result as a quaternion.
        inline void GetRotQuat(QUATERNION &rQuat, const nw4r::g3d::ChrAnmResult *pResult)
        {
            if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                BCS_Quat_FromMatrix(rQuat, *reinterpret_cast<const FMTX34 *>(pResult->rt));
            } else {
                rQuat.z = 0.0f;
                rQuat.y = 0.0f;
                rQuat.x = 0.0f;
                rQuat.w = 1.0f;
            }
        }

        // Adds the x and y axes of pResult, weighted by fWeight, to pOut's.
        inline void AddRotAxes(nw4r::g3d::ChrAnmResult *pOut, const nw4r::g3d::ChrAnmResult *pResult, float fWeight)
        {
            if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_ROT_ZERO)) {
                pOut->rt[0][0] += pResult->rt[0][0] * fWeight;
                pOut->rt[0][1] += pResult->rt[0][1] * fWeight;
                pOut->rt[0][2] += pResult->rt[0][2] * fWeight;
                pOut->rt[1][0] += pResult->rt[1][0] * fWeight;
                pOut->rt[1][1] += pResult->rt[1][1] * fWeight;
                pOut->rt[1][2] += pResult->rt[1][2] * fWeight;
            } else {
                pOut->rt[0][0] += fWeight;
                pOut->rt[1][1] += fWeight;
            }
        }

        // Adds the translation of pResult, weighted by fWeight, to pOut's.
        inline void AddTrans(nw4r::g3d::ChrAnmResult *pOut, const nw4r::g3d::ChrAnmResult *pResult, float fWeight)
        {
            if (!(pResult->flags & nw4r::g3d::ChrAnmResult::FLAG_TRANS_ZERO)) {
                pOut->rt[0][3] += pResult->rt[0][3] * fWeight;
                pOut->rt[1][3] += pResult->rt[1][3] * fWeight;
                pOut->rt[2][3] += pResult->rt[2][3] * fWeight;
            }
        }
    }

    void BlendChrAnmResult(nw4r::g3d::ChrAnmResult *pOut, const nw4r::g3d::ChrAnmResult *pResult,
                           const nw4r::g3d::ChrAnmResult *pPrev, int bFlag, float fWeight, float fPrevWeight)
    {
        pOut->flags = 0xFFFFFFFF;
        pOut->s.z = 0.0f;
        pOut->s.y = 0.0f;
        pOut->s.x = 0.0f;
        nw4r::math::MTX34Zero(reinterpret_cast<nw4r::math::MTX34 *>(pOut->rt));
        AddScale(pOut, pResult, fWeight);
        AddScale(pOut, pPrev, fPrevWeight);

        if (bFlag) {
            // Rotations blended as quaternions.
            QUATERNION quat;
            QUATERNION quatPrev;
            QUATERNION quatOut;
            GetRotQuat(quat, pResult);
            GetRotQuat(quatPrev, pPrev);
            BCS_Quat_Slerp(quatOut, quatPrev, quat, fWeight);
            PSMTXQuat(pOut->rt, reinterpret_cast<const Quaternion *>(&quatOut));
        } else {
            // The x and y axes blended, then made orthonormal again.
            AddRotAxes(pOut, pResult, fWeight);
            AddRotAxes(pOut, pPrev, fPrevWeight);
            FVEC3 &rX = MtxRow(pOut->rt, 0);
            FVEC3 &rY = MtxRow(pOut->rt, 1);
            FVEC3 &rZ = MtxRow(pOut->rt, 2);
            PSVECCrossProduct(reinterpret_cast<const Vec *>(&rX), reinterpret_cast<const Vec *>(&rY), reinterpret_cast<Vec *>(&rZ));
            BCS_Vec_Normalize(rX, rX);
            BCS_Vec_Normalize(rZ, rZ);
            PSVECCrossProduct(reinterpret_cast<const Vec *>(&rZ), reinterpret_cast<const Vec *>(&rX), reinterpret_cast<Vec *>(&rY));
        }

        pOut->rt[2][3] = 0.0f;
        pOut->rt[1][3] = 0.0f;
        pOut->rt[0][3] = 0.0f;
        AddTrans(pOut, pResult, fWeight);
        AddTrans(pOut, pPrev, fPrevWeight);

        pOut->flags &= pResult->flags & pPrev->flags;
        pOut->flags &= ~0x80000000;
    }

    void ApplyTexSrtAnmResult(nw4r::g3d::ResTexSrt srt, nw4r::g3d::ResMatIndMtxAndScale ind,
                              const nw4r::g3d::TexSrtAnmResult *pResult)
    {
        nw4r::g3d::ApplyTexSrtAnmResult(srt, ind, pResult);
    }
}

namespace nw4r {
    namespace g3d {

        void ApplyTexPatAnmResult(ResTexObj texObj, ResTlutObj tlutObj, const TexPatAnmResult *pResult)
        {
            unsigned long bTex = pResult->bTexExist;
            for (unsigned long i = 0; bTex != 0; bTex >>= 1, i++) {
                if (!(bTex & 1)) {
                    continue;
                }

                ResTex tex = pResult->tex[i];
                GXTexObj *pTexObj = texObj.GetTexObj(static_cast<GXTexMapID>(i));

                // Keep the object's filter and wrap settings.
                GXTexFilter minFilter;
                GXTexFilter magFilter;
                float minLod;
                float maxLod;
                float lodBias;
                unsigned char biasClamp;
                unsigned char edgeLod;
                GXAnisotropy maxAniso;
                GXGetTexObjLODAll(pTexObj, &minFilter, &magFilter, &minLod, &maxLod, &lodBias, &biasClamp, &edgeLod, &maxAniso);
                GXTexWrapMode wrapS = GXGetTexObjWrapS(pTexObj);
                GXTexWrapMode wrapT = GXGetTexObjWrapT(pTexObj);

                void *pImage;
                GXTexFmt fmt;
                GXCITexFmt fmtCI;
                unsigned short width;
                unsigned short height;
                unsigned char mipmap;
                if (tex.IsCIFmt()) {
                    tex.GetTexObjCIParam(&pImage, &width, &height, &fmtCI, &minLod, &maxLod, &mipmap);
                    GXInitTexObjCI(pTexObj, pImage, width, height, fmtCI, wrapS, wrapT, mipmap, i);
                } else {
                    tex.GetTexObjParam(&pImage, &width, &height, &fmt, &minLod, &maxLod, &mipmap);
                    GXInitTexObj(pTexObj, pImage, width, height, fmt, wrapS, wrapT, mipmap);
                }
                GXInitTexObjLOD(pTexObj, minFilter, magFilter, minLod, maxLod, lodBias, biasClamp, edgeLod, maxAniso);
            }

            unsigned long bPltt = pResult->bPlttExist;
            for (unsigned long i = 0; bPltt != 0; bPltt >>= 1, i++) {
                if (!(bPltt & 1)) {
                    continue;
                }

                ResPltt pltt = pResult->pltt[i];
                void *pData = pltt.GetPlttData();
                GXTlutFmt fmt = pltt.GetFmt();
                unsigned short num = pltt.GetNumEntries();
                GXTlutObj *pTlutObj = tlutObj.GetTlut(static_cast<GXTlut>(i));
                GXInitTlutObj(pTlutObj, pData, fmt, num);
            }
        }

        namespace {
            // The animated colour: the bits the animation sets, plus the bits
            // of the current colour its mask keeps.
            inline unsigned long BlendColor(unsigned long rgba, unsigned long mask, const ut::Color &color)
            {
                return rgba | (mask & color.ToU32());
            }
        }

        void ApplyClrAnmResult(ResMatChan chan, ResMatTevColor tev, const ClrAnmResult *pResult)
        {
            ut::Color color;

            if (pResult->bRgbaExist & 0xF) {
                if (pResult->bRgbaExist & 0x1) {
                    GXColor &rDst = chan.ref().chan[0].matColor;
                    color = rDst;
                    rDst = ut::Color(BlendColor(pResult->rgba[0], pResult->rgbaMask[0], color));
                }
                if (pResult->bRgbaExist & 0x4) {
                    GXColor &rDst = chan.ref().chan[0].ambColor;
                    color = rDst;
                    rDst = ut::Color(BlendColor(pResult->rgba[2], pResult->rgbaMask[2], color));
                }
                if (pResult->bRgbaExist & 0x2) {
                    GXColor &rDst = chan.ref().chan[1].matColor;
                    color = rDst;
                    rDst = ut::Color(BlendColor(pResult->rgba[1], pResult->rgbaMask[1], color));
                }
                if (pResult->bRgbaExist & 0x8) {
                    GXColor &rDst = chan.ref().chan[1].ambColor;
                    color = rDst;
                    rDst = ut::Color(BlendColor(pResult->rgba[3], pResult->rgbaMask[3], color));
                }
            }

            if (pResult->bRgbaExist & 0x7F0) {
                if (pResult->bRgbaExist & 0x10) {
                    if (!tev.GXGetTevColor(GX_TEVREG0, color.ToGXColorPtr())) {
                        color = 0xFF;
                    }
                    tev.GXSetTevColor(GX_TEVREG0, ut::Color(BlendColor(pResult->rgba[4], pResult->rgbaMask[4], color)));
                }
                if (pResult->bRgbaExist & 0x20) {
                    if (!tev.GXGetTevColor(GX_TEVREG1, color.ToGXColorPtr())) {
                        color = 0xFF;
                    }
                    tev.GXSetTevColor(GX_TEVREG1, ut::Color(BlendColor(pResult->rgba[5], pResult->rgbaMask[5], color)));
                }
                if (pResult->bRgbaExist & 0x40) {
                    if (!tev.GXGetTevColor(GX_TEVREG2, color.ToGXColorPtr())) {
                        color = 0xFF;
                    }
                    tev.GXSetTevColor(GX_TEVREG2, ut::Color(BlendColor(pResult->rgba[6], pResult->rgbaMask[6], color)));
                }
                if (pResult->bRgbaExist & 0x80) {
                    if (!tev.GXGetTevKColor(GX_KCOLOR0, color.ToGXColorPtr())) {
                        color = 0xFF;
                    }
                    tev.GXSetTevKColor(GX_KCOLOR0, ut::Color(BlendColor(pResult->rgba[7], pResult->rgbaMask[7], color)));
                }
                if (pResult->bRgbaExist & 0x100) {
                    if (!tev.GXGetTevKColor(GX_KCOLOR1, color.ToGXColorPtr())) {
                        color = 0xFF;
                    }
                    tev.GXSetTevKColor(GX_KCOLOR1, ut::Color(BlendColor(pResult->rgba[8], pResult->rgbaMask[8], color)));
                }
                if (pResult->bRgbaExist & 0x200) {
                    if (!tev.GXGetTevKColor(GX_KCOLOR2, color.ToGXColorPtr())) {
                        color = 0xFF;
                    }
                    tev.GXSetTevKColor(GX_KCOLOR2, ut::Color(BlendColor(pResult->rgba[9], pResult->rgbaMask[9], color)));
                }
                if (pResult->bRgbaExist & 0x400) {
                    if (!tev.GXGetTevKColor(GX_KCOLOR3, color.ToGXColorPtr())) {
                        color = 0xFF;
                    }
                    tev.GXSetTevKColor(GX_KCOLOR3, ut::Color(BlendColor(pResult->rgba[10], pResult->rgbaMask[10], color)));
                }
                tev.DCStore(false);
            }
        }
    }
}
