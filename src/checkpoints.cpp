// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
    (     0, uint256("0x0000001ce91d2acda7b3d67fb5f0df3650ecff20faa107f349f25bd4e07d7add") )
    (     1, uint256("0x1edab580a94c28941b8741a19c02f48b0a8f3de04e57d5993a01f650c2fd7324") )
    (     2, uint256("0x2b501142dd663f02e15b40199fc3fc4a007b1851572ae6e911bfeca2532e6fb8") )
    (     3, uint256("0x0a8fed25366dac940f24c487a0534a484b52cdce8c85f862325df1dbb250c672") )
    (     4, uint256("0xc431f7ac70aae163a4e0605e98c65f89dee6d4cb190f7c42b686fa8653ccd11d") )
    (     5, uint256("0x46f7773ede99e040c8ed150a0a0f1d7012c5ba4b6762ea3223c4d6104d41c8a2") )
    (     6, uint256("0x7a8aa14c0033af9bfda495da9274fc7fe992d657e9ce2875bee38466b786c4dd") )
    (     7, uint256("0xb8968e645b2af67ee194e466c2da0769de9a85ac450983d402a22f9be89af331") )
    (     8, uint256("0x001583f14eedb949dbcd98fc383e12647bf90a0fb3489a5ecf9a21aba73a86ce") )
    (     9, uint256("0x958270aa2b6b8bff84886b9746aac14b5ea111177798077d2bb390f99077fad8") )
    (    10, uint256("0xb874688d8c6c4cc7354ea2dca58cd4b8bf383d20e0636ba14d528ece59c2fdf8") )
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
