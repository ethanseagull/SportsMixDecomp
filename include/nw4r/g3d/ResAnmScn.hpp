#ifndef NW4R_G3D_RESANMSCN_HPP_
#define NW4R_G3D_RESANMSCN_HPP_

namespace nw4r {
    namespace g3d {

        struct ResAnmScnData;
        struct ResAnmLightData;

        // Handle to a scene animation. Minimal declaration written for this
        // project (added for DOL game/bcs/EF/CResource.cpp): one pointer.
        class ResAnmScn {
            public:
                ResAnmScn() : m_pData(0) {}
                explicit ResAnmScn(ResAnmScnData *pData) : m_pData(pData) {}

                // DOL function.
                void Release();

            private:
                ResAnmScnData *m_pData; // 0x00
        };

        // Handle to the light animation of a scene animation; one pointer.
        class ResAnmLight {
            public:
                ResAnmLight() : m_pData(0) {}
                explicit ResAnmLight(ResAnmLightData *pData) : m_pData(pData) {}

            private:
                ResAnmLightData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESANMSCN_HPP_ */
