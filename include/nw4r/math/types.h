#ifndef NW4R_MATH_TYPES_H_
#define NW4R_MATH_TYPES_H_

// Vector types of nw4r::math. Only what the game's code needs so far. The
// file name follows how other NW4R decompilations lay the library out
// (kiwi515/ogws); written for this project, no code was taken from them.

namespace nw4r {
    namespace math {

        // Added for BasketF game/basket/CCockpitBasketHarapeko.cpp (a pane's
        // scale).
        struct VEC2 {
            float x; // 0x00
            float y; // 0x04

            VEC2()
            {
            }

            VEC2(float fx, float fy)
            {
                x = fx;
                y = fy;
            }
        };

        struct VEC3 {
            float x; // 0x00
            float y; // 0x04
            float z; // 0x08

            VEC3()
            {
            }

            VEC3(float fx, float fy, float fz)
            {
                x = fx;
                y = fy;
                z = fz;
            }

            // Added for BasketF game/basket/CChrCoreBasketNinja.cpp, which
            // emits the first and third weak and calls all three (defined
            // below, after the routines they use).
            VEC3 operator+(const VEC3 &rhs) const;
            VEC3 operator-(const VEC3 &rhs) const;
            VEC3 operator*(float f) const;
            // Added for TourF game/tour/CTournamentRouteMan.cpp
            // (msw::CRouteDokan::PushRoute), which emits it weak and calls
            // it out of line.
            VEC3 &operator+=(const VEC3 &rhs);
        };

        // pOut = p1 + p2, p1 - p2 and p * f. In the original these are inline
        // routines written in paired-single assembly (x and y as a pair, z
        // alone), as the weak VEC3 operators of BasketF's
        // CChrCoreBasketNinja show; the compiler does not produce these
        // instructions from float code. The same exception as bcs::FVEC3Add
        // of bcs/BCS_Vec.hpp, with a plain C++ body for other compilers.
        inline VEC3 *VEC3Add(register VEC3 *pOut, register const VEC3 *p1, register const VEC3 *p2)
        {
#ifdef __MWERKS__
            register float a;
            register float b;
            register float work;

            asm {
                psq_l a, 0(p1), 0, 0
                psq_l b, 0(p2), 0, 0
                ps_add work, a, b
                psq_st work, 0(pOut), 0, 0
                psq_l a, 8(p1), 1, 0
                psq_l b, 8(p2), 1, 0
                ps_add work, a, b
                psq_st work, 8(pOut), 1, 0
            }
#else
            pOut->x = p1->x + p2->x;
            pOut->y = p1->y + p2->y;
            pOut->z = p1->z + p2->z;
#endif
            return pOut;
        }

        inline VEC3 *VEC3Sub(register VEC3 *pOut, register const VEC3 *p1, register const VEC3 *p2)
        {
#ifdef __MWERKS__
            register float a;
            register float b;
            register float work;

            asm {
                psq_l a, 0(p1), 0, 0
                psq_l b, 0(p2), 0, 0
                ps_sub work, a, b
                psq_st work, 0(pOut), 0, 0
                psq_l a, 8(p1), 1, 0
                psq_l b, 8(p2), 1, 0
                ps_sub work, a, b
                psq_st work, 8(pOut), 1, 0
            }
#else
            pOut->x = p1->x - p2->x;
            pOut->y = p1->y - p2->y;
            pOut->z = p1->z - p2->z;
#endif
            return pOut;
        }

        inline VEC3 *VEC3Scale(register VEC3 *pOut, register const VEC3 *p, register float f)
        {
#ifdef __MWERKS__
            register float a;
            register float work;

            asm {
                psq_l a, 0(p), 0, 0
                ps_muls0 work, a, f
                psq_st work, 0(pOut), 0, 0
                psq_l a, 8(p), 1, 0
                ps_muls0 work, a, f
                psq_st work, 8(pOut), 1, 0
            }
#else
            pOut->x = p->x * f;
            pOut->y = p->y * f;
            pOut->z = p->z * f;
#endif
            return pOut;
        }

        // x * x + y * y + z * z (added for bcs::CAnmBlendMotion, which
        // inlines it as paired-single code; same exception and fallback as
        // the three above).
        inline float VEC3LenSq(register const VEC3 *p)
        {
#ifdef __MWERKS__
            register float yz;
            register float x;

            asm {
                psq_l yz, 4(p), 0, 0
                psq_l x, 0(p), 1, 0
                ps_mul yz, yz, yz
                ps_madd x, x, x, yz
                ps_sum0 x, x, yz, yz
            }
            return x;
#else
            return p->x * p->x + p->y * p->y + p->z * p->z;
#endif
        }

        inline VEC3 VEC3::operator+(const VEC3 &rhs) const
        {
            VEC3 tmp;
            VEC3Add(&tmp, this, &rhs);
            return tmp;
        }

        inline VEC3 VEC3::operator-(const VEC3 &rhs) const
        {
            VEC3 tmp;
            VEC3Sub(&tmp, this, &rhs);
            return tmp;
        }

        inline VEC3 VEC3::operator*(float f) const
        {
            VEC3 tmp;
            VEC3Scale(&tmp, this, f);
            return tmp;
        }

        inline VEC3 &VEC3::operator+=(const VEC3 &rhs)
        {
            VEC3Add(this, this, &rhs);
            return *this;
        }

        // An axis-aligned box. Added for MenuF game/menu/CBgChr.cpp (the
        // screen boxes of the top menu's characters).
        struct AABB {
            VEC3 min; // 0x00
            VEC3 max; // 0x0C

            AABB()
            {
            }

            // Added for HockeyF game/hockey/CStageHockey17.cpp: the two
            // corners are copied as words and the box normalised.
            AABB(const VEC3 &rMin, const VEC3 &rMax) : min(rMin), max(rMax) { Normalize(); }

            // (DOL) Orders each axis's two values so min <= max.
            void Normalize();
        };

        // (DOL) Whether the two boxes overlap.
        bool IntersectionAABB(const AABB *pBox1, const AABB *pBox2);

        // A 3x4 matrix, row by row; column 3 is the translation. Added for
        // HockeyF game/hockey/CCockpitHockeyCoin.cpp (a pane's global
        // matrix, copied as words).
        struct MTX34 {
            // Named elements and the 3x4 array share the storage, as in NW4R:
            // the union makes an assignment copy words (lwz/stw), which
            // nw4r::lyt::DrawInfo::SetViewMtx does in game/msw/CNowLoading.cpp.
            // Checked on the 51 units naming MTX34 or CameraData: no score changed.
            union {
                struct {
                    float _00, _01, _02, _03;
                    float _10, _11, _12, _13;
                    float _20, _21, _22, _23;
                };
                float m[3][4];
            };

            // (added for game/msw/CNowLoading.cpp) Passes the matrix to the
            // SDK's Mtx functions.
            typedef float (*MtxPtr)[4];
            typedef const float (*ConstMtxPtr)[4];
            operator MtxPtr() { return reinterpret_cast<MtxPtr>(&_00); }
            operator ConstMtxPtr() const { return reinterpret_cast<ConstMtxPtr>(&_00); }
        };

        // (DOL; added for bcs::CAnmBlendMotion) Sets every element to 0.
        MTX34 *MTX34Zero(MTX34 *pOut);
        // (DOL; added for bcs::CalcChrAnmResultFromMtx) Whether the matrix
        // is the identity.
        bool MTX34IsIdentity(const MTX34 *pM);

        // (DOL) *pOut = *pM scaled by *pS. Added for the DOL's
        // msw::CStgSrt::GetMtx.
        MTX34 *MTX34Scale(MTX34 *pOut, const MTX34 *pM, const VEC3 *pS);

        // (DOL) *pOut = the rotation about X, Y and Z by the three angles,
        // given in units of 1/256 of a turn. Added for bcs::CScnObj::CalcRootMtx.
        MTX34 *MTX34RotXYZFIdx(MTX34 *pOut, float fIdxX, float fIdxY, float fIdxZ);
        // (DOL; added for MenuF's msw::CBBoxObj) *pIn turned by pMtx
        // without its translation.
        VEC3 *VEC3TransformNormal(VEC3 *pOut, const MTX34 *pMtx, const VEC3 *pIn);

        // (DOL) *pOut = the rotation about *pAxis by fIdx (1/256 of a
        // turn). Added for the DOL's pinball ball, which inlines the radian
        // form below.
        MTX34 *MTX34RotAxisFIdx(MTX34 *pOut, const VEC3 *pAxis, float fIdx);

        inline MTX34 *MTX34RotAxisRad(MTX34 *pOut, const VEC3 *pAxis, float fRad)
        {
            return MTX34RotAxisFIdx(pOut, pAxis, 40.743664f * fRad);
        }

        // The same with the angles in radians (256 / 2 pi = 40.743664 units
        // per radian). Inline in bcs::CScnObj::CalcRootMtx.
        inline MTX34 *MTX34RotXYZRad(MTX34 *pOut, float fRadX, float fRadY, float fRadZ)
        {
            return MTX34RotXYZFIdx(pOut, 40.743664f * fRadX, 40.743664f * fRadY, 40.743664f * fRadZ);
        }
    }
}

#endif /* !NW4R_MATH_TYPES_H_ */
