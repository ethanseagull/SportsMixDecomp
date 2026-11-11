#ifndef NW4R_G3D_RESANMCLR_HPP_
#define NW4R_G3D_RESANMCLR_HPP_

namespace nw4r {
    namespace g3d {

        struct ResAnmClrData;
        // The colour result of one material. Size 0x5C
        // (bcs::CAnmMatClr::Create allocates them); the fields are not
        // mapped yet.
        struct ClrAnmResult {
            unsigned char m_Unk00[0x5C];
        };

        // Handle to a colour animation resource: one pointer, null when the
        // resource was not found. Minimal declaration written for this
        // project; the member and function names are guessed.
        class ResAnmClr {
            public:
                explicit ResAnmClr(ResAnmClrData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                ResAnmClrData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMCLR_HPP_ */
