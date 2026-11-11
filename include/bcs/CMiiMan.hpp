#ifndef BCS_CMIIMAN_HPP_
#define BCS_CMIIMAN_HPP_

#include "revolution/rfl.h"

namespace bcs {

    // The engine's Mii manager (DOL). Only the members used so far; argument
    // names are guesses. Added for MenuF game/menu/CFriendInfo.cpp.
    class CMiiMan {
        public:
            // Fills rData with the Mii createId of source; static (it is
            // called with no object).
            static void GetStoreData(RFLStoreData &rData, RFLDataSource source, RFLCreateID &createId);
            // (added for MenuF game/menu/CFriendList.cpp; static, return
            // type and the names of the two longs guessed: the caller
            // passes 0 and 1) Draws the face of Mii rData into the texture
            // memory pBuffer as rSetting says.
            static void MakeFaceIcon(void *pBuffer, RFLStoreData &rData, long nUnk0, RFLIconSetting &rSetting,
                                     long nUnk1);
    };
}

#endif /* !BCS_CMIIMAN_HPP_ */
