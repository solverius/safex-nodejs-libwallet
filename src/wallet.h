#pragma once

#include <napi.h>

#include <map>
#include <mutex>

#include <wallet_api.h>

#include "safexnativewallet.h"


namespace exawallet {


//using CopyablePersistentFunction = Napi::CopyablePersistentTraits<v8::Function>::CopyablePersistent;

class Wallet : public Napi::ObjectWrap<Wallet>, public SafexNativeWalletListener {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env,  SafexNativeWallet* wallet);

    static Napi::Value WalletExists(const Napi::CallbackInfo& info);
    static void CreateWallet(const Napi::CallbackInfo& info);
    static void CreateWalletFromKeys(const Napi::CallbackInfo& info);
    static void OpenWallet(const Napi::CallbackInfo& info);
    static void RecoveryWallet(const Napi::CallbackInfo& info);
    static Napi::Value GenPaymentId(const Napi::CallbackInfo& info);
    static Napi::Value PaymentIdValid(const Napi::CallbackInfo& info);
    static Napi::Value AddressValid(const Napi::CallbackInfo& info);


    explicit Wallet(SafexNativeWallet* wallet, const Napi::CallbackInfo& info): wallet_(wallet), Napi::ObjectWrap<Wallet>(info) {}
    explicit Wallet( const Napi::CallbackInfo& info): wallet_(nullptr), Napi::ObjectWrap<Wallet>(info) {}
    ~Wallet();

 private:



    virtual void moneySpent(const std::string &txId, uint64_t amount) override;
    virtual void moneyReceived(const std::string &txId, uint64_t amount) override;
    virtual void unconfirmedMoneyReceived(const std::string &txId, uint64_t amount) override;
    virtual void tokensSpent(const std::string &txId, uint64_t token_amount) override;
    virtual void tokensReceived(const std::string &txId, uint64_t token_amount) override;
    virtual void unconfirmedTokensReceived(const std::string &txId, uint64_t token_amount) override;
    virtual void advancedReceived(const std::string &txId, const uint8_t output_type) override;


    virtual void newBlock(uint64_t height) override;
    virtual void updated() override;
    virtual void refreshed() override;

    void Close(const Napi::CallbackInfo& info);

    void On(const Napi::CallbackInfo& info);
    void Off(const Napi::CallbackInfo& info);

    Napi::Value Address(const Napi::CallbackInfo& info);
    Napi::Value Seed(const Napi::CallbackInfo& info);
    void        SetSeedLanguage(const Napi::CallbackInfo& info);
    void        Store(const Napi::CallbackInfo& info);
    Napi::Value Path(const Napi::CallbackInfo& info);
    Napi::Value NetType(const Napi::CallbackInfo& info);

    Napi::Value SecretViewKey(const Napi::CallbackInfo& info);
    Napi::Value PublicViewKey(const Napi::CallbackInfo& info);
    Napi::Value SecretSpendKey(const Napi::CallbackInfo& info);
    Napi::Value PublicSpendKey(const Napi::CallbackInfo& info);

    Napi::Value SetPassword(const Napi::CallbackInfo& info);

    void SetRefreshFromBlockHeight(const Napi::CallbackInfo& info);
    Napi::Value GetRefreshFromBlockHeight(const Napi::CallbackInfo& info);

    void RescanBlockchain(const Napi::CallbackInfo& info);
    void RescanBlockchainAsync(const Napi::CallbackInfo& info);

    Napi::Value Connected(const Napi::CallbackInfo& info);
    void SetTrustedDaemon(const Napi::CallbackInfo& info);
    Napi::Value TrustedDaemon(const Napi::CallbackInfo& info);

    Napi::Value Balance(const Napi::CallbackInfo& info);
    Napi::Value UnlockedBalance(const Napi::CallbackInfo& info);
    Napi::Value TokenBalance(const Napi::CallbackInfo& info);
    Napi::Value UnlockedTokenBalance(const Napi::CallbackInfo& info);
    Napi::Value StakedTokenBalance(const Napi::CallbackInfo& info);
    Napi::Value UnlockedStakedTokenBalance(const Napi::CallbackInfo& info);
    Napi::Value GetMyInterest(const Napi::CallbackInfo& info);
    Napi::Value GetMyFeedbackTokens(const Napi::CallbackInfo& info);
    Napi::Value GetMyFeedbacks(const Napi::CallbackInfo& info);

    Napi::Value BlockChainHeight(const Napi::CallbackInfo& info);
    Napi::Value DaemonBlockChainHeight(const Napi::CallbackInfo& info);

    Napi::Value Synchronized(const Napi::CallbackInfo& info);

    Napi::Value DefaultMixin(const Napi::CallbackInfo& info);
    void SetDefaultMixin(const Napi::CallbackInfo& info);

    void StartRefresh(const Napi::CallbackInfo& info);
    void PauseRefresh(const Napi::CallbackInfo& info);

    Napi::Value TransactionHistory(const Napi::CallbackInfo& info);

    void CreateTransaction(const Napi::CallbackInfo& info);
    void CreateAdvancedTransaction(const Napi::CallbackInfo& info);

    Napi::Value CreateSafexAccount(const Napi::CallbackInfo& info);
    Napi::Value GetSafexAccounts(const Napi::CallbackInfo& info);
    Napi::Value GetSafexAccount(const Napi::CallbackInfo& info);
    Napi::Value RecoverSafexAccount(const Napi::CallbackInfo& info);
    Napi::Value RemoveSafexAccount(const Napi::CallbackInfo& info);

    Napi::Value GetMySafexOffers(const Napi::CallbackInfo& info);
    Napi::Value ListSafexOffers(const Napi::CallbackInfo& info);

    // Proofs and verification functions
    Napi::Value GetTxKey(const Napi::CallbackInfo& info);
    Napi::Value CheckTxKey(const Napi::CallbackInfo& info);
    Napi::Value GetTxProof(const Napi::CallbackInfo& info);
    Napi::Value CheckTxProof(const Napi::CallbackInfo& info);
    Napi::Value GetSpendProof(const Napi::CallbackInfo& info);
    Napi::Value CheckSpendProof(const Napi::CallbackInfo& info);
    Napi::Value GetReserveProof(const Napi::CallbackInfo& info);
    Napi::Value CheckReserveProof(const Napi::CallbackInfo& info);
    Napi::Value SignMessage(const Napi::CallbackInfo& info);
    Napi::Value VerifySignedMessage(const Napi::CallbackInfo& info);

    // AddressBook methods
    Napi::Value AddressBook_GetAll(const Napi::CallbackInfo& info);
    Napi::Value AddressBook_AddRow(const Napi::CallbackInfo& info);
    Napi::Value AddressBook_DeleteRow(const Napi::CallbackInfo& info);
    Napi::Value AddressBook_ErrorString(const Napi::CallbackInfo& info);
    Napi::Value AddressBook_LookupPID(const Napi::CallbackInfo& info);

    static Napi::FunctionReference constructor;


    SafexNativeWallet* wallet_ = nullptr;
    std::map<std::string, Napi::ThreadSafeFunction> callbacks_;
};

} //namespace exawallet
