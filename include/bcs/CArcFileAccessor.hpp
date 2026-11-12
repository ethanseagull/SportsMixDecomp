#ifndef BCS_CARCFILEACCESSOR_HPP_
#define BCS_CARCFILEACCESSOR_HPP_

#include "revolution/arc.h"

namespace bcs {

    // Reads files out of an archive. Lives in the DOL. Size 0x124
    // (msw::menu::CMessage::AttachResource allocates it); the vptr follows
    // the members (the constructor, fn_80026D48, stores it at 0x120).
    // Member names other than GetResource are guesses (the DOL exports no
    // name for them). The data comes first so that the vptr follows it.
    class CArcFileAccessor {
        private:
            const void *m_pArc;  // 0x000
            ARCHandle m_Handle;  // 0x004
            char m_Root[0x100];  // 0x020
            // 0x120: vptr

        public:
            // fn_80026D48
            CArcFileAccessor();
            // fn_80026D64
            virtual ~CArcFileAccessor();

            // fn_80026DA4: opens the archive pArc with root directory pRoot.
            // False if the archive cannot be read.
            bool Attach(const void *pArc, const char *pRoot);
            // fn_80026E18: forgets the archive and returns it.
            void *Detach();
            // Finds the file `pName` of four-character type `type`. Returns
            // null if there is none; *pSize receives its size.
            unsigned char *GetResource(unsigned long type, const char *pName, unsigned long *pSize);
            // fn_80026E4C: puts up to nMax names of files with extension
            // pExt (without the dot) into pNames; returns how many.
            long GetFileNames(const char *pExt, const char **pNames, long nMax);
    };
}

#endif /* !BCS_CARCFILEACCESSOR_HPP_ */
