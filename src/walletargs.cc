#include "walletargs.h"

#include <stdexcept>

#include <napi.h>
#include <string>

using namespace Napi;

namespace exawallet {

const uint32_t CreateTransactionArgs::DEFAULT_MIXIN = 6;
const uint32_t CreateAdvancedTransactionArgs::DEFAULT_MIXIN = 6;

template<typename T>
T convertValue(Napi::Value value);

template<typename T>
bool getRequiredProperty(Napi::Object obj, const char* name, T& value) {

    bool has = obj.HasOwnProperty(name);
    if (!has){
        return false;
    }

    auto valueLocal = obj[name];
    value = convertValue<T>(valueLocal);
    return true;
}

template<typename T>
T getOptionalProperty(Napi::Object obj, const char* name, const T& defaultValue) {

    bool has = obj.HasOwnProperty(name);
    if (!has){
        return defaultValue;
    }

    auto valueLocal = obj[name];
    return convertValue<T>(valueLocal);
}

template<>
std::string convertValue<std::string>(Napi::Value value) {
    std::string str = value.As<Napi::String>();
    return str;
}

template<>
double convertValue<double>(Napi::Value value) {
    return value.ToNumber().DoubleValue();
}

template<>
uint32_t convertValue<uint32_t>(Napi::Value value) {
    return value.ToNumber().Uint32Value();
}

template<>
uint64_t convertValue<uint64_t>(Napi::Value value) {
    return value.ToNumber().Int64Value();
}

std::string CreateWalletArgs::Init(const Napi::CallbackInfo& args) {

    Napi::Value val = args[0];

    if (!val.IsObject()) {
        return "Argument must be an object";
    }
    Napi::Env env = args.Env();

    auto obj = Napi::Object(env, args[0]);
    if (!getRequiredProperty<std::string>(obj, "path", path)) {
        return std::string("Required property not found: path");
    }

    if (!getRequiredProperty<std::string>(obj, "password", password)) {
        return std::string("Required property not found: password");
    }

    if (!getRequiredProperty<std::string>(obj, "daemonAddress", daemonAddress)) {
        return std::string("Required property not found: daemonAddress");
    }

    language = getOptionalProperty<std::string>(obj, "language", "English");
    auto net = getOptionalProperty<std::string>(obj, "network", "mainnet");
    if (net == "mainnet") {
        nettype = Safex::MAINNET;
    } else if (net == "testnet") {
        nettype = Safex::TESTNET;
    } else if (net == "stagenet") {
        nettype = Safex::STAGENET;
    } else {
        return "Invalid value for network: " + net;
    }

    return {};
}

  std::string CreateWalletFromKeysArgs::Init(const Napi::CallbackInfo& args) {

      Napi::Value val = args[0];

      if (!val.IsObject()) {
          return "Argument must be an object";
      }
      Napi::Env env = args.Env();

      auto obj = Napi::Object(env, args[0]);
      if (!getRequiredProperty<std::string>(obj, "path", path)) {
          return std::string("Required property not found: path");
      }

      if (!getRequiredProperty<std::string>(obj, "password", password)) {
          return std::string("Required property not found: password");
      }

      if (!getRequiredProperty<std::string>(obj, "daemonAddress", daemonAddress)) {
          return std::string("Required property not found: daemonAddress");
      }

      language = getOptionalProperty<std::string>(obj, "language", "English");
      auto net = getOptionalProperty<std::string>(obj, "network", "mainnet");
      if (net == "mainnet") {
          nettype = Safex::MAINNET;
      } else if (net == "testnet") {
          nettype = Safex::TESTNET;
      } else if (net == "stagenet") {
          nettype = Safex::STAGENET;
      } else {
          return "Invalid value for network: " + net;
      }

      restoreHeight = getOptionalProperty<uint64_t>(obj, "restoreHeight", 0);

      if (!getRequiredProperty<std::string>(obj, "addressString", addressString)) {
          return std::string("Required property not found: addressString");
      }

      if (!getRequiredProperty<std::string>(obj, "viewKeyString", viewKeyString)) {
          return std::string("Required property not found: viewKeyString");
      }

      if (!getRequiredProperty<std::string>(obj, "spendKeyString", spendKeyString)) {
          return std::string("Required property not found: spendKeyString");
      }

      return {};
  }

std::string OpenWalletArgs::Init(const Napi::CallbackInfo& args) {

    Napi::Value val = args[0];

    if (!val.IsObject()) {
        return "Argument must be an object";
    }
    Napi::Env env = args.Env();

    auto obj = Napi::Object(env, args[0]);
    if (!getRequiredProperty<std::string>(obj, "path", path)) {
        return std::string("Required property not found: path");
    }

    if (!getRequiredProperty<std::string>(obj, "password", password)) {
        return std::string("Required property not found: password");
    }

    if (!getRequiredProperty<std::string>(obj, "daemonAddress", daemonAddress)) {
        return std::string("Required property not found: daemonAddress");
    }

    auto net = getOptionalProperty<std::string>(obj, "network", "mainnet");
    if (net == "mainnet") {
        nettype = Safex::MAINNET;
    } else if (net == "testnet") {
        nettype = Safex::TESTNET;
    } else if (net == "stagenet") {
        nettype = Safex::STAGENET;
    } else {
        return "Invalid value for network: " + net;
    }

    return {};
}


std::string RecoveryWalletArgs::Init(const Napi::CallbackInfo& args) {

    Napi::Value val = args[0];

    if (!val.IsObject()) {
        return "Argument must be an object";
    }
    Napi::Env env = args.Env();

    auto obj = Napi::Object(env, args[0]);
    if (!getRequiredProperty<std::string>(obj, "path", path)) {
        return std::string("Required property not found: path");
    }

    if (!getRequiredProperty<std::string>(obj, "password", password)) {
        return std::string("Required property not found: password");
    }

    if (!getRequiredProperty<std::string>(obj, "daemonAddress", daemonAddress)) {
        return std::string("Required property not found: daemonAddress");
    }

    auto net = getOptionalProperty<std::string>(obj, "network", "mainnet");
    if (net == "mainnet") {
        nettype = Safex::MAINNET;
    } else if (net == "testnet") {
        nettype = Safex::TESTNET;
    } else if (net == "stagenet") {
        nettype = Safex::STAGENET;
    } else {
        return "Invalid value for network: " + net;
    }

    restoreHeight = getOptionalProperty<uint64_t>(obj, "restoreHeight", 0);

    if(!getRequiredProperty<std::string>(obj, "mnemonic", mnemonic)) {
        return std::string("Required property not found: mnemonic");
    }

    return {};
}


std::string CreateTransactionArgs::Init(const Napi::CallbackInfo& args) {

    Napi::Value val = args[0];

    if (!val.IsObject()) {
        return "Argument must be an object";
    }
    Napi::Env env = args.Env();

    auto obj = Napi::Object(env, args[0]);

    if (!getRequiredProperty<std::string>(obj, "address", address)) {
        return std::string("Required property not found: address");
    }

    std::string amountStr;
    if (!getRequiredProperty<std::string>(obj, "amount", amountStr)) {
        return std::string("Required property not found: amount");
    }
    amount = std::stoull(amountStr);
    paymentId = getOptionalProperty<std::string>(obj, "paymentId", "");
    mixin = getOptionalProperty<uint32_t>(obj, "mixin", DEFAULT_MIXIN);
    priority = static_cast<Safex::PendingTransaction::Priority>(getOptionalProperty<uint32_t>(obj, "priority",
            static_cast<uint32_t>(Safex::PendingTransaction::Priority::Priority_Medium)));
    tx_type = static_cast<Safex::TransactionType>(getOptionalProperty<uint32_t>(obj, "tx_type",
            static_cast<uint32_t>(Safex::TransactionType::CashTransaction)));

    return {};
}

std::string CreateAdvancedTransactionArgs::Init(const Napi::CallbackInfo& args) {

    Napi::Value val = args[0];

    if (!val.IsObject()) {
        return "Argument must be an object";
    }
    Napi::Env env = args.Env();

    auto obj = Napi::Object(env, args[0]);

    address = getOptionalProperty<std::string>(obj, "address", "");

    std::string amountStr;
    amountStr = getOptionalProperty<std::string>(obj, "amount", "0");

    safexUsername = getOptionalProperty<std::string>(obj, "safex_username", "");

    safexData = getOptionalProperty<std::string>(obj, "safex_data", "");

    safexOfferTitle = getOptionalProperty<std::string>(obj, "safex_offer_title", "");

    std::string priceStr;
    priceStr = getOptionalProperty<std::string>(obj, "safex_offer_price", "0");
    safexOfferPrice = std::stoull(priceStr);

    std::string quantityStr;
    quantityStr = getOptionalProperty<std::string>(obj, "safex_offer_quantity", "0");
    safexOfferQuantity = std::stoull(quantityStr);

    std::string minSfxPriceStr;
    minSfxPriceStr = getOptionalProperty<std::string>(obj, "safex_offer_min_sfx_price", "0");
    safexOfferMinSfxPrice = std::stoull(minSfxPriceStr);

    safexOfferDescription = getOptionalProperty<std::string>(obj, "safex_offer_description", "");
    safexOfferPricePegUsed = getOptionalProperty<uint64_t>(obj, "safex_offer_price_peg_used", 0);
    safexOfferPricePegId = getOptionalProperty<std::string>(obj, "safex_offer_price_peg_id", "");
    safexOfferActive = getOptionalProperty<uint64_t>(obj, "safex_offer_active", 1);
    safexOfferId = getOptionalProperty<std::string>(obj, "safex_offer_id", "");

    safexPricePegTitle = getOptionalProperty<std::string>(obj, "safex_price_peg_title", "");
    safexPricePegCreator = getOptionalProperty<std::string>(obj, "safex_price_peg_creator", "");
    safexPricePegDescription = getOptionalProperty<std::string>(obj, "safex_price_peg_description", "");
    safexPricePegCurrency = getOptionalProperty<std::string>(obj, "safex_price_peg_currency", "");
    safexPricePegRate = getOptionalProperty<double>(obj, "safex_price_peg_rate", 0);
    safexPricePegId = getOptionalProperty<std::string>(obj, "safex_price_peg_id", "");

    safexPurchaseQuantity = getOptionalProperty<uint64_t>(obj, "safex_purchase_quantity", 0);

    safexFeedbackStarsGiven = getOptionalProperty<uint64_t>(obj, "safex_feedback_stars_given", 0);
    safexFeedbackComment = getOptionalProperty<std::string>(obj, "safex_feedback_comment", "");

    safexStakedTokenHeight = getOptionalProperty<uint64_t>(obj, "safex_staked_token_height", 0);

    amount = std::stoull(amountStr);
    paymentId = getOptionalProperty<std::string>(obj, "paymentId", "");
    mixin = getOptionalProperty<uint32_t>(obj, "mixin", DEFAULT_MIXIN);
    priority = static_cast<Safex::PendingTransaction::Priority>(getOptionalProperty<uint32_t>(obj, "priority",
            static_cast<uint32_t>(Safex::PendingTransaction::Priority::Priority_Medium)));
    tx_type = static_cast<Safex::TransactionType>(getOptionalProperty<uint32_t>(obj, "tx_type",
            static_cast<uint32_t>(Safex::TransactionType::CashTransaction)));

    return {};
}

}
