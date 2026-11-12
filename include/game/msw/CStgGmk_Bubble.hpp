#ifndef MSW_CSTGGMK_BUBBLE_HPP_
#define MSW_CSTGGMK_BUBBLE_HPP_

namespace bcs {
    class CTask;
    struct FVEC3;
}

namespace msw {

    // (class name guessed) A bubble of stage 11, made by
    // CGimmickMan::StgGmkCreate_Bubble_ (0x104 bytes, constructor
    // fn_801CDB64 in the DOL, which has no exported names for it).
    //
    // INCOMPLETE: only the constructor.
    class CStgGmk_Bubble {
        public:
            CStgGmk_Bubble(bcs::CTask *pParent, const bcs::FVEC3 &rPos);

        private:
            unsigned char m_Unk00[0x104]; // (not mapped; size from the allocation)
    };
}

#endif /* !MSW_CSTGGMK_BUBBLE_HPP_ */
