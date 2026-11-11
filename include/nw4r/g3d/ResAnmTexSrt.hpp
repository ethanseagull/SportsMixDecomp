#ifndef NW4R_G3D_RESANMTEXSRT_HPP_
#define NW4R_G3D_RESANMTEXSRT_HPP_

namespace nw4r {
    namespace g3d {

        struct ResAnmTexSrtData;

        // Handle to a texture matrix animation resource: one pointer, null
        // when the resource was not found. Minimal declaration written for
        // this project; the member and function names are guessed.
        // Result of evaluating a texture matrix animation for one material.
        // Size 0xE8 (bcs::CAnmTexSrt::Create allocates an array of them
        // through __construct_new_array). Contents not mapped. Minimal
        // declaration written for this project.
        struct TexSrtAnmResult {
            TexSrtAnmResult() {}

            unsigned char m_Data[0xE8];
        };

        class ResAnmTexSrt {
            public:
                explicit ResAnmTexSrt(ResAnmTexSrtData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                ResAnmTexSrtData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMTEXSRT_HPP_ */
