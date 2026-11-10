#ifndef EVDEMO_HPP_
#define EVDEMO_HPP_

namespace bcs {
    class CTask;
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

        // Looks up the model resource `pName` in archive `nArc` (a
        // msw::CLoadData entry; a null pSub picks a default sub-folder) and
        // returns its data, or 0. nIndex receives the archive entry's
        // number (-1 when not found). Read from the DOL function; parameter
        // names guessed.
        void *GetArcResDataMdl(long &nIndex, long nArc, const char *pName, const char *pSub);
    }
}

#endif /* !EVDEMO_HPP_ */
