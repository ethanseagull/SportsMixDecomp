#ifndef NW4R_UT_DVDFILESTREAM_HPP_
#define NW4R_UT_DVDFILESTREAM_HPP_

namespace nw4r {
    namespace ut {

        // Only what the game's code uses so far (added for DOL
        // game/bcs/CMovie.cpp). 0x70 bytes, vptr at 0x00 (the size from the
        // constructor's last store at 0x6E and the member after it in DOL
        // fn_8004EB14's class).
        class DvdFileStream {
            public:
                DvdFileStream();
                virtual ~DvdFileStream();

            private:
                unsigned char m_Unk04[0x6C];
        };
    }
}

#endif /* !NW4R_UT_DVDFILESTREAM_HPP_ */
