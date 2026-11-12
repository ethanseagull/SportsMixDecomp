#ifndef CGAMECAMERA_HPP_
#define CGAMECAMERA_HPP_

#include "bcs/CCamera.hpp"

namespace msw {

    class CBall;
    class CChrCore;

    // Size 0x254 (from its allocation site). The camera of a match; each
    // sport derives its own. The class lives in the DOL: its constructor,
    // destructor and most virtuals are there, the four inline virtuals are
    // emitted weak by the module units that emit a derived vtable (BasketF
    // game/basket/CCameraBasket.cpp). Vtable from the DOL (17 words).
    // Return types of the virtuals are guessed unless stated. The inline
    // virtuals are declared in the reverse of the order the module emits
    // them (MessageHandler, GetFrameRateF, IsNormalCamMode, GetCamKind);
    // overrides keep their base's vtable slot wherever they are declared.
    class CGameCamera : public bcs::CCamera {
        public:
            // (contents not mapped)
            enum MSG_ID {};
            struct MSG_PARAM;

            CGameCamera(long view, short priority);
            virtual ~CGameCamera();
            virtual void Disp();
            virtual long GetCamKind() const { return 3; }
            virtual void MainCamera();
            virtual int OnActive();
            virtual void Reset();
            virtual void StartArtsEventDemo();
            virtual void EndArtsEventDemo();
            virtual bool IsNormalCamMode() const { return true; }
            virtual bool CanbeMain() const;
            // Returns a word: its callers compare it with cmpwi and turn it
            // into a mode with neg/or/srawi.
            virtual int IsOffense() const;
            virtual float GetFrameRateF() const
            {
                if (m_UnkFC & 0x80) {
                    return 0.0f;
                }
                return bcs::CMainApp::GetAppFrameRateF();
            }
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam) { return 0; }

            // Static: the bodies in the DOL take no object.
            static void Initialize();
            static void Finalize();
            static CGameCamera *GetInstance(long view);
            static long GetInstanceNum();
            // (DOL; added for BossF game/boss/CBallBoss.cpp, meaning of the
            // first argument not known) Where the ball is heading.
            void SetBallTargetPos(int, const bcs::FVEC3 &rPos);
            // Static (the DOL body takes no object and returns 1). HockeyF's
            // CGoalMdlHockey::NewMdl gives the goal a fading model when it
            // is true.
            static int IsBehindCamera();

            // One camera per view, made by each sport's Initialize.
            static CGameCamera *g_pInstance[4];
            static long g_nInstanceNum;

            // DOL members used by the sports' cameras (BasketF
            // game/basket/CCameraBasketHB.cpp). `this` and the arguments are
            // read from the callers and the DOL bodies; return types and
            // parameter names are guessed.
            // Sets the camera mode; the DOL body returns 0 when `mode` is
            // the current one and `bForce` is zero.
            int SetCamMode(long mode, int bForce);
            void CalcCamData(float rate);
            void CalcLookPos(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rArg2, const bcs::FVEC3 &rArg3, float rate,
                             long mask);
            void MakeingFrontCamData(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget, float, float, float, float,
                                     float, float);
            float CalcCurveBlend(float, float, float, float, float, float, float, float, float, float) const;
            // The ball's and the character's position, x and z negated when
            // bit 0x2 of m_UnkFC is set.
            bcs::FVEC3 GetBallPosXZ(const CBall *pBall) const;
            bcs::FVEC3 GetCorePosXZ(const CChrCore *pCore) const;
            // (added for VolleyF CCameraVolleyRR; return meaning guessed) The
            // point the ball is heading for, x and z as in GetBallPosXZ; zero
            // when there is none.
            int GetBallTargetPosXZ(bcs::FVEC3 &rPos, const CBall *pBall) const;
            void NormalizePersPos(bcs::FVEC3 &rPos, float, float min, float max);
            // DOL. (return type guessed) Not zero when the camera looks at
            // the other goal (HockeyF CStageHockey01::Main_ turns the stage's
            // background half a turn then).
            long GetTargetGoalIdx() const;
            // DOL members used by BossF's CCameraBoss (return types and
            // parameter names guessed).
            long GetTeamNo() const;
            CChrCore *GetHoldBallChrCore(long) const;
            // The ball in play, or null.
            CBall *GetActiveBall() const;
            // DOL. (added for VolleyF CCameraVolleyBoss) The leader of the
            // camera's team, or null.
            CChrCore *GetMyTeamLeaderCore() const;
            // (guessed) Where fValue lies between fMin and fMax, from 0 to 1.
            float GetRate(float fValue, float fMin, float fMax) const;
            void MakeingFrontCamDataBase(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rTarget, float, float, float,
                                         float, float, float, float, float);
            // DOL members used by HockeyF's CCameraHockeyGame (return types and
            // parameter names guessed).
            int IsSingleChrCamera() const;
            int IsSpModeForBaHo() const;
            CChrCore *GetFirstPadChrCore() const;
            int IsComCore(const CChrCore *pCore) const;
            void GetHoldBallChrPos(bcs::FVEC3 &rPos, long) const;
            // The camera target of the slanted attack and defence views.
            void MakeingOffSlantCamTar(bcs::FVEC3 &rTarget, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rBase,
                                       float) const;
            void MakeingDefSlantCamTar(bcs::FVEC3 &rTarget, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rBase,
                                       float) const;
            // The floats at 0x108 to 0x110 as one vector (HockeyF passes their
            // address to the slant helpers above).
            // DOL members used by DodgeF's CCameraDodgeGame (return types and
            // parameter names guessed). The character doing an air trick, or
            // null; whether a character is on the camera's team, and whether
            // a pad (not the computer) drives it.
            CChrCore *GetAirTrickCoreAll() const;
            // DOL (added for BasketF CCameraBasketGame; meaning guessed from
            // the name): the character that did the last air trick, or null.
            CChrCore *GetLastAirTrickCoreAll() const;
            // (name guessed) The camera mode set by SetCamMode (m_Unk228) as
            // a signed number: DodgeF's CCameraDodgeGame compares it with
            // cmpwi where the member gives cmplwi.
            long GetCamMode() const { return m_Unk228; }
            int IsMyTeamCore(const CChrCore *pCore) const;
            int IsPadCore(const CChrCore *pCore) const;
            const bcs::FVEC3 &GetUnk108Vec() const { return *reinterpret_cast<const bcs::FVEC3 *>(&m_Unk108); }

        protected:
            // 0xFC: flags. With 0x80 set the camera stands still
            // (GetFrameRateF returns 0); 0x8 is set by the constructor when
            // the view's aspect type is 1. Meaning of the other bits not known.
            unsigned long m_UnkFC;
            // 0x100 to 0x110: floats set by the constructor (named by offset
            // until their meaning is known).
            float m_Unk100;
            float m_Unk104;
            float m_Unk108;
            float m_Unk10C;
            float m_Unk110;

        private:
            unsigned char m_Unk114[0x118 - 0x114];

        protected:
            bcs::FVEC3 m_Unk118; // 0x118

        private:
            unsigned char m_Unk124[0x1BC - 0x124];

        protected:
            // 0x1BC to 0x1C8 and 0x1D8: cleared by BossF's CCameraBoss
            // constructor (meanings not known).
            // 0x1BC and 0x1C8 are compared signed (VolleyF CCameraVolleyBoss:
            // a zoom state 0/1 and the ball's touch count).
            long m_Unk1BC;
            float m_Unk1C0; // 0x1C0
            float m_Unk1C4;
            long m_Unk1C8;

        protected:
            // 0x1CC to 0x1D4: DodgeF's CCameraDodgeGame zoom (MakeingCamZoom:
            // the zoom's three spline heights; meanings guessed).
            float m_Unk1CC;
            float m_Unk1D0;
            float m_Unk1D4;

        protected:
            unsigned long m_Unk1D8;

        private:
            unsigned char m_Unk1DC[0x224 - 0x1DC];

        protected:
            // 0x224: HockeyF's CCameraHockeyGame::IsNormalCamMode wants it
            // below 0.1 (meaning not known; a float).
            float m_Unk224;

        protected:
            // 0x228, 0x230, 0x234: read by VolleyF's CCameraVolleyBoss
            // MakeingCamZoom (meanings not known; 0x230/0x234 are written by
            // the DOL's GetBallTargetPos, 0x228 by SetCamMode).
            unsigned long m_Unk228;

        private:
            unsigned char m_Unk22C[0x230 - 0x22C];

        protected:
            unsigned long m_Unk230;
            bcs::FVEC3 m_Unk234;

        protected:
            // 0x240 to 0x24C: floats set by the constructor.
            float m_Unk240;
            float m_Unk244;
            float m_Unk248;
            float m_Unk24C;
            // 0x250: no DOL function of this class touches it, and the
            // DOL's own derived camera (allocated with 0x254 bytes) and
            // CCameraBasketHB use it as their own, so the class is probably
            // 0x250 bytes and this the first member of each derived class.
            // Kept here so the size stays 0x254 until that is checked with
            // a full build.
            // DodgeF's CCameraDodgeGame clears it as a word (stw), so it is
            // an integer there.
            union {
                float m_Unk250;
                long m_nUnk250;
            };
    };
}

#endif /* !CGAMECAMERA_HPP_ */
