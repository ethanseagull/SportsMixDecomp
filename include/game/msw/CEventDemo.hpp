#ifndef CEVENTDEMO_HPP_
#define CEVENTDEMO_HPP_

namespace bcs {
    class CTask;
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

            // Ends the demo objects of WORK (asks each task to delete
            // itself and clears its flags); with bWaitFade set, only once
            // the master fade ends (through CEventDemoBase::AddFadeEndFunc).
            // (parameter name guessed)
            static void Cleanup(int bWaitFade);
            // Sets bits nFlags in the four flag words of WORK.
            static void SetFlags(unsigned long nFlags);
            // (fn_80119CCC, unnamed in the DOL) Passes a message on to the
            // running demo's CEventDemoBase::SendMessage; CGame::SendMessage_
            // calls it with its own four arguments.
            static void SendMessage(long, long, long, long);
            // (DOL, static; added for BossF game/boss/GameBoss.cpp; return
            // type and parameter meanings guessed) Starts demo nDemo under
            // pParent; the boss game's result passes 1 and -1, -1.
            static void CreateDemo(long nDemo, bcs::CTask *pParent, float, float);

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
                unsigned char m_Unk04[0x14 - 0x04];
                // 0x14: flag words of the four demo objects (SetFlags,
                // Cleanup; names guessed).
                unsigned long anFlags[4];
                // 0x24: the four demo objects' tasks; Cleanup deletes them.
                bcs::CTask *apTask[4];
                unsigned char m_Unk34[0x38 - 0x34];
            };
            static WORK m_Work;

            static int m_bDemonstration;  // 4 bytes; type guessed
            static long m_nDemonstration; // 4 bytes; type guessed
    };
}

#endif /* !CEVENTDEMO_HPP_ */
