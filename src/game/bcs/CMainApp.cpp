#include "bcs/CMainApp.hpp"

#include "bcs/BCS_Rand.hpp"
#include "bcs/CHBMenu.hpp"
#include "bcs/CMovie.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "nw4r/g3d/G3DState.hpp"
#include "nw4r/ut/Color.hpp"
#include "revolution/base/PPCArch.h"
#include "revolution/dvd.h"
#include "revolution/gx/GXAttr.h"
#include "revolution/gx/GXFrameBuf.h"
#include "revolution/gx/GXGeometry.h"
#include "revolution/gx/GXLighting.h"
#include "revolution/gx/GXMisc.h"
#include "revolution/gx/GXPixel.h"
#include "revolution/gx/GXTev.h"
#include "revolution/gx/GXTexture.h"
#include "revolution/gx/GXTransform.h"
#include "revolution/os/OSAlarm.h"
#include "revolution/os/OSInterrupt.h"
#include "revolution/os/OSReset.h"
#include "revolution/os/OSThread.h"
#include "revolution/os/OSTime.h"
#include "revolution/sc.h"
#include "revolution/vi.h"

// The application frame of the game: start-up (render mode, GX, VI, the
// alarm that paces the frames), the frame begin and end, the frame rate and
// the clocks, the language, pausing layers, and leaving the game (reset,
// power off, Wii Menu, data management). The original file was
// "BCS_MainApp.cpp" (its name is in the start-up panic message). Most of its
// functions have no name in the DOL; they keep their symbol names here.

// Functions of the DOL without a name yet. The SDK's ones are named in the
// comments where the calls make them certain.
extern "C" {
    // The HOME button menu (bcs::CHBMenu): whether it is open; closes it.
    int fn_80046D24(bcs::CHBMenu *pMenu);
    void fn_80046D2C(bcs::CHBMenu *pMenu);
    void fn_8004BFDC();
    void fn_80065B24();
    // The frame buffers (the file after this one): the GX FIFO buffer and
    // its size, the buffer VI shows, the one GX draws to, setting it, and
    // swapping the two.
    void *fn_80065EE4();
    unsigned long fn_80065EEC();
    void *fn_80065EF4();
    void *fn_80065EFC();
    void *fn_80065F04();
    void fn_80065F0C(void *pBuffer);
    void fn_80065F14();
    void fn_8006BE2C();
    long fn_8006C4F4();
    void fn_8006F3D0();
    void fn_8007360C();
    void fn_80073AF8();
    void fn_80071128();
    void fn_8007BDD4();
    void fn_8007C9B4();
    void fn_8007D270();
    void fn_8001114C();
    void fn_80014620();
    void fn_800785D8();
    void fn_800789D4();
    // bcs::CSnd: stops every sound over fFadeFrames.
    void fn_800797A4(float fFadeFrames);
    // The fades (bcs::CMainApp's fade functions pass straight through).
    void fn_80078174(long nType, float fFrames);
    int fn_800782E0();
    void fn_80078308(long nType, float fFrames);
    int fn_80078474();
    void fn_8007849C(float fSpeed, long nType, int b);
    void fn_800784C0();
    void fn_80078508(float fSpeed, long nType, int b);
    void fn_80078570();
    int fn_80078590();
    int fn_800784E0();
    void fn_8041D750(int);
    void fn_8041D840();

    // SDK: OSInitSemaphore, OSSignalSemaphore, OSSetAlarmUserData (guess),
    // OSSleepTicks,
    // OSSetResetCallback, OSSetPowerCallback, VISetPostRetraceCallback,
    // VIFlush, VIGetRetraceCount, GXSetViewport.
    void fn_8027CA60(OSSemaphore *pSem, long nCount);
    long fn_8027CB30(OSSemaphore *pSem);
    void fn_80274730(OSAlarm *pAlarm, void *pData);
    void fn_8027E210(OSTime nTicks);
    void fn_8027EC20(void (*pCallback)());
    void fn_8027ED10(void (*pCallback)());
    void fn_8028B610(void (*pCallback)(unsigned long nRetraceCount));
    void fn_8028CEE0();
    unsigned long fn_8028D0F0();
    void fn_8029C7C0(float left, float top, float wd, float ht, float nearz, float farz);

    // A flag of another file, cleared once start-up is done.
    extern long lbl_805C0A80;

    // This file's functions without a name, in the order they are defined.
    void fn_80050BFC();
    void fn_80050C9C();
    void fn_80050D0C(void (*pExitCallback)());
    void fn_80050F3C();
    void fn_80050F40();
    void fn_80050F74();
    void fn_80050FAC();
    void fn_80051504();
    void fn_80051610();
    void fn_800516EC();
    void fn_800517CC();
    void fn_800518CC();
    void fn_80051A18(OSAlarm *pAlarm, OSContext *pContext);
    void fn_80051A54(unsigned long nRetraceCount);
    int fn_80051AC0(int bOn);
    int fn_800520E4();
    int fn_800520EC();
    void fn_800520FC(int bWaitRetrace);
    const char *fn_80052104();
    const char *fn_8005210C();
    void fn_8005211C();
    const char *fn_8005219C();
    unsigned char fn_800521B4();
    void fn_800521EC();
    float fn_800523A4();
    void fn_8005243C(int bFixed);
    void fn_80052444();
    void fn_800524B4();
    void fn_80052524();
    int fn_80052594();
    void fn_800525D4();
    void fn_800526D4();
    void fn_80052734();
}

namespace bcs {

    GXRenderModeObj CMainApp::g_Rmode;

}

using bcs::CMainApp;

// A semaphore and a lock made ready by the file's static initialiser.
struct InitSemaphore : OSSemaphore {
    InitSemaphore() { fn_8027CA60(this, 0); }
};

struct InitMutex : OSMutex {
    InitMutex() { OSInitMutex(this); }
};

// The alarm that counts the frames when the frame is paced by it, the
// semaphore the frame end waits on, and the lock of the frame rate.
static OSAlarm s_Alarm;
static InitSemaphore s_Semaphore;
static InitMutex s_Mutex;

// The game's title in Japanese and in English.
static const char *s_pTitleJP = "マリオスポーツミックス";
static const char *s_pTitleEN = "Mario Sports Mix";
// Set until the first frame was shown (the screen stays black until then).
static int s_bFirstFrame = 1;

static unsigned char s_nLanguage;       // the text language (GetLanguage)
static unsigned char s_nRegionLanguage; // the language picked from the console's own
static void (*s_pExitCallback)();       // called before the game is left
static int s_bProgressive;              // progressive scan
static int s_bPal;                      // 50 Hz PAL
static GXFifoObj *s_pFifo;
static OSTime s_PrevTime;               // the time of the last frame
static float s_fAppFrameRate;           // frames of 1/60 s the last frame took
static float s_fMaxFrames;              // the most frames one frame may count
static float s_fAppSpeed;
static float s_fAppSpeedReq;
static float s_fUnivSpeed;
static float s_fUnivSpeedReq;
static int s_bFixedFrameRate;           // count every frame as one
static int s_bReset;                    // leaving: reset
static int s_bShutdown;                 // leaving: power off
static int s_bReturnToMenu;             // leaving: the Wii Menu
static int s_bReturnToDataManager;      // leaving: data management
static unsigned long s_nPause;          // paused layers (low 16 bits), asked for (high 16)
static unsigned long s_nRetraceCount;   // the frame count at the last frame end
static int s_bInitialized;
static int s_bAlarmFrame;               // frames paced by the alarm
static unsigned long s_nAlarmCount;
static int s_bWaiting;                  // the frame end waits on the semaphore
static long s_nFps;
static int s_bHalfFps;
static int s_bWaitRetrace;              // always wait for the retrace
static void *s_pNextBuffer;             // the buffer to show at the next retrace
static void *s_pShownBuffer;            // the buffer shown

// The names of the console's languages as used in file paths.
static const char s_LanguageName[7][4] = {"jp", "en", "ge", "fr", "sp", "it", "du"};

// The HOME button menu closes on reset; otherwise the reset is taken.
extern "C" void fn_80050BFC()
{
    bcs::CHBMenu *pMenu = bcs::CHBMenu::GetInstance();
    if (pMenu != 0 && fn_80046D24(pMenu) != 0) {
        fn_80046D2C(pMenu);
        return;
    }
    if (!fn_80052594()) {
        VISetBlack(1);
        s_bReset = 1;
    }
}

// The power button.
extern "C" void fn_80050C9C()
{
    if (!fn_80052594()) {
        VISetBlack(1);
        s_bShutdown = 1;
    }
}

// Start-up. pExitCallback is called before the game is left.
extern "C" void fn_80050D0C(void (*pExitCallback)())
{
    s_pExitCallback = pExitCallback;

    unsigned char nLanguage;
    switch (SCGetLanguage()) {
        case SC_LANG_FRENCH:
            nLanguage = 2;
            break;
        case SC_LANG_SPANISH:
            nLanguage = 3;
            break;
        default:
            nLanguage = 1;
            break;
    }
    if (nLanguage == 9) {
        nLanguage = 4;
    }
    s_nRegionLanguage = nLanguage;
    fn_8005211C();

    s_bReset = 0;
    s_bShutdown = 0;
    s_bReturnToMenu = 0;
    s_bReturnToDataManager = 0;
    s_nRetraceCount = 0;
    s_bFixedFrameRate = 0;
    s_bInitialized = 0;
    s_bAlarmFrame = 0;
    s_nFps = 60;
    s_bHalfFps = 0;
    s_bWaitRetrace = 0;
    s_nPause = 0;

    OSMutex *pMutex = &s_Mutex;
    OSLockMutex(pMutex);
    s_PrevTime = OSGetTime();
    s_fAppFrameRate = 1.0f;
    s_fMaxFrames = 3.0f;
    s_fAppSpeed = 1.0f;
    s_fAppSpeedReq = 1.0f;
    s_fUnivSpeed = 1.0f;
    s_fUnivSpeedReq = 1.0f;
    OSUnlockMutex(pMutex);

    fn_8041D750(1);
    DVDInit();
    VIInit();
    fn_80050FAC();
    fn_80065B24();
    fn_8006F3D0();
    bcs::BCS_SRand(OSGetTime());
    bcs::BCS_Rand();
    fn_8007C9B4();
    fn_800785D8();
    fn_8007BDD4();
    fn_8004BFDC();
    s_pFifo = GXInit(fn_80065EE4(), fn_80065EEC());
    fn_80051504();
    nw4r::g3d::G3DState::SetRenderModeObj(CMainApp::g_Rmode);
    fn_8006BE2C();
    fn_8027EC20(fn_80050BFC);
    fn_8027ED10(fn_80050C9C);
    bcs::CTaskMan::Initialize();
    bcs::CScnObj::Initialize();
    fn_8001114C();
    fn_8007D270();
    fn_8007360C();
    fn_80014620();
    fn_800789D4();
    bcs::CMovie::CreateInstance();
    fn_80051610();

    s_nFps = 60;
    s_bInitialized = 1;
    s_bAlarmFrame = 0;
    s_bWaiting = 0;
    s_nAlarmCount = 0;
    OSCreateAlarm(&s_Alarm);
    fn_80274730(&s_Alarm, OSGetCurrentThread());
    OSTime period = (OSTime)OS_TIMER_CLOCK / s_nFps;
    VIWaitForRetrace();
    OSSetPeriodicAlarm(&s_Alarm, OSGetTime(), period, fn_80051A18);
    fn_8028B610(fn_80051A54);
    lbl_805C0A80 = 0;
}

extern "C" void fn_80050F3C()
{
    fn_80073AF8();
}

// Calls the exit callback and stops.
extern "C" void fn_80050F40()
{
    if (s_pExitCallback != 0) {
        s_pExitCallback();
    }
    PPCHalt();
}

// Stops what plays before the game is left.
extern "C" void fn_80050F74()
{
    fn_80071128();
    fn_800797A4(0.0f);
    bcs::CMovie *pMovie = bcs::CMovie::GetInstance();
    if (pMovie != 0) {
        pMovie->Stop();
    }
}

// Picks the render mode for the console's TV settings and sizes the frame
// buffer to the screen.
extern "C" void fn_80050FAC()
{
    if (SCGetProgressiveMode() == 1 && VIGetDTVStatus() != 0) {
        s_bProgressive = 1;
    } else {
        s_bProgressive = 0;
    }
    s_bPal = 0;

    if (s_bProgressive) {
        switch (VIGetTvFormat()) {
            case VI_NTSC:
                CMainApp::g_Rmode = GXNtsc480Prog;
                break;
            case VI_EURGB60:
                CMainApp::g_Rmode = GXEurgb60Hz480Prog;
                break;
            default:
                OSPanic("BCS_MainApp.cpp", 328, "DEMOInit: invalid TV format\n");
                break;
        }
    } else {
        switch (VIGetTvFormat()) {
            case VI_NTSC:
                CMainApp::g_Rmode = GXNtsc480IntDf;
                break;
            case VI_PAL:
                CMainApp::g_Rmode = GXPal528IntDf;
                s_bPal = 1;
                break;
            case VI_EURGB60:
                CMainApp::g_Rmode = GXEurgb60Hz480IntDf;
                break;
            default:
                OSPanic("BCS_MainApp.cpp", 355, "DEMOInit: invalid TV format\n");
                break;
        }
    }

    int bWide = 0;
    if (SCGetAspectRatio() == 1) {
        bWide = 1;
    }

    CMainApp::g_Rmode.fbWidth = 640;
    CMainApp::g_Rmode.efbHeight = 456;
    if (!s_bPal) {
        CMainApp::g_Rmode.xfbHeight = 456;
        if (bWide) {
            CMainApp::g_Rmode.viWidth = 686;
        } else {
            CMainApp::g_Rmode.viWidth = 670;
        }
        CMainApp::g_Rmode.viHeight = CMainApp::g_Rmode.xfbHeight;
        CMainApp::g_Rmode.viXOrigin = (720 - CMainApp::g_Rmode.viWidth) / 2;
        CMainApp::g_Rmode.viYOrigin = (480 - CMainApp::g_Rmode.viHeight) / 2;
    } else {
        CMainApp::g_Rmode.xfbHeight = 542;
        if (bWide) {
            CMainApp::g_Rmode.viWidth = 682;
        } else {
            CMainApp::g_Rmode.viWidth = 666;
        }
        CMainApp::g_Rmode.viHeight = CMainApp::g_Rmode.xfbHeight;
        CMainApp::g_Rmode.viXOrigin = (720 - CMainApp::g_Rmode.viWidth) / 2;
        CMainApp::g_Rmode.viYOrigin = (574 - CMainApp::g_Rmode.viHeight) / 2;
    }
}

// Sets up GX for the render mode and clears the first frame buffer.
extern "C" void fn_80051504()
{
    const GXRenderModeObj &rm = CMainApp::g_Rmode;
    fn_8029C7C0(0.0f, 0.0f, rm.fbWidth, rm.efbHeight, 0.0f, 1.0f);
    GXSetScissor(0, 0, rm.fbWidth, rm.efbHeight);
    GXSetDispCopySrc(0, 0, rm.fbWidth, rm.efbHeight);
    float fYScale = GXGetYScaleFactor(rm.efbHeight, rm.xfbHeight);
    unsigned short xfbHeight = GXSetDispCopyYScale(fYScale);
    GXSetDispCopyDst(rm.fbWidth, xfbHeight);
    GXSetCopyFilter(rm.aa, rm.sample_pattern, GX_TRUE, rm.vfilter);
    GXSetDispCopyGamma(GX_GM_1_0);
    if (rm.aa) {
        GXSetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    } else {
        GXSetPixelFmt(GX_PF_RGBA6_Z24, GX_ZC_LINEAR);
        GXSetDither(GX_TRUE);
    }
    GXCopyDisp(fn_80065EFC(), GX_TRUE);
}

// Sets up VI for the render mode and waits until the TV has caught up.
extern "C" void fn_80051610()
{
    GXRenderModeObj &rm = CMainApp::g_Rmode;
    unsigned long nScanMode = VIGetScanMode();
    int bProgressive = s_bProgressive;
    int nWait = 1;
    if (nScanMode == VI_NON_INTERLACE && !bProgressive) {
        nWait = 2;
    } else if (nScanMode == VI_NON_INTERLACE && bProgressive) {
        nWait = 100;
    } else if (nScanMode == VI_INTERLACE && bProgressive) {
        nWait = 100;
    } else if (nScanMode == VI_PROGRESSIVE && !bProgressive) {
        nWait = 100;
    }

    VIWaitForRetrace();
    VIConfigure(&rm);
    VISetNextFrameBuffer(fn_80065EF4());
    fn_80065F0C(fn_80065EFC());
    VISetBlack(1);
    fn_8028CEE0();
    for (int i = 0; i < nWait; i++) {
        VIWaitForRetrace();
    }
}

// The start of a frame's drawing.
extern "C" void fn_800516EC()
{
    fn_8041D840();
    CMainApp::ResetGP();
    const GXRenderModeObj &rm = CMainApp::g_Rmode;
    if (rm.field_rendering) {
        GXSetViewportJitter(0.0f, 0.0f, rm.fbWidth, rm.efbHeight, 0.0f, 1.0f, VIGetNextField());
    } else {
        fn_8029C7C0(0.0f, 0.0f, rm.fbWidth, rm.efbHeight, 0.0f, 1.0f);
    }
    GXSetScissor(0, 0, rm.fbWidth, rm.efbHeight);
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
}

// The end of a frame's drawing: copies it out and queues it to be shown
// (at the next retrace when frames are paced by the alarm).
extern "C" void fn_800517CC()
{
    if (s_bAlarmFrame) {
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);
        BOOL bEnabled = OSDisableInterrupts();
        if (s_pShownBuffer == fn_80065F04()) {
            fn_80065F14();
        }
        OSRestoreInterrupts(bEnabled);
        GXCopyDisp(fn_80065F04(), GX_TRUE);
        GXDrawDone();
        bEnabled = OSDisableInterrupts();
        s_pNextBuffer = fn_80065F04();
        fn_80065F14();
        OSRestoreInterrupts(bEnabled);
    } else {
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);
        GXCopyDisp(fn_80065F04(), GX_TRUE);
        GXDrawDone();
        VISetNextFrameBuffer(fn_80065F04());
        fn_80065F14();
        if (s_bFirstFrame) {
            VISetBlack(0);
            s_bFirstFrame = 0;
        }
        fn_8028CEE0();
    }
}

// Waits for the next frame.
extern "C" void fn_800518CC()
{
    if (s_bAlarmFrame) {
        if (s_nRetraceCount == s_nAlarmCount) {
            if (!s_bWaiting) {
                s_bWaiting = 1;
                OSWaitSemaphore(&s_Semaphore);
            }
        } else {
            fn_8027E210(OSMicrosecondsToTicks((OSTime)128));
        }
        s_nRetraceCount = s_nAlarmCount;
    } else {
        int bWaitRetrace = s_bWaitRetrace;
        if (s_bHalfFps) {
            bWaitRetrace = 1;
            while (s_nRetraceCount == fn_8028D0F0()) {
            }
        }
        if (s_nRetraceCount == fn_8028D0F0() || bWaitRetrace) {
            VIWaitForRetrace();
        } else {
            fn_8027E210(OSMicrosecondsToTicks((OSTime)128));
        }
        s_nRetraceCount = fn_8028D0F0();
    }
}

// The frame alarm.
extern "C" void fn_80051A18(OSAlarm *, OSContext *)
{
    if (!s_bAlarmFrame) {
        return;
    }
    s_nAlarmCount++;
    if (s_bWaiting) {
        s_bWaiting = 0;
        fn_8027CB30(&s_Semaphore);
    }
}

// After each retrace: shows the frame the frame end queued.
extern "C" void fn_80051A54(unsigned long)
{
    if (s_bAlarmFrame && s_pNextBuffer != 0) {
        VISetNextFrameBuffer(s_pNextBuffer);
        s_pShownBuffer = s_pNextBuffer;
        s_pNextBuffer = 0;
        if (s_bFirstFrame) {
            VISetBlack(0);
            s_bFirstFrame = 0;
        }
        fn_8028CEE0();
    }
}

// Paces the frames by the alarm (bOn) or by the retrace.
extern "C" int fn_80051AC0(int bOn)
{
    if (!s_bInitialized) {
        return 0;
    }
    if (bOn) {
        if (s_bAlarmFrame) {
            return 1;
        }
        BOOL bEnabled = OSDisableInterrupts();
        s_bAlarmFrame = 1;
        s_pShownBuffer = 0;
        s_pNextBuffer = 0;
        OSRestoreInterrupts(bEnabled);
    } else {
        if (!s_bAlarmFrame) {
            return 1;
        }
        BOOL bEnabled = OSDisableInterrupts();
        s_bAlarmFrame = 0;
        s_pShownBuffer = 0;
        s_pNextBuffer = 0;
        OSRestoreInterrupts(bEnabled);
    }
    return 1;
}

namespace bcs {

    // Puts GX back in the state the game's drawing expects.
    // Puts GX back in the state the game's drawing expects.
    void CMainApp::ResetGP()
    {
        GXSetLineWidth(6, GX_TO_ZERO);
        GXSetPointSize(6, GX_TO_ZERO);
        GXEnableTexOffsets(GX_TEXCOORD0, GX_FALSE, GX_FALSE);
        GXEnableTexOffsets(GX_TEXCOORD1, GX_FALSE, GX_FALSE);
        GXEnableTexOffsets(GX_TEXCOORD2, GX_FALSE, GX_FALSE);
        GXEnableTexOffsets(GX_TEXCOORD3, GX_FALSE, GX_FALSE);
        GXEnableTexOffsets(GX_TEXCOORD4, GX_FALSE, GX_FALSE);
        GXEnableTexOffsets(GX_TEXCOORD5, GX_FALSE, GX_FALSE);
        GXEnableTexOffsets(GX_TEXCOORD6, GX_FALSE, GX_FALSE);
        GXEnableTexOffsets(GX_TEXCOORD7, GX_FALSE, GX_FALSE);
        GXSetCullMode(GX_CULL_BACK);
        GXSetClipMode(GX_CLIP_ENABLE);
        GXSetCoPlanar(GX_FALSE);
        GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, nw4r::ut::Color(0xFFFFFFFF));
        GXSetFogRangeAdj(GX_FALSE, 0, 0);
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_COPY);
        GXSetColorUpdate(GX_TRUE);
        GXSetAlphaUpdate(GX_TRUE);
        GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GXSetZCompLoc(GX_FALSE);
        GXSetDstAlpha(GX_FALSE, 0xFF);
        GXSetNumTevStages(1);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
        GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE4, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE5, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE6, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE7, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE8, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE9, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE10, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE11, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE12, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE13, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE14, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode(GX_TEVSTAGE15, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
        GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
        GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
        GXSetNumIndStages(0);
        GXSetTevDirect(GX_TEVSTAGE0);
        GXSetTevDirect(GX_TEVSTAGE1);
        GXSetTevDirect(GX_TEVSTAGE2);
        GXSetTevDirect(GX_TEVSTAGE3);
        GXSetTevDirect(GX_TEVSTAGE4);
        GXSetTevDirect(GX_TEVSTAGE5);
        GXSetTevDirect(GX_TEVSTAGE6);
        GXSetTevDirect(GX_TEVSTAGE7);
        GXSetTevDirect(GX_TEVSTAGE8);
        GXSetTevDirect(GX_TEVSTAGE9);
        GXSetTevDirect(GX_TEVSTAGE10);
        GXSetTevDirect(GX_TEVSTAGE11);
        GXSetTevDirect(GX_TEVSTAGE12);
        GXSetTevDirect(GX_TEVSTAGE13);
        GXSetTevDirect(GX_TEVSTAGE14);
        GXSetTevDirect(GX_TEVSTAGE15);
        GXSetNumChans(0);
        GXSetNumTexGens(0);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetTexCoordGen2(GX_TEXCOORD2, GX_TG_MTX2x4, GX_TG_TEX2, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetTexCoordGen2(GX_TEXCOORD3, GX_TG_MTX2x4, GX_TG_TEX3, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetTexCoordGen2(GX_TEXCOORD4, GX_TG_MTX2x4, GX_TG_TEX4, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetTexCoordGen2(GX_TEXCOORD5, GX_TG_MTX2x4, GX_TG_TEX5, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetTexCoordGen2(GX_TEXCOORD6, GX_TG_MTX2x4, GX_TG_TEX6, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXSetTexCoordGen2(GX_TEXCOORD7, GX_TG_MTX2x4, GX_TG_TEX7, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
        GXInvalidateTexAll();
        GXSetTexCoordScaleManually(GX_TEXCOORD0, GX_FALSE, 0, 0);
        GXSetTexCoordScaleManually(GX_TEXCOORD1, GX_FALSE, 0, 0);
        GXSetTexCoordScaleManually(GX_TEXCOORD2, GX_FALSE, 0, 0);
        GXSetTexCoordScaleManually(GX_TEXCOORD3, GX_FALSE, 0, 0);
        GXSetTexCoordScaleManually(GX_TEXCOORD4, GX_FALSE, 0, 0);
        GXSetTexCoordScaleManually(GX_TEXCOORD5, GX_FALSE, 0, 0);
        GXSetTexCoordScaleManually(GX_TEXCOORD6, GX_FALSE, 0, 0);
        GXSetTexCoordScaleManually(GX_TEXCOORD7, GX_FALSE, 0, 0);
        GXSetTexCoordCylWrap(GX_TEXCOORD0, GX_FALSE, GX_FALSE);
        GXSetTexCoordCylWrap(GX_TEXCOORD1, GX_FALSE, GX_FALSE);
        GXSetTexCoordCylWrap(GX_TEXCOORD2, GX_FALSE, GX_FALSE);
        GXSetTexCoordCylWrap(GX_TEXCOORD3, GX_FALSE, GX_FALSE);
        GXSetTexCoordCylWrap(GX_TEXCOORD4, GX_FALSE, GX_FALSE);
        GXSetTexCoordCylWrap(GX_TEXCOORD5, GX_FALSE, GX_FALSE);
        GXSetTexCoordCylWrap(GX_TEXCOORD6, GX_FALSE, GX_FALSE);
        GXSetTexCoordCylWrap(GX_TEXCOORD7, GX_FALSE, GX_FALSE);
        GXSetDispCopyGamma(GX_GM_1_0);
    }

    GXRenderModeObj *CMainApp::GetRenderModeObj()
    {
        return &g_Rmode;
    }

}

// Whether the TV is 50 Hz PAL.
extern "C" int fn_800520E4()
{
    return s_bPal;
}

extern "C" int fn_800520EC()
{
    return s_bHalfFps;
}

void bcs::CMainApp::SetHalfFps(int bHalf)
{
    s_bHalfFps = bHalf;
}

extern "C" void fn_800520FC(int bWaitRetrace)
{
    s_bWaitRetrace = bWaitRetrace;
}

extern "C" const char *fn_80052104()
{
    return s_pTitleJP;
}

extern "C" const char *fn_8005210C()
{
    return s_pTitleEN;
}

unsigned char bcs::CMainApp::GetRegionLanguage()
{
    return s_nRegionLanguage;
}

// Picks the text language from the region's language.
extern "C" void fn_8005211C()
{
    unsigned char nRegionLanguage = s_nRegionLanguage;
    switch (nRegionLanguage) {
        case 0:
            s_nLanguage = 0;
            break;
        case 5:
            s_nLanguage = 2;
            break;
        case 2:
        case 6:
            s_nLanguage = 3;
            break;
        case 3:
        case 7:
            s_nLanguage = 4;
            break;
        case 8:
            s_nLanguage = 5;
            break;
        case 9:
            s_nLanguage = 1;
            break;
        default:
            s_nLanguage = 1;
            break;
    }
}

unsigned char bcs::CMainApp::GetLanguage()
{
    return s_nLanguage;
}

// The name of the text language as used in file paths.
extern "C" const char *fn_8005219C()
{
    return s_LanguageName[s_nLanguage];
}

extern "C" unsigned char fn_800521B4()
{
    return SCGetLanguage();
}

const char *bcs::CMainApp::GetSystemLanguageName()
{
    return s_LanguageName[SCGetLanguage()];
}

int bcs::CMainApp::CheckPCMessageRestriction()
{
    return SCCheckPCMessageRestriction();
}

// Measures the last frame: the application frame rate is the time it took
// in frames of 1/60 s (at most s_fMaxFrames), times the application speed.
extern "C" void fn_800521EC()
{
    OSMutex *pMutex = &s_Mutex;
    OSLockMutex(pMutex);
    s_fAppSpeed = s_fAppSpeedReq;
    s_fUnivSpeed = s_fUnivSpeedReq;

    OSTime now = OSGetTime();
    OSTime diff = now - s_PrevTime;
    // The time in units of 1/3000 s (a 60 Hz frame is 50 of them).
    OSTime nUnits = diff <= 0 ? 50 : (diff * 3000 + OS_TIMER_CLOCK / 2) / OS_TIMER_CLOCK;

    float fMax;
    if (s_bHalfFps) {
        if (s_bPal) {
            fMax = 120.0f;
        } else {
            fMax = 100.0f;
        }
    } else if (s_bPal) {
        fMax = 60.0f * s_fMaxFrames;
    } else {
        fMax = 50.0f * s_fMaxFrames;
    }

    float fUnits = nUnits;
    if (fUnits > fMax) {
        fUnits = fMax;
    }

    if (s_bFixedFrameRate) {
        if (s_bPal) {
            if (s_bAlarmFrame) {
                s_fAppFrameRate = 1.0f;
            } else {
                s_fAppFrameRate = 1.2f;
            }
        } else {
            s_fAppFrameRate = 1.0f;
        }
    } else {
        s_fAppFrameRate = fUnits * s_fAppSpeed / 50.0f;
    }
    s_PrevTime = now;
    OSUnlockMutex(pMutex);
}

float bcs::CMainApp::GetAppFrameRateF()
{
    return s_fAppFrameRate;
}

// GetAppFrameRateF under the lock.
extern "C" float fn_800523A4()
{
    OSLockMutex(&s_Mutex);
    float fRate = s_fAppFrameRate;
    OSUnlockMutex(&s_Mutex);
    return fRate;
}

void bcs::CMainApp::SetAppSpeed(float fSpeed)
{
    // Stored, then clamped where it is (the original rounds the value it
    // reads back: frsp).
    s_fAppSpeedReq = fSpeed;
    s_fAppSpeedReq = s_fAppSpeedReq > 16.0f ? 16.0f : (s_fAppSpeedReq < 0.00390625f ? 0.00390625f : s_fAppSpeedReq);
}

float bcs::CMainApp::GetAppSpeed()
{
    return s_fAppSpeed;
}

void bcs::CMainApp::SetUnivSpeed(float fSpeed)
{
    s_fUnivSpeedReq = fSpeed;
}

float bcs::CMainApp::GetUnivSpeed()
{
    return s_fUnivSpeed;
}

extern "C" void fn_8005243C(int bFixed)
{
    s_bFixedFrameRate = bFixed;
}

// Leaving the game: asks for a reset, the Wii Menu or the data management
// screen, unless leaving was already asked for.
extern "C" void fn_80052444()
{
    if (!fn_80052594()) {
        VISetBlack(1);
        s_bReset = 1;
    }
}

extern "C" void fn_800524B4()
{
    if (!fn_80052594()) {
        VISetBlack(1);
        s_bReturnToMenu = 1;
    }
}

extern "C" void fn_80052524()
{
    if (!fn_80052594()) {
        VISetBlack(1);
        s_bReturnToDataManager = 1;
    }
}

// Whether leaving the game was asked for.
extern "C" int fn_80052594()
{
    if (s_bReset || s_bShutdown || s_bReturnToMenu || s_bReturnToDataManager) {
        return 1;
    }
    return 0;
}

// Leaves the game when that was asked for (once nothing is loading).
extern "C" void fn_800525D4()
{
    if (fn_80052594()) {
        if (fn_8006C4F4() != -1) {
            GXDrawDone();
            fn_80050F74();
            VISetBlack(1);
            fn_8028CEE0();
            VIWaitForRetrace();
            VIWaitForRetrace();
            VIWaitForRetrace();
            VIWaitForRetrace();
            if (s_pExitCallback != 0) {
                s_pExitCallback();
            }
            if (s_bShutdown) {
                OSShutdownSystem();
            }
            if (s_bReset) {
                OSRestart(0);
            }
            if (s_bReturnToMenu) {
                OSReturnToMenu();
            }
            if (s_bReturnToDataManager) {
                OSReturnToDataManager();
            }
        }
    }
}

extern "C" void fn_800526D4()
{
    s_nPause = 0;
}

int bcs::CMainApp::IsPause(unsigned long nMask)
{
    return (s_nPause & nMask) != 0;
}

int bcs::CMainApp::IsPauseReady(unsigned long nMask)
{
    return (s_nPause & (nMask << 16)) != 0;
}

void bcs::CMainApp::SetPause(unsigned long nMask, int bPause)
{
    if (bPause) {
        s_nPause |= nMask << 16;
    } else {
        s_nPause &= ~(nMask << 16);
    }
}

// Takes the pauses asked for: a layer whose request bit is set is paused,
// the others are resumed.
extern "C" void fn_80052734()
{
    unsigned long nPause = s_nPause;
    for (int i = 0; i < 16; i++) {
        unsigned long nReq = 1 << (i + 16);
        unsigned long nBit = 1 << i;
        nPause = nPause & nReq ? nPause | nBit : nPause & ~(nBit | nReq);
    }
    s_nPause = nPause;
}

void bcs::CMainApp::Start3DFade(long nType, float fFrames)
{
    fn_80078174(nType, fFrames);
}

int bcs::CMainApp::IsPlay3DFade()
{
    return fn_800782E0();
}

void bcs::CMainApp::StartMasterFade(long nType, float fFrames)
{
    fn_80078308(nType, fFrames);
}

int bcs::CMainApp::IsPlayMasterFade()
{
    return fn_80078474();
}

void bcs::CMainApp::StartCrossFade(float fSpeed, long nType, int b)
{
    fn_80078508(fSpeed, nType, b);
}

void bcs::CMainApp::StopCrossFade()
{
    fn_80078570();
}

int bcs::CMainApp::IsPlayCrossFade()
{
    return fn_80078590();
}

void bcs::CMainApp::Start3DCrossFade(float fSpeed, long nType, int b)
{
    fn_8007849C(fSpeed, nType, b);
}

void bcs::CMainApp::Stop3DCrossFade()
{
    fn_800784C0();
}

int bcs::CMainApp::IsPlay3DCrossFade()
{
    return fn_800784E0();
}
