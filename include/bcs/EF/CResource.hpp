#ifndef BCS_EF_CRESOURCE_HPP_
#define BCS_EF_CRESOURCE_HPP_

#include "bcs/CTask.hpp"
#include "nw4r/ut/List.hpp"
#include "nw4r/g3d/ResAnmScn.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "nw4r/ef/Resource.hpp"

namespace bcs {
    class CArcFileAccessor;

    namespace EF {

        class CEffect;

        // (name guessed) The base of the objects the effect manager keeps
        // in its six lists (the DOL's fn_8001BE44 builds a derived class on
        // it, likely bcs::EF::CEffect). Constructor
        // fn_80023DB0 and destructor fn_80023E74 in the DOL; the vtable is
        // lbl_80464E30. The vptr comes after the data (at 0x1C).
        class CEffectBase {
            private:
                // Declared before the virtuals: the vptr follows them.
                nw4r::ut::Link m_Link;      // 0x00
                unsigned long m_hHandle;    // 0x08 (name guessed)
                // (name guessed) The constructor stores the object itself;
                // CManager::GetEffect returns it.
                CEffectBase *m_pBody;       // 0x0C
                unsigned short m_nGroup;    // 0x10, the manager list it is in
                unsigned short m_Unk12;     // 0x12
                unsigned char m_Unk14;      // 0x14
                // (name guessed) Compared signed by CManager::CalcAll.
                long m_nFlag;               // 0x18

            public:
                virtual ~CEffectBase();
                // (names guessed) Empty in the base. Calc (slot 0x0C) runs
                // once a frame, CalcView (0x10) once per view, Disp (0x18)
                // from CManager::Disp; slot 0x14 is never called there.
                virtual void Calc();
                virtual void CalcView(long nView);
                virtual void Unk14();
                virtual void Disp();

                // (names guessed) Bits of m_nFlag: 0x4 asks the manager to
                // delete the object, 0x8 stops Calc, 0x10 stops CalcView.
                enum {
                    FLAG_DELETE = 0x4,
                    FLAG_STOP_CALC = 0x8,
                    FLAG_STOP_CALC_VIEW = 0x10
                };
                CEffectBase *GetBody() const { return m_pBody; }
                long GetFlag() const { return m_nFlag; }
        };

        // The effect system, a task (0x198 bytes, created by the DOL's
        // fn_800229A0). Only the members used so far.
        class CManager : public CTask {
            public:
                static CManager *GetInstance();
                // (the DOL's fn_80022A1C, name guessed) Asks the task
                // manager to delete the instance and forgets it.
                static void Destroy();

                virtual ~CManager();
                virtual void Main();
                virtual void Disp();

                // (the DOL's fn_80022B90, name guessed) Deletes the objects
                // marked for it and runs Calc on the others.
                void CalcAll();
                // (the DOL's fn_80022C60, name guessed) Sets the effect
                // system's camera to view nView and runs CalcView on the
                // objects of list nGroup.
                void CalcView(long nGroup, long nView);
                // Added for game/msw/cockpit/CMissionCount.cpp: the effect
                // with handle hEffect, null when it is gone.
                CEffect *GetEffect(unsigned long hEffect);
                // (name guessed; the DOL's fn_80023B2C, added for
                // msw::CEffectCluster::IsValid) Nonzero while the effect with
                // handle hEffect is in one of the manager's lists.
                int IsExist(unsigned long hEffect);

                // (name guessed) The byte at 0x95; msw::CTournamentMan
                // (TourF) clears it once the map is loaded.
                void SetUnk95(bool b) { m_bUnk95 = b; }

            private:
                // (name guessed) Set by Create; Destroy clears it.
                static CManager *s_pInstance;

                // The six lists of CEffectBase objects (one per group).
                nw4r::ut::List m_aList[6];  // 0x34
                unsigned char m_Unk7C[0x95 - 0x7C];
                bool m_bUnk95; // 0x95
                unsigned char m_Unk96[0x198 - 0x96];
        };

        // The effect archives known to the effect system. Lives in the DOL
        // (game/bcs/EF/CResource.cpp). 0x38 bytes (CreateInstance), vptr at
        // 0x34 after the data; the vtable is lbl_80464E68.
        class CResource {
            private:
                // Declared before the virtuals: the vptr follows them.
                static CResource *s_pInstance;

                nw4r::ut::List m_DataList;  // 0x00, DataNode
                nw4r::ut::List m_MdlList;   // 0x0C, MdlNode
                nw4r::ut::List m_ScnList;   // 0x18, ScnNode
                unsigned long m_nEffectNum; // 0x24
                unsigned long m_nTextureNum; // 0x28
                unsigned long m_nMdlNum;    // 0x2C
                unsigned long m_nArcNum;    // 0x30
                // 0x34: vptr

            public:
                // What a loaded file was registered as (DataNode::m_nType).
                // TYPE_ANY only selects in GetDataName.
                enum {
                    TYPE_EFFECT = 0,  // an effect project (.breff)
                    TYPE_TEXTURE = 1, // an effect texture project (.breft)
                    TYPE_G3D = 2,     // a g3d file (.brres)
                    TYPE_ARC = 3,     // an archive (.arc)
                    TYPE_ANY = 4
                };

                // One loaded file (0x98 bytes, no constructor: allocated
                // without a null check). Names guessed.
                struct DataNode {
                    nw4r::ut::Link m_Link;  // 0x00
                    unsigned char *m_pData; // 0x08
                    char m_szName[0x80];    // 0x0C, the path it was loaded from
                    unsigned long m_nSize;  // 0x8C, as read (packed size for a .bin)
                    void *m_pRes;           // 0x90, EffectProject or TextureProject
                    unsigned short m_nType; // 0x94, TYPE_*
                    // 0x96: bit 0 set when the data belongs to the caller
                    // (GetResourceAfterLoad) and is not freed here.
                    unsigned short m_nFlag;
                };

                // One model of a TYPE_G3D file (0x18 bytes). The two names
                // share one allocation, the model's name first.
                struct MdlNode {
                    MdlNode() {}

                    nw4r::ut::Link m_Link;  // 0x00
                    unsigned char *m_pData; // 0x08, the g3d file
                    unsigned long m_Unk0C;  // 0x0C
                    char *m_pMdlName;       // 0x10
                    char *m_pFileName;      // 0x14, the path of the file
                };

                // A scene animation and its light (0x10 bytes).
                struct ScnNode {
                    nw4r::ut::Link m_Link;            // 0x00
                    nw4r::g3d::ResAnmScn m_AnmScn;    // 0x08
                    nw4r::g3d::ResAnmLight m_AnmLight; // 0x0C
                };

                CResource();
                virtual ~CResource();

                // (the DOL's fn_80020694, name guessed) Makes the instance on
                // the first call.
                static CResource *CreateInstance();
                static CResource *GetInstance();
                // (the DOL's fn_80020728, name guessed)
                static void DestroyInstance();

                // (the DOL's fn_80020770, name guessed) Forgets every file and
                // model and removes all projects from the effect library.
                void Clear();
                // (the DOL's fn_80020884, fn_80020928 and fn_800209D0, names
                // guessed) Load `pPath` and register it as an effect project,
                // a texture project or a g3d file. All return the file's
                // index or a negative error (see Load).
                long AddEffect(const char *pPath);
                long AddTexture(const char *pPath);
                long AddG3DResFile(const char *pPath);
                // Load `pPath` as an archive; GetArcAccesor opens it.
                long AddArchive(const char *pPath);
                // (the DOL's fn_80020C10, name guessed) The number of files.
                int GetDataNum();
                // (the DOL's fn_80020C5C, name guessed) The path of file
                // nIndex if it is of nType (or nType is TYPE_ANY), else null.
                const char *GetDataName(long nIndex, long nType);
                void RelocateCommand();
                void InvalidateCache();
                // (the DOL's fn_80020D14, name guessed) The effect project
                // named `pName`, or null.
                nw4r::ef::EffectProject *FindEffectProject(const char *pName);
                // The g3d file `pName` of the loaded archives (a null handle
                // when there is none).
                nw4r::g3d::ResFile GetG3DResFile(const char *pName);
                // The g3d file holding the model `pName` (a null handle when
                // there is none).
                nw4r::g3d::ResFile GetG3DResFileByMdlName(const char *pName);
                // An accessor for the archive `pPath`, root directory pRoot;
                // null when `pPath` is not a loaded archive.
                CArcFileAccessor *GetArcAccesor(const char *pPath, const char *pRoot);
                void ReleaseArcAccesor(CArcFileAccessor *pAccessor);
                // Registers data the caller already has under `pPath` (not
                // freed here). 1 if `pPath` is already registered, -1 without
                // data, else 0.
                int GetResourceAfterLoad(const char *pPath, void *pData, unsigned long size);

            private:
                // (the DOL's fn_80021070, name guessed) Reads `pPath` (a .bin
                // is LH-unpacked) unless it is loaded already. The file's
                // index; -1 for no path, -4 if it cannot be opened, -5 if it
                // cannot be read.
                long Load(const char *pPath);

                inline DataNode *FindDataNode(const char *pPath);
                inline DataNode *FindEffectNode(const char *pName);
        };
    }
}

#endif /* !BCS_EF_CRESOURCE_HPP_ */
