#ifndef NW4R_UT_FONT_HPP_
#define NW4R_UT_FONT_HPP_

namespace nw4r {
    namespace ut {

        // A font. Not mapped: only declared complete so that the derived
        // fonts convert to it.
        class Font {
        };

        // A font read from a .brfna archive font resource (DOL code builds
        // them with bcs::G2D::CreateArchiveFont). Not mapped.
        class ArchiveFont : public Font {
        };
    }
}

#endif /* !NW4R_UT_FONT_HPP_ */
