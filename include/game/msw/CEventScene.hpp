#ifndef MSW_CEVENTSCENE_HPP_
#define MSW_CEVENTSCENE_HPP_

#include "bcs/CTask.hpp"

namespace bcs {
    class CCameraAnm;
}

namespace msw {

    class CEvNodeData;
    class CEventModel;

    // An event scene: a camera animation with the event models that play
    // in it. A DOL class (constructor __ct__Q23msw11CEventSceneFlPQ23bcs5CTaskl,
    // vtable 0x8046F4C8). Only the members msw::CSceneCalc reads are mapped
    // so far; names guessed.
    class CEventScene : public bcs::CTask {
        public:
            CEventScene(long, bcs::CTask *pParent, long);
            virtual ~CEventScene();

            // The model nChr when that slot is in use (bit nChr of the mask
            // at 0x98), null otherwise.
            CEventModel *GetChr(long nChr) const
            {
                if (nChr != -1 && (m_nChrMask & (1 << nChr))) {
                    return m_ppChr[nChr];
                }
                return 0;
            }

        private:
            unsigned char m_Unk34[0x20]; // 0x34

        public:
            bcs::CCameraAnm *m_pCamera;  // 0x54: made by the constructor, 0x1A4 bytes

        private:
            CEvNodeData *m_pNodeData;    // 0x58
            CEventModel **m_ppChr;       // 0x5C: one per slot, count in the byte at 0x9B
            unsigned char m_Unk60[0x38]; // 0x60
            unsigned short m_nChrMask;   // 0x98: bit n set when slot n holds a model
    };
}

#endif /* !MSW_CEVENTSCENE_HPP_ */
