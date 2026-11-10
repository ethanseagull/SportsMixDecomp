#ifndef BCS_CMDLREF_HPP_
#define BCS_CMDLREF_HPP_

#include "bcs/CMdlCopyBase.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace bcs {

    // A model drawn with the pose of another (SetLink). A DOL class, size
    // 0x1D8. Only what the modules use so far is declared; how its members
    // divide between it and CMdlCopyBase is not mapped.
    class CMdlRef : public CMdlCopyBase {
        public:
            CMdlRef();
            virtual ~CMdlRef();

            // (meanings of the last two arguments not known)
            void Create(nw4r::g3d::ResMdl mdl, unsigned long, long);

        private:
            unsigned char m_Unk1C8[0x10]; // 0x1C8
    };
}

#endif /* !BCS_CMDLREF_HPP_ */
