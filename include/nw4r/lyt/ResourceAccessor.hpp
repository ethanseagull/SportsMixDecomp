#ifndef NW4R_LYT_RESOURCEACCESSOR_HPP_
#define NW4R_LYT_RESOURCEACCESSOR_HPP_

namespace nw4r {
    namespace ut {
        class Font;
    }

    namespace lyt {

        // Only the virtuals the game calls so far, in vtable order (the
        // destructor at 0x08, GetResource at 0x0C). Shape of the NW4R
        // layout library's resource accessors; no members are needed yet.
        class ResourceAccessor {
            public:
                virtual ~ResourceAccessor();
                // The resource called pName, or null; *pSize gets its size
                // when pSize is not null.
                virtual void *GetResource(unsigned long nType, const char *pName, unsigned long *pSize) = 0;
                virtual ut::Font *GetFont(const char *pName);
        };

        // What msw::CLytCtrl::NewArcResourceAccessor returns.
        class ArcResourceAccessor : public ResourceAccessor {
        };
    }
}

#endif /* !NW4R_LYT_RESOURCEACCESSOR_HPP_ */
