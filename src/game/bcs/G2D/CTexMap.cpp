#include "bcs/G2D/CTexMap.hpp"

namespace bcs {
    namespace G2D {

        CTexMap::CTexMap(const _GXTexObj &rTexObj) : nw4r::lyt::TexMap(rTexObj)
        {
            // GX hands back the physical address of the image; make it a
            // cached pointer.
            SetImage(reinterpret_cast<void *>(reinterpret_cast<unsigned long>(GetImage()) | 0x80000000));
        }
    }
}
