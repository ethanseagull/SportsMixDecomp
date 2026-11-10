#ifndef BCS_FVEC3_HPP_
#define BCS_FVEC3_HPP_

namespace bcs {

    // Three floats. Only the layout is known (msw::CTableSetMdlObj::
    // GetNodePosition in MenuF writes them at 0x0, 0x4 and 0x8).
    struct FVEC3 {
        float x; // 0x00
        float y; // 0x04
        float z; // 0x08

        FVEC3()
        {
        }

        FVEC3(float fx, float fy, float fz)
        {
            x = fx;
            y = fy;
            z = fz;
        }

        // A copy moves the three floats one by one through a float register
        // (lfs/stfs), not as words: msw::CStageMan::GetGoalAssumedPos in BossF
        // and msw::CDepthOfFieldValue::Main in MenuF show it. A struct without
        // this constructor is copied with lwz/stw.
        FVEC3(const FVEC3 &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

        // (name guessed) Inline in the modules.
        void Set(float fx, float fy, float fz)
        {
            x = fx;
            y = fy;
            z = fz;
        }
    };

    // (name guessed) pOut = p1 + (p2 - p1) * t.
    // The original is an inline routine written in paired-single assembly
    // (the compiler does not produce these instructions from float code), so
    // this one is too. It was written from what msw::CDepthOfFieldValue::Main
    // in MenuF shows after inlining; x and y go through as a pair, z alone.
    // The instructions are in the order bcs::FVEC3Lerp2 of bcs/BCS_Vec.hpp has
    // them (the TourF camera code needs that order; CDepthOfFieldValue::Main
    // comes out the same with either), so the two routines are now the same
    // and FVEC3Lerp2 can go.
    inline FVEC3 *FVEC3Lerp(register FVEC3 *pOut, register const FVEC3 *p1, register const FVEC3 *p2,
                            register float t)
    {
        register float from;
        register float to;
        register float work;

        asm {
            psq_l from, 0(p1), 0, 0
            psq_l to, 0(p2), 0, 0
            ps_sub work, to, from
            ps_madds0 work, work, t, from
            psq_st work, 0(pOut), 0, 0
            psq_l from, 8(p1), 1, 0
            psq_l to, 8(p2), 1, 0
            ps_sub work, to, from
            ps_madds0 work, work, t, from
            psq_st work, 8(pOut), 1, 0
        }
        return pOut;
    }

    // (name guessed) pOut = p1 * rate + p2 * (1 - rate): FVEC3Lerp with the
    // two ends the other way round. msw::CDepthOfFieldValue::Main reaches the
    // assembly routine through one more inline level with the pointers in
    // this order: called directly, FVEC3Lerp gives the two pointers and the
    // three float registers the opposite numbering.
    inline FVEC3 *FVEC3Blend(FVEC3 *pOut, const FVEC3 *p1, const FVEC3 *p2, float rate)
    {
        return FVEC3Lerp(pOut, p2, p1, rate);
    }
}

#endif /* !BCS_FVEC3_HPP_ */
