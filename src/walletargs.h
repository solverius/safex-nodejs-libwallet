#pragma once

#include <nan.h>

#include <wallet_api.h>

namespace exawallet {

struct CreateWalletArgs {
    //returns error string
    std::string Init(const Nan::FunctionCallbackInfo<v8::Value>& args);

    std::string path;
    std::string password;
    std::string language;
    std::string daemonAddress;
    Safex::NetworkType nettype;
};

struct CreateWalletFromKeysArgs {
  //returns error string
  std::string Init(const Nan::FunctionCallbackInfo<v8::Value>& args);

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
    std::string Init(const Nan::FunctionCallbackInfo<v8::Value>& args);

    std::string path;
    std::string password;
    std::string daemonAddress;
    Safex::NetworkType nettype;
};


struct RecoveryWalletArgs {
    //returns error string
    std::string Init(const Nan::FunctionCallbackInfo<v8::Value>& args);

    std::string path;
    std::string password;
    std::string daemonAddress;
    Safex::NetworkType nettype;
    std::string mnemonic;
    uint64_t restoreHeight;
};


struct CreateTransactionArgs {
    //returns error string
    std::string Init(const Nan::FunctionCallbackInfo<v8::Value>& args);
    static const uint32_t MINIMAL_MIXIN;

    std::string address;
    uint64_t amount;
    std::string paymentId;
    uint32_t mixin;
    Safex::PendingTransaction::Priority priority = Safex::PendingTransaction::Priority_Low;
    Safex::TransactionType tx_type;
};

}
