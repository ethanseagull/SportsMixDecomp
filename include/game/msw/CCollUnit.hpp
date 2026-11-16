#ifndef MSW_CCOLLUNIT_HPP_
#define MSW_CCOLLUNIT_HPP_

#include "bcs/FVEC3.hpp"
#include "nw4r/ut/List.hpp"

namespace msw {

    // One collision unit, the base of the attack, body and defence units
    // (msw::CCollAtkUnit, CCollBodyUnit, CCollDefUnit). A DOL class, 0xA0
    // bytes: the constructor initialises everything up to 0x9E and the
    // derived units' own members start at 0xA0. Vtable
    // __vt__Q23msw9CCollUnit (only the destructor).
    //
    // Each derived unit joins one of CCollMan's unit lists through m_Link
    // when it is built and leaves it when it is destroyed.
    //
    // Member names are guesses, taken from the set classes
    // (CBodyCollision, CAtkCollision, CDefCollision) that write them.
    class CCollUnit {
        public:
            // (arguments: the owner's kind, the shape and the owner's
            // handle; names guessed) The kind is stored twice (0x12, 0x15).
            CCollUnit(long kind, long type, unsigned long handle);
            virtual ~CCollUnit();

            void SetCollKind(long kind);
            void SetCollType(long type);
            // (added for game/basket/unit_2FD1C.cpp) The kind of attack (bits
            // 8 to 15; the smash cut sets 0x20000; also msw::CBombExplosion::Create_).
            void SetAtkType(unsigned long type);
            void SetPos(const bcs::FVEC3 &rPos);
            // Recomputes the bounding box (m_AabbMin, m_AabbMax) for the
            // shape; CBodyCollision::CalcAABBAll.
            void CalcAABB();
            // The index of the character that owns the unit, or -1 when the
            // owner is not a character (kind 1).
            long GetCollOwnerChrIdx() const;
            // (name guessed; the DOL's msw::CChrCore calls it) Plays the hit
            // sound effect m_nHitSe, if any, at rPos.
            void PlayHitSe(const bcs::FVEC3 &rPos, unsigned long arg) const;

            // (names guessed) Inline: compared as signed ints (DodgeF,
            // GetCollOwnerChrIdx).
            long GetOwnerKind() const { return m_nOwnerKind; }
            long GetUnk15() const { return m_Unk15; }
            // (name guessed; VolleyF's CStgGmk_Pinball_Act_Volley)
            const bcs::FVEC3 &GetPos() const { return m_Pos; }
            // (name guessed; BasketF's CcSiromaMagicCircle) The other end.
            const bcs::FVEC3 &GetPos2() const { return m_Pos2; }

            // (name guessed) Inline in msw::CStgGmk_WaterPillar_Act. It sets
            // the word at 0x18, which is the target flags (m_nTargetFlag).
            void SetShape(unsigned long shape) { m_nShape = shape; }
            void SetSize(const bcs::FVEC3 &rSize) { m_Size = rSize; }
            // (added for game/msw/CStgGmk_Case.cpp)
            const bcs::FVEC3 &GetSize() const { return m_Size; }
            void SetSize(float x, float y, float z)
            {
                m_Size.x = x;
                m_Size.y = y;
                m_Size.z = z;
            }
            void OnFlag10(unsigned short flag) { m_nFlag10 |= flag; }
            void OffFlag10(unsigned short flag) { m_nFlag10 &= ~flag; }
            // (name guessed; added for BossF game/boss/CChrCoreBoss.cpp,
            // which sets the defence kind of its two units this way)
            void SetIndex(long n) { m_nIndex = n; }
            // (name guessed; VolleyF's CStgGmk_Trolley_Act_Volley) Radians, 0 to
            // 2 pi: the callers wrap the angle first.
            void SetRotY(float rot) { m_fRotY = rot; }
            // (name guessed; inline in game/msw/CStgGmk_Trolley.cpp, DOL)
            // The same, first bringing the angle into [0, 2 pi).
            void SetRotYWrapped(float rot)
            {
                while (rot > 6.2831855f) {
                    rot -= 6.2831855f;
                }
                while (rot < 0.0f) {
                    rot += 6.2831855f;
                }
                m_fRotY = rot;
            }
            // (names guessed) Inline in BasketF's CChrCoreBasketYoshi:
            // both set to the bit of the owner's player number.
            // These two predate the merge of the unit headers and keep what
            // they did: SetNoHitFlag writes 0x1C (m_nMyNoHitFlag) and
            // SetMyNoHitFlag 0x20 (m_nNoHitFlag). Which is which is not known.
            void SetNoHitFlag(unsigned long flag) { m_nMyNoHitFlag = flag; }
            void SetMyNoHitFlag(unsigned long flag) { m_nNoHitFlag = flag; }

            // 0x00: vtable
            nw4r::ut::Link m_Link;       // 0x04, in one of CCollMan's unit lists
            // 0x0C (name guessed): the owner's handle, the last constructor
            // argument (CBodyCollision::SetOwnerHandleAll).
            // Also m_nUnk0C, its name in the attack unit and CDefCollision.
            union {
                unsigned long m_nOwnerHandle;
                unsigned long m_nUnk0C;
            };
            // 0x10: bit 0 on (the set classes' SetEnable), bit 1 the first
            // position not taken yet, bit 2 a unit that keeps its previous
            // position.
            unsigned short m_nFlag10;
            // 0x12 (name guessed): what owns the unit (1: a character);
            // SetCollKind.
            // Also m_nCollKind (CDefCollision).
            union {
                unsigned char m_nOwnerKind;
                unsigned char m_nCollKind;
            };
            // 0x13: the shape (SetCollType). 0 sphere, 1 and 5 cylinder,
            // 2 a capsule lying in XZ, 3 a capsule between m_Pos and m_Pos2,
            // 4 a box turned by m_fRotY, 6 a swept capsule (see CalcAABB).
            unsigned char m_nCollType;
            // 0x14 (name guessed): the owner's index (the character's, when
            // the kind is 1); read signed. For a defence unit, its kind
            // (CDefCollision::SetIndexAll).
            // Also m_nOwnerNo (attack units) and m_nIndex (CDefCollision).
            union {
                signed char m_nOwnerIdx;
                signed char m_nOwnerNo;
                signed char m_nIndex;
            };
            // 0x15: the constructor's kind again (CAtkCollision::SetUnk15All);
            // GetCollOwnerChrIdx takes either as a character.
            unsigned char m_Unk15;
            signed char m_nTeamNo;       // 0x16 (name guessed): the owner's team
            // 0x17 (name guessed): the index of the character behind the unit
            // (GetCollOwnerChrIdx; DodgeF's bomb blast sets the thrower's).
            // Also m_nUnk17 (body units) and m_nOwnerChrIdx.
            union {
                signed char m_nChrNo;
                signed char m_nUnk17;
                signed char m_nOwnerChrIdx;
            };
            // 0x18: the flags of what the unit hits or is hit by
            // (CBodyCollision::SetTargetFlag; 0x7F for a body unit).
            // Also m_nFlag (attack units) and m_nShape (SetShape; an older
            // guess).
            union {
                unsigned long m_nTargetFlag;
                unsigned long m_nFlag;
                unsigned long m_nShape;
            };
            // 0x1C (the set classes' SetMyNoHitFlagAll). CCollUnit's header
            // called it m_nNoHitFlag before the merge; CDefCollision.cpp was
            // changed to this name, as m_nNoHitFlag is 0x20 for the others.
            unsigned long m_nMyNoHitFlag;
            unsigned long m_nNoHitFlag;    // 0x20 (SetNoHitFlag)
            unsigned long m_nTmpNoHitFlag; // 0x24 (SetTmpNoHitFlag)
            unsigned long m_Unk28;
            // 0x2C (name guessed): the unit's position. The attack units'
            // callers read it as three floats (m_fPosX..Z). FVEC3 has
            // constructors, so this union is a compiler extension (MWCC warns,
            // 10402); a port may give FVEC3 trivial constructors or drop the
            // float names.
            union {
                bcs::FVEC3 m_Pos;
                struct {
                    float m_fPosX;
                    float m_fPosY;
                    float m_fPosZ;
                };
            };
            bcs::FVEC3 m_Pos2;           // 0x38 (name guessed): the other end of a capsule
            // 0x44 (name guessed): a third point the box of shape 6 is
            // grown to take in.
            bcs::FVEC3 m_Pos3;
            bcs::FVEC3 m_PrevPos;        // 0x50 (name guessed; SetPos)
            // 0x5C: the unit's size; x is the radius of a sphere, y the
            // height of a cylinder.
            bcs::FVEC3 m_Size;
            bcs::FVEC3 m_AabbMin;        // 0x68 (name guessed; CalcAABB)
            bcs::FVEC3 m_AabbMax;        // 0x74
            float m_fRotY;               // 0x80, radians in [0, 2 pi) (SetRotY)
            // 0x84, (name guessed) bits 8 to 15 are the kind of attack
            // (DodgeF's time bomb reacts to kind 0x15).
            unsigned long m_nAtkType;
            // 0x88 to 0x9E (names guessed; the attack's parameters, set by
            // CChrCoreBasketKinopio from CChrCoreBasket::GetAtkVanishPower and
            // GetAtkDownFrame).
            float m_fVanishPower;        // 0x88
            // 0x8C: bit 0 set with the attack direction, read by
            // CAtkCollision::IsAtkHit (read signed).
            short m_nAtkHitFlag;
            short m_nHitCamVibID;        // 0x8E (CAtkCollision::SetHitCamVibIDAll)
            short m_nUnk90;              // 0x90, the effect of a hit (CAtkCollision::SetHitEffectID)
            short m_nHitSe;              // 0x92, a sound id (PlayHitSe; -1 none)
            float m_fDownFrame;          // 0x94 (CAtkCollision::SetHitEffectFrame)
            float m_fUnk98;              // 0x98 (CAtkCollision::SetUnk98All)
            short m_nUnk9C;              // 0x9C (CAtkCollision::SetUnk9CAll)
            short m_nUnk9E;              // 0x9E
    };
}

#endif /* !MSW_CCOLLUNIT_HPP_ */
