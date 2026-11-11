#include "game/msw/menu/CMovieScnObj.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/BCS_Draw2D.hpp"

// The original file is wider than this unit: the 0.0f constant the
// constructor loads is shared with the functions from 0x80235D80 on, and the
// static initialiser right after SetDrawTextureST (0x802366DC) builds a
// global whose destructor is at 0x80235D40.

namespace msw {
    namespace menu {

        // An empty display list: the object only needs one set for the
        // opaque pass to run its ObjProc task 6 (name guessed).
        static unsigned char sDummyDL[0x20] __attribute__((aligned(32)));

        CMovieScnObj::CMovieScnObj()
        {
            SetDrawPriorityOpa(0);
            SetDLOpa(sDummyDL, sizeof(sDummyDL));
            m_fDrawX = 0.0f;
            m_fDrawY = -4.0f;
            m_fDrawW = 640.0f;
            m_fDrawH = 464.0f;
            m_fTexS0 = 0.0f;
            m_fTexT0 = 0.0f;
            m_fTexS1 = 1.0f;
            m_fTexT1 = 1.0f;
        }

        CMovieScnObj::~CMovieScnObj()
        {
            SetDLXlu(0, 0);
            DetachFromParent();
        }

        void CMovieScnObj::ObjProc(unsigned long nTask, unsigned long nParam, void *pInfo)
        {
            if (IsProcDisabled(nTask)) {
                return;
            }
            if (nTask != 6) {
                CGDLObj::ObjProc(nTask, nParam, pInfo);
                return;
            }
            if (!HasDLOpa()) {
                return;
            }
            bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
            if (pMovie != 0 && pMovie->CanbeDraw()) {
                bcs::Draw2D_SetProjection(-1, 0);
                pMovie->BeginDraw();
                pMovie->Draw(m_fDrawX, m_fDrawY, m_fDrawW, m_fDrawH, m_fTexS0, m_fTexT0, m_fTexS1, m_fTexT1);
                pMovie->EndDraw();
            }
        }

        void CMovieScnObj::SetDrawFlag(int bDraw)
        {
            if (bDraw) {
                SetDLOpa(sDummyDL, sizeof(sDummyDL));
            } else {
                SetDLOpa(0, 0);
            }
        }

        void CMovieScnObj::SetDrawPositionXYWH(float fX, float fY, float fW, float fH)
        {
            m_fDrawX = fX;
            m_fDrawY = fY;
            m_fDrawW = fW;
            m_fDrawH = fH;
        }

        void CMovieScnObj::SetDrawTextureST(float fS0, float fT0, float fS1, float fT1)
        {
            m_fTexS0 = fS0;
            m_fTexT0 = fT0;
            m_fTexS1 = fS1;
            m_fTexT1 = fT1;
        }

        // Not in the original: its vtable has no slot for UpdateFrame (it
        // ends at 0x108), but include/bcs/CScnObj.hpp declares one at 0x10C
        // as pure, so the class needs a body to be made. Empty, as nothing
        // calls it.
        void CMovieScnObj::UpdateFrame(float) {}
    }
}
