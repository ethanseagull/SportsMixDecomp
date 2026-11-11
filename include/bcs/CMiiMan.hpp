#ifndef BCS_CMIIMAN_HPP_
#define BCS_CMIIMAN_HPP_

#include "bcs/CTask.hpp"
#include "revolution/rfl.h"

namespace bcs {

    // The engine's Mii manager (DOL), a task, size 0x64 (Create makes the
    // one instance with new(0x64)). Argument names are guesses. Added for
    // MenuF game/menu/CFriendInfo.cpp.
    //
    // Main loads the RFL resource archive and starts the library. The
    // static functions are the interface: each does nothing and returns
    // RFLErrcode_NotAvailable while there is no instance or the RFL library
    // is not ready yet (s_bReady, set by Main once it is done).
    //
    // Face icons and character models are not made at once: MakeFaceIcon
    // and RequestCharModel queue them, and Update (called by the renderer)
    // makes them all.
    class CMiiMan : public CTask {
        public:
            // (name guessed, DOL fn_8006832C) Makes the instance if there is
            // none.
            static void Create();

            virtual ~CMiiMan();
            virtual void Main();

            // (name guessed, DOL fn_80068474) A one-Mii middle database
            // holding rData, or null.
            static RFLMiddleDB *GetMiddleDB(const RFLStoreData &rData);
            // (name guessed, DOL fn_80068500) Queues the CMiiScnObj with
            // handle hScnObj to have its character model made in Update.
            static void RequestCharModel(unsigned long hScnObj);
            // (name guessed, DOL fn_800685E0) Makes what was queued.
            static void Update();

            // Fills rData with the Mii at index of source.
            static RFLErrcode GetStoreData(RFLStoreData &rData, RFLDataSource source, long index);
            // (name guessed, DOL fn_80068984) Fills rData with the first
            // valid Mii of source (the official database or the six default
            // Miis) at or after rIndex, and leaves rIndex after it.
            static RFLErrcode GetNextStoreData(RFLStoreData &rData, RFLDataSource source, long &rIndex);
            // Fills rData with the Mii createId of source.
            static RFLErrcode GetStoreData(RFLStoreData &rData, RFLDataSource source, RFLCreateID &createId);
            // (name guessed, DOL fn_80068B48) Fills rInfo from the Mii at
            // index of source and pDB, or from *pData when it is given.
            static RFLErrcode GetAdditionalInfo(RFLAdditionalInfo &rInfo, RFLDataSource source,
                                                const RFLMiddleDB *pDB, RFLStoreData *pData, long index);
            // Fills rInfo from the Mii rData.
            static RFLErrcode GetAdditionalInfo(RFLAdditionalInfo &rInfo, RFLStoreData &rData);
            // (name guessed, DOL fn_80068CB0) Queues drawing the face of the
            // Mii at index of source and pDB (or *pData when it is given)
            // into the texture memory pBuffer as rSetting says, with an
            // outline nOutline texels wide (0 for none, at most 3).
            static RFLErrcode MakeFaceIcon(void *pBuffer, RFLDataSource source, const RFLMiddleDB *pDB,
                                           RFLStoreData *pData, long index, long expression,
                                           RFLIconSetting &rSetting, long nOutline);
            // (added for MenuF game/menu/CFriendList.cpp) The same for the
            // Mii rData.
            static RFLErrcode MakeFaceIcon(void *pBuffer, RFLStoreData &rData, long expression,
                                           RFLIconSetting &rSetting, long nOutline);
            // (name guessed, DOL fn_800691C4) Non-zero when loading the Mii
            // database failed (RFLErrcode_Loadfail).
            static int IsLoadFailed();

        private:
            // A queued face icon, 0x74 bytes. index is -1 when the Mii is
            // m_Data.
            struct FaceIconReq {
                void *m_pBuffer;            // 0x00
                RFLDataSource m_Source;     // 0x04
                const RFLMiddleDB *m_pDB;   // 0x08
                long m_nIndex;              // 0x0C
                long m_Expression;          // 0x10
                long m_nOutline;            // 0x14
                RFLStoreData m_Data;        // 0x18
                RFLIconSetting m_Setting;   // 0x64
            };

            enum {
                NUM_CHAR_MODEL_REQ = 64,
                NUM_FACE_ICON_REQ = 16,
                STATE_END = 99
            };

            inline CMiiMan();

            // (name guessed; inlined everywhere in the original) GetMiddleDB's
            // work.
            inline RFLMiddleDB *GetMiddleDB_(const RFLStoreData &rData);
            // (name guessed, DOL fn_80068538) Queues a face icon.
            void MakeFaceIcon_(void *pBuffer, RFLDataSource source, const RFLMiddleDB *pDB, RFLStoreData *pData,
                               long index, long expression, RFLIconSetting &rSetting, long nOutline);
            // (name guessed; inlined into Update_ in the original) Makes the
            // queued character models; non-zero when any was made.
            inline int MakeCharModels_();
            // (name guessed, DOL fn_800685F4) Update's work.
            void Update_();
            // (name guessed, DOL fn_8006880C) Makes the queued face icons;
            // non-zero when there were any.
            int MakeFaceIcons_();
            // (name guessed, DOL fn_800689B4) GetNextStoreData's work.
            RFLErrcode GetNextStoreData_(RFLStoreData &rData, RFLDataSource source, long &rIndex);
            // (name guessed, DOL fn_80068FF0) Sets up the two queues once
            // RFL is running.
            void InitQueues_();
            // (names guessed, DOL fn_800691CC and fn_8006927C) Draws a dark
            // outline of nKernel + 1 texels around the opaque texels of the
            // RGB5A3 texture pTex (nWidth x nHeight).
            static void StampOutline_(unsigned char *pDst, long nWidth, long nHeight, long x, long y, long nKernel);
            static void DrawOutline_(unsigned short *pTex, long nWidth, long nHeight, long nKernel);

            static CMiiMan *s_pInstance;        // lbl_805C0970 (name guessed)
            static unsigned char *s_pWorkBuffer; // lbl_805C0974 (name guessed) RFL's work memory
            static int s_bReady;                // lbl_805C0978 (name guessed)
            static int s_bLoadFailed;           // lbl_805C097C (name guessed)
            static RFLErrcode s_nErrcode;       // lbl_805C0980 (name guessed) the last RFL start-up result

            long m_nState;                      // 0x34 (name guessed) step of Main
            RFLMiddleDB m_MiddleDB;             // 0x38
            unsigned char *m_pMiddleDBBuffer;   // 0x50
            unsigned long *m_pCharModelReq;     // 0x54, CMiiScnObj handles
            long m_nCharModelReq;               // 0x58
            FaceIconReq *m_pFaceIconReq;        // 0x5C
            long m_nFaceIconReq;                // 0x60
    };
}

#endif /* !BCS_CMIIMAN_HPP_ */
