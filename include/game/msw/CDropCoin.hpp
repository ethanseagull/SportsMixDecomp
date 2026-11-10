#ifndef MSW_CDROPCOIN_HPP_
#define MSW_CDROPCOIN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CDropCoinMan.hpp"
#include "nw4r/ut/List.hpp"

namespace bcs {
    class CMdlObj;
    class CMdlRef;
    class CMdlShadow;
}

namespace msw {

    // A coin (or star piece) lying on the court, made by CDropCoinMan. A
    // DOL class, size 0x80 (CDropCoinMan allocates that much). Defined in
    // game/msw/CDropCoinMan.cpp. Names of the members and of the functions
    // without a symbol are guessed.
    class CDropCoin : public bcs::CUnivTaskEx {
            friend class CDropCoinMan;

        public:
            // Bits of m_nFlag (names guessed from their use).
            enum {
                FLAG_ENABLE = 0x1,     // can be picked up
                FLAG_WAIT_BOUND = 0x2, // the life does not run while bounds are left
                FLAG_GROUND = 0x4,     // on the ground this frame
                FLAG_LANDED = 0x8,     // has touched the ground once
                FLAG_LAND_EFFECT = 0x10,
                FLAG_FALL = 0x20,      // fell into a hole in the court
                FLAG_NO_GRAVITY = 0x40,
                FLAG_STAR_PIECE = 0x80,
                FLAG_BURY = 0x100      // rises out of the ground
            };

            // An unmoving coin of type 0 at the origin, switched off.
            CDropCoin(CDropCoinMan *pMan);
            // A coin of `type` at rPos, switched on.
            CDropCoin(CDropCoinMan *pMan, const bcs::FVEC3 &rPos, DROP_COIN::CoinType type);
            virtual ~CDropCoin();
            virtual void Main();

            // True while the coin is switched on, has its model and was not
            // asked to be deleted.
            bool CanbePickup() const;
            // Throws the coin so that it lands at rTarget after fFrame frames
            // (at least 1), going up to fTopY.
            void DoParabolaThrow(const bcs::FVEC3 &rTarget, float fTopY, float fFrame);

        private:
            void CreateMdl_();
            // Turns the coin into one that rises out of the ground.
            void SetBury_();
            void MainMove_();
            // Bounces the coin off the ground; true when it touched it.
            int CheckBound_();
            void MainBury_();

            nw4r::ut::Link m_Link;      // 0x34, in CDropCoinMan's list
            CDropCoinMan *m_pMan;       // 0x3C
            long m_nType;               // 0x40, a DROP_COIN::CoinType
            unsigned long m_nFlag;      // 0x44
            bcs::CMdlObj *m_pMdl;       // 0x48
            bcs::CMdlShadow *m_pShadow; // 0x4C
            bcs::CMdlRef *m_pRef;       // 0x50
            bcs::FVEC3 m_vPos;          // 0x54
            // 0x60, per frame. A buried coin keeps its alpha in y while it
            // fades in.
            bcs::FVEC3 m_vVel;
            float m_fLife;              // 0x6C, frames
            float m_fGravity;           // 0x70, added to m_vVel.y per frame
            float m_fScale;             // 0x74
            long m_nValue;              // 0x78, 1 for a coin (meaning guessed)
            // 0x7C: bounds left; the step of a buried coin (2 waiting, 1
            // fading in, 0 done).
            long m_nCount;
    };
}

#endif /* !MSW_CDROPCOIN_HPP_ */
