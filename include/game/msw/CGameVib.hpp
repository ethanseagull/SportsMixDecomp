#ifndef MSW_CGAMEVIB_HPP_
#define MSW_CGAMEVIB_HPP_

#include "bcs/CTask.hpp"
#include "bcs/CLoadMan.hpp"

namespace msw {

    // Controller rumble (DOL, game/msw/CGameVib.cpp). A singleton task
    // (0x50 bytes, vtable lbl_80476DA0) that loads the five rumble files
    // (vib/wi_*.vib) and plays their patterns through bcs::CPadWI. Names of
    // members and arguments are guesses.
    class CGameVib : public bcs::CTask {
        public:
            // One rumble pattern of a file (0x24 bytes, from offset 0x10 of
            // the file): up to eight (strength, time) pairs as shorts, the
            // default length and the number of pairs.
            struct PATTERN {
                short m_Data[8][2];  // 0x00
                short m_nFrame;      // 0x20, used when Start gets -2
                signed char m_nNum;  // 0x22
            };
            // A loaded .vib file: a 0x10-byte header, then the patterns.
            struct FILE {
                unsigned char m_Header[0x10]; // 0x00
                PATTERN m_Pattern[1];         // 0x10
            };

            virtual ~CGameVib();
            virtual void Main();

            // (fn_8018DCEC) Creates the task once, without a parent.
            static void Create();
            // (fn_8018DD6C) Non-zero once the files have loaded.
            static int IsLoaded();

            // Starts rumble pattern nPattern of file nTable on pad nPad.
            // nFrame -2 takes the pattern's own length.
            static int Start(long nTable, long nPattern, long nFrame, long nPad);
            // (added for HockeyF game/hockey/CChrDrvPadHockey.cpp) Rumble on
            // pad nPad for fFrame frames (meaning guessed).
            static int Start(float fFrame, long nPad);
            // Stops the rumble of pad nPad (local pads only).
            static void Stop(long nPad);

            static CGameVib *m_pInstance; // lbl_805C0CF8
            static bcs::CLoadMan::FILE_INPUT g_sFileInput[5]; // lbl_80476850 (name guessed)

        private:
            CGameVib() : bcs::CTask(0, -1), m_nStep(0), m_bLoaded(0)
            {
                for (long i = 0; i < 5; i++) {
                    m_pFile[i] = 0;
                }
            }

            long m_nStep;        // 0x34
            int m_bLoaded;       // 0x38
            FILE *m_pFile[5];    // 0x3C, the loaded files
    };
}

#endif /* !MSW_CGAMEVIB_HPP_ */
