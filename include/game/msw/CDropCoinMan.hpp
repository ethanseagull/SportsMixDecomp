#ifndef MSW_CDROPCOINMAN_HPP_
#define MSW_CDROPCOINMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/ut/List.hpp"

namespace msw {

    class CDropCoin;

    // (namespace or class: not known from the symbols)
    namespace DROP_COIN {
        // The kind of a dropped coin: the index into the coin table of
        // game/msw/CDropCoinMan.cpp. 0 and 1 are coins, 2 to 4 star pieces
        // (they get a random colour). Names guessed.
        enum CoinType {
            COIN_TYPE_COIN = 0
        };
    }

    // Owns the coins lying on the court. A DOL class, size 0x50 (every
    // module's CGimmickMan<Game>::Initialize_ allocates that much). The
    // functions without a symbol are reached through CGimmickMan's
    // functions of the same name with a trailing underscore; their names
    // are taken from those.
    class CDropCoinMan : public bcs::CUnivTaskEx {
        public:
            CDropCoinMan(bcs::CTask *pParent);
            virtual ~CDropCoinMan();
            virtual void Main();
            // (name guessed) Empty; the only virtual the class adds.
            virtual void SendMessage(long nMsg, long a, long b, long c); // (name guessed: CGimmickMan::SendMessage_ forwards its four arguments here; empty)

            // The model file of coin type `type` in the item archive.
            static nw4r::g3d::ResFile GetRsrcFile(DROP_COIN::CoinType type);

            // Deletes every coin.
            void AllReset();
            // Drops nNum coins around rPos. Each lives fLife seconds (10.5 s
            // when not positive) plus up to fLifeRand seconds. Returns the
            // number of coins made. (parameter meanings guessed)
            long CreateCoin(const bcs::FVEC3 &rPos, long nNum, float fLife, float fLifeRand);
            // One switched-off coin at the origin.
            CDropCoin *CreateCoinSimple();
            // nNum coins that rise out of the ground at random places
            // between rMin and rMax. Returns the number made.
            long CreateBuryCoin(const bcs::FVEC3 &rMin, const bcs::FVEC3 &rMax, long nNum);
            // A star piece (or coin) of `type` thrown from rPos with rVel.
            // (meanings of the two longs guessed)
            CDropCoin *CreateStarPiece(DROP_COIN::CoinType type, long nValue, const bcs::FVEC3 &rPos,
                                       const bcs::FVEC3 &rVel, long nBound);
            long GetStarPieceNum() const;
            void DeleteAllStarPiece();
            // How much of the speed a bounce keeps: fY vertically, fXZ
            // along the ground. The spelling is the game's.
            void SetStarPieceRefelctVelReduce(float fY, float fXZ);
            void SetCoinRefelctVelReduce(float fY, float fXZ);
            CDropCoin *DropCoinGetFirst();
            CDropCoin *DropCoinGetNext(CDropCoin *pCoin);
            // Calls pFunc for each coin inside a cylinder of radius fRadius
            // and height fHeight standing on rPos, and collects it.
            void DropCoinPickup(const bcs::FVEC3 &rPos, float fRadius, float fHeight,
                                void (*pFunc)(const CDropCoin *, void *), void *pArg);
            // Empty in the DOL; CGimmickMan::EffectCoinGetCreate_ calls it.
            void EffectCoinGetCreate(const bcs::FVEC3 &rPos, DROP_COIN::CoinType type);

        private:
            nw4r::ut::List m_CoinList;  // 0x34
            float m_fCoinReduceY;       // 0x40
            float m_fCoinReduceXZ;      // 0x44
            float m_fStarPieceReduceY;  // 0x48
            float m_fStarPieceReduceXZ; // 0x4C

            friend class CDropCoin;
    };
}

#endif /* !MSW_CDROPCOINMAN_HPP_ */
