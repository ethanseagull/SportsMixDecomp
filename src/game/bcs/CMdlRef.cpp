#include "bcs/CMdlRef.hpp"
#include "nw4r/g3d/CalcWorld.hpp"
#include "nw4r/g3d/ResMat.hpp"

// The original file is the DOL's whole bcs model file (CMdlObj through
// CMultViewAlphaMdlObj; see notes/toolchain.md, "Learned on DOL
// game/bcs/CMdlShadow"). Its compiler inlined ~CMdlCopyBase (with ~CMdlObj)
// into ~CMdlRef and SetLink into the constructor; here they stay calls.

namespace bcs {

    inline void CMdlRef::FlipCullMode()
    {
        unsigned long nMatNum = GetResMdl().GetResMatNumEntries();
        for (unsigned long i = 0; i < nMatNum; i++) {
            CopiedMatAccess access(this, i);
            nw4r::g3d::ResGenMode genMode = access.GetResGenMode();
            if (genMode.IsValid()) {
                if (genMode.GXGetCullMode() == GX_CULL_BACK) {
                    genMode.GXSetCullMode(GX_CULL_FRONT);
                } else if (genMode.GXGetCullMode() == GX_CULL_FRONT) {
                    genMode.GXSetCullMode(GX_CULL_BACK);
                }
            }
        }
    }

    CMdlRef::CMdlRef()
    {
    }

    CMdlRef::CMdlRef(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, unsigned long nArg, unsigned long nLinkFlag, long nHeap)
    {
        if (CreateLink(pMdl, mdl, nArg, nLinkFlag, nHeap)) {
            FlipCullMode();
        }
    }

    CMdlRef::~CMdlRef()
    {
    }

    int CMdlRef::Create(nw4r::g3d::ResMdl mdl, unsigned long nArg, long nHeap)
    {
        bool bCreated = CMdlObj::Create(mdl, nArg, nHeap);
        if (!bCreated) {
            return 0;
        }
        FlipCullMode();
        return 1;
    }

    void CMdlRef::CopyNodeMtx(const CMdlObj *pSrc)
    {
        FVEC3 scale(1.0f, -1.0f, 1.0f);
        nw4r::math::MTX34 *pSrcMtx = pSrc->GetNodeMtxArray();
        nw4r::math::MTX34 *pDstMtx = m_pNodeMtx;
        const unsigned long *pSrcAttrib = pSrc->GetNodeMtxAttribArray();
        for (long i = 0; i < m_nNodeNum; i++, pDstMtx++) {
            unsigned short nNode = m_pNodeMap[i];
            if (!(nNode & 0x8000)) {
                PSMTXScaleApply(pSrcMtx[nNode].m, pDstMtx->m, scale.x, scale.y, scale.z);
                m_pNodeMtxAttrib[i] = (m_pNodeMtxAttrib[i] & 0xFFFFFF00) | (pSrcAttrib[nNode] & 0xFF);
            }
        }
        SetMtx(SCNOBJMTXTYPE_ROOT, pSrc->GetMtxPtr(SCNOBJMTXTYPE_ROOT));
        SetMtx(SCNOBJMTXTYPE_1, pSrc->GetMtxPtr(SCNOBJMTXTYPE_1));
        PSMTXScaleApply(m_Mtx[SCNOBJMTXTYPE_ROOT].m, m_Mtx[SCNOBJMTXTYPE_ROOT].m, scale.x, scale.y, scale.z);
        PSMTXScaleApply(m_Mtx[SCNOBJMTXTYPE_1].m, m_Mtx[SCNOBJMTXTYPE_1].m, scale.x, scale.y, scale.z);
        if (m_pEnvByteCode != 0) {
            nw4r::g3d::CalcSkinning(m_pNodeMtx, m_pNodeMtxAttrib, GetResMdl(), m_pEnvByteCode);
        }
        OnFlag(0x20000);
    }
}
