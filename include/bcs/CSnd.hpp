#ifndef BCS_CSND_HPP_
#define BCS_CSND_HPP_

namespace bcs {

    class CSndGroupList;
    class CTask;

    // The sound system (DOL). Only the members used so far.
    class CSnd {
        public:
            // Static: the DOL function takes no object. Returns 1 when the
            // sound system is ready (the body ends in `x == 1`).
            static int IsReady();

            // Added for game/msw/CGameSnd.cpp (all names guessed from the
            // nw4r calls in their bodies).
            // Bytes needed to load the groups of rList
            // (SoundArchivePlayer::GetTotalGroupSize). fn_800792F8
            static unsigned long GetGroupListSize(const CSndGroupList &rList);
            // Loads the groups of rList into the current heap. fn_80079384
            static int LoadGroupList(const CSndGroupList &rList);
            // Whether group nGroup is loaded (SoundArchivePlayer::
            // IsLoadedGroup). fn_80079488
            static int IsGroupLoaded(unsigned long nGroup);
            // Makes sound heap nHeap of nSize bytes. fn_800794AC
            static int CreateHeap(long nHeap, unsigned long nSize, long nArg2, long nAlign);
            // Frees sound heap nHeap. fn_800795C0
            static int DestroyHeap(long nHeap);
            // SoundHeap::SaveState of heap nHeap; the level for
            // LoadHeapState. fn_80079668
            static long SaveHeapState(long nHeap);
            // SoundHeap::LoadState: frees what heap nHeap got after level
            // nLevel. fn_800796FC
            static int LoadHeapState(long nHeap, long nLevel);
            // Stops every sound. fn_800797A4
            static void StopAllSound();
            // Pauses (bPause != 0) or resumes every playing sound over
            // fFadeFrames (BasicSound::Pause on each). fn_800799DC
            static void PauseAll(int bPause, float fFadeFrames);
            // Set on all four 3D listeners. fn_80079C48, fn_80079D20 (calls
            // fn_803C42A0), fn_80079CB4 (Sound3DListener::SetInteriorSize)
            static void SetListenerMaxVolumeDistance(float fDistance);
            static void SetListenerUnitDistance(float fDistance);
            static void SetListenerInteriorSize(float fSize);
            // (name guessed) The sound id of a sound archive label
            // (SoundArchive::ConvertLabelStringToSoundId), -1 when there is
            // none, 0 while the sound system is not running. fn_80079E30
            static long GetSoundID(const char *pLabel);

            // (names guessed) The state of the sound system (5 once it is
            // running, lbl_805C0A30), and the task every sound task is made
            // under (lbl_805C0A58, a plain bcs::CTask made with priority 0x3E).
            static long s_nState;
            static CTask *s_pTaskRoot;
    };

    // A list of sound groups to load, 0x404 bytes (the DOL builds it on the
    // stack). Each group appears once; at most 256. (names guessed)
    class CSndGroupList {
        public:
            enum { ENTRY_MAX = 256 };

            struct ENTRY {
                short m_nGroup; // 0x00
                short m_nFlag;  // 0x02, what CSnd does with it is not known
            };

            CSndGroupList() : m_nNum(0) {}

            long GetNum() const { return m_nNum; }
            short GetGroup(long i) const { return m_aEntry[i].m_nGroup; }

            bool IsExist(unsigned long nGroup) const
            {
                for (long i = 0; i < GetNum(); i++) {
                    if (nGroup == GetGroup(i)) {
                        return true;
                    }
                }
                return false;
            }

            // Adds group nGroup unless it is there already or the list is
            // full; 1 when it was added.
            int Add(unsigned long nGroup, short nFlag)
            {
                if (IsExist(nGroup)) {
                    return 0;
                }
                if (m_nNum >= ENTRY_MAX) {
                    return 0;
                }
                ENTRY &rEntry = m_aEntry[m_nNum];
                rEntry.m_nGroup = nGroup;
                rEntry.m_nFlag = nFlag;
                m_nNum++;
                return 1;
            }

            ENTRY m_aEntry[ENTRY_MAX]; // 0x000
            long m_nNum;               // 0x400
    };
}

#endif /* !BCS_CSND_HPP_ */
