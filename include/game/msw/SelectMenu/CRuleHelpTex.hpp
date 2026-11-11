#ifndef SELECTMENU_CRULEHELPTEX_HPP_
#define SELECTMENU_CRULEHELPTEX_HPP_

#include <string.h>

#include "bcs/CLoadMan.hpp"
#include "bcs/CTask.hpp"
#include "revolution/gx/GXTexture.h"

namespace msw {
    namespace SelectMenu {

        // Size 0x78. The pictures of the rules screen (SelectMenu::CRuleInfo):
        // two texture objects used in turn, each made from a texture file
        // loaded into g_sFileInput. Names are guesses except the symbols'.
        class CRuleHelpTex : public bcs::CTask {
            public:
                // Inline in the game: only CRuleInfo::Set builds one.
                CRuleHelpTex() : bcs::CTask(0, -1)
                {
                    m_nLoad = 0;
                    m_nTex = 0;
                    for (long i = 0; i < 2; i++) {
                        memset(g_sFileInput[i].m_szPath, 0, sizeof(g_sFileInput[i].m_szPath));
                        g_sFileInput[i].m_nFlags = (g_sFileInput[i].m_nFlags & 0xFFFE) | 2;
                    }
                }
                virtual ~CRuleHelpTex();
                virtual void Main();
                virtual void Disp();

                // The folder of the player's language under menu/rulehelp
                // ("UsEn" and so on), or null for an unknown one.
                const char *GetLanguageFileName();
                // Makes the next texture object from texture nIndex of the
                // file last loaded into g_sFileInput, null when it is not
                // there.
                GXTexObj *CreateTex(unsigned long nIndex);

                // The two files the pictures are loaded into in turn.
                static bcs::CLoadMan::FILE_INPUT g_sFileInput[2];

                GXTexObj m_TexObj[2]; // 0x34
                short m_nLoad;        // 0x74, the entry of g_sFileInput to load next
                signed char m_nTex;   // 0x76, the entry of m_TexObj (and g_sFileInput) CreateTex uses next
        };
    }
}

#endif /* !SELECTMENU_CRULEHELPTEX_HPP_ */
