#ifndef NW4R_G3D_RESANMTEXPAT_HPP_
#define NW4R_G3D_RESANMTEXPAT_HPP_

namespace nw4r {
    namespace g3d {

        struct ResAnmTexPatData;

        // Handle to a texture pattern animation resource: one pointer, null
        // when the resource was not found. Minimal declaration written for
        // this project; the member and function names are guessed.
        // Result of evaluating a texture pattern animation for one material.
        // Size 0x44 (bcs::CAnmTexPat::Create allocates an array of them
        // through __construct_new_array). Contents not mapped. Minimal
        // declaration written for this project.
        struct TexPatAnmResult {
            TexPatAnmResult() {}

            unsigned char m_Data[0x44];
        };

        class ResAnmTexPat {
            public:
                explicit ResAnmTexPat(ResAnmTexPatData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                ResAnmTexPatData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMTEXPAT_HPP_ */
