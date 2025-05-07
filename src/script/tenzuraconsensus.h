// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Tenzura Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TENZURA_TENZURACONSENSUS_H
#define TENZURA_TENZURACONSENSUS_H

#include <stdint.h>

#if defined(BUILD_TENZURA_INTERNAL) && defined(HAVE_CONFIG_H)
#include "config/tenzura-config.h"
  #if defined(_WIN32)
    #if defined(DLL_EXPORT)
      #if defined(HAVE_FUNC_ATTRIBUTE_DLLEXPORT)
        #define EXPORT_SYMBOL __declspec(dllexport)
      #else
        #define EXPORT_SYMBOL
      #endif
    #endif
  #elif defined(HAVE_FUNC_ATTRIBUTE_VISIBILITY)
    #define EXPORT_SYMBOL __attribute__ ((visibility ("default")))
  #endif
#elif defined(MSC_VER) && !defined(STATIC_LIBTENZURACONSENSUS)
  #define EXPORT_SYMBOL __declspec(dllimport)
#endif

#ifndef EXPORT_SYMBOL
  #define EXPORT_SYMBOL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TENZURACONSENSUS_API_VER 1

typedef enum tenzuraconsensus_error_t
{
    tenzuraconsensus_ERR_OK = 0,
    tenzuraconsensus_ERR_TX_INDEX,
    tenzuraconsensus_ERR_TX_SIZE_MISMATCH,
    tenzuraconsensus_ERR_TX_DESERIALIZE,
    tenzuraconsensus_ERR_AMOUNT_REQUIRED,
    tenzuraconsensus_ERR_INVALID_FLAGS,
} tenzuraconsensus_error;

/** Script verification flags */
enum
{
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_NONE                = 0,
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_P2SH                = (1U << 0), // evaluate P2SH (BIP16) subscripts
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_DERSIG              = (1U << 2), // enforce strict DER (BIP66) compliance
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_NULLDUMMY           = (1U << 4), // enforce NULLDUMMY (BIP147)
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY = (1U << 9), // enable CHECKLOCKTIMEVERIFY (BIP65)
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY = (1U << 10), // enable CHECKSEQUENCEVERIFY (BIP112)
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_WITNESS             = (1U << 11), // enable WITNESS (BIP141)
    tenzuraconsensus_SCRIPT_FLAGS_VERIFY_ALL                 = tenzuraconsensus_SCRIPT_FLAGS_VERIFY_P2SH | tenzuraconsensus_SCRIPT_FLAGS_VERIFY_DERSIG |
                                                               tenzuraconsensus_SCRIPT_FLAGS_VERIFY_NULLDUMMY | tenzuraconsensus_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY |
                                                               tenzuraconsensus_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY | tenzuraconsensus_SCRIPT_FLAGS_VERIFY_WITNESS
};

/// Returns 1 if the input nIn of the serialized transaction pointed to by
/// txTo correctly spends the scriptPubKey pointed to by scriptPubKey under
/// the additional constraints specified by flags.
/// If not nullptr, err will contain an error/success code for the operation
EXPORT_SYMBOL int tenzuraconsensus_verify_script(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen,
                                                 const unsigned char *txTo        , unsigned int txToLen,
                                                 unsigned int nIn, unsigned int flags, tenzuraconsensus_error* err);

EXPORT_SYMBOL int tenzuraconsensus_verify_script_with_amount(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen, int64_t amount,
                                    const unsigned char *txTo        , unsigned int txToLen,
                                    unsigned int nIn, unsigned int flags, tenzuraconsensus_error* err);

EXPORT_SYMBOL unsigned int tenzuraconsensus_version();

#ifdef __cplusplus
} // extern "C"
#endif

#undef EXPORT_SYMBOL

#endif // TENZURA_TENZURACONSENSUS_H
