#include "bcs/CMovie.hpp"

#include "bcs/CMainApp.hpp"
#include "bcs/CThread.hpp"
#include "nw4r/ut/DvdFileStream.hpp"
#include "revolution/base/PPCArch.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/gx/GXVert.h"
#include "revolution/os/OSCache.h"
#include "revolution/os/OSInterrupt.h"
#include "revolution/os/OSThread.h"
#include "revolution/thp.h"
#include "string.h"

// The original file is wider than this unit: it goes on past EndDraw with
// the movie's own functions (Prepare at 0x8006A458, Close, DecodeFrame, the
// sampling callback), the reading thread's functions (0x8006AF70 to
// 0x8006B24C and after) and the GX set-up for the frames (THPGXRestore at
// 0x8006B86C, THPGXYuv2RgbSetup at 0x8006B984); its float constants
// (0.0f at 0x805C1EF0 to 0x805C1F34) are shared by all of them.

namespace bcs {

    // (fn_8006B86C, fn_8006B984; names of the THP library's sample
    // functions, of which MenuF's msw::THPGXRestore is a copy) Set the GX
    // state to draw a frame from its Y, U and V planes, and back.
    void THPGXRestore();
    void THPGXYuv2RgbSetup(const GXRenderModeObj *pRmode);

    // (class name guessed) Reads the movie file on its own thread. The
    // state (0 idle, 1 asked to open, 3 ready, 4 playing, 5 asked to stop,
    // 6 failed) is shared with the task under a mutex. Its other functions
    // follow later in the original file (destructor fn_8006AF70, Run
    // fn_8006B148, Init fn_8006B024).
    class CMovieThread : public CThread {
        public:
            CMovieThread()
            {
                OSInitMutex(&m_StateMutex);
                OSInitMutex(&m_Mutex);
                m_nState = 0;
                m_pBuffer = new unsigned char[0x80];
                Init();
            }
            virtual ~CMovieThread();
            virtual void Run();

            long GetState()
            {
                OSMutex *pMutex = &m_StateMutex;
                OSLockMutex(pMutex);
                long nState = m_nState;
                OSUnlockMutex(pMutex);
                return nState;
            }
            void SetState(long nState)
            {
                OSMutex *pMutex = &m_StateMutex;
                OSLockMutex(pMutex);
                m_nState = nState;
                OSUnlockMutex(pMutex);
            }

            // Hands the file to the thread when it is idle.
            int Open(const char *pFileName, int nArg0, long nStartFrame)
            {
                if (GetState() != 0) {
                    return 0;
                }
                strcpy(m_szFileName, pFileName);
                m_nArg0 = nArg0;
                m_nStartFrame = nStartFrame;
                SetState(1);
                return 1;
            }
            // From ready to playing.
            int Play()
            {
                if (GetState() != 3) {
                    return 0;
                }
                SetState(4);
                return 1;
            }
            // Asks the thread to stop when a file is open.
            int Stop()
            {
                if (GetState() == 0) {
                    return 0;
                }
                SetState(5);
                return 1;
            }

        private:
            void Init();

            long m_nState;                   // 0x320
            unsigned char *m_pBuffer;        // 0x324
            char m_szFileName[0x100];        // 0x328
            nw4r::ut::DvdFileStream m_Stream; // 0x428
            unsigned char m_Unk498[0xEC];    // 0x498
            int m_nArg0;                     // 0x584
            long m_nStartFrame;              // 0x588
            OSMutex m_StateMutex;            // 0x58C
            OSMutex m_Mutex;                 // 0x5A4
            unsigned long m_Unk5BC;          // 0x5BC
    };

    CMovie *CMovie::s_pInstance;

    void CMovie::CreateInstance()
    {
        s_pInstance = new CMovie();
    }

    CMovie *CMovie::GetInstance()
    {
        return s_pInstance;
    }

    CMovie::CMovie(CTask *pParent, short nPriority) : CTask(pParent, nPriority)
    {
        SetUnk08(0xFC00);
        if (!(PPCMfhid2() & HID2_LCE)) {
            LCEnable();
        }
        THPInit();

        m_nFlags = 0;
        m_fTime = 0.0f;
        m_nStartFrame = 0;
        m_pWork = new unsigned char[0x1000];
        memset(&m_Header, 0, sizeof(m_Header));
        memset(&m_CompInfo, 0, sizeof(m_CompInfo));
        memset(&m_VideoInfo, 0, sizeof(m_VideoInfo));
        memset(&m_AudioInfo, 0, sizeof(m_AudioInfo));
        m_pTextureY = 0;
        m_pTextureU = 0;
        m_pTextureV = 0;
        m_aBuffer[0].m_p0 = 0;
        m_aBuffer[1].m_p0 = 0;
        m_aBuffer[2].m_p0 = 0;
        m_aBuffer[0].m_p1 = 0;
        m_aBuffer[1].m_p1 = 0;
        m_aBuffer[2].m_p1 = 0;
        m_UnkC8 = 0;
        m_nDecodedFrame = -1;
        m_UnkD0 = 0;
        m_UnkD4 = 0;
        m_UnkD8 = 0;
        m_nCurFrame = -1;
        m_aUnkE0[0] = 0;
        m_aUnkE0[1] = 0;
        m_aUnkE0[2] = 0;
        m_aUnkE0[3] = 0;
        m_aUnkE0[4] = 0;
        m_aUnkE0[5] = 0;
        m_aUnkE0[6] = 0;
        m_aUnkE0[7] = 0;
        m_aUnkE0[8] = 0;
        m_aUnkE0[9] = 0;
        m_aUnkE0[10] = 0;
        m_aUnkE0[11] = 0;
        m_pAudioBuffer0 = new unsigned char[0x180];
        m_pAudioBuffer1 = new unsigned char[0x180];
        m_Unk118 = 0;
        m_Unk11C = 0;
        m_Unk120 = 0;
        m_fCurVolume = 127.0f;
        m_fTargetVolume = 127.0f;
        m_fDeltaVolume = 0.0f;
        m_nRampCount = 0;
        m_nDrawX = 0;
        m_nDrawY = 0;
        m_fScaleX = 1.0f;
        m_fScaleY = 1.0f;

        BOOL bEnabled = OSDisableInterrupts();
        m_pOldSamplingCallback = PADSetSamplingCallback(SamplingCallback);
        OSRestoreInterrupts(bEnabled);

        m_pThreadStack = new unsigned char[0x10000];
        m_pThread = new CMovieThread();
        m_pThread->Create(m_pThreadStack + 0x10000, 0x10000, 30, 1);
        m_pThread->Resume();
    }

    CMovie::~CMovie()
    {
        BOOL bEnabled = OSDisableInterrupts();
        PADSetSamplingCallback(m_pOldSamplingCallback);
        OSRestoreInterrupts(bEnabled);

        if (m_pThread != 0) {
            delete m_pThread;
        }
        m_pThread = 0;
        if (m_pThreadStack != 0) {
            delete[] m_pThreadStack;
        }
        m_pThreadStack = 0;
        if (m_pAudioBuffer0 != 0) {
            delete[] m_pAudioBuffer0;
        }
        m_pAudioBuffer0 = 0;
        if (m_pAudioBuffer1 != 0) {
            delete[] m_pAudioBuffer1;
        }
        m_pAudioBuffer1 = 0;
        Close();
    }

    void CMovie::SetDrawEnv(long nX, long nY, float fScaleX, float fScaleY)
    {
        m_nDrawX = nX;
        m_nDrawY = nY;
        m_fScaleX = fScaleX;
        m_fScaleY = fScaleY;
    }

    int CMovie::IsWait() const
    {
        return m_pThread->GetState() == 0;
    }

    int CMovie::IsReady() const
    {
        if ((m_nFlags & 1) && m_pThread->GetState() == 3) {
            return 1;
        }
        return 0;
    }

    int CMovie::IsPlay() const
    {
        return (m_nFlags >> 3) & 1;
    }

    int CMovie::IsPause() const
    {
        return (m_nFlags >> 7) & 1;
    }

    void CMovie::SetPause(int bPause)
    {
        if (bPause) {
            m_nFlags |= 0x80;
        } else {
            m_nFlags &= ~0x80;
        }
    }

    int CMovie::Open(const char *pFileName, int nArg0, long nStartFrame)
    {
        if (IsWait()) {
            m_nStartFrame = nStartFrame;
            return m_pThread->Open(pFileName, nArg0, nStartFrame);
        }
        return 0;
    }

    int CMovie::Start(unsigned long nFlags)
    {
        if (IsReady() && m_pThread->Play()) {
            m_nStartFrame %= GetNumFrames();
            if (Is30fps()) {
                m_fTime = m_nStartFrame * 2;
            } else {
                m_fTime = m_nStartFrame;
            }
            m_nFlags = (m_nFlags & ~0x60) | (nFlags | 0x18);
            return 1;
        }
        return 0;
    }

    int CMovie::Stop()
    {
        Close();
        return m_pThread->Stop();
    }

    int CMovie::IsFrameEnd() const
    {
        if ((m_nFlags & 1) && m_nCurFrame == m_Header.numFrames - 1) {
            return 1;
        }
        return 0;
    }

    long CMovie::GetFrame() const
    {
        if (m_nFlags & 1) {
            return m_nCurFrame;
        }
        return -1;
    }

    int CMovie::Is30fps() const
    {
        if ((m_nFlags & 1) && m_Header.frameRate >= 29.9f && m_Header.frameRate <= 30.1f) {
            return 1;
        }
        return 0;
    }

    int CMovie::IsError() const
    {
        return (m_nFlags >> 15) & 1;
    }

    void CMovie::Main()
    {
        if (m_nFlags & 0x8000) {
            return;
        }
        switch (m_pThread->GetState()) {
            case 3:
                Prepare();
                break;
            case 4:
                if ((m_nFlags & 0x10) && !(m_nFlags & 0x80)) {
                    if ((m_nFlags & 2) && IsFrameEnd()) {
                        m_fTime = 0.0f;
                        m_nCurFrame = -1;
                    }
                    long nFrame;
                    if (Is30fps()) {
                        nFrame = 0.5f * m_fTime;
                    } else {
                        nFrame = m_fTime;
                    }
                    if (nFrame > m_nCurFrame) {
                        DecodeFrame();
                    }
                    m_fTime += GetFrameRateF();
                }
                break;
            case 6:
                m_nFlags |= 0x8000;
                break;
        }
    }

    void CMovie::Disp()
    {
        if (!CanbeDraw()) {
            return;
        }
        m_nCurFrame = m_nDecodedFrame;
        if (!(m_nFlags & 0x20)) {
            return;
        }

        unsigned long nWidth;
        if (m_fScaleX < 0.0f) {
            nWidth = CMainApp::g_Rmode.fbWidth;
        } else {
            nWidth = m_VideoInfo.xSize * m_fScaleX;
        }
        unsigned long nHeight;
        if (m_fScaleY < 0.0f) {
            nHeight = CMainApp::g_Rmode.efbHeight;
        } else {
            nHeight = m_VideoInfo.ySize * m_fScaleY;
        }

        BeginDraw();
        Draw(m_nDrawX, m_nDrawY, nWidth, nHeight, 0.0f, 0.0f, 1.0f, 1.0f);
        EndDraw();
    }

    int CMovie::CanbeDraw() const
    {
        if ((m_nFlags & 1) == 0) {
            return 0;
        }
        if ((m_nFlags & 8) == 0) {
            return 0;
        }
        return m_nDecodedFrame >= 0;
    }

    void CMovie::BeginDraw()
    {
        if (!CanbeDraw()) {
            return;
        }
        THPGXYuv2RgbSetup(CMainApp::GetRenderModeObj());

        short nWidth = m_VideoInfo.xSize;
        short nHeight = m_VideoInfo.ySize;
        void *pTextureU = m_pTextureU;
        void *pTextureV = m_pTextureV;
        GXTexObj texY;
        GXTexObj texU;
        GXTexObj texV;

        GXInitTexObj(&texY, m_pTextureY, nWidth, nHeight, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(&texY, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXLoadTexObj(&texY, GX_TEXMAP0);

        GXInitTexObj(&texU, pTextureU, nWidth >> 1, nHeight >> 1, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(&texU, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXLoadTexObj(&texU, GX_TEXMAP1);

        GXInitTexObj(&texV, pTextureV, nWidth >> 1, nHeight >> 1, GX_TF_I8, GX_CLAMP, GX_CLAMP, GX_FALSE);
        GXInitTexObjLOD(&texV, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXLoadTexObj(&texV, GX_TEXMAP2);
    }

    long CMovie::Draw(long nX, long nY, long nW, long nH, float fS0, float fT0, float fS1, float fT1)
    {
        if (!CanbeDraw()) {
            return -1;
        }
        GXBegin(GX_QUADS, GX_VTXFMT7, 4);
        GXPosition3s16(nX, nY, 0);
        GXTexCoord2f32(fS0, fT0);
        GXPosition3s16(nX + nW, nY, 0);
        GXTexCoord2f32(fS1, fT0);
        GXPosition3s16(nX + nW, nY + nH, 0);
        GXTexCoord2f32(fS1, fT1);
        GXPosition3s16(nX, nY + nH, 0);
        GXTexCoord2f32(fS0, fT1);
        return m_nDecodedFrame;
    }

    void CMovie::EndDraw()
    {
        THPGXRestore();
        CMainApp::ResetGP();
    }
}
