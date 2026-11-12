#include "bcs/CGDLObj.hpp"
#include "bcs/CScnObjGather.hpp"
#include "nw4r/g3d/G3DState.hpp"
#include "revolution/gx/GXDisplayList.h"

namespace bcs {

    CGDLObj::CGDLObj()
    {
        m_pDLOpa = 0;
        m_pDLXlu = 0;
        m_nDLOpaSize = 0;
        m_nDLXluSize = 0;
    }

    CGDLObj::~CGDLObj()
    {
        DetachFromParent();
    }

    void CGDLObj::SetDLOpa(void *pDL, unsigned long nSize)
    {
        m_pDLOpa = pDL;
        m_nDLOpaSize = nSize;
    }

    void CGDLObj::SetDLXlu(void *pDL, unsigned long nSize)
    {
        m_pDLXlu = pDL;
        m_nDLXluSize = nSize;
    }

    void CGDLObj::ObjProc(unsigned long nTask, unsigned long nParam, void *pInfo)
    {
        if (IsProcDisabled(nTask)) {
            return;
        }

        switch (static_cast<long>(nTask)) {
            case 8:
                // The original tests pInfo and does nothing either way. A
                // guess at what it had: the frame is read and not used,
                // since the class has no animation (UpdateFrame stays pure).
                if (pInfo != 0) {
                    float fFrame = *static_cast<const float *>(pInfo);
                    (void)fFrame;
                }
                break;
            case 1:
                if (GetFlag() & 0x4000) {
                    CalcRootMtx();
                }
                if (!(GetFlag() & 0x8000)) {
                    CalcWorldMtx(static_cast<const FMTX34 *>(pInfo));
                }
                break;
            case 4:
                CalcViewMtx(static_cast<const FMTX34 *>(pInfo));
                break;
            case 5: {
                // The original computes the translucent flag first.
                int bXlu = HasDLXlu();
                static_cast<CScnObjGather *>(pInfo)->Add(this, HasDLOpa(), bXlu, reinterpret_cast<const CFrustum *>(nParam));
                break;
            }
            case 6:
                if (HasDLOpa()) {
                    GXCallDisplayList(m_pDLOpa, m_nDLOpaSize);
                    nw4r::g3d::G3DState::Invalidate(0x7FF);
                }
                break;
            case 7:
                if (HasDLXlu()) {
                    GXCallDisplayList(m_pDLXlu, m_nDLXluSize);
                    nw4r::g3d::G3DState::Invalidate(0x7FF);
                }
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
