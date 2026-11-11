// bcs::CMiiMan, the engine's Mii manager: a task that loads the RFL (Mii)
// library's resources and starts it, static entry points into RFL that do
// nothing until it is ready, and queues of face icons and character models
// made once a frame from Update.
#include "bcs/CMiiMan.hpp"

#include "bcs/BCS_Util.hpp"
#include "bcs/CLoadMan.hpp"
#include "bcs/CMiiScnObj.hpp"
#include "bcs/New.hpp"
#include "revolution/arc.h"
#include "revolution/gx/GXTev.h"

namespace bcs {

    namespace {
        // The archive on the disc that holds RFL's resource file.
        const char *s_pResArcName = "/RFLRes01.arc";

        // RGB5A3 texels for the outline: black, with the alpha (in
        // eighths) the outline strength gives.
        const unsigned short s_aOutlineColor[8] = {
            0x0000, 0x1000, 0x2000, 0x3000, 0x4000, 0x5000, 0x6000, 0x7000,
        };

        // The strengths stamped around an edge texel (7 x 7 used, in rows
        // of 8), for outlines of one, two and three texels.
        const unsigned char s_aOutlineKernel[3][8][8] = {
            {
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 1, 3, 1, 0, 0, 0},
                {0, 0, 3, 7, 3, 0, 0, 0},
                {0, 0, 1, 3, 1, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
            },
            {
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 1, 0, 0, 0, 0},
                {0, 0, 1, 5, 1, 0, 0, 0},
                {0, 0, 0, 1, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
            },
            {
                {0, 0, 1, 1, 1, 0, 0, 0},
                {0, 1, 3, 3, 3, 1, 0, 0},
                {1, 3, 3, 5, 3, 3, 1, 0},
                {1, 3, 5, 7, 5, 3, 1, 0},
                {1, 3, 3, 5, 3, 3, 1, 0},
                {0, 1, 3, 3, 3, 1, 0, 0},
                {0, 0, 1, 1, 1, 0, 0, 0},
                {0, 0, 0, 0, 0, 0, 0, 0},
            },
        };
    }

    CMiiMan *CMiiMan::s_pInstance;
    unsigned char *CMiiMan::s_pWorkBuffer;
    int CMiiMan::s_bReady;
    int CMiiMan::s_bLoadFailed;
    RFLErrcode CMiiMan::s_nErrcode;

    inline CMiiMan::CMiiMan()
        : CTask(0, -1),
          m_nState(0),
          m_pMiddleDBBuffer(0),
          m_pCharModelReq(0),
          m_nCharModelReq(0),
          m_pFaceIconReq(0),
          m_nFaceIconReq(0)
    {
        SetUnk08(0);
    }

    void CMiiMan::Create()
    {
        if (s_pInstance == 0) {
            s_pInstance = new CMiiMan;
        }
    }

    CMiiMan::~CMiiMan()
    {
        if (m_pMiddleDBBuffer != 0) {
            delete[] m_pMiddleDBBuffer;
            m_pMiddleDBBuffer = 0;
        }
        if (m_pCharModelReq != 0) {
            delete[] m_pCharModelReq;
            m_pCharModelReq = 0;
        }
        m_nCharModelReq = 0;
        if (m_pFaceIconReq != 0) {
            delete[] m_pFaceIconReq;
            m_pFaceIconReq = 0;
        }
        m_nFaceIconReq = 0;
    }

    inline RFLMiddleDB *CMiiMan::GetMiddleDB_(const RFLStoreData &rData)
    {
        if (!s_bReady) {
            return 0;
        }
        RFLInitMiddleDB(&m_MiddleDB, RFLMiddleDBType_UserSet, m_pMiddleDBBuffer, 1);
        if (RFLAddMiddleDBStoreData(&m_MiddleDB, &rData) == RFLErrcode_Success) {
            return &m_MiddleDB;
        }
        return 0;
    }

    RFLMiddleDB *CMiiMan::GetMiddleDB(const RFLStoreData &rData)
    {
        if (s_pInstance != 0) {
            return s_pInstance->GetMiddleDB_(rData);
        }
        return 0;
    }

    void CMiiMan::RequestCharModel(unsigned long hScnObj)
    {
        CMiiMan *pMan = s_pInstance;
        if (pMan == 0) {
            return;
        }
        if (!s_bReady) {
            return;
        }
        pMan->m_pCharModelReq[pMan->m_nCharModelReq] = hScnObj;
        pMan->m_nCharModelReq++;
    }

    void CMiiMan::MakeFaceIcon_(void *pBuffer, RFLDataSource source, const RFLMiddleDB *pDB, RFLStoreData *pData,
                                long index, long expression, RFLIconSetting &rSetting, long nOutline)
    {
        if (!s_bReady) {
            return;
        }

        long nReq = m_nFaceIconReq;
        FaceIconReq &rReq = m_pFaceIconReq[nReq];
        rReq.m_pBuffer = pBuffer;
        rReq.m_Source = source;
        rReq.m_pDB = pDB;
        rReq.m_nIndex = index;
        rReq.m_Expression = expression;
        rReq.m_nOutline = nOutline;
        rReq.m_Setting = rSetting;
        if (pData != 0) {
            rReq.m_Data = *pData;
            rReq.m_nIndex = -1;
        }
        m_nFaceIconReq++;
    }

    void CMiiMan::Update()
    {
        if (s_pInstance != 0) {
            s_pInstance->Update_();
        }
    }

    inline int CMiiMan::MakeCharModels_()
    {
        unsigned long *pHandle = m_pCharModelReq;
        if (pHandle == 0) {
            return false;
        }

        int bMade = false;
        for (long i = 0; i < m_nCharModelReq; i++, pHandle++) {
            CScnObj *pObj = CScnObj::GetScnObj(*pHandle);
            if (pObj != 0) {
                static_cast<CMiiScnObj *>(pObj)->InitCharModel();
                bMade = true;
            }
        }
        m_nCharModelReq = 0;
        return bMade;
    }

    void CMiiMan::Update_()
    {
        if (!s_bReady) {
            return;
        }

        int bMadeModel = MakeCharModels_();
        int bMadeIcon = MakeFaceIcons_();
        if (!bMadeModel && !bMadeIcon) {
            return;
        }

        // RFL changes the swap tables; put back GX's defaults.
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
    }

    int CMiiMan::MakeFaceIcons_()
    {
        int bMade;
        FaceIconReq *pReq = m_pFaceIconReq;
        if (pReq == 0) {
            return false;
        }

        bMade = false;
        for (long i = 0; i < m_nFaceIconReq; i++, pReq++) {
            bMade = true;

            RFLDataSource source = pReq->m_Source;
            const RFLMiddleDB *pDB = pReq->m_pDB;
            long index = pReq->m_nIndex;
            if (index < 0) {
                pDB = GetMiddleDB_(pReq->m_Data);
                source = RFLDataSource_MiddleDB;
                index = 0;
            }

            if (RFLMakeIcon(pReq->m_pBuffer, source, pDB, index, pReq->m_Expression, &pReq->m_Setting) ==
                    RFLErrcode_Success &&
                pReq->m_nOutline > 0) {
                long nKernel = pReq->m_nOutline - 1;
                DrawOutline_(static_cast<unsigned short *>(pReq->m_pBuffer), pReq->m_Setting.width,
                             pReq->m_Setting.height, nKernel > 2 ? 2 : (nKernel < 0 ? 0 : nKernel));
            }
        }
        m_nFaceIconReq = 0;
        return bMade;
    }

    RFLErrcode CMiiMan::GetStoreData(RFLStoreData &rData, RFLDataSource source, long index)
    {
        if (s_pInstance != 0) {
            if (!s_bReady) {
                return RFLErrcode_NotAvailable;
            }
            return RFLGetStoreData(&rData, source, index);
        }
        return RFLErrcode_NotAvailable;
    }

    RFLErrcode CMiiMan::GetNextStoreData(RFLStoreData &rData, RFLDataSource source, long &rIndex)
    {
        if (s_pInstance != 0) {
            return s_pInstance->GetNextStoreData_(rData, source, rIndex);
        }
        return RFLErrcode_NotAvailable;
    }

    RFLErrcode CMiiMan::GetNextStoreData_(RFLStoreData &rData, RFLDataSource source, long &rIndex)
    {
        if (!s_bReady) {
            return RFLErrcode_NotAvailable;
        }

        long nNum = 0;
        switch (source) {
            case RFLDataSource_Official:
                nNum = 100;
                break;
            case RFLDataSource_Default:
                nNum = 6;
                break;
        }

        if (nNum <= 0 || rIndex < 0 || rIndex >= nNum) {
            return RFLErrcode_DBNodata;
        }

        RFLErrcode err;
        do {
            err = RFLGetStoreData(&rData, source, rIndex);
            rIndex++;
        } while (err == RFLErrcode_DBNodata && rIndex < nNum);
        return err;
    }

    RFLErrcode CMiiMan::GetStoreData(RFLStoreData &rData, RFLDataSource source, RFLCreateID &createId)
    {
        if (s_pInstance != 0) {
            if (!s_bReady) {
                return RFLErrcode_NotAvailable;
            }

            unsigned short defaultIndex;
            unsigned short officialIndex;
            switch (source) {
                case RFLDataSource_Official:
                    if (RFLSearchOfficialData(&createId, &officialIndex)) {
                        return RFLGetStoreData(&rData, source, officialIndex);
                    }
                    break;
                case RFLDataSource_Default:
                    if (RFLSearchDefaultData(&createId, &defaultIndex)) {
                        return RFLGetStoreData(&rData, source, defaultIndex);
                    }
                    break;
            }
            return RFLErrcode_DBNodata;
        }
        return RFLErrcode_NotAvailable;
    }

    RFLErrcode CMiiMan::GetAdditionalInfo(RFLAdditionalInfo &rInfo, RFLDataSource source, const RFLMiddleDB *pDB,
                                          RFLStoreData *pData, long index)
    {
        if (s_pInstance != 0) {
            if (!s_bReady) {
                return RFLErrcode_NotAvailable;
            }
            if (pData != 0) {
                pDB = s_pInstance->GetMiddleDB_(*pData);
                source = RFLDataSource_MiddleDB;
                index = 0;
            }
            return RFLGetAdditionalInfo(&rInfo, source, pDB, index);
        }
        return RFLErrcode_NotAvailable;
    }

    RFLErrcode CMiiMan::GetAdditionalInfo(RFLAdditionalInfo &rInfo, RFLStoreData &rData)
    {
        CMiiMan *pMan = s_pInstance;
        if (pMan != 0) {
            if (!s_bReady) {
                return RFLErrcode_NotAvailable;
            }
            RFLInitMiddleDB(&pMan->m_MiddleDB, RFLMiddleDBType_UserSet, pMan->m_pMiddleDBBuffer, 1);
            RFLErrcode err = RFLAddMiddleDBStoreData(&pMan->m_MiddleDB, &rData);
            return RFLGetAdditionalInfo(&rInfo, RFLDataSource_MiddleDB,
                                        err == RFLErrcode_Success ? &pMan->m_MiddleDB : 0, 0);
        }
        return RFLErrcode_NotAvailable;
    }

    RFLErrcode CMiiMan::MakeFaceIcon(void *pBuffer, RFLDataSource source, const RFLMiddleDB *pDB, RFLStoreData *pData,
                                     long index, long expression, RFLIconSetting &rSetting, long nOutline)
    {
        if (s_pInstance != 0) {
            if (!s_bReady) {
                return RFLErrcode_NotAvailable;
            }
            s_pInstance->MakeFaceIcon_(pBuffer, source, pDB, pData, index, expression, rSetting, nOutline);
            return RFLErrcode_Success;
        }
        return RFLErrcode_NotAvailable;
    }

    RFLErrcode CMiiMan::MakeFaceIcon(void *pBuffer, RFLStoreData &rData, long expression, RFLIconSetting &rSetting,
                                     long nOutline)
    {
        if (s_pInstance != 0) {
            if (!s_bReady) {
                return RFLErrcode_NotAvailable;
            }
            s_pInstance->MakeFaceIcon_(pBuffer, RFLDataSource_MiddleDB, 0, &rData, 0, expression, rSetting, nOutline);
            return RFLErrcode_Success;
        }
        return RFLErrcode_NotAvailable;
    }

    void CMiiMan::Main()
    {
        switch (m_nState) {
            case 0:
                CLoadMan::Load(s_pResArcName, 0, 0, 0x101, -0x20, 2);
                m_nState++;
                break;

            case 1: {
                if (!CLoadMan::IsLoadEnd(s_pResArcName, 0)) {
                    break;
                }
                CLoadMan::GetSize(s_pResArcName, 0);
                void *pArc = CLoadMan::GetPtr(s_pResArcName, 0);

                ARCHandle handle;
                ARCFileInfo info;
                unsigned long nResSize;
                void *pRes;
                int bFound = false;
                if (ARCInitHandle(pArc, &handle) && ARCOpen(&handle, RFLGetArcFilePath(), &info)) {
                    pRes = ARCGetStartAddrInMem(&info);
                    nResSize = ARCGetLength(&info);
                    ARCClose(&info);
                    bFound = true;
                }

                if (bFound) {
                    unsigned long nWorkSize = RFLGetWorkSize(false);
                    s_pWorkBuffer = new (0x101, 0x20) unsigned char[nWorkSize];
                    unsigned long nDBSize = RFLGetMiddleDBBufferSize(1);
                    if (nDBSize != 0) {
                        m_pMiddleDBBuffer = new (1, 0x20) unsigned char[nDBSize];
                        RFLInitMiddleDB(&m_MiddleDB, RFLMiddleDBType_UserSet, m_pMiddleDBBuffer, 1);
                    }

                    RFLErrcode err = RFLInitRes(s_pWorkBuffer, pRes, nResSize, false);
                    s_nErrcode = err;
                    switch (err) {
                        case RFLErrcode_Success:
                            InitQueues_();
                            m_nState = STATE_END;
                            s_bReady = true;
                            break;
                        case RFLErrcode_Fatal:
                            m_nState = STATE_END;
                            s_bReady = true;
                            break;
                        default:
                            // Loading goes on in the background.
                            m_nState++;
                            break;
                    }
                } else {
                    // No resources: go on without Miis.
                    m_nState = STATE_END;
                    s_bReady = true;
                }
                break;
            }

            case 2: {
                RFLErrcode err = RFLGetAsyncStatus();
                if (err != RFLErrcode_Busy) {
                    s_nErrcode = err;
                    switch (err) {
                        case RFLErrcode_Success:
                            InitQueues_();
                            break;
                        case RFLErrcode_Loadfail:
                            s_bLoadFailed = true;
                            InitQueues_();
                            break;
                    }
                    s_bReady = true;
                    m_nState++;
                }
                break;
            }
        }
    }

    void CMiiMan::InitQueues_()
    {
        RFLSetCoordinate(RFLCoordinate_YP, RFLCoordinate_ZP);

        if (m_pCharModelReq == 0) {
            m_pCharModelReq = new (0x101, 0x20) unsigned long[NUM_CHAR_MODEL_REQ];
            for (int i = 0; i < NUM_CHAR_MODEL_REQ; i++) {
                m_pCharModelReq[i] = 0;
            }
            m_nCharModelReq = 0;
        }

        if (m_pFaceIconReq == 0) {
            m_pFaceIconReq = new (0x101, 0x20) FaceIconReq[NUM_FACE_ICON_REQ];
            for (int i = 0; i < NUM_FACE_ICON_REQ; i++) {
                m_pFaceIconReq[i].m_pBuffer = 0;
            }
            m_nFaceIconReq = 0;
        }
    }

    int CMiiMan::IsLoadFailed()
    {
        return s_bLoadFailed;
    }

    void CMiiMan::StampOutline_(unsigned char *pDst, long nWidth, long nHeight, long x, long y, long nKernel)
    {
        for (long dy = -3; dy <= 3; dy++) {
            if (y + dy < 0 || y + dy >= nHeight) {
                continue;
            }
            for (long dx = -3; dx <= 3; dx++) {
                if (x + dx < 0 || x + dx >= nWidth) {
                    continue;
                }
                unsigned char nValue = s_aOutlineKernel[nKernel][dy + 3][dx + 3];
                if (nValue != 0 && nValue > pDst[dy * nWidth + dx]) {
                    pDst[dy * nWidth + dx] = nValue;
                }
            }
        }
    }

    void CMiiMan::DrawOutline_(unsigned short *pTex, long nWidth, long nHeight, long nKernel)
    {
        unsigned long nSize = (nWidth * nHeight + 31) & ~31;
        unsigned char *pWork = new (0x101, -0x20) unsigned char[nSize * 2];
        if (pWork == 0) {
            return;
        }
        BCS_CpuClear(pWork, nSize * 2);

        // Which texels are opaque, in rows (the texture is in 4 x 4 tiles).
        unsigned char *pOpaque = pWork;
        unsigned char *pOutline = pWork + nSize;
        unsigned short *pTexel = pTex;
        for (long ty = 0; ty < nHeight; ty += 4) {
            for (long tx = 0; tx < nWidth; tx += 4) {
                unsigned char *pRow = pOpaque + ty * nWidth + tx;
                for (long i = 0; i < 4; i++) {
                    for (long j = 0; j < 4; j++) {
                        if (pTexel[j] & 0x8000) {
                            pRow[j] = 0xFF;
                        }
                    }
                    pRow += nWidth;
                    pTexel += 4;
                }
            }
        }

        // Mark opaque texels and stamp the outline around every clear one
        // next to an opaque one.
        unsigned char *pSrc = pOpaque;
        unsigned char *pDst = pOutline;
        for (long y = 0; y < nHeight; y++) {
            for (long x = 0; x < nWidth; x++, pSrc++, pDst++) {
                if (*pSrc != 0) {
                    *pDst = 0xFF;
                    continue;
                }

                int bEdge = false;
                if (y > 0) {
                    unsigned char *pUp = pSrc - nWidth;
                    if (*pUp != 0) {
                        bEdge = true;
                    }
                    if (x > 0 && pUp[-1] != 0) {
                        bEdge = true;
                    }
                    if (x < nWidth - 1 && pUp[1] != 0) {
                        bEdge = true;
                    }
                }
                if (y < nHeight - 1) {
                    unsigned char *pDown = pSrc + nWidth;
                    if (*pDown != 0) {
                        bEdge = true;
                    }
                    if (x > 0 && pDown[-1] != 0) {
                        bEdge = true;
                    }
                    if (x < nWidth - 1 && pDown[1] != 0) {
                        bEdge = true;
                    }
                }
                if (x > 0 && pSrc[-1] != 0) {
                    bEdge = true;
                }
                if (x < nWidth - 1 && pSrc[1] != 0) {
                    bEdge = true;
                }
                if (bEdge) {
                    StampOutline_(pDst, nWidth, nHeight, x, y, nKernel);
                }
            }
        }

        // Write the outline into the texture, back in tiles. Opaque texels
        // (0xFF) are left alone.
        for (long ty = 0; ty < nHeight; ty += 4) {
            for (long tx = 0; tx < nWidth; tx += 4) {
                unsigned char *pRow = pOutline + ty * nWidth + tx;
                for (long i = 0; i < 4; i++) {
                    for (long j = 0; j < 4; j++) {
                        unsigned char nValue = pRow[j];
                        if (nValue != 0 && nValue < 8) {
                            pTex[j] = s_aOutlineColor[nValue];
                        }
                    }
                    pRow += nWidth;
                    pTex += 4;
                }
            }
        }

        delete[] pWork;
    }
}
