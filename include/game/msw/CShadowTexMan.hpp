#ifndef MSW_CSHADOWTEXMAN_HPP_
#define MSW_CSHADOWTEXMAN_HPP_

#include "bcs/CTask.hpp"
#include "revolution/gx/GXTexture.h"

namespace msw {

    // The shadow texture task of a game (DOL class, 0x5C bytes, vtable
    // lbl_804764C8; defined in game/msw/CGame.cpp). Each sport's
    // GenerateInstance creates it first (BossF game/boss/GameBoss.cpp).
    // Member names are guesses.
    class CShadowTexMan : public bcs::CTask {
        public:
            virtual ~CShadowTexMan();

            // Creates the task under pParent once ("zz_test" of resource
            // 0x23 as an I8 texture); returns the instance.
            static CShadowTexMan *CreateInstance(bcs::CTask *pParent);
            // (fn_8018D820, name guessed) The texture object, null without
            // the task.
            static GXTexObj *GetTexObj();
            // (fn_8018D83C, name guessed) Whether shadows of kind nType are
            // drawn with the texture: kinds 1 and 2 always; kind 0 with a
            // split screen (and in an online dodgeball game).
            static int IsEnable(long nType);

            static CShadowTexMan *m_pInstance; // lbl_805C0CF4

            int m_bEnable;         // 0x34, the texture was found
            GXTexObj m_TexObj;     // 0x38
            GXTexObj *m_pTexObj;   // 0x58, &m_TexObj once set up

        private:
            CShadowTexMan(bcs::CTask *pParent);
    };
}

#endif /* !MSW_CSHADOWTEXMAN_HPP_ */
