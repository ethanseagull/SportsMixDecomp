#ifndef NW4R_EF_RESOURCE_HPP_
#define NW4R_EF_RESOURCE_HPP_

#include "nw4r/ut/List.hpp"

namespace nw4r {
    namespace ef {

        // Header of an effect project (.breff) once registered. Minimal
        // declaration written for this project (added for DOL
        // game/bcs/EF/CResource.cpp, which compares the name at 0x10).
        struct EffectProject {
            unsigned long headersize;      // 0x00
            ut::Link mLink;                // 0x04
            unsigned short projectnameLen; // 0x0C
            unsigned short padding;        // 0x0E
            char projectname[4];           // 0x10, as long as projectnameLen
        };

        struct TextureProject;

        // The effect library's table of loaded projects. Minimal declaration
        // written for this project from the DOL's mangled names and the
        // calls in bcs::EF::CResource. Return types nobody reads are guessed
        // as void.
        class Resource {
            public:
                // (the DOL's fn_803F0D80, name guessed) The one instance, a
                // global.
                static Resource *GetInstance();

                EffectProject *Add(unsigned char *pData);
                TextureProject *AddTexture(unsigned char *pData);
                void RemoveEffectProject(EffectProject *pProject);
                void RemoveEffectProjectAll();
                void RemoveTextureProject(TextureProject *pProject);
                void RemoveTextureProjectAll();
                // (the DOL's fn_803F1590, name guessed)
                void RelocateCommand();
                // (the DOL's fn_803F19F0 and fn_803F1A00, names guessed) The
                // sizes of the two project lists.
                unsigned short GetNumEffectProject() const;
                unsigned short GetNumTextureProject() const;

            private:
                ut::List mEffectProjectList;  // 0x00
                unsigned char m_Unk0C[4];
                ut::List mTextureProjectList; // 0x10
        };
    }
}

#endif /* !NW4R_EF_RESOURCE_HPP_ */
