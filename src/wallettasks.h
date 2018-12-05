#pragma once

#include "deferredtask.h"
#include "walletargs.h"

#include "safexnativewallet.h"

#if _MSC_VER //windows node-gyp build
#include <windows_wrapper.h>
#include <winwallet.h>
#include <winwalletlistener.h>
#include <winwalletmanager.h>
#include <winpendingtransaction.h>
#endif

namespace exawallet {

class CreateWalletTask: public DeferredTask {
public:
    CreateWalletTask(const CreateWalletArgs& args): args_(args) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
    CreateWalletArgs args_;
  SafexNativeWallet* wallet_ = nullptr;
};

class CreateWalletFromKeysTask: public DeferredTask {
public:
  CreateWalletFromKeysTask(const CreateWalletFromKeysArgs& args): args_(args) {}

  virtual std::string doWork() override;
  virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
  CreateWalletFromKeysArgs args_;
  SafexNativeWallet* wallet_ = nullptr;
};

class OpenWalletTask: public DeferredTask {
public:
    OpenWalletTask(const OpenWalletArgs& args): args_(args) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
    OpenWalletArgs args_;
    SafexNativeWallet* wallet_ = nullptr;
};

class CloseWalletTask: public DeferredTask {
public:
    CloseWalletTask(SafexNativeWallet* wallet, bool store): wallet_(wallet), store_(store) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
  SafexNativeWallet* wallet_ = nullptr;
    bool store_;
};

class RecoveryWalletTask: public DeferredTask {
public:
    RecoveryWalletTask(const RecoveryWalletArgs& args): args_(args) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
    RecoveryWalletArgs args_;
    SafexNativeWallet* wallet_ = nullptr;
};


class StoreWalletTask: public DeferredTask {
public:
    StoreWalletTask(SafexNativeWallet* wallet): wallet_(wallet) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;
private:
  SafexNativeWallet* wallet_;
};

class CreateTransactionTask: public DeferredTask {
public:
    CreateTransactionTask(const CreateTransactionArgs& args, SafexNativeWallet* wallet): args_(args), wallet_(wallet) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
    CreateTransactionArgs args_;
    SafexNativePendingTransaction* transaction_ = nullptr;
    SafexNativeWallet* wallet_;
};

class CommitTransactionTask: public DeferredTask {
public:
    CommitTransactionTask(SafexNativePendingTransaction* transaction): transaction_(transaction) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
    SafexNativePendingTransaction* transaction_;
};

class RestoreMultisigTransactionTask: public DeferredTask {
public:
    RestoreMultisigTransactionTask(const std::string& transactionData, SafexNativeWallet* wallet)
        : transactionData_(transactionData), wallet_(wallet) {}

    virtual std::string doWork() override;
    virtual v8::Local<v8::Value> afterWork(std::string& error) override;

private:
    std::string transactionData_;
    SafexNativePendingTransaction* transaction_ = nullptr;
    SafexNativeWallet * wallet_;
};

} //namespace exawallet
