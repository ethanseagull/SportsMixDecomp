#ifndef NW4R_G3D_RESFILE_HPP_
#define NW4R_G3D_RESFILE_HPP_

#include "nw4r/db/Panic.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResAnmClr.hpp"
#include "nw4r/g3d/ResAnmShp.hpp"
#include "nw4r/g3d/ResAnmTexPat.hpp"
#include "nw4r/g3d/ResAnmTexSrt.hpp"
#include "nw4r/g3d/ResMdl.hpp"

namespace nw4r {
    namespace g3d {

        struct ResFileData;

        // Handle to a model resource file: one pointer. Minimal declaration
        // written for this project. The three getters are DOL functions;
        // each returns its handle in r3 (a null handle when the file has no
        // resource of that name). The other names are guessed.
        class ResFile {
            public:
                // The default constructor makes NW4R's alignment check (of a
                // null pointer, so it folds away), but it still keeps a member
                // array of handles (MenuF CCourtChr, CCupChr) built by a plain
                // store loop as in the original, not peeled and unrolled. The
                // strings are in CCourtChr's .data. Construction from a pointer
                // has no check: TourF CTournamentTrophy::CreateModel and
                // CTournamentRouteMan_Stage32::Initialize lose it otherwise.
                ResFile() : m_pData(0)
                {
                    ResFileData *p = 0;
                    if ((unsigned long)p & 0x1F) {
                        db::Panic("g3d_resfile_ac.h", 60, "NW4R:Failed assertion !((u32)p & 0x1f)");
                    }
                }
                explicit ResFile(ResFileData *pData) : m_pData(pData) {}

                bool IsValid() const { return m_pData != 0; }

                ResAnmChr GetResAnmChr(const char *pName) const;
                ResAnmClr GetResAnmClr(const char *pName) const;
                ResAnmTexSrt GetResAnmTexSrt(const char *pName) const;
                ResMdl GetResMdl(const char *pName) const;
                // (DOL; added for VolleyF game/volley/CBallTargetMarker.cpp)
                ResAnmTexPat GetResAnmTexPat(const char *pName) const;
                // (DOL fn_80408540, unnamed; name after the others; used by
                // msw::CEventModel)
                ResAnmShp GetResAnmShp(const char *pName) const;
                // DOL functions (MenuF msw::SelectMenu::CCourtChr).
                ResTex GetResTex(const char *pName) const;
                // (added for MenuF game/menu/CRuleInfo.cpp)
                ResTex GetResTex(unsigned long index) const;
                unsigned long GetResTexNumEntries() const;

                // DOL functions; the three calls a file needs before use,
                // in this order. Bind resolves the file's references against
                // the file given (itself, for a file that is complete).
                // Return types guessed: the callers in the modules ignore them.
                bool CheckRevision() const;
                void Init();
                bool Bind(ResFile file);

                // DOL functions, from msw::CItemBall's model loading. The
                // int forms take the index of the resource. Return types of
                // the Has functions guessed (the caller tests a full word).
                void *GetExternalData(const char *pName) const;
                ResMdl GetResMdl(int index) const;
                ResAnmClr GetResAnmClr(int index) const;
                ResAnmTexSrt GetResAnmTexSrt(int index) const;
                ResAnmTexPat GetResAnmTexPat(int index) const;
                int HasResAnmChr() const;
                // (DOL; named after GetResTexNumEntries) Number of skeletal
                // animations in the file.
                unsigned long GetResAnmChrNumEntries() const;
                int HasResAnmClr() const;
                int HasResAnmTexSrt() const;
                int HasResAnmTexPat() const;
                int HasResAnmShp() const;

                // (name written for this project) A handle to file data that
                // must be 32-byte aligned, with the check NW4R's constructor
                // makes (g3d_resfile_ac.h, line 60; the strings are the ones
                // in the DOL). A function, not a second constructor, so that
                // ResFile(0) stays unambiguous.
                static ResFile FromAligned(void *p)
                {
                    if ((unsigned long)p & 0x1F) {
                        db::Panic("g3d_resfile_ac.h", 60, "NW4R:Failed assertion !((u32)p & 0x1f)");
                    }
                    return ResFile(static_cast<ResFileData *>(p));
                }

            private:
                ResFileData *m_pData; // 0x00
        };
    }
}

#endif /* !NW4R_G3D_RESFILE_HPP_ */
