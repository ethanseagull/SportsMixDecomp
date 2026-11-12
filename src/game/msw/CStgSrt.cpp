#include "game/msw/CStgSrt.hpp"
#include "bcs/BCS_Math.hpp"
#include "bcs/BCS_Vec.hpp"
#include "nw4r/math/types.h"
#include "revolution/mtx.h"

// msw::CStgSrt, the position, direction and scale of a stage object, and
// msw::CStgSrtMove, the moving version msw::CStgAct embeds. Both vtables
// are emitted here (CStgSrtMove's first, as in the DOL's .data).

namespace msw {

    CStgSrt::CStgSrt()
    {
        m_Pos.Set(0.0f, 0.0f, 0.0f);
        m_Dir.Set(0.0f, 0.0f, 1.0f);
        m_Up.Set(0.0f, 1.0f, 0.0f);
        m_Scale.Set(1.0f, 1.0f, 1.0f);
        m_nFlag = 7;
    }

    // Everything to zero, the matrices to the identity, all flagged.
    void CStgSrt::Reset()
    {
        m_nFlag = 7;
        m_Pos.Set(0.0f, 0.0f, 0.0f);
        m_Dir.Set(0.0f, 0.0f, 0.0f);
        m_Up.Set(0.0f, 0.0f, 0.0f);
        m_Scale.Set(0.0f, 0.0f, 0.0f);
        PSMTXIdentity(m_Mtx.m);
        PSMTXIdentity(m_RotMtx.m);
        PSMTXIdentity(m_ScaleMtx.m);
    }

    // A zero direction becomes the default front (0, 0, 1).
    void CStgSrt::SetFront(const bcs::FVEC3 &rFront)
    {
        m_Dir = rFront;
        if (bcs::FVEC3LenSq(&m_Dir) == 0.0f) {
            m_Dir.Set(0.0f, 0.0f, 1.0f);
        }
        m_nFlag |= 2;
    }

    void CStgSrt::SetFront(float x, float y, float z)
    {
        m_Dir.Set(x, y, z);
        if (bcs::FVEC3LenSq(&m_Dir) == 0.0f) {
            m_Dir.Set(0.0f, 0.0f, 1.0f);
        }
        m_nFlag |= 2;
    }

    // A zero up vector becomes the default (0, 1, 0).
    void CStgSrt::SetUp(float x, float y, float z)
    {
        m_Up.Set(x, y, z);
        if (bcs::FVEC3LenSq(&m_Up) == 0.0f) {
            m_Up.Set(0.0f, 1.0f, 0.0f);
        }
        m_nFlag |= 2;
    }

    // Turns the rotation the part rate (0 to 1) of the way to rTo: the
    // blend of the two as quaternions.
    inline void CStgSrt::BlendRotMtx(const bcs::FMTX34 &rTo, float rate)
    {
        rate = rate > 1.0f ? 1.0f : (rate < 0.0f ? 0.0f : rate);

        bcs::QUATERNION quat;
        bcs::QUATERNION from;
        bcs::QUATERNION to;
        bcs::BCS_Quat_FromMatrix(from, GetRotMtx());
        bcs::BCS_Quat_FromMatrix(to, rTo);
        bcs::BCS_Quat_Slerp(quat, from, to, rate);

        bcs::FMTX34 mtx;
        PSMTXQuat(mtx.m, reinterpret_cast<const Quaternion *>(&quat));
        m_Dir.x = mtx.m[0][2];
        m_Dir.y = mtx.m[1][2];
        m_Dir.z = mtx.m[2][2];
        m_Up.x = mtx.m[0][1];
        m_Up.y = mtx.m[1][1];
        m_Up.z = mtx.m[2][1];
        bcs::BCS_Vec_Normalize(m_Dir, m_Dir);
        bcs::BCS_Vec_Normalize(m_Up, m_Up);
        PSMTXCopy(mtx.m, m_RotMtx.m);
        m_nFlag &= ~2;
    }

    void CStgSrt::LookAt(const bcs::FVEC3 &rTarget, float maxAngle)
    {
        bcs::FVEC3 dir;
        bcs::FVEC3FromTo(&dir, &m_Pos, &rTarget);
        bcs::BCS_Vec_Normalize(dir, dir);

        float cos = bcs::FVEC3Dot(&m_Dir, &dir);
        cos = cos > 1.0f ? 1.0f : (cos < -1.0f ? -1.0f : cos);
        float angle = bcs::BCS_ACosT(cos);

        bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
        bcs::FMTX34 target;
        CalcRotMtx(target, dir, up);
        BlendRotMtx(target, angle < maxAngle ? 1.0f : maxAngle / angle);
    }

    void CStgSrt::SetRotY(float rad)
    {
        bcs::FVEC3 front;
        bcs::FVEC3 up(0.0f, 1.0f, 0.0f);
        PSMTXMultVec(GetRotMtx().m, (const Vec *)&up, (Vec *)&up);

        bcs::FVEC3 side(1.0f, 0.0f, 0.0f);
        bcs::FMTX34 mtx;
        CalcRotMtxSide(mtx, side, up);
        nw4r::math::MTX34 rot;
        nw4r::math::MTX34RotXYZRad(&rot, 0.0f, rad, 0.0f);
        PSMTXConcat(mtx.m, rot, mtx.m);

        bcs::FVEC3 z(0.0f, 0.0f, 1.0f);
        PSMTXMultVec(mtx.m, (const Vec *)&z, (Vec *)&front);
        bcs::FVEC3 y(0.0f, 1.0f, 0.0f);
        PSMTXMultVec(mtx.m, (const Vec *)&y, (Vec *)&up);

        SetFront(front);
        SetUp(up);
    }

    void CStgSrt::AddRotY(float rad)
    {
        bcs::FMTX34 mtx;
        CalcRotMtx(mtx, m_Dir, m_Up);
        bcs::FVEC3 axis;
        axis.x = mtx.m[0][1];
        axis.y = mtx.m[1][1];
        axis.z = mtx.m[2][1];

        nw4r::math::MTX33 rot33;
        nw4r::math::MTX34 rot;
        PSMTXRotAxisRad(rot, (const Vec *)&axis, rad);
        nw4r::math::MTX34ToMTX33(&rot33, &rot);
        nw4r::math::MTX34 turn;
        nw4r::math::MTX33ToMTX34(&turn, &rot33);
        nw4r::math::VEC3TransformNormal(reinterpret_cast<nw4r::math::VEC3 *>(&m_Dir), &turn,
                                        reinterpret_cast<const nw4r::math::VEC3 *>(&m_Dir));
        m_nFlag |= 2;
    }

    namespace {

        // The matrix with the axes rX, rY and rZ as its columns and no
        // translation.
        inline void SetAxes(bcs::FMTX34 &rMtx, const bcs::FVEC3 &rX, const bcs::FVEC3 &rY, const bcs::FVEC3 &rZ)
        {
            rMtx.m[0][0] = rX.x;
            rMtx.m[1][0] = rX.y;
            rMtx.m[2][0] = rX.z;
            rMtx.m[0][1] = rY.x;
            rMtx.m[1][1] = rY.y;
            rMtx.m[2][1] = rY.z;
            rMtx.m[0][2] = rZ.x;
            rMtx.m[1][2] = rZ.y;
            rMtx.m[2][2] = rZ.z;
            rMtx.m[0][3] = 0.0f;
            rMtx.m[1][3] = 0.0f;
            rMtx.m[2][3] = 0.0f;
        }
    }

    // The rotation whose z axis is rDir, with the y axis as close to rUp
    // as it can be. An up vector (almost) along the direction is replaced
    // by the z axis, away from the direction.
    void CStgSrt::CalcRotMtx(bcs::FMTX34 &rMtx, const bcs::FVEC3 &rDir, const bcs::FVEC3 &rUp)
    {
        bcs::FVEC3 x;
        bcs::FVEC3 y;
        bcs::FVEC3 z;
        bcs::BCS_Vec_Normalize(z, rDir);
        bcs::BCS_Vec_Normalize(y, rUp);
        float cos = bcs::FVEC3Dot(&z, &y);
        if (cos >= 0.999f) {
            y.Set(0.0f, 0.0f, -1.0f);
        } else if (cos <= -0.999f) {
            y.Set(0.0f, 0.0f, 1.0f);
        }
        PSVECCrossProduct((const Vec *)&y, (const Vec *)&z, (Vec *)&x);
        bcs::BCS_Vec_Normalize(x, x);
        PSVECCrossProduct((const Vec *)&z, (const Vec *)&x, (Vec *)&y);
        SetAxes(rMtx, x, y, z);
    }

    // The same with the x axis given: the rotation whose x axis is rSide,
    // with the y axis as close to rUp as it can be.
    void CStgSrt::CalcRotMtxSide(bcs::FMTX34 &rMtx, const bcs::FVEC3 &rSide, const bcs::FVEC3 &rUp)
    {
        bcs::FVEC3 x;
        bcs::FVEC3 y;
        bcs::FVEC3 z;
        bcs::BCS_Vec_Normalize(x, rSide);
        bcs::BCS_Vec_Normalize(y, rUp);
        float cos = bcs::FVEC3Dot(&x, &y);
        if (cos >= 0.999f) {
            y.Set(0.0f, 0.0f, 1.0f);
        } else if (cos <= -0.999f) {
            y.Set(0.0f, 0.0f, -1.0f);
        }
        PSVECCrossProduct((const Vec *)&x, (const Vec *)&y, (Vec *)&z);
        bcs::BCS_Vec_Normalize(z, z);
        PSVECCrossProduct((const Vec *)&z, (const Vec *)&x, (Vec *)&y);
        SetAxes(rMtx, x, y, z);
    }

    CStgSrtMove::CStgSrtMove()
    {
        InitMove();
    }

    CStgSrtMove::~CStgSrtMove()
    {
    }

    void CStgSrtMove::Reset()
    {
        CStgSrt::Reset();
        InitMove();
    }

    void CStgSrtMove::CopyFrom(const CStgSrtMove &rSrc)
    {
        m_Pos = rSrc.m_Pos;
        m_Dir = rSrc.m_Dir;
        m_Up = rSrc.m_Up;
        m_Scale = rSrc.m_Scale;
        PSMTXCopy(rSrc.m_Mtx.m, m_Mtx.m);
        PSMTXCopy(rSrc.m_RotMtx.m, m_RotMtx.m);
        PSMTXCopy(rSrc.m_ScaleMtx.m, m_ScaleMtx.m);
        m_nFlag = rSrc.m_nFlag;
        m_nFlagC8 = rSrc.m_nFlagC8;
        m_Vel = rSrc.m_Vel;
        m_WorldVel = rSrc.m_WorldVel;
        m_UnkE4 = rSrc.m_UnkE4;
        m_UnkF0 = rSrc.m_UnkF0;
        m_fUnkFC = rSrc.m_fUnkFC;
        m_Unk100 = rSrc.m_Unk100;
        m_Unk10C = rSrc.m_Unk10C;
        m_VelMax = rSrc.m_VelMax;
        m_VelMin = rSrc.m_VelMin;
        m_fAccelRate = rSrc.m_fAccelRate;
        m_fDecRate = rSrc.m_fDecRate;
    }

    namespace {

        inline nw4r::math::VEC3 *AsVEC3(bcs::FVEC3 *p) { return reinterpret_cast<nw4r::math::VEC3 *>(p); }
        inline const nw4r::math::VEC3 *AsVEC3(const bcs::FVEC3 *p)
        {
            return reinterpret_cast<const nw4r::math::VEC3 *>(p);
        }
    }

    // One frame of motion, rate frames of 1/60 s long. The velocity is
    // worked on in the object's own axes: the acceleration is added, the
    // deceleration takes the speed down towards zero, the resistance
    // (flag 0x10) works against each component, and the limits (flags 1
    // and 2) clamp it. The distance moved, the velocity's plus half the
    // changes', is turned back to world axes; the fall vector at 0xE4
    // grows by the gravity at 0x10C. The old position is kept in
    // m_PrevPos.
    void CStgSrtMove::Update(float rate)
    {
        bcs::FVEC3 pos;
        bcs::FVEC3 vel;
        bcs::FVEC3 accel;
        bcs::FVEC3 fall;
        bcs::FVEC3 move;
        bcs::FVEC3 dv;
        bcs::FVEC3 decv;
        bcs::FVEC3 resist;
        bcs::FVEC3 limit;
        bcs::FVEC3 min;
        bcs::FVEC3 max;
        bcs::FVEC3 prev;
        bcs::FVEC3 half;
        bcs::FVEC3 halfResist;
        bcs::FVEC3 halfGravity;
        bcs::FVEC3 gravity;
        bcs::FVEC3 fallMove;
        bcs::FMTX34 rot;
        bcs::FMTX34 inv;

        PSMTXCopy(GetRotMtx().m, rot.m);
        PSMTXTranspose(rot.m, inv.m);

        bcs::FVEC3Scale(&accel, &m_UnkF0, m_fAccelRate);
        if ((m_nFlagC8 >> 4) & 1) {
            PSMTXMultVec(inv.m, (const Vec *)&m_WorldVel, (Vec *)&vel);
        } else {
            vel = m_Vel;
        }

        float dt = (1.0f / 60.0f) * rate;
        bcs::FVEC3Scale(&dv, &accel, dt);
        bcs::FVEC3Scale(&move, &vel, dt);
        bcs::FVEC3Add(&vel, &vel, &dv);
        pos.Set(0.0f, 0.0f, 0.0f);

        // Deceleration along the velocity.
        float speed = PSVECMag((const Vec *)&vel);
        float dec = m_fUnkFC * m_fDecRate;
        if (0.0f > dec) {
            dec = 0.0f;
        }
        if (speed > 0.0f && dec > 0.0f) {
            bcs::BCS_Vec_Normalize(decv, vel);
            bcs::FVEC3Scale(&decv, &decv, -dec * ((1.0f / 60.0f) * rate));
            if (speed > dec * ((1.0f / 60.0f) * rate)) {
                bcs::FVEC3Add(&vel, &vel, &decv);
                speed = PSVECMag((const Vec *)&vel);
            } else {
                speed = 0.0f;
                vel.Set(speed, speed, speed);
            }
            bcs::FVEC3Scale(&decv, &decv, 0.5f * ((1.0f / 60.0f) * rate));
            dec = PSVECMag((const Vec *)&decv);
        } else {
            dec = 0.0f;
            decv.Set(dec, dec, dec);
        }

        // Resistance against each component, no more than the component.
        if (((m_nFlagC8 >> 4) & 1) && speed > 0.0f) {
            bcs::FVEC3Scale(&limit, &m_Unk100, (1.0f / 60.0f) * rate);
            bcs::FVEC3Scale(&resist, &vel, -1.0f);
            resist.x = resist.x > limit.x ? limit.x : (resist.x < -limit.x ? -limit.x : resist.x);
            resist.y = resist.y > limit.y ? limit.y : (resist.y < -limit.y ? -limit.y : resist.y);
            resist.z = resist.z > limit.z ? limit.z : (resist.z < -limit.z ? -limit.z : resist.z);
            bcs::FVEC3Add(&vel, &vel, &resist);
        } else {
            resist.Set(0.0f, 0.0f, 0.0f);
        }

        if (m_nFlagC8 & 1) {
            nw4r::math::VEC3Maximize(AsVEC3(&vel), AsVEC3(&vel), AsVEC3(&m_VelMin));
        }
        if ((m_nFlagC8 >> 1) & 1) {
            nw4r::math::VEC3Minimize(AsVEC3(&vel), AsVEC3(&vel), AsVEC3(&m_VelMax));
        }

        // The distance moved this frame.
        bcs::FVEC3Scale(&half, &dv, 0.5f * ((1.0f / 60.0f) * rate));
        bcs::FVEC3Add(&move, &move, &half);
        if (dec > 0.0f) {
            if (PSVECMag((const Vec *)&move) > dec) {
                bcs::FVEC3Add(&move, &move, &decv);
            } else {
                move.Set(0.0f, 0.0f, 0.0f);
            }
        }
        float t = (1.0f / 60.0f) * rate;
        bcs::FVEC3Scale(&halfResist, &resist, 0.5f * t);
        bcs::FVEC3Add(&move, &move, &halfResist);
        if (m_nFlagC8 & 1) {
            bcs::FVEC3Scale(&min, &m_VelMin, t);
            nw4r::math::VEC3Maximize(AsVEC3(&move), AsVEC3(&move), AsVEC3(&min));
        }
        if ((m_nFlagC8 >> 1) & 1) {
            bcs::FVEC3Scale(&max, &m_VelMax, (1.0f / 60.0f) * rate);
            nw4r::math::VEC3Minimize(AsVEC3(&move), AsVEC3(&move), AsVEC3(&max));
        }
        bcs::FVEC3Add(&pos, &pos, &move);

        // Back to world axes.
        m_Vel = vel;
        PSMTXMultVec(rot.m, (const Vec *)&vel, (Vec *)&m_WorldVel);
        PSMTXMultVec(rot.m, (const Vec *)&pos, (Vec *)&pos);

        // The fall, in world axes.
        float dt2 = (1.0f / 60.0f) * rate;
        bcs::FVEC3Scale(&gravity, &m_Unk10C, dt2);
        bcs::FVEC3Scale(&fallMove, &m_UnkE4, dt2);
        bcs::FVEC3Scale(&halfGravity, &gravity, 0.5f * dt2);
        bcs::FVEC3Add(&m_UnkE4, &m_UnkE4, &gravity);
        bcs::FVEC3Add(&fallMove, &fallMove, &halfGravity);
        fall.Set(0.0f, 0.0f, 0.0f);
        bcs::FVEC3Add(&fall, &fall, &fallMove);
        bcs::FVEC3Add(&pos, &pos, &m_Pos);
        bcs::FVEC3Add(&pos, &pos, &fall);

        prev = m_Pos;
        SetPos(pos);
        m_PrevPos = prev;
    }

    void CStgSrtMove::OnUpdate()
    {
    }
}
