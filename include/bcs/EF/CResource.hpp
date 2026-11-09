#ifndef BCS_EF_CRESOURCE_HPP_
#define BCS_EF_CRESOURCE_HPP_

namespace bcs {
    class CArcFileAccessor;

    namespace EF {

        // The effect system. Lives in the DOL; only the member used so far.
        class CManager {
            public:
                static CManager *GetInstance();
        };

        // The effect archives known to the effect system. Lives in the DOL;
        // only the members used so far. Return types nobody has used yet are
        // guessed as void.
        class CResource {
            public:
                static CResource *GetInstance();

                CArcFileAccessor *GetArcAccesor(const char *pPath, const char *pRoot);
                void ReleaseArcAccesor(CArcFileAccessor *pAccessor);
                // Non-zero if the archive `pPath` is already registered.
                int GetResourceAfterLoad(const char *pPath, void *pData, unsigned long size); // return type guessed
                void AddArchive(const char *pPath);
                void RelocateCommand();
                void InvalidateCache();
        };
    }
}

#endif /* !BCS_EF_CRESOURCE_HPP_ */
