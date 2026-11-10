#ifndef BCS_CRENDERMAN_HPP_
#define BCS_CRENDERMAN_HPP_

namespace bcs {

    // Only the members used so far. Both are static: their bodies in the DOL
    // write globals and take no object. What the three floats mean is not
    // known.
    class CRenderMan {
        public:
            static void EnableDepthOfFieldEffect(int bEnable);
            static void SetDepthOfFieldParam(long type, float fx, float fy, float fz);
    };
}

#endif /* !BCS_CRENDERMAN_HPP_ */
