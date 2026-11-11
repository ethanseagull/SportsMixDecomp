#ifndef MSW_CCHRCOREBASKET_HPP_
#define MSW_CCHRCOREBASKET_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrCore.hpp"

namespace msw {

    namespace CHR_CORE_BASKET {
        // The second parameter block of a basketball character core
        // (GetCcParam2_; __ct__Q33msw15CHR_CORE_BASKET16CC_PARAM_BASKET2Fv
        // in BasketF).
        //
        // INCOMPLETE: only what the character units read; names guessed.
        class CC_PARAM_BASKET2 {
            public:
                unsigned char m_Unk000[0x1E8];
                // 0x1E8: the frames Yoshi's smash cut holds its second step
                // (CChrCoreBasketYoshi::CtorStateSmashCut).
                long m_nSmashCutHoldFrame;
                unsigned char m_Unk1EC[0x210 - 0x1EC];
                // 0x210: the height of Toad's trick jump
                // (CChrCoreBasketKinopio::SubStateArtsShotSetTrickJumpParam_).
                float m_fArtsJumpHeight;
                unsigned char m_Unk214[0x240 - 0x214];
                // 0x240 (name guessed): Waluigi's special shot gives each of
                // its balls this value / 60 (CBallBasket::SetArtsShotUnk).
                float m_fArtsBallUnk240;
        };
    }

    class CBall;

    // The basketball character core (BasketF).
    //
    // INCOMPLETE: the virtual functions are all declared (the 852 slots of
    // __vt__Q23msw14CChrCoreBasket, read from BasketF, as CChrCoreHockey.hpp
    // does), but few members are mapped and CChrCore is incomplete too (see
    // there). The size, 0x5E8, is where CChrCoreBasketLuigi's own members
    // start (CChrManBasket allocates it with 0x5F4 bytes). Each destructor
    // is declared first and without a body, so that it decides where the
    // vtable goes: the character units (game/basket/CChrCoreBasket<Name>.cpp)
    // define it and so emit the vtable.
    class CChrCoreBasket : public CChrCore {
        public:
            // The work of the air trick (the "arts shot"), passed to the
            // SubStateArtsShot* virtuals (contents mapped only where used).
            struct STATE_WORK_AIR_TRICK;
            // The work of the smash cut (state 0x32; contents mapped only
            // where used).
            struct STATE_WORK_SMASH_CUT;
            // (values not known) Which part of the air trick a motion is
            // asked for (GetAirTrickMotID).
            enum AIR_TIRCK_FLOW {};

            CChrCoreBasket();
            virtual ~CChrCoreBasket();

            // Not zero when pChr (holding the ball) is in reach of a cut
            // (steal) by this character. (meaning guessed from the name)
            int CutHitCheck(const CChrCore *pChr) const;

            // Added for game/basket/CChrDrvPadBasket.cpp (BasketF, all in
            // this module; return types guessed). Not zero when the
            // character takes a pass from a partner automatically.
            int IsAutoChrChangeToPassCatch() const;
            // Starts the combination pass in direction fRotY; zero when it
            // cannot.
            int MethdCombinationPass(float fRotY);

            // Added for game/basket/CChrCoreBasketKinopio.cpp (BasketF;
            // meanings guessed from the names). The down frames and the
            // vanish power of attack kind nKind, and the motion of the air
            // trick for flow (bAlt: bit 2 of the status flags).
            float GetAtkDownFrame(long nKind) const;
            float GetAtkVanishPower(long nKind) const;
            long GetAirTrickMotID(AIR_TIRCK_FLOW flow, int bAlt);

            // (added for game/basket/CChrCoreBasketKuroma.cpp; values not
            // known) Kinds of effect for Util_SetEffect_; the Black Mage's
            // smash cut sets kind 0 when he lands.
            enum SET_EFFECT_TYPE_BASKET {};
            void Util_SetEffect_(SET_EFFECT_TYPE_BASKET type);

            // (added for game/basket/CChrCoreBasketWaluigi.cpp) Sets
            // pWork->m_fShotFrame from the current motion's shot trigger.
            void SubStateArtsShotDoGetBallShotTrg_(STATE_WORK_AIR_TRICK *pWork);

            // Added for game/basket/CChrCoreBasketSaboten.cpp (BasketF;
            // return types and meanings guessed from the names and calls).
            // The effect slots of the special move (values not known).
            enum ARTS_EFF_SLOT {};
            void ArtsEff_CreateAttachAuto(ARTS_EFF_SLOT slot, long nEff);
            void ArtsEff_CreateAttach(ARTS_EFF_SLOT slot, long nEff, unsigned long nNode, unsigned long);
            void ArtsEff_Stop(ARTS_EFF_SLOT slot);
            void ArtsEff_StopAll();
            void ArtsEff_PlayAttackSign();
            void PlaySeSPStart();
            // The radius of the three-point line.
            float GetCourt3PointSize() const;
            // The goal the team attacks, moved along Z, and its ring.
            bcs::FVEC3 GetTargetGoalPosOffZ_() const;
            bcs::FVEC3 GetTargetGoalRing_() const;
            // Steps of the air trick shared by the characters.
            void SubStateAirTrickCmnCtor_();
            void SubStateAirTrickMove_(STATE_WORK_AIR_TRICK *pWork, float, float);
            void SubStateAirTrickCmnDunk_(STATE_WORK_AIR_TRICK *pWork);
            void SubBSplineMove_(float fFrame, float fFrameNum, float &rRate, float fLen, float fRotY, float,
                                 const bcs::FVEC3 &rOfs);
            // Blocks pChr's air trick dunk when it runs into this one.
            void CheckAirTrickDunkBlock_(CChrCore *pChr);
            // Added for game/basket/CChrCoreBasketMoogle.cpp (names from the
            // module's imports; parameter meanings guessed). The body
            // collision's push settings, and the shot of the ball with sound
            // slot nSlot.
            void SetCollBodyPushType_(CChrCore::BODY_COLL_SIZE_SET_TYPE type, float);
            void Util_ShootBall(long nSlot, float, long, long);
            // (added for game/basket/CChrCoreBasketKoopa.cpp; meaning
            // guessed from the name) Turns on the attack collision of the
            // lay-up.
            void SetCCLayUpAtkCol();

            // Overrides of CChrCore's virtuals, in vtable order (the slots of
            // __vt__Q23msw14CChrCoreBasket, read from BasketF, that name
            // CChrCoreBasket; return types as guessed in CChrCore).
            virtual long GetQueueWorkSize() const;                     // 0x10
            virtual void SetConditionEffect_(long);                    // 0x24
            virtual int IsNowBlockWork() const;                        // 0x30
            virtual int Is1on1() const;                                // 0x44
            virtual void GetTargetMarkerPos(bcs::FVEC3 &);             // 0x58
            virtual int MoveTargetMarkerPos(float);                    // 0x5C
            virtual int IsBattle() const;                              // 0x68
            virtual int IsDisp1On1Maker() const;                       // 0x6C
            virtual int IsDunkBlockEnable() const;                     // 0x70
            virtual int CanbePassRequest();                            // 0x74
            virtual int MethdPassRequest();                            // 0x78
            virtual float GetBlockWorkDir() const;                     // 0x7C
            virtual float GetJumpBlockLocalFrame() const;              // 0x80
            virtual int GetStarRemainderRate() const;                  // 0x84
            virtual void GetVel(bcs::FVEC3 &) const;                   // 0x294
            virtual float GetCollSize_BodyPush() const;                // 0x340
            virtual int Util_GetIdleMotID();                           // 0x348
            virtual int Util_GetRunMotID();                            // 0x34C
            virtual int Util_GetLandMotID(long);                       // 0x354
            virtual int Util_GetJumpUpMotID(int);                      // 0x358
            virtual void Util_GetJumpLandMotID(int);                   // 0x35C
            virtual int Util_GetDamageMotID(long, long, long);         // 0x360
            virtual void Util_SetOffenseLeaderFirstState(long);        // 0x364
            virtual void Util_SetDefenseLeaderFirstState();            // 0x368
            virtual void CreateColl_();                                // 0x374
            virtual void Create_();                                    // 0x378
            virtual void MainFunc_();                                  // 0x37C
            virtual void UpdateFunc_();                                // 0x380
            virtual void ResetCmn_();                                  // 0x388
            virtual float GetPrmMoveSpeed() const;                     // 0x38C
            virtual long GetPrmBodySize() const;                       // 0x390
            virtual void SendMessage_(long, long, long, long);         // 0x394
            virtual void CtorStateNeutral();                           // 0x3BC
            virtual void LoopStateNeutral();                           // 0x3C0
            virtual void DtorStateNeutral();                           // 0x3C4
            virtual void CtorStateRun();                               // 0x3E0
            virtual void LoopStateRun();                               // 0x3E4
            virtual void DtorStateRun();                               // 0x3E8
            virtual void CtorStateJump();                              // 0x3EC
            virtual void LoopStateJump();                              // 0x3F0
            virtual void DtorStateJump();                              // 0x3F4
            virtual void CtorStateDamage();                            // 0x3F8
            virtual void LoopStateDamage();                            // 0x3FC
            virtual void DtorStateDamage();                            // 0x400
            virtual void CtorStateSideStep();                          // 0x404
            virtual void LoopStateSideStep();                          // 0x408
            virtual void DtorStateSideStep();                          // 0x40C
            virtual void CtorStateItemShoot();                         // 0x410
            virtual void LoopStateItemShoot();                         // 0x414
            virtual void DtorStateItemShoot();                         // 0x418
            virtual void CtorStateBlockCut();                          // 0x41C
            virtual void LoopStateBlockCut();                          // 0x420
            virtual void DtorStateBlockCut();                          // 0x424
            virtual void CtorStateJumpUpper();                         // 0x434
            virtual void LoopStateJumpUpper();                         // 0x438
            virtual void DtorStateJumpUpper();                         // 0x43C
            virtual void CtorStateMotPlay();                           // 0x44C
            virtual void LoopStateMotPlay();                           // 0x450
            virtual void DtorStateMotPlay();                           // 0x454
            virtual void CtorStateSizeChange();                        // 0x458
            virtual void LoopStateSizeChange();                        // 0x45C
            virtual void DtorStateSizeChange();                        // 0x460
            virtual void CtorStateDizzy();                             // 0x470
            virtual void LoopStateDizzy();                             // 0x474
            virtual void DtorStateDizzy();                             // 0x478
            virtual void CtorStateSliding();                           // 0x47C
            virtual void LoopStateSliding();                           // 0x480
            virtual void DtorStateSliding();                           // 0x484
            virtual void CtorStatePass();                              // 0x4AC
            virtual void LoopStatePass();                              // 0x4B0
            virtual void DtorStatePass();                              // 0x4B4
            virtual void CtorStateAerialPass();                        // 0x4B8
            virtual void LoopStateAerialPass();                        // 0x4BC
            virtual void DtorStateAerialPass();                        // 0x4C0
            virtual void CtorStatePassCatch();                         // 0x4C4
            virtual void LoopStatePassCatch();                         // 0x4C8
            virtual void DtorStatePassCatch();                         // 0x4CC
            virtual void CtorStateSDB();                               // 0x4DC
            virtual void LoopStateSDB();                               // 0x4E0
            virtual void DtorStateSDB();                               // 0x4E4
            virtual void CtorStateSDB_Chain();                         // 0x4E8
            virtual void LoopStateSDB_Chain();                         // 0x4EC
            virtual void DtorStateSDB_Chain();                         // 0x4F0
            virtual void CtorStateRDB();                               // 0x4F4
            virtual void LoopStateRDB();                               // 0x4F8
            virtual void DtorStateRDB();                               // 0x4FC
            virtual void CtorStateTrickMove();                         // 0x518
            virtual void LoopStateTrickMove();                         // 0x51C
            virtual void DtorStateTrickMove();                         // 0x520
            virtual void CtorStateCatch();                             // 0x530
            virtual void LoopStateCatch();                             // 0x534
            virtual void DtorStateCatch();                             // 0x538
            virtual void CtorStateSDB_Catch();                         // 0x53C
            virtual void LoopStateSDB_Catch();                         // 0x540
            virtual void DtorStateSDB_Catch();                         // 0x544
            virtual void CtorStateJumpShoot();                         // 0x548
            virtual void LoopStateJumpShoot();                         // 0x54C
            virtual void DtorStateJumpShoot();                         // 0x550
            virtual void CtorStateReboundDunk();                       // 0x560
            virtual void LoopStateReboundDunk();                       // 0x564
            virtual void DtorStateReboundDunk();                       // 0x568
            virtual void CtorStateLayUp();                             // 0x56C
            virtual void LoopStateLayUp();                             // 0x570
            virtual void DtorStateLayUp();                             // 0x574
            virtual void CtorStateAlleyoop();                          // 0x578
            virtual void LoopStateAlleyoop();                          // 0x57C
            virtual void DtorStateAlleyoop();                          // 0x580
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CtorStateDunk();                              // 0x590
            virtual void LoopStateDunk();                              // 0x594
            virtual void DtorStateDunk();                              // 0x598
            virtual void CtorStateBlockWorkLose();                     // 0x5D8
            virtual void LoopStateBlockWorkLose();                     // 0x5DC
            virtual void DtorStateBlockWorkLose();                     // 0x5E0
            virtual void CtorStateAutoDribblePosture();                // 0x5E4
            virtual void LoopStateAutoDribblePosture();                // 0x5E8
            virtual void DtorStateAutoDribblePosture();                // 0x5EC
            virtual void CtorStateJumpBlock();                         // 0x5F0
            virtual void LoopStateJumpBlock();                         // 0x5F4
            virtual void DtorStateJumpBlock();                         // 0x5F8
            virtual void CtorStateSwatIt();                            // 0x5FC
            virtual void LoopStateSwatIt();                            // 0x600
            virtual void DtorStateSwatIt();                            // 0x604
            virtual void CtorStateSmashCut();                          // 0x608
            virtual void LoopStateSmashCut();                          // 0x60C
            virtual void DtorStateSmashCut();                          // 0x610
            virtual void CtorStateDash();                              // 0x620
            virtual void LoopStateDash();                              // 0x624
            virtual void DtorStateDash();                              // 0x628
            virtual void CtorStateJumpPass();                          // 0x62C
            virtual void LoopStateJumpPass();                          // 0x630
            virtual void DtorStateJumpPass();                          // 0x634
            virtual void CtorStateJumpCatch();                         // 0x638
            virtual void LoopStateJumpCatch();                         // 0x63C
            virtual void DtorStateJumpCatch();                         // 0x640
            virtual void CtorStateDribbleMiss();                       // 0x644
            virtual void LoopStateDribbleMiss();                       // 0x648
            virtual void DtorStateDribbleMiss();                       // 0x64C
            virtual void CtorStateTurn();                              // 0x680
            virtual void LoopStateTurn();                              // 0x684
            virtual void DtorStateTurn();                              // 0x688
            virtual void CtorStateFall();                              // 0x7D0
            virtual void LoopStateFall();                              // 0x7D4
            virtual void DtorStateFall();                              // 0x7D8
            virtual void CtorStateRebirth();                           // 0x7DC
            virtual void LoopStateRebirth();                           // 0x7E0
            virtual void DtorStateRebirth();                           // 0x7E4
            virtual int CanbeRun();                                    // 0x7E8
            virtual int MethdRun(float);                               // 0x7EC
            virtual int CanbeJump();                                   // 0x800
            virtual int MethdJump();                                   // 0x804
            virtual int CanbeSliding();                                // 0x808
            virtual int MethdSliding(float);                           // 0x80C
            virtual int CanbeSizeChange(long);                         // 0x810
            virtual int MethdSizeChange(long, float);                  // 0x814
            virtual int MethdChgStarState();                           // 0x818
            virtual int CanbeChgStarState();                           // 0x81C
            virtual int CanbeJumpUpper() const;                        // 0x834
            virtual int CanbeNowJumpUpper() const;                     // 0x838
            virtual int MethdJumpUpper();                              // 0x83C
            virtual int CanbeSideStep(float, float);                   // 0x840
            virtual int MethdSideStep(float, float);                   // 0x844
            virtual int CanbeCatch();                                  // 0x848
            virtual int MethdCatch();                                  // 0x84C
            virtual int CanbeCatchToSDB();                             // 0x850
            virtual int MethdCatchToSDB();                             // 0x854
            virtual int CanbeDribble();                                // 0x860
            virtual int CanbeSDB();                                    // 0x864
            virtual int MethdSDB(float);                               // 0x868
            virtual int CanbeRDB();                                    // 0x86C
            virtual int MethdRDB(INPUT_WORK *, float);                 // 0x870
            virtual int MethdRDB(INPUT_WORK *, float, float);          // 0x874
            virtual int CanbeTrickMove();                              // 0x884
            virtual int MethdTrickMove(float);                         // 0x888
            virtual int CanbeBlockCut(int);                            // 0x894
            virtual int CanbeNowBlockCut();                            // 0x898
            virtual int MethdBlockCut(float);                          // 0x89C
            virtual int CanbeJumpShoot();                              // 0x8BC
            virtual int MethdJumpShoot();                              // 0x8C0
            virtual int MethdJumpShoot(float, int);                    // 0x8C4
            virtual int CanbeRebound();                                // 0x8D0
            virtual int CanbeReboundDunk();                            // 0x8D4
            virtual int MethdReboundDunk();                            // 0x8D8
            virtual int CanbeLayUp();                                  // 0x8DC
            virtual int MethdLayUp();                                  // 0x8E0
            virtual int MethdLayUp(int);                               // 0x8E4
            virtual int CanbeAirTrick();                               // 0x8E8
            virtual int MethdAirTrick();                               // 0x8EC
            virtual int CanbeDunk();                                   // 0x8F8
            virtual int MethdDunk();                                   // 0x8FC
            virtual int CanbePass(CChrCore *);                         // 0x908
            virtual int MethdPass(float, int);                         // 0x90C
            virtual int MethdPass(CChrCore *, int);                    // 0x910
            virtual int CanbePassIdx(long);                            // 0x914
            virtual int MethdPassIdx(long);                            // 0x918
            virtual int CanbeDash() const;                             // 0x91C
            virtual int MethdDash();                                   // 0x920
            virtual int CanbeJumpPass();                               // 0x924
            virtual int MethdJumpPass(long);                           // 0x928
            virtual int CanbeJumpCatch();                              // 0x92C
            virtual int MethdJumpCatch(float);                         // 0x930
            virtual int CanbeDribbleMiss();                            // 0x934
            virtual int MethdDribbleMiss(float);                       // 0x938
            virtual int CanbeAerialPass(CChrCore *);                   // 0x93C
            virtual int MethdAerialPass(float, int);                   // 0x940
            virtual int CanbeAerialPassIdx(long);                      // 0x944
            virtual int MethdAerialPassIdx(long);                      // 0x948
            virtual int CanbeItemShoot();                              // 0x94C
            virtual int MethdItemShoot(float);                         // 0x950
            virtual int CanbeSetMoveRevise();                          // 0x954
            virtual int MethdSetMoveRevise(float);                     // 0x958
            virtual int MethdDamage(unsigned long, float);             // 0x95C
            virtual int MethdDamage(unsigned long, float, float, float, long, float, long, long); // 0x960
            virtual int CanbeUseItem();                                // 0x968
            virtual int MethdUseItem();                                // 0x96C
            virtual int MethdTurnOver(long);                           // 0x970
            virtual int MethdTimeOver(long);                           // 0x974
            virtual int MethdGameSet();                                // 0x978
            virtual int MethdGameResult(int);                          // 0x97C
            virtual int CanbeChgLeaderState();                         // 0x980
            virtual int CanbeChgNewLeaderState();                      // 0x984
            virtual int CanbeChgLeader(CChrCore *);                    // 0x988
            virtual int MethdChgLeader(float, long, long, int);        // 0x98C
            virtual int MethdChgLeader(long, long, long);              // 0x990
            virtual int MethdChgLeader();                              // 0x994
            virtual int Canbe1on1();                                   // 0x9BC
            virtual int Methd1on1(int);                                // 0x9C0
            virtual int CanbeAutoDPM();                                // 0x9C4
            virtual int MethdAutoDPM(float, float);                    // 0x9C8
            virtual int CanbeBlockWorkLose();                          // 0x9CC
            virtual int MethdBlockWorkLose();                          // 0x9D0
            virtual int CanbeJumpBlock();                              // 0x9D4
            virtual int MethdJumpBlock(float);                         // 0x9D8
            virtual int CanbeSwatIt() const;                           // 0x9DC
            virtual int MethdSwatIt();                                 // 0x9E0
            virtual int CanbeSmashCut();                               // 0x9E4
            virtual int MethdSmashCut(float);                          // 0x9E8
            virtual int MethdAirTrickDownSpeed(float);                 // 0x9EC
            virtual int CanbeSpecialBattle();                          // 0x9F0
            virtual int MethdSpecialBattle();                          // 0x9F4
            virtual int CanbePassCatchState();                         // 0xA6C
            virtual int CanbeAlleyoop();                               // 0xA70
            virtual int MethdSetPassCatchState(CBall *, int, const bcs::FVEC3 &, long); // 0xA74
            virtual int MethdSetPassCatchJumpWaitFrame(const bcs::FVEC3 &, float); // 0xA78
            virtual int MethdAlleyoop();                               // 0xA7C
            virtual float GetArtsEventRot();                           // 0xA88
            virtual void DoPlayArtsEvent();                            // 0xA8C
            virtual int CanbeCombination() const;                      // 0xB5C
            virtual int MethdCombination();                            // 0xB60
            virtual int CanbeLanding() const;                          // 0xB64
            virtual int CanbeFall() const;                             // 0xB68
            virtual int MethdFall();                                   // 0xB6C
            virtual int CanbeRebirth() const;                          // 0xB70
            virtual int MethdRebirth();                                // 0xB74
            virtual void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *); // 0xC44
            virtual void CallbackBodyDamage_(const CCollAtkUnit *, const bcs::FVEC3 &); // 0xC4C
            virtual int CallbackBallDamage_(CBall *, const CCollAtkUnit *, const bcs::FVEC3 &); // 0xC50
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *); // 0xC54
            virtual void CallbackBallCatch_(CBall *);                  // 0xC58
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &, unsigned long); // 0xC5C
            virtual void CallbackBallSnatched_(CBall *);               // 0xC60
            virtual int CallbackDefReflectHit_(const CCollDefUnit *, const CBall *, const bcs::FVEC3 &); // 0xC6C
            virtual void CallbackMotTrg_SeFoot_(long, long);           // 0xC70
            virtual void SubStateParalyzeSetEffect_();                 // 0xC94
            virtual void SubStateParalyzeEndEffect_();                 // 0xC98

            // CChrCoreBasket's own virtuals, 0xC9C to 0xD4C (return types
            // guessed: "int" for a word, "float" for f1, "void *" for a
            // pointer to a type not mapped yet).
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual const CHR_CORE_BASKET::CC_PARAM_BASKET2 *GetCcParam2_() const; // 0xCA4
            // (return type from CChrCoreBasketSaboten's: 1 when it wrote a
            // position)
            virtual int GetSpSetBackPos(bcs::FVEC3 &) const;           // 0xCA8
            virtual void UpDateCollBody3_();                           // 0xCAC
            virtual void UpdateCollAtk_();                             // 0xCB0
            virtual void SubStateArtsShotStartTrick_(STATE_WORK_AIR_TRICK *); // 0xCB4
            virtual void SubStateArtsShotSetTrickFrameParam_(STATE_WORK_AIR_TRICK *, long); // 0xCB8
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *, long); // 0xCBC
            virtual void SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *, long); // 0xCC0
            virtual float SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *, long); // 0xCC4
            virtual float SubStateArtsShotGetTrickDoFrame_(STATE_WORK_AIR_TRICK *, long); // 0xCC8
            virtual float SubStateArtsShotGetTrickRotFrame_(STATE_WORK_AIR_TRICK *); // 0xCCC
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCD0
            virtual int SubStateArtsShotIsStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCD4
            virtual void SubStateArtsShotCalcRotParam_(CChrCore::ROT_LERP1 &, float, float); // 0xCD8
            virtual void SubStateArtsShotClearMoveParamWork_(CC_DATA::CHR_PARAM::MOVE_PARAM &); // 0xCDC
            virtual void SubStateArtsShotDoJumpMove_(STATE_WORK_AIR_TRICK *); // 0xCE0
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *, long); // 0xCE4
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCE8
            virtual void SubStateArtsShotReadyArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCEC
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCF0
            virtual void SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK *); // 0xCF4
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCF8
            virtual void SubStateArtsShotAskTargetMarkerMove_(STATE_WORK_AIR_TRICK *); // 0xCFC
            virtual int SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *); // 0xD00
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *); // 0xD04
            virtual void SubStateArtsShotStartArtsHitback_(STATE_WORK_AIR_TRICK *); // 0xD08
            virtual int SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *); // 0xD0C
            virtual void SubStateArtsShotDoLandStart_(STATE_WORK_AIR_TRICK *); // 0xD10
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD14
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD18
            virtual int CanbePassSign();                               // 0xD1C
            virtual int MethdPassSign();                               // 0xD20
            virtual void CallbackBodyPush3_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *); // 0xD24
            virtual void StartDunkBattle(CChrCore *);                  // 0xD28
            virtual void EndDunkBattle();                              // 0xD2C
            virtual int CheckDunkBattle();                             // 0xD30
            virtual float GetColBodyRadius() const;                    // 0xD34
            virtual float GetPush1Radius() const;                      // 0xD38
            virtual void ResultAirTrickBattle(int);                    // 0xD3C
            virtual int IsDunkBattle();                                // 0xD40
            virtual int GetArtsShootType() const;                      // 0xD44
            virtual void GetArtsFuturePos(float &, float &);           // 0xD48
            // Added for BasketF game/basket/CNpcGhostBasketBase.cpp (return
            // type guessed): where the character will be after fFrame frames.
            virtual void GetFuturePos(bcs::FVEC3 &rPos, float fFrame); // 0xD4C

            // 0x57C (name guessed): flag bits; with 0x800 set a catch can be
            // followed by an aerial pass (CChrDrvPadBasket::MethdPass).
            unsigned char m_Unk574[0x57C - 0x574];
            unsigned long m_nBasketFlag;
            // Not mapped yet.
            unsigned char m_Unk580[0x5E8 - 0x580];
    };

    // (field names guessed; only what the character units read is mapped)
    struct CChrCoreBasket::STATE_WORK_AIR_TRICK {
        // 0x00-0x04: three motions of the trick (GetAirTrickMotID of flows
        // 7, 8 and 15; the cactus stores them, SubStateAirTrickCmnDunk_
        // presumably plays them).
        short m_nMotId[3];
        unsigned char m_Unk06[0x8 - 0x6];
        unsigned long m_nFlag;         // 0x08, bit 9 set while Luigi's vacuum pulls
        // 0x0C-0x24, as the cactus's trick uses them (names guessed): the
        // frames of the move so far and in all, the motion frame of the last
        // update, a wait, the length and the place on the curve of the move
        // (SubBSplineMove_, SubStateAirTrickMove_).
        float m_fMoveFrame;            // 0x0C
        float m_fMoveFrameNum;         // 0x10
        unsigned char m_Unk14[0x18 - 0x14];
        float m_fMoveUnk18;            // 0x18, the speed of the move, or the last motion frame
        float m_fWaitFrame;            // 0x1C
        float m_fMoveLen;              // 0x20
        float m_fMoveRate;             // 0x24
        float m_fRotY;                 // 0x28, the yaw the trick turns to
        unsigned char m_Unk2C[0x30 - 0x2C];
        // 0x30: the cactus's speed per frame, or the point the move ends at.
        bcs::FVEC3 m_MoveVec;
        unsigned char m_Unk3C[0x40 - 0x3C];
        long m_nStep;                  // 0x40, the step of the trick (state numbers 0 to 0x12)
        unsigned char m_Unk44[0x48 - 0x44];
        float m_fJumpUnk;              // 0x48, passed on to CChrCore::m_fJumpUnk224
        bcs::FVEC3 m_TargetPos;        // 0x4C, where the trick moves to
        float m_fUnk58;                // 0x58, the frame of the move by m_MoveParam (the Moogle)
        CC_DATA::CHR_PARAM::MOVE_PARAM m_MoveParam; // 0x5C
        CChrCore::DIR_MOVE m_DirMove;  // 0x6C
        CChrCore::ROT_LERP1 m_RotLerp; // 0x7C, the turn (SubStateArtsShotCalcRotParam_)
        long m_nTrickStep;             // 0x8C, the step of the trick (Toad: 0 to 2)
        float m_fTrickFrame;           // 0x90, the frames of the current step
        // 0x94: the frames of the current step so far (the Black Mage).
        float m_fTrickCount;
        unsigned char m_Unk98[0x9C - 0x98];
        float m_fAnimSpeed;            // 0x9C, the speed of the trick motions (Waluigi)
        long m_nUnkA0;                 // 0xA0, (meaning not known) below 1 the shot motion stops twice
        // 0xA4: the frame of the shot motion the ball leaves at
        // (SubStateArtsShotDoGetBallShotTrg_ sets it).
        float m_fShotFrame;
        long m_nUnkA8;                 // 0xA8, Luigi sets 40
        unsigned char m_UnkAC[0x4];
        // 0xB0: the yaw the shot aims at; negative for straight at the goal
        // (Luigi's target marker sets it).
        float m_fTargetRotY;
        // 0xB4 (name guessed): the step after the shot (the Moogle: 0 to 2).
        long m_nAfterStep;
    };

    // (field names guessed; only what the character units read is mapped)
    struct CChrCoreBasket::STATE_WORK_SMASH_CUT {
        long m_nStep;                  // 0x00, the step of the cut (Yoshi: 0 to 4)
        unsigned char m_Unk04[0x10 - 0x4];
        // 0x10: not negative while Toad may put his mushroom down
        // (meaning guessed); Yoshi adds the frame step to it.
        float m_fUnk10;
        unsigned char m_Unk14[0x18 - 0x14];
        float m_fHoldFrame;            // 0x18, Yoshi: the frames his second step is held
        float m_fRotY;                 // 0x1C, Yoshi: the yaw he turns to
        float m_fAnimSpeed;            // 0x20, Yoshi: the speed of his motions
        unsigned char m_Unk24[0x2C - 0x24];
        float m_fRadius;               // 0x2C, the reach of the cut (Toad's mushroom takes it as its size)
    };

    // Luigi: his special shot pulls the other team in with his vacuum
    // (CC_CMN::LUIGI::CcOvacuumForce). Allocated with 0x5F4 bytes.
    class CChrCoreBasketLuigi : public CChrCoreBasket {
        public:
            CChrCoreBasketLuigi();
            virtual ~CChrCoreBasketLuigi();
            virtual int MoveTargetMarkerPos(float fRotY);              // 0x5C
            virtual void Reset_();                                     // 0x384
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CtorStateJumpBlock();                         // 0x5F0
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *); // 0xCE8
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *); // 0xCF0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *); // 0xCF8
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *); // 0xD14
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *); // 0xD18

        protected:
            // (inline helper, name guessed) Deletes the vacuum task, if any.
            void DeleteVacuum_();

            // (names guessed)
            unsigned long m_nUnk5E8;   // 0x5E8, cleared by the constructor only
            int m_bVacuum;             // 0x5EC, the vacuum pulls
            unsigned long m_hVacuum;   // 0x5F0, task id of the vacuum, 0 for none
    };

    // Toad: his smash cut puts a mushroom (CC09_BASKET::CcKinopioReceiver)
    // in front of the ball holder, and his special shot two bouncing
    // mushrooms in front of the goal.
    class CChrCoreBasketKinopio : public CChrCoreBasket {
        public:
            CChrCoreBasketKinopio();
            virtual ~CChrCoreBasketKinopio();
            virtual void CtorStateJumpBlock();                         // 0x5F0
            virtual void CtorStateSmashCut();                          // 0x608
            virtual void LoopStateSmashCut();                          // 0x60C
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual void UpdateCollAtk_();                             // 0xCB0
            virtual void SubStateArtsShotSetTrickJumpParam_(STATE_WORK_AIR_TRICK *pWork, long nStep); // 0xCBC
            virtual void SubStateArtsShotSetTrickMoveParam_(STATE_WORK_AIR_TRICK *pWork, long nStep); // 0xCC0
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xCD0
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *pWork, long); // 0xCE4
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCE8
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF8

        protected:
            unsigned long m_hReceiver; // 0x5E8 (name guessed), task id of the smash cut's mushroom, 0 for none
    };

    // Yoshi: his smash cut is a tongue lash in four motions; his jump block
    // shows an effect turned with him.
    class CChrCoreBasketYoshi : public CChrCoreBasket {
        public:
            // Two colours of the point light of Yoshi's effects, per colour
            // variant (names guessed).
            struct YOSHI_COL_SET {
                bcs::BCS_Color m_Color0;
                bcs::BCS_Color m_Color1;
            };

            CChrCoreBasketYoshi();
            virtual ~CChrCoreBasketYoshi();
            virtual void DtorStateJumpShoot();                         // 0x550
            virtual void DtorStateDunk();                              // 0x598
            virtual void CtorStateJumpBlock();                         // 0x5F0
            virtual void LoopStateJumpBlock();                         // 0x5F4
            virtual void DtorStateJumpBlock();                         // 0x5F8
            virtual void CtorStateSmashCut();                          // 0x608
            virtual void LoopStateSmashCut();                          // 0x60C
            virtual void DtorStateSmashCut();                          // 0x610
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCE8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF8
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork); // 0xD04

            // The colours of colour variant nColor (0 to 3; the caller
            // clamps it). CChrCoreBasket's jump block and smash cut set them
            // on Yoshi's effects.
            static const YOSHI_COL_SET *GetEffectUnit(long nColor);
    };

    // The cactus: his special shot is a dunk; on the way up he fires a
    // needle (CC13_BASKET::CcSabotenNeedleBasket), and anyone of the other
    // team in the way of the dunk is blocked.
    class CChrCoreBasketSaboten : public CChrCoreBasket {
        public:
            CChrCoreBasketSaboten();
            virtual ~CChrCoreBasketSaboten();
            virtual int IsAirTrickDunkMode() const;                    // 0x88
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void LoopStateAirTrick();                          // 0x588
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush); // 0xC54
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual int GetSpSetBackPos(bcs::FVEC3 &rPos) const;       // 0xCA8

        protected:
            // 0x5E8 (name guessed): where the cactus puts the others back
            // during his dunk (GetSpSetBackPos).
            bcs::FVEC3 m_SetBackPos;
    };

    // The Black Mage: his smash cut is a step back while a spell
    // (CC17_BASKET::CcKuromaSmash) flies to the ball holder of the other
    // team; his special shot puts a lock-on mark on every character of the
    // other team and strikes them all a while later.
    class CChrCoreBasketKuroma : public CChrCoreBasket {
        public:
            CChrCoreBasketKuroma();
            virtual ~CChrCoreBasketKuroma();
            virtual void Reset_();                                     // 0x384
            virtual void CtorStateJumpUpper();                         // 0x434
            virtual void DtorStateJumpUpper();                         // 0x43C
            virtual void CtorStateLayUp();                             // 0x56C
            virtual void DtorStateLayUp();                             // 0x574
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void CtorStateSmashCut();                          // 0x608
            virtual void LoopStateSmashCut();                          // 0x60C
            virtual void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *pTrg); // 0xC44
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xCD0
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCE8
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF8
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xD14
            virtual void SubStateArtsShotPlayLandEffect_(STATE_WORK_AIR_TRICK *pWork); // 0xD18

        protected:
            // (inline helper, name guessed) Deletes the strike task, if any.
            void ReleaseMagic_();

            // (names guessed)
            unsigned long m_hMagic;    // 0x5E8, task id of the strike (CcKuromaMagicMgr), 0 for none
            unsigned long m_hSmash;    // 0x5EC, task id of the smash cut's spell, 0 for none
            unsigned long m_hEffect;   // 0x5F0, the effect on the option model's item node
    };
    // Waluigi: his special shot throws three copies of the ball
    // (CBallManBasket::CreateDummyBall_) from the hands of his option model
    // before the shot with the ball itself. (size not known; at least 0x5F4)
    class CChrCoreBasketWaluigi : public CChrCoreBasket {
        public:
            // The three copies, and the four hand nodes (the last for the ball).
            enum { ARTS_BALL_NUM = 3, ARTS_BALL_NODE_NUM = ARTS_BALL_NUM + 1 };

            CChrCoreBasketWaluigi();
            virtual ~CChrCoreBasketWaluigi();
            virtual void CtorStateAirTrick();                          // 0x584
            virtual void DtorStateAirTrick();                          // 0x58C
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual float SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *pWork, long nStep); // 0xCC4
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xCD0
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCE8
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF0
            virtual void SubStateArtsShotStartArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xCF4
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF8
            virtual int SubStateArtsShotIsEndArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xD00
            virtual void SubStateArtsShotPlayLandMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xD14

        protected:
            // Makes the three copies and hangs them and the ball from the
            // option model's hands.
            void CreateArtsBall_();
            // Throws copy nIndex (0 to 2).
            void ShotArtsBall_(long nIndex);
            // (inline helper, name guessed) Removes the copies not thrown.
            void DeleteArtsBall_();

            // 0x5E8: task ids of the three copies, 0 once thrown or gone.
            unsigned long m_ahArtsBall[ARTS_BALL_NUM];
    };

    // The Moogle: his jump to the upper floor keeps a small body
    // collision, and his special shot has its own motions after the ball
    // has left. (size not known; no members of its own found)
    class CChrCoreBasketMoogle : public CChrCoreBasket {
        public:
            CChrCoreBasketMoogle();
            virtual ~CChrCoreBasketMoogle();
            virtual void CtorStateJumpUpper();                         // 0x434
            virtual void LoopStateJumpUpper();                         // 0x438
            virtual void DtorStateJumpUpper();                         // 0x43C
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xCD0
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCE8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF8
            virtual void SubStateArtsShotShootBall_(STATE_WORK_AIR_TRICK *pWork); // 0xD04
            virtual int SubStateArtsShotIsEndArtsShotAfter_(STATE_WORK_AIR_TRICK *pWork); // 0xD0C
    };

    // Bowser: his lay-up has an attack collision, and his special shot
    // breathes a fireball (CC_CMN::KOOPA::CcFireBall) from his mouth at the
    // goal. (size not known)
    class CChrCoreBasketKoopa : public CChrCoreBasket {
        public:
            CChrCoreBasketKoopa();
            virtual ~CChrCoreBasketKoopa();
            virtual void CallbackMotTrg_AtkOn_(long, long);            // 0xC80
            virtual void CallbackMotTrg_AtkOff_(long, long);           // 0xC84
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual float SubStateArtsShotGetTrickMotFrame_(STATE_WORK_AIR_TRICK *pWork, long nStep); // 0xCC4
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xCD0
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *pWork, long); // 0xCE4
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCE8
            virtual void SubStateArtsShotPlayLoopMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF0
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF8

        protected:
            unsigned char m_Unk5E8[0x5EC - 0x5E8];
            unsigned long m_nUnk5EC;   // 0x5EC (meaning not known), cleared by the constructor
    };

    // Peach: her special shot throws a fan of five heart bombs
    // (CC_CMN::PEACH::CcHeartBomb) at the goal. (size not known; no members
    // of its own found)
    class CChrCoreBasketPeach : public CChrCoreBasket {
        public:
            CChrCoreBasketPeach();
            virtual ~CChrCoreBasketPeach();
            virtual void *GetCcParam_() const;                         // 0xC9C
            virtual void *NewCcParam2_(long) const;                    // 0xCA0
            virtual int SubStateArtsShotIsReadyArtsShot_(STATE_WORK_AIR_TRICK *pWork); // 0xCD0
            virtual void SubStateArtsShotFireBullet_(STATE_WORK_AIR_TRICK *pWork, long); // 0xCE4
            virtual void SubStateArtsShotPlayTrickMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCE8
            virtual void SubStateArtsShotPlayArtsShotMotion_(STATE_WORK_AIR_TRICK *pWork); // 0xCF8
    };
}

#endif /* !MSW_CCHRCOREBASKET_HPP_ */
