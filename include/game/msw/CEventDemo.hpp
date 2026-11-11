#ifndef CEVENTDEMO_HPP_
#define CEVENTDEMO_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {

    // Only the members used so far.
    class CEventDemo {
        public:
            static void SetRandomDemoGameMode();
            // (added for MenuF game/menu/CTitle.cpp; return type and
            // argument guessed) A game kind (0 to 3) for the attract demo.
            static long GetRandomDemoGameMode(int);

            // The demo camera (DOL, static; read from DodgeF's
            // CEventDemoDodge, which plays its damage demos through them;
            // return types and parameter names guessed).
            static void SetupDemoCamera(const char *pName, long nType);
            static void SetDemoCameraInfo(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget, long);
            static void SetDemoCameraFrame(float fFrame, int);
            static void PlayDemoCamera(int);
            static void SetEndDemoCamera();
            // Added for the DOL's game/msw/CGameExDemoBase.cpp (DOL, static;
            // return types guessed).
            static int IsEndDemoCamera();
            // (fn_80119F24, unnamed in the DOL; name guessed) Stores the
            // value at 0x4D4 of the running demo's camera.
            static void SetDemoCameraParam(float);

            // Whether event demos are played: true when the byte at
            // g_GameWork + 0x151 is set, otherwise a flag in small data.
            // (read from the DOL body; return type guessed)
            static int IsEnable();

            // 0x38 bytes in the DOL's .bss. Only the byte BossF's
            // CEventDemoBoss::CreateDemo sets when it plays no demo; its
            // meaning and all names are guessed.
            struct WORK {
                unsigned char m_Unk00[2];
                unsigned char bSkip;        // 0x02
                // 0x03 (name guessed): set when a demo is cancelled
                // (VolleyF CReadyStartDemoVolley::EventCansel); IsEventSkip
                // then skips.
                unsigned char bCancel;
                unsigned char m_Unk04[0x34];
            };
            static WORK m_Work;

            static int m_bDemonstration;  // 4 bytes; type guessed
            static long m_nDemonstration; // 4 bytes; type guessed
    };
}

#endif /* !CEVENTDEMO_HPP_ */
