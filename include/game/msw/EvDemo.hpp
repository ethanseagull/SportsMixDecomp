#ifndef EVDEMO_HPP_
#define EVDEMO_HPP_

namespace bcs {
    class CTask;
}

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace msw {

    // What a sport passes to EvDemo::CreateVictoryDemo: four lists of
    // halfwords. What each list holds is not known (structure and member
    // name guessed from HockeyF's CEventDemoHockey::CreateDemo).
    struct VICTORY_DATA {
        const short *pData[4];
    };

    // The demos in the DOL that every sport starts. Whether "EvDemo" is a
    // namespace or a class with static members cannot be told from the
    // symbols; a namespace is a guess, as are the return types.
    namespace EvDemo {
        void CreateEntranceDemo(bcs::CTask *pParent, const short *pData);
        void CreateVictoryDemo(bcs::CTask *pParent, const VICTORY_DATA *pData);
        void CreateReadyStartDemo(bcs::CTask *pParent, const short *pData);
        void CreateChangeCourtDemo(bcs::CTask *pParent, const short *pData);
        // (DOL; return type guessed) The player the ready-start demo
        // shows (VolleyF CReadyStartDemoVolley).
        long GetTargetReadyStart();

        // Looks up the model resource `pName` in archive `nArc` (a
        // msw::CLoadData entry; a null pSub picks a default sub-folder) and
        // returns it (an invalid file when not found; the DOL function
        // returns the handle in r3 and makes NW4R's alignment check, and
        // TourF callers copy-construct from it). nIndex receives the archive entry's
        // number (-1 when not found). Read from the DOL function; parameter
        // names guessed.
        nw4r::g3d::ResFile GetArcResDataMdl(long &nIndex, long nArc, const char *pName, const char *pSub);
        // Sets up the model resources of a loaded archive (pArc may be
        // null; a null pSub picks the default sub-folder). Return type and
        // parameter names guessed.
        void InitArcResData(void *pArc, const char *pSub);
    }
}

#endif /* !EVDEMO_HPP_ */
