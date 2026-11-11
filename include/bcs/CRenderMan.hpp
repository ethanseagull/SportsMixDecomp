#ifndef BCS_CRENDERMAN_HPP_
#define BCS_CRENDERMAN_HPP_

namespace bcs {

    class CScnObj;

    // Only the members used so far. All are static: their bodies in the DOL
    // write globals and take no object. What the three floats of
    // SetDepthOfFieldParam mean is not known.
    class CRenderMan {
        public:
            // Adds pObj to the scene. (meaning of the second argument not
            // known: a render group, 2 or 4 in the volley stage)
            static void PushBack(CScnObj *pObj, long);
            static void EnableDepthOfFieldEffect(int bEnable);
            static void SetDepthOfFieldParam(long type, float fx, float fy, float fz);
            static void ReqCleanUp();
    };
}

#endif /* !BCS_CRENDERMAN_HPP_ */
