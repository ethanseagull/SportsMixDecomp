#ifndef BCS_EF_CRESOURCE_HPP_
#define BCS_EF_CRESOURCE_HPP_

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace bcs {
    class CArcFileAccessor;

    namespace EF {

        class CEffect;

        // The effect system. Lives in the DOL; only the member used so far.
        class CManager {
            public:
                static CManager *GetInstance();
                // Added for game/msw/cockpit/CMissionCount.cpp: the effect
                // with handle hEffect, null when it is gone.
                CEffect *GetEffect(unsigned long hEffect);
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
                // The g3d file `pName` of the loaded archives (a null handle
                // when there is none).
                nw4r::g3d::ResFile GetG3DResFile(const char *pName);
        };
    }
}

#endif /* !BCS_EF_CRESOURCE_HPP_ */
