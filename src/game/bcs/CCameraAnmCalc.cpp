#include "bcs/CCameraAnmCalc.hpp"
#include "bcs/BCS_Vec.hpp"
#include "bcs/CCameraAnm.hpp"

namespace bcs {

    // (rRoll is not used.) Scales the position and the target, turns them
    // about Y and moves them by the animation's offsets.
    void CCameraAnmCalc::_CalcRotOffset(CCameraAnm &rAnm, FVEC3 &rPos, FVEC3 &rAt, float &rRoll)
    {
        rPos.x *= rAnm.m_vScale.x;
        rPos.y *= rAnm.m_vScale.y;
        rPos.z *= rAnm.m_vScale.z;
        rAt.x *= rAnm.m_vScale.x;
        rAt.y *= rAnm.m_vScale.y;
        rAt.z *= rAnm.m_vScale.z;

        float rotY = rAnm.m_fRotY;
        if (rotY > 0.0f) {
            BCS_Vec_RotY(rPos, rPos, rotY);
            BCS_Vec_RotY(rAt, rAt, rotY);
        }

        FVEC3Add(&rPos, &rPos, &rAnm.m_vPosOffset);
        FVEC3Add(&rAt, &rAt, &rAnm.m_vAtOffset);
    }

    // (rRoll is not used.) Mirrors the position and the target in x and z
    // about the point at 0x140, as the animation's flags ask.
    void CCameraAnmCalc::_CalcRev(CCameraAnm &rAnm, FVEC3 &rPos, FVEC3 &rAt, float &rRoll)
    {
        if (rAnm.m_nAnmFlag & 0x400) {
            rPos.x = rAnm.m_vUnk140.x - rPos.x;
            rAt.x = rAnm.m_vUnk140.x - rAt.x;
        }
        if (rAnm.m_nAnmFlag & 0x800) {
            rPos.z = rAnm.m_vUnk140.z - rPos.z;
            rAt.z = rAnm.m_vUnk140.z - rAt.z;
        }
    }
}
