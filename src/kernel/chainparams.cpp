// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 0 << CScriptNum(999) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Mount Fuji is the most beautiful mountain in Japan, altitude is 3776.24m";
    const CScript genesisOutputScript = CScript();
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
public:
    CMainParams() {
        m_chain_type = ChainType::MAIN;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000; //fujicoin do not use

        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256S("0xadb6d9cfd74075e7f91608add4bd2a2ea636f70856183086842667a1597714a0");
        consensus.BIP65Height = 1965600; // 9e3dfb12b8fce00cfdf96850b6f80f3635c2ecdfd26dbf32278c5046c06aaae8
        consensus.BIP66Height = 1965600; // 9e3dfb12b8fce00cfdf96850b6f80f3635c2ecdfd26dbf32278c5046c06aaae8
        consensus.CSVHeight = 1965600; // 9e3dfb12b8fce00cfdf96850b6f80f3635c2ecdfd26dbf32278c5046c06aaae8
        consensus.SegwitHeight = 1965600; // 9e3dfb12b8fce00cfdf96850b6f80f3635c2ecdfd26dbf32278c5046c06aaae8
        consensus.MinBIP9WarningHeight = 1965600 + 2016; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1.4 * 24 * 60 * 60; //1.4 days
        consensus.nPowTargetSpacing = 1.0 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256S("0x000000000000000000000000000000000000000000000000030e252a431bfc0e"); // 4600000
        consensus.defaultAssumeValid = uint256S("0x03ed5bdfb0e76a78452181fba99d0992a1d0be57d12f2904c776fe1c8b63a330"); // 4600000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x66;
        pchMessageStart[1] = 0x75;
        pchMessageStart[2] = 0x6a;
        pchMessageStart[3] = 0x69;
        nDefaultPort = 3777;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 6;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1403910000, 2560786, 0x1e0ffff0, 1, 1 * COIN); //(nTime, nNonce, nBits, nVersion, genesisReward)
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0xadb6d9cfd74075e7f91608add4bd2a2ea636f70856183086842667a1597714a0"));
        assert(genesis.hashMerkleRoot == uint256S("0xf951a273c3055d1bb36b4291e7f9edd491c2d435bd5737318ef8a643cab84b61"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as an addrfetch if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("seed1.fujicoin.org"); // Fujicoin official seed
        vSeeds.emplace_back("seed2.fujicoin.org"); // Fujicoin official seed

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,36);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,16);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,164);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "fc";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {      0, uint256S("0xadb6d9cfd74075e7f91608add4bd2a2ea636f70856183086842667a1597714a0")},
                { 200000, uint256S("0x588a542ff995394bc16731e3b53ea62f403fdfec4df5f8b4a38ad61a451523ec")},
                { 400000, uint256S("0x3c45c562ebccdf9ee7c0cb7509d9eebcbda9298636c9f4e3950fef2f6d29b8f5")},
                { 600000, uint256S("0xc7105925a54a06fb27b4574f3b5732965d7c5b975d71b91c12c1672b68045c38")},
                { 800000, uint256S("0xf3cb5bb24c7ae26f9ffe8001d594d4a951cc9a58589ac74519357add723f5602")},
                {1000000, uint256S("0xe267f45f1552c735a0f879403030c3f0904e8e9e0dc84cf2e6d13fcc32520dc3")},
                {1500000, uint256S("0xda3ce4ee4d6715aa5b9b6b94099b0d05cf28585473ab206c8b179c7a73ebbde8")},
                {2000000, uint256S("0xbead1047afe803c3e3b5eb246f5449e74beea3d629703c194a3fb5e78b1df6d9")},
                {2500000, uint256S("0xab1b07c414b4e73d14bdd4b9c607e702c6c7f654c02ce647fdb44f823a208274")},
                {3000000, uint256S("0x6cb15b85c8753757deb55cfceadbdb99cd483ca4f65003a88b18ab0966f50949")},
                {3500000, uint256S("0x88c4e1793c1a19c754191c6c900f7246948b90af5b47d3a3d89380d30af18b6e")},
                {4000000, uint256S("0x0e4addf4e6489e23723fa8cc1d4f45f4b017aed5729ccd87a5be34e2bf86a7a8")},
                {4500000, uint256S("0xe2a68073ecbbf8fa2250ff464f98661ffb4d860764b082ec2132b0c635481601")},
            }
        };

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 000000000000000000035c3f0d31e71a5ee24c5aaf3354689f65bd7b07dee632
            .nTime    = 1573445634,
            .nTxCount = 3399778,
            .dTxRate  = 0.02147296260938681,
        };
    }
};

/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        m_chain_type = ChainType::TESTNET;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 210000;

        consensus.BIP34Height = 1800000;
        consensus.BIP34Hash = uint256S("0x0f5353f137b5e3f0e45d5643843ba452ac84bb6470ba608eae71994f0be87412");
        consensus.BIP65Height = 1800000; // 0x0f5353f137b5e3f0e45d5643843ba452ac84bb6470ba608eae71994f0be87412
        consensus.BIP66Height = 1800000; // 0x0f5353f137b5e3f0e45d5643843ba452ac84bb6470ba608eae71994f0be87412
        consensus.CSVHeight = 1800000; // 0x0f5353f137b5e3f0e45d5643843ba452ac84bb6470ba608eae71994f0be87412
        consensus.SegwitHeight = 1800000; // 0x0f5353f137b5e3f0e45d5643843ba452ac84bb6470ba608eae71994f0be87412
        consensus.MinBIP9WarningHeight = 1800000 + 2016; // segwit activation height + miner confirmation window
        consensus.powLimit = uint256S("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1.4 * 24 * 60 * 60; 
        consensus.nPowTargetSpacing = 1.0 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000000000100010");
        consensus.defaultAssumeValid = uint256S("0x96bd214d68bcbfe9c786c4da26cf71fb6bbb6f24032065bdf2f4cd2b003d9c72"); // 0

        pchMessageStart[0] = 0x69;
        pchMessageStart[1] = 0x6a;
        pchMessageStart[2] = 0x75;
        pchMessageStart[3] = 0x66;
        nDefaultPort = 13777;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 6;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1402725600, 3578955, 0x1e0ffff0, 1, 1 * COIN); //(nTime, nNonce, nBits, nVersion, genesisReward)
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x96bd214d68bcbfe9c786c4da26cf71fb6bbb6f24032065bdf2f4cd2b003d9c72"));
        assert(genesis.hashMerkleRoot == uint256S("0xf951a273c3055d1bb36b4291e7f9edd491c2d435bd5737318ef8a643cab84b61"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        //vSeeds.emplace_back("testseed.fujicoin.org");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,74);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,202);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tf";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {0, uint256S("0x96bd214d68bcbfe9c786c4da26cf71fb6bbb6f24032065bdf2f4cd2b003d9c72")},
            }
        };

        m_assumeutxo_data = {
            // TODO to be specified in a future patch.
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096
            .nTime    = 0,
            .nTxCount = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
public:
    explicit SigNetParams(const SigNetOptions& options)
    {
        std::vector<uint8_t> bin;
        vSeeds.clear();

        if (!options.challenge) {
            bin = ParseHex("512102f8de1564b580413ebf081d45eaff093b9aff28d3694b9e48447112cd30cf4f2a2103d852c1588eb1114d080cdd0fca6eec632f548ad9df21daf4a4975a8406888fa652ae");

            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 1;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096
                .nTime    = 0,
                .nTxCount = 0,
                .dTxRate  = 0,
            };
        } else {
            bin = *options.challenge;
            consensus.nMinimumChainWork = uint256{};
            consensus.defaultAssumeValid = uint256{};
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
            chainTxData = ChainTxData{
                0,
                0,
                0,
            };
            LogPrintf("Signet with challenge %s\n", HexStr(bin));
        }

        if (options.seeds) {
            vSeeds = *options.seeds;
        }

        m_chain_type = ChainType::SIGNET;
        consensus.signet_blocks = true;
        consensus.signet_challenge.assign(bin.begin(), bin.end());
        consensus.nSubsidyHalvingInterval = 210000; //fujicoin do not use
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.nPowTargetTimespan = 1.4 * 24 * 60 * 60; // 1.4 days
        consensus.nPowTargetSpacing = 1.0 * 60;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% of 2016
        consensus.nMinerConfirmationWindow = 2016; // nPowTargetTimespan / nPowTargetSpacing
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Activation of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        // message start is defined as the first 4 bytes of the sha256d of the block script
        HashWriter h{};
        h << consensus.signet_challenge;
        uint256 hash = h.GetHash();
        std::copy_n(hash.begin(), 4, pchMessageStart.begin());

        nDefaultPort = 33777;
        nPruneAfterHeight = 1000;

        genesis = CreateGenesisBlock(1608811200, 912612, 0x1f0ffff0, 1, 1 * COIN); //(nTime, nNonce, nBits, nVersion, genesisReward)
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("db09c4e01dc7a930cba3a5fce730f37103715c29c1b6b674553457091d3106e7"));
        assert(genesis.hashMerkleRoot == uint256S("f951a273c3055d1bb36b4291e7f9edd491c2d435bd5737318ef8a643cab84b61"));

        vFixedSeeds.clear();

        m_assumeutxo_data = {
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,74);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,202);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tf";

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;
    }
};

/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowTargetTimespan = 1.4 * 24 * 60 * 60; // 1.4 days
        consensus.nPowTargetSpacing = 1.0 * 60;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0x69;
        pchMessageStart[1] = 0x6a;
        pchMessageStart[2] = 0x75;
        pchMessageStart[3] = 0x66;
        nDefaultPort = 16777;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1402725600, 10, 0x207fffff, 1, 1 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("b875cf0a8c9697c338d22bcb5c854497bcf34ff5ea31ee2150312aa4dcd25510"));
        assert(genesis.hashMerkleRoot == uint256S("0xf951a273c3055d1bb36b4291e7f9edd491c2d435bd5737318ef8a643cab84b61"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();
        vSeeds.emplace_back("dummySeed.invalid.");

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256S("b875cf0a8c9697c338d22bcb5c854497bcf34ff5ea31ee2150312aa4dcd25510")},
            }
        };

        m_assumeutxo_data = {
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,74);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,202);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "fcrt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}
