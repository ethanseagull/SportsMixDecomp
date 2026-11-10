#ifndef BCS_G2D_CTEXMAP_HPP_
#define BCS_G2D_CTEXMAP_HPP_

// The SDK's texture object (GXTexObj). Only named here.
struct _GXTexObj;

namespace bcs {
    namespace G2D {

        // A layout texture slot made from a GX texture object. Lives in the
        // DOL. Size 0x20: the constructor fills 0x1C bytes through an NW4R
        // function (they are copied into a material's nw4r::lyt::TexMap
        // field by field in msw::CRadarIcon2D::CreateCharaIconPicture) and
        // stores the vptr at 0x1C. Most likely derived from
        // nw4r::lyt::TexMap; written as opaque bytes until that class has a
        // header.
        class CTexMap {
            public:
                CTexMap(const _GXTexObj &rTexObj);

            private:
                unsigned char m_Unk00[0x1C];

            public:
                // vptr at 0x1C
                virtual ~CTexMap();
        };
    }
}

#endif /* !BCS_G2D_CTEXMAP_HPP_ */
