#ifndef BCS_CSCNOBJGATHER_HPP_
#define BCS_CSCNOBJGATHER_HPP_

namespace bcs {

    class CScnObj;
    class CFrustum;

    // Collects the scene objects to draw in a frame. Only what is used so
    // far.
    class CScnObjGather {
        public:
            // (parameter meaning guessed) Adds pObj to the opaque and/or
            // the translucent list, culled against pFrustum. Return type
            // guessed.
            void Add(CScnObj *pObj, int bOpa, int bXlu, const CFrustum *pFrustum);
    };
}

#endif /* !BCS_CSCNOBJGATHER_HPP_ */
