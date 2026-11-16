#ifndef MSW_CCENDURESMOKEEFFECT_HPP_
#define MSW_CCENDURESMOKEEFFECT_HPP_

namespace bcs {
    class CTask;
}

namespace msw {
    class CChrCore;

    namespace CC_CMN {

        // (DOL class; meaning guessed from the name) The smoke behind a
        // character enduring a special shot in volleyball (VolleyF
        // game/volley/unit_3B628.cpp creates it with 0x64 bytes).
        // INCOMPLETE: the constructor is in main.dol and only declared;
        // the base and members are not mapped. Arguments (meanings
        // guessed): the parent task, the character, a kind, the yaw the
        // smoke goes to, an effect id and a rate.
        class CcEndureSmokeEffect {
            public:
                CcEndureSmokeEffect(bcs::CTask *pParent, CChrCore *pOwner, long, float fRotY, long nEff, float);

            private:
                unsigned char m_Unk00[0x64];
        };
    }
}

#endif /* !MSW_CCENDURESMOKEEFFECT_HPP_ */
