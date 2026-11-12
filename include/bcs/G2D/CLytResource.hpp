#ifndef BCS_G2D_CLYTRESOURCE_HPP_
#define BCS_G2D_CLYTRESOURCE_HPP_

#include "bcs/CLoadMan.hpp"
namespace nw4r {
    namespace lyt {
        class ArcResourceAccessor;
        class ArcResourceLink;
        class FontRefLink;
        class MultiArcResourceAccessor;
    }
}

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the members used so far.
        class CLytResource {
            public:
                static CLytResource *GetInstance();

                // Releases the archive loaded under pName (return type guessed).
                void Cleanup(const char *pName);
                // (added for HockeyF game/hockey/CRadarIcon2DHockeyGoal.cpp)
                // An accessor for folder pRootFolderName of loaded archive
                // pFileName; null on failure.
                nw4r::lyt::ArcResourceAccessor *NewArcResourceAccessor(const char *pFileName, const char *pRootFolderName);
                // Added for MenuF game/menu/CMenu.cpp (return type guessed;
                // the menu passes 0 as the second argument).
                void GetResourceFromDvdMan(const char *pPath, unsigned long);
                // (added for MenuF game/menu/CSelectMenu.cpp; return types
                // guessed) Loading an archive in the background, and its
                // state: negative while it is not complete.
                void LoadDataASync(const CLoadMan::FILE_INPUT *pFile);
                long IsCompleteDataASync(const CLoadMan::FILE_INPUT *pFile);

                // Added for DOL game/bcs/G2D/CLayoutBase.cpp (no symbols;
                // names guessed). A new font link, kept in the list at 0x30;
                // null when out of memory.
                nw4r::lyt::FontRefLink *NewFontRefLink();
                // Each deletes an object the instance made and drops it from
                // its list (0x0C, 0x18, 0x24, 0x30). Static: they read the
                // instance themselves and do nothing without one.
                static void DeleteArcResourceAccessor(nw4r::lyt::ArcResourceAccessor *pAccessor);
                static void DeleteArcResourceLink(nw4r::lyt::ArcResourceLink *pLink);
                static void DeleteMultiArcResourceAccessor(nw4r::lyt::MultiArcResourceAccessor *pAccessor);
                static void DeleteFontRefLink(nw4r::lyt::FontRefLink *pLink);
        };
    }
}

#endif /* !BCS_G2D_CLYTRESOURCE_HPP_ */
