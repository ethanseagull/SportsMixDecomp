#ifndef MSW_CGAMEVIB_HPP_
#define MSW_CGAMEVIB_HPP_

namespace msw {

    // INCOMPLETE: only what game/msw/menu/CVibEff.cpp needs.
    //
    // Controller rumble (DOL). All members are static. (argument names
    // guessed)
    class CGameVib {
        public:
            // Starts rumble pattern nPattern of table nTable on pad nPad.
            // The first argument picks a word at 0x3C of an instance, the
            // second a 0x24-byte entry. (the meaning of nUnk is not known)
            static int Start(long nTable, long nPattern, long nUnk, long nPad);
            // Stops the rumble of pad nPad (local pads only).
            static void Stop(long nPad);
    };
}

#endif /* !MSW_CGAMEVIB_HPP_ */
