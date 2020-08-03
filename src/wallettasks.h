#pragma once

#include "walletargs.h"

#include "safexnativewallet.h"

namespace exawallet {

class CreateWalletTask: public Napi::AsyncWorker {
public:
    CreateWalletTask(const CreateWalletArgs& args, Napi::Function& callback): args_(args), Napi::AsyncWorker(callback) {}

    void Execute();
    void OnOK();

private:
  CreateWalletArgs args_;
  SafexNativeWallet* wallet_ = nullptr;
  Napi::String error;
};

class CreateWalletFromKeysTask: public Napi::AsyncWorker {
public:
    CreateWalletFromKeysTask(const CreateWalletFromKeysArgs& args, Napi::Function& callback): args_(args), Napi::AsyncWorker(callback) {}

    void Execute();
    void OnOK();
private:
  CreateWalletFromKeysArgs args_;
  SafexNativeWallet* wallet_ = nullptr;
};

class OpenWalletTask: public Napi::AsyncWorker {
public:
    OpenWalletTask(const OpenWalletArgs& args, Napi::Function& callback): args_(args), Napi::AsyncWorker(callback) {}

    void Execute();
    void OnOK();
private:
    OpenWalletArgs args_;
    SafexNativeWallet* wallet_ = nullptr;
};

class CloseWalletTask: public Napi::AsyncWorker {
public:
    CloseWalletTask(SafexNativeWallet* wallet, bool store, Napi::Function& callback): wallet_(wallet), store_(store), Napi::AsyncWorker(callback) {}

    void Execute();

private:
  SafexNativeWallet* wallet_ = nullptr;
    bool store_;
};

class RecoveryWalletTask: public Napi::AsyncWorker {
public:
    RecoveryWalletTask(const RecoveryWalletArgs& args, Napi::Function& callback): args_(args), Napi::AsyncWorker(callback) {}

    void Execute();
    void OnOK();

private:
    RecoveryWalletArgs args_;
    SafexNativeWallet* wallet_ = nullptr;
};


class StoreWalletTask: public Napi::AsyncWorker {
public:
    StoreWalletTask(SafexNativeWallet* wallet, Napi::Function& callback): wallet_(wallet), Napi::AsyncWorker(callback) {}


    void Execute();

private:
  SafexNativeWallet* wallet_;
};

class CreateTransactionTask: public Napi::AsyncWorker {
public:
    CreateTransactionTask(const CreateTransactionArgs& args, SafexNativeWallet* wallet, Napi::Function& callback): args_(args), wallet_(wallet), Napi::AsyncWorker(callback) {}

    void Execute();
    void OnOK();

private:
    CreateTransactionArgs args_;
    SafexNativePendingTransaction* transaction_ = nullptr;
    SafexNativeWallet* wallet_;
};

class CreateAdvancedTransactionTask: public Napi::AsyncWorker {
public:
    CreateAdvancedTransactionTask(const CreateAdvancedTransactionArgs& args, SafexNativeWallet* wallet, Napi::Function& callback): args_(args), wallet_(wallet), Napi::AsyncWorker(callback) {}

    void Execute();
    void OnOK();

private:
    CreateAdvancedTransactionArgs args_;
    SafexNativePendingTransaction* transaction_ = nullptr;
    SafexNativeWallet* wallet_;
};

class CommitTransactionTask: public Napi::AsyncWorker {
public:
    CommitTransactionTask(SafexNativePendingTransaction* transaction, Napi::Function& callback): transaction_(transaction), Napi::AsyncWorker(callback) {}

    void Execute();

private:
    SafexNativePendingTransaction* transaction_;
};

class RestoreMultisigTransactionTask: public Napi::AsyncWorker {
public:
    RestoreMultisigTransactionTask(const std::string& transactionData, SafexNativeWallet* wallet, Napi::Function& callback)
        : transactionData_(transactionData), wallet_(wallet), Napi::AsyncWorker(callback) {}

    void Execute();
    void OnOK();

private:
    std::string transactionData_;
    SafexNativePendingTransaction* transaction_ = nullptr;
    SafexNativeWallet * wallet_;
};

} //namespace exawallet
