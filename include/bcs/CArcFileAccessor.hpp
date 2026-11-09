#ifndef BCS_CARCFILEACCESSOR_HPP_
#define BCS_CARCFILEACCESSOR_HPP_

namespace bcs {

    // Reads files out of an archive. Lives in the DOL; only the member used
    // so far.
    class CArcFileAccessor {
        public:
            // Finds the file `pName` of four-character type `type`. Returns
            // null if there is none; *pSize receives its size.
            unsigned char *GetResource(unsigned long type, const char *pName, unsigned long *pSize);
    };
}

#endif /* !BCS_CARCFILEACCESSOR_HPP_ */
