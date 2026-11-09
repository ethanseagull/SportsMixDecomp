#ifndef NW4R_G3D_RESFILE_HPP_
#define NW4R_G3D_RESFILE_HPP_

#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"

namespace nw4r {
    namespace g3d {

        struct ResFileData;

        // Handle to a model resource file: one pointer. Minimal declaration
        // written for this project. The three getters are DOL functions;
        // each returns its handle in r3 (a null handle when the file has no
        // resource of that name). The other names are guessed.
        class ResFile {
            public:
                explicit ResFile(ResFileData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                ResAnmChr GetResAnmChr(const char *pName) const;
                ResAnmClr GetResAnmClr(const char *pName) const;
                ResAnmTexSrt GetResAnmTexSrt(const char *pName) const;

            private:
                ResFileData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESFILE_HPP_ */
