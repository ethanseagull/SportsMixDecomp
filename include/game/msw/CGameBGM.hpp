#ifndef MSW_CGAMEBGM_HPP_
#define MSW_CGAMEBGM_HPP_

#include "bcs/CTask.hpp"
#include "bcs/CFsm.hpp"

namespace msw {

    // Background music of a game: a task whose state machine is the music
    // state. bcs::CTask at 0x00, bcs::CFsm<CGameBGM> at 0x34 (vptr at 0x78).
    // Only the members seen so far; the size is not known.
    class CGameBGM : public bcs::CTask, public bcs::CFsm<CGameBGM> {
        public:
            virtual ~CGameBGM();
            virtual void Main();

            // Changes the music state, except that states 4 and 5 do not
            // replace themselves or states 6 to 8.
            static void SetBgmState(long state);
            // The current music state, 0 when there is no instance.
            static long GetBgmState();
            static void HurryUp();

            // (name guessed) A virtual call made from an inline body: that is
            // what makes the unit emit CFsm<CGameBGM>::ChangeState right after
            // SetBgmState, as in the original.
            void RequestState(long state) { ChangeState(state); }

            static CGameBGM *s_pInstance; // lbl_805C0C6C (name guessed)
    };
}

#endif /* !MSW_CGAMEBGM_HPP_ */
