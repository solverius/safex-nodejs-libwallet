#pragma once

#include <napi.h>
#include <uv.h>

#include <wallet_api.h>

namespace exawallet {

struct CreateWalletArgs {
    //returns error string
    std::string Init(const Napi::CallbackInfo& args);

    std::string path;
    std::string password;
    std::string language;
    std::string daemonAddress;
    Safex::NetworkType nettype;
};

struct CreateWalletFromKeysArgs {
  //returns error string
  std::string Init(const Napi::CallbackInfo& args);

  std::string path;
  std::string password;
  std::string language;
  std::string daemonAddress;
  Safex::NetworkType nettype;
  uint64_t restoreHeight;
  std::string addressString;
  std::string viewKeyString;
  std::string spendKeyString;
};

struct OpenWalletArgs {
    //returns error string
    std::string Init(const Napi::CallbackInfo& args);

    std::string path;
    std::string password;
    std::string daemonAddress;
    Safex::NetworkType nettype;
};


struct RecoveryWalletArgs {
    //returns error string
    std::string Init(const Napi::CallbackInfo& args);

    std::string path;
    std::string password;
    std::string daemonAddress;
    Safex::NetworkType nettype;
    std::string mnemonic;
    uint64_t restoreHeight;
};


struct CreateTransactionArgs {
    //returns error string
    std::string Init(const Napi::CallbackInfo& args);
    static const uint32_t DEFAULT_MIXIN;

    std::string address;
    uint64_t amount;
    std::string paymentId;
    uint32_t mixin;
    Safex::PendingTransaction::Priority priority = Safex::PendingTransaction::Priority_Low;
    Safex::TransactionType tx_type;
};

struct CreateAdvancedTransactionArgs {
    //returns error string
    std::string Init(const Napi::CallbackInfo& args);
    static const uint32_t DEFAULT_MIXIN;

    std::string address;
    std::string safexUsername;
    std::string safexData;
    std::string safexOfferTitle;
    uint64_t    safexOfferPrice;
    uint64_t    safexOfferQuantity;
    std::string safexOfferDescription;
    bool        safexOfferPricePegUsed;
    std::string safexOfferPricePegId;
    uint64_t    safexOfferMinSfxPrice;
    bool        safexOfferActive;
    std::string safexOfferId;
    std::string safexPricePegTitle;
    std::string safexPricePegCreator;
    std::string safexPricePegDescription;
    std::string safexPricePegCurrency;
    double      safexPricePegRate;
    std::string safexPricePegId;
    uint64_t    safexPurchaseQuantity;
    uint64_t    safexFeedbackStarsGiven;
    std::string safexFeedbackComment;
    uint64_t    safexStakedTokenHeight;
    uint64_t amount;
    std::string paymentId;
    uint32_t mixin;
    Safex::PendingTransaction::Priority priority = Safex::PendingTransaction::Priority_Low;
    Safex::TransactionType tx_type;
};

}
