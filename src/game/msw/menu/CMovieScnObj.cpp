#include "game/msw/menu/CMoviePlayer.hpp"
#include "game/msw/menu/CMovieScnObj.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/BCS_Draw2D.hpp"
#include "bcs/CMainApp.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/G2DUtil.hpp"

// The original file is wider than this unit: the 0.0f constant the
// constructor loads is shared with the functions from 0x80235D80 on, and the
// static initialiser right after SetDrawTextureST (0x802366DC) builds a
// global whose destructor is at 0x80235D40.

namespace msw {
    namespace menu {

        CMoviePlayer g_MoviePlayer;

        CMoviePlayer::~CMoviePlayer() {}

        void CMoviePlayer::Init()
        {
            if (m_bActive == 0) {
                m_bActive = 1;
                m_nState = STATE_IDLE;
                m_fTimer = 0.0f;
                m_nOpenArg = 1;
                m_bLoop = 0;
                for (int i = 0; i < NAME_LEN; i++) {
                    m_aName[ENTRY_REQUEST][i] = 0;
                    m_aName[ENTRY_OPEN][i] = 0;
                    m_aName[ENTRY_PENDING][i] = 0;
                    m_afArg[ENTRY_REQUEST] = 0.0f;
                    m_afArg[ENTRY_OPEN] = 0.0f;
                    m_afArg[ENTRY_PENDING] = 0.0f;
                }
                bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
                if (pMovie != 0 && pMovie->IsPlay()) {
                    pMovie->Stop();
                }
            }
        }

        void CMoviePlayer::Term()
        {
            if (m_bActive != 0) {
                m_bActive = 0;
                m_nState = STATE_IDLE;
                bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
                if (pMovie != 0 && pMovie->IsPlay()) {
                    pMovie->Stop();
                }
            }
        }

        void CMoviePlayer::Update()
        {
            if (m_bActive) {
                Calc();
            }
        }

        void CMoviePlayer::Calc()
        {
            bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
            if (pMovie == 0) {
                return;
            }
            if (pMovie->IsError()) {
                pMovie->Stop();
                return;
            }
            switch (m_nState) {
                case STATE_IDLE:
                    break;
                case STATE_STOP:
                    m_nState = STATE_IDLE;
                    pMovie->Stop();
                    break;
                case STATE_RESTART:
                    m_nState = STATE_WAIT_OPEN;
                    pMovie->Stop();
                    break;
                case STATE_WAIT_OPEN:
                    if (pMovie->IsWait() == 1 && GetName(ENTRY_REQUEST) != 0) {
                        Copy(ENTRY_PENDING, ENTRY_REQUEST);
                        Copy(ENTRY_OPEN, ENTRY_PENDING);
                        if (pMovie->Open(GetName(ENTRY_OPEN), m_nOpenArg, m_afArg[ENTRY_OPEN]) == 1) {
                            m_nState = STATE_WAIT_READY;
                        }
                    }
                    break;
                case STATE_WAIT_READY:
                    if (pMovie->IsReady()) {
                        unsigned long nFlag = 0;
                        if (m_bLoop == 1) {
                            nFlag = 0x40;
                        }
                        if (pMovie->Start(nFlag)) {
                            m_nState = STATE_WAIT_PLAY;
                        } else {
                            m_nState = STATE_RESTART;
                        }
                    }
                    break;
                case STATE_WAIT_PLAY:
                    if (pMovie->IsPlay()) {
                        m_nState = STATE_PLAY;
                    }
                    break;
                case STATE_PLAY:
                    if (strcmp(GetName(ENTRY_REQUEST), GetName(ENTRY_OPEN)) != 0 || m_afArg[ENTRY_REQUEST] != m_afArg[ENTRY_OPEN]) {
                        m_fTimer = 10.0f;
                        Copy(ENTRY_PENDING, ENTRY_REQUEST);
                        m_nState = STATE_CHANGE_WAIT;
                    }
                    break;
                case STATE_CHANGE_WAIT: {
                    float fTimer = m_fTimer;
                    fTimer -= bcs::CMainApp::GetAppFrameRateF();
                    if (strcmp(GetName(ENTRY_REQUEST), GetName(ENTRY_PENDING)) != 0 || m_afArg[ENTRY_REQUEST] != m_afArg[ENTRY_PENDING]) {
                        // The request changed again: wait again.
                        m_fTimer = 10.0f;
                        Copy(ENTRY_PENDING, ENTRY_REQUEST);
                    } else if (fTimer <= 0.0f) {
                        if (strcmp(GetName(ENTRY_OPEN), GetName(ENTRY_PENDING)) != 0 || m_afArg[ENTRY_OPEN] != m_afArg[ENTRY_PENDING]) {
                            m_fTimer = 0.0f;
                            m_nState = STATE_CHANGE_STOP;
                            Copy(ENTRY_PENDING, ENTRY_REQUEST);
                        } else {
                            m_nState = STATE_PLAY;
                        }
                    } else {
                        m_fTimer = fTimer;
                    }
                    break;
                }
                case STATE_CHANGE_STOP: {
                    float fTimer = m_fTimer;
                    fTimer -= bcs::CMainApp::GetAppFrameRateF();
                    if (fTimer <= 0.0f) {
                        fTimer = 0.0f;
                        pMovie->Stop();
                        m_nState = STATE_RESTART;
                    }
                    m_fTimer = fTimer;
                    break;
                }
            }
        }

        int CMoviePlayer::IsPlaying() const
        {
            int bPlaying = 0;
            if (m_nState >= STATE_PLAY && m_nState <= STATE_CHANGE_WAIT) {
                bPlaying = 1;
            }
            return bPlaying;
        }

        void CMoviePlayer::Request(const char *pName, float fArg)
        {
            if (pName == 0) {
                m_aName[ENTRY_REQUEST][0] = 0;
                m_afArg[ENTRY_REQUEST] = 0.0f;
            } else {
                // The length is unused, as in Copy.
                size_t nLen = strlen(pName);
                strncpy(m_aName[ENTRY_REQUEST], pName, NAME_LEN);
                m_afArg[ENTRY_REQUEST] = fArg;
            }
        }

        int CMoviePlayer::RemapPane(nw4r::lyt::Pane *pPane)
        {
            bcs::G2D::RemapMoviePane(pPane);
            return 0;
        }

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
    }
}
