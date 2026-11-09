#ifndef TOURNAMENTDATA_HPP_
#define TOURNAMENTDATA_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    // Whether TournamentData is a namespace or a class is not known; the
    // mangled names are the same.
    namespace TournamentData {

        // Size 0x20. Where the tournament camera stands relative to the
        // point it follows. Layout from msw::CTournamentCamera::
        // SetValueCamera; the member names are guessed.
        struct CAMERA_INFO {
            bcs::FVEC3 ofs; // 0x00: added to the followed point
            bcs::FVEC3 rot; // 0x0C: radians; x and y give the viewing direction, z the roll
            float dist;     // 0x18
            float fovy;     // 0x1C
        };
    }
}

#endif /* !TOURNAMENTDATA_HPP_ */
