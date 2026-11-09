#ifndef NW4R_G3D_RESANMCLR_HPP_
#define NW4R_G3D_RESANMCLR_HPP_

namespace nw4r {
    namespace g3d {

        struct ResAnmClrData;

        // Handle to a colour animation resource: one pointer, null when the
        // resource was not found. Minimal declaration written for this
        // project; the member and function names are guessed.
        class ResAnmClr {
            public:
                bool IsValid() const { return m_pData != 0; }

            private:
                ResAnmClrData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMCLR_HPP_ */
