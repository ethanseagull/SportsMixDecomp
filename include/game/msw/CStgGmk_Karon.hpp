#ifndef MSW_CSTGGMK_KARON_HPP_
#define MSW_CSTGGMK_KARON_HPP_

namespace msw {

    // (class name guessed) A karon (dry bones) that walks out of a broken
    // case of the ship stage (stage 15): made by
    // CGimmickMan::StgGmkCreate_Karon_ (0x114 bytes, constructor
    // fn_801CF110 in the DOL, which has no exported names for it).
    //
    // INCOMPLETE: only what game/msw/CStgGmk_Case.cpp calls.
    class CStgGmk_Karon {
        public:
            // (DOL fn_801D01A8; names guessed) The size of the model and of
            // its collision and the walking speed, each kept above a
            // minimum, and a fourth value (-1 from the case).
            void SetParam(float fScale, float fSpeed, float fCollScale, float f);
            // (DOL fn_801D024C; name guessed) Starts it moving.
            void Start();
    };
}

#endif /* !MSW_CSTGGMK_KARON_HPP_ */
