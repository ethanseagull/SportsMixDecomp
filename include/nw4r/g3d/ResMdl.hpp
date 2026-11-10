#ifndef NW4R_G3D_RESMDL_HPP_
#define NW4R_G3D_RESMDL_HPP_

namespace nw4r {
    namespace g3d {

        struct ResMdlData;

        // Handle to a model resource: one pointer, null when the resource
        // was not found. Minimal declaration written for this project.
        class ResMdl {
            public:
                explicit ResMdl(ResMdlData *pData = 0) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

            private:
                ResMdlData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESMDL_HPP_ */
