// bcs::CMdlShadow: a model drawn as the shadow of another model.
//
// In the original this is part of one large file with bcs::CMdlObj and its
// other subclasses (see notes/toolchain.md); only the constructors, the
// destructor and Create are written here.
#include "bcs/CMdlShadow.hpp"
#include "bcs/New.hpp"

namespace bcs {

    CMdlShadow::CMdlShadow()
        : m_nLinkHandle(0)
        , m_nAlpha(0xFF)
        , m_Unk1CD(0)
        , m_nTexSize(0)
        , m_Unk1D0(0)
        , m_pNodeData(0)
        , m_pTexBuf(0)
        , m_pTexObj(&m_TexObj)
        , m_Unk200(0.0f)
    {
    }

    CMdlShadow::CMdlShadow(const CMdlObj *pMdl, nw4r::g3d::ResMdl mdl, long nTexSize, unsigned long nArg, long nHeap)
        : m_nLinkHandle(0)
        , m_nAlpha(0xFF)
        , m_Unk1CD(0)
        , m_nTexSize(0)
        , m_Unk1D0(0)
        , m_pNodeData(0)
        , m_pTexBuf(0)
        , m_pTexObj(&m_TexObj)
        , m_Unk200(0.0f)
    {
        Create(mdl, nTexSize, nArg, nHeap);
        if (pMdl != 0) {
            SetLink(pMdl);
        }
    }

    CMdlShadow::~CMdlShadow()
    {
        if (m_pTexBuf != 0) {
            delete[] (unsigned char *)m_pTexBuf;
        }
        m_pTexBuf = 0;
        if (m_pNodeData != 0) {
            delete[] (unsigned char *)m_pNodeData;
        }
        m_pNodeData = 0;
        m_nLinkHandle = 0;
        m_Unk1D0 = 0;
    }

    int CMdlShadow::Create(nw4r::g3d::ResMdl mdl, long nTexSize, unsigned long nArg, long nHeap)
    {
        if (!CMdlObj::Create(mdl, nArg, nHeap)) {
            return 0;
        }

        m_nTexSize = nTexSize;
        m_pTexBuf = operator new[](GXGetTexBufferSize(nTexSize, nTexSize, GX_TF_I8, 0, 0), nHeap | 0x100);
        GXInitTexObj(&m_TexObj, m_pTexBuf, m_nTexSize, m_nTexSize, GX_TF_I8, GX_CLAMP, GX_CLAMP, 0);
        GXInitTexObjLOD(&m_TexObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
        return 1;
    }

    void CMdlShadow::SetLink(const CMdlObj *pMdl)
    {
        BuildLink(pMdl);
        m_nLinkHandle = pMdl->GetHandle();
    }
}
