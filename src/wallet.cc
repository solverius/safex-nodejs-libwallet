#include "wallet.h"

#include <iostream>
#include <algorithm>

#include "walletargs.h"
#include "wallettasks.h"


using namespace Napi;

namespace exawallet {

namespace {

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

template<>
bool convertValue<bool>(Napi::Value value) {
    return value.ToBoolean().Value();
}

std::string toStdString(const Napi::Value& val) {
    std::string nanStr = val.As<Napi::String>();
    return nanStr;
}

bool toVectorString(Napi::Value args, std::vector<std::string>& append) {
    Napi::Env env = args.Env();
    Napi::Array items = args.As<Napi::Array>();
    append.reserve(append.size() + items.Length());
    for (uint32_t i = 0; i < items.Length(); ++i) {
        Napi::Value msigInfo = items[i];
        if (!msigInfo.IsString()) {
            return false;
        }

        append.emplace_back(toStdString(msigInfo));
    }

    return true;
}

std::map<std::string, Safex::NetworkType> nettypes {
    {"mainnet", Safex::MAINNET},
    {"testnet", Safex::TESTNET},
    {"stagenet", Safex::STAGENET}
};

bool getNettype(const std::string& netstring, Safex::NetworkType& type) {
    auto it = nettypes.find(netstring);
    if (it == nettypes.end()) {
        return false;
    }

    type = it->second;
    return true;
}

bool convertNettype(Safex::NetworkType type, std::string& netstring) {
    auto it = std::find_if(nettypes.begin(), nettypes.end(), [type] (const std::pair<std::string, Safex::NetworkType> item) { return item.second == type; });
    if (it == nettypes.end()) {
        return false;
    }

    netstring = it->first;
    return true;
}

Napi::String convertAmount(Napi::Env env, uint64_t amount) {
    return Napi::String::New(env, std::to_string(amount).c_str());
}

Napi::Object makeAddressBookRowInfoObject(Napi::Env env, SafexNativeAddressBookRow* row) {
    auto result = Napi::Object::New(env);

     result.Set( "rowID",row->getRowId());
     result.Set( "address",row->getAddress());
     result.Set( "description",row->getDescription());
     result.Set( "paymentId",row->getPaymentId());

    return result;
}

Napi::Object makeTransactionInfoObject(Napi::Env env, const SafexNativeTransactionInfo* transaction) {

     auto transfersNative = transaction->transfers();
     auto transfers = Napi::Array::New(env, transfersNative.size());

     for (size_t i = 0; i < transfersNative.size(); ++i) {
         const auto& transfer = transfersNative[i];

         auto trObj = Napi::Object::New(env);

         trObj.Set("amount", convertAmount(env, transfer.amount));
         trObj.Set("tokenAmount", convertAmount(env, transfer.token_amount));

         transfers[i] = trObj;
     }

     auto result = Napi::Object::New(env);

    result.Set("transfers", transfers);

     const char* direction = transaction->direction() == Safex::TransactionInfo::Direction_In ? "in" : "out";
     result.Set("direction", direction);

    result.Set("pending", transaction->isPending());
    result.Set("failed", transaction->isFailed());
    result.Set("amount",  convertAmount(env, transaction->amount()));
    result.Set("tokenAmount", convertAmount(env, transaction->token_amount()));
    result.Set("fee", convertAmount(env, transaction->fee()));
    result.Set("blockHeight", transaction->blockHeight());
    result.Set("subAddrAccount", transaction->subaddrAccount());
    result.Set("label", transaction->label());
    result.Set("confirmations", transaction->confirmations());
    result.Set("unlockTime", transaction->unlockTime());
    result.Set("id", transaction->hash());
    result.Set("timestamp", transaction->timestamp());
    result.Set("paymentId", transaction->paymentId());

    uint32_t transactionType = static_cast<uint32_t>(transaction->transactionType());
    result.Set("transactionType", transactionType);

    return result;
}

Napi::Object makeInterstObject(Napi::Env env, const std::pair<uint64_t, std::pair<uint64_t, uint64_t>>& interestData) {
    auto result = Napi::Object::New(env);

    result.Set("tokenStaked", std::to_string(interestData.first));
    result.Set("collectedInterest", std::to_string(interestData.second.first));
    result.Set("blockHeight", std::to_string(interestData.second.second));


    return result;
}

Napi::Object makeFeedbackTokenObject(Napi::Env env, const std::pair<std::string, std::string>& feedbackTokenData) {
    auto result = Napi::Object::New(env);

    result.Set("offerID", feedbackTokenData.first);
    result.Set("offerTitle", feedbackTokenData.second);

    return result;
}


Napi::Object makeFeedbackObject(Napi::Env env, const SafexNativeSafexFeedback& feedbackData) {
    auto result = Napi::Object::New(env);

    result.Set("offerID", feedbackData.getOffer_id());
    result.Set("offerTitle", feedbackData.getTitle());
    result.Set("ratingGiven", feedbackData.getRating());  
    result.Set("comment", feedbackData.getComment());

    return result;
}

Napi::Object makeSafexAccountObject(Napi::Env env, const SafexNativeSafexAccount& safexAccount) {

    auto result = Napi::Object::New(env);

    result.Set( "username", Napi::String::New(env, safexAccount.getUsername()));
    result.Set( "data", Napi::String::New(env, safexAccount.getData()));
    result.Set( "publicKey", Napi::String::New(env, safexAccount.getPubKey()));
    result.Set( "privateKey", Napi::String::New(env, safexAccount.getSecKey()));
    result.Set( "status", Napi::Number::New(env, safexAccount.getStatus()));

    return result;
}

Napi::Object makeSafexOfferObject(Napi::Env env, const SafexNativeSafexOffer& safexOffer) {

    auto result = Napi::Object::New(env);

    result.Set("title", safexOffer.getTitle());
    result.Set("quantity", convertAmount(env, safexOffer.getQuantity()));
    result.Set("price", convertAmount(env, safexOffer.getPrice()));
    result.Set("minSfxPrice",convertAmount(env, safexOffer.getMin_sfx_price()));
    result.Set("description", safexOffer.getDescription());
    result.Set("active", safexOffer.getActive());
    result.Set("pricePegUsed", safexOffer.getPrice_peg_used());
    result.Set("offerID", safexOffer.getOffer_id());
    result.Set("pricePegID", safexOffer.getPrice_peg_id());
    result.Set("seller", safexOffer.getSeller());
    result.Set("currency", safexOffer.getCurrency());

    return result;

}

} // namespace

Napi::FunctionReference Wallet::constructor;


Wallet::~Wallet() {
    if (wallet_) {
        delete wallet_;
    }
}

Napi::Value Wallet::WalletExists(const Napi::CallbackInfo& info) {

      Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function accepts path to wallet").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string path = toStdString(info[0]);
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    if (!manager) {
        Napi::TypeError::New(env, "Wallet manager could not be instantiated!").ThrowAsJavaScriptException();
        return env.Null();
    }
    bool exists = manager->walletExists(path);

    return Napi::Boolean::New(env, exists);
}

void Wallet::CreateWallet(const Napi::CallbackInfo& info) {
      Napi::Env env = info.Env();

    CreateWalletArgs walletArgs;
    std::string error = walletArgs.Init(info);
    if (!error.empty()) {
        Napi::Error::New(env, error.c_str()).ThrowAsJavaScriptException();
        return;
    }

    Function callback = info[1].As<Function>();

    CreateWalletTask* task = new CreateWalletTask(walletArgs, callback);
    task->Queue();
    return;
}

void Wallet::CreateWalletFromKeys(const Napi::CallbackInfo& info) {
      Napi::Env env = info.Env();

        CreateWalletFromKeysArgs walletArgs;
        std::string error = walletArgs.Init(info);
        if (!error.empty()) {
          Napi::Error::New(env, error.c_str()).ThrowAsJavaScriptException();
          return;
        }

        Function callback = info[1].As<Function>();

        CreateWalletFromKeysTask* task = new CreateWalletFromKeysTask(walletArgs, callback);
        task->Queue();
        return;
}

void Wallet::OpenWallet(const Napi::CallbackInfo& info) {
      Napi::Env env = info.Env();

    OpenWalletArgs walletArgs;
    std::string error = walletArgs.Init(info);
    if (!error.empty()) {
        Napi::Error::New(env, error.c_str()).ThrowAsJavaScriptException();
        return;
    }

    Function callback = info[1].As<Function>();

    OpenWalletTask* task = new OpenWalletTask(walletArgs, callback);
    task->Queue();
    return;
}

void Wallet::RecoveryWallet(const Napi::CallbackInfo& info) {
      Napi::Env env = info.Env();

    RecoveryWalletArgs walletArgs;
    std::string error = walletArgs.Init(info);
    if (!error.empty()) {
        Napi::Error::New(env, error.c_str()).ThrowAsJavaScriptException();
        return;
    }

    Function callback = info[1].As<Function>();

    RecoveryWalletTask* task = new RecoveryWalletTask(walletArgs, callback);
    task->Queue();
    return;
}

Napi::Object Wallet::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "Wallet",
                {
                InstanceMethod("close", &Wallet::Close),
                InstanceMethod("on", &Wallet::On),
                InstanceMethod("off", &Wallet::Off),
                InstanceMethod("address", &Wallet::Address),
                InstanceMethod("seed", &Wallet::Seed),
                InstanceMethod("setSeedLanguage", &Wallet::SetSeedLanguage),
                InstanceMethod("store", &Wallet::Store),
                InstanceMethod("path", &Wallet::Path),
                InstanceMethod("network", &Wallet::NetType),
                InstanceMethod("secretViewKey", &Wallet::SecretViewKey),
                InstanceMethod("publicViewKey", &Wallet::PublicViewKey),
                InstanceMethod("secretSpendKey", &Wallet::SecretSpendKey),
                InstanceMethod("publicSpendKey", &Wallet::PublicSpendKey),
                InstanceMethod("setPassword", &Wallet::SetPassword),
                InstanceMethod("setRefreshFromBlockHeight", &Wallet::SetRefreshFromBlockHeight),
                InstanceMethod("getRefreshFromBlockHeight", &Wallet::GetRefreshFromBlockHeight),
                InstanceMethod("rescanBlockchain", &Wallet::RescanBlockchain),
                InstanceMethod("rescanBlockchainAsync", &Wallet::RescanBlockchainAsync),
                InstanceMethod("connected", &Wallet::Connected),
                InstanceMethod("setTrustedDaemon", &Wallet::SetTrustedDaemon),
                InstanceMethod("trustedDaemon", &Wallet::TrustedDaemon),
                InstanceMethod("balance", &Wallet::Balance),
                InstanceMethod("unlockedBalance", &Wallet::UnlockedBalance),
                InstanceMethod("tokenBalance", &Wallet::TokenBalance),
                InstanceMethod("unlockedTokenBalance", &Wallet::UnlockedTokenBalance),
                InstanceMethod("stakedTokenBalance", &Wallet::StakedTokenBalance),
                InstanceMethod("unlockedStakedTokenBalance", &Wallet::UnlockedStakedTokenBalance),
                InstanceMethod("getMyStake", &Wallet::GetMyInterest),
                InstanceMethod("getMyFeedbackTokens", &Wallet::GetMyFeedbackTokens),
                InstanceMethod("getMyFeedbacks", &Wallet::GetMyFeedbacks),
                InstanceMethod("blockchainHeight", &Wallet::BlockChainHeight),
                InstanceMethod("daemonBlockchainHeight", &Wallet::DaemonBlockChainHeight),
                InstanceMethod("synchronized", &Wallet::Synchronized),
                InstanceMethod("defaultMixin", &Wallet::DefaultMixin),
                InstanceMethod("setDefaultMixin", &Wallet::SetDefaultMixin),
                InstanceMethod("startRefresh", &Wallet::StartRefresh),
                InstanceMethod("pauseRefresh", &Wallet::PauseRefresh),
                InstanceMethod("createTransaction", &Wallet::CreateTransaction),
                InstanceMethod("createAdvancedTransaction", &Wallet::CreateAdvancedTransaction),
                InstanceMethod("createSafexAccount", &Wallet::CreateSafexAccount),
                InstanceMethod("getSafexAccounts", &Wallet::GetSafexAccounts),
                InstanceMethod("getSafexAccount", &Wallet::GetSafexAccount),
                InstanceMethod("recoverSafexAccount", &Wallet::RecoverSafexAccount),
                InstanceMethod("removeSafexAccount", &Wallet::RemoveSafexAccount),
                InstanceMethod("getMySafexOffers", &Wallet::GetMySafexOffers),
                InstanceMethod("listSafexOffers", &Wallet::ListSafexOffers),
                InstanceMethod("getTxKey", &Wallet::GetTxKey),
                InstanceMethod("checkTxKey", &Wallet::CheckTxKey),
                InstanceMethod("getTxProof", &Wallet::GetTxProof),
                InstanceMethod("checkTxProof", &Wallet::CheckTxProof),
                InstanceMethod("getSpendProof", &Wallet::GetSpendProof),
                InstanceMethod("checkSpendProof", &Wallet::CheckSpendProof),
                InstanceMethod("getReserveProof", &Wallet::GetReserveProof),
                InstanceMethod("checkReserveProof", &Wallet::CheckReserveProof),
                InstanceMethod("signMessage", &Wallet::SignMessage),
                InstanceMethod("verifySignedMessage", &Wallet::VerifySignedMessage),
                InstanceMethod("history", &Wallet::TransactionHistory),
                InstanceMethod("addressBook_GetAll", &Wallet::AddressBook_GetAll),
                InstanceMethod("addressBook_AddRow", &Wallet::AddressBook_AddRow),
                InstanceMethod("addressBook_DeleteRow", &Wallet::AddressBook_DeleteRow),
                InstanceMethod("addressBook_ErrorString", &Wallet::AddressBook_ErrorString),
                InstanceMethod("addressBook_LookupPID", &Wallet::AddressBook_LookupPID)
                                      });



    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Wallet", func);

    return exports;
}

Napi::Object Wallet::NewInstance(Napi::Env env, SafexNativeWallet *wallet) {


    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor.New({});

    Wallet* w = Napi::ObjectWrap<Wallet>::Unwrap(obj);

    w->wallet_ = wallet;

    wallet->setListener(w);
    //TODO: make it configurable
    wallet->segregatePreForkOutputs(false);
    wallet->keyReuseMitigation2(false);

    return scope.Escape(napi_value(obj)).ToObject();

}


void Wallet::Close(const Napi::CallbackInfo& info)  {

    Napi::Env env = info.Env();

     if (info.Length() > 1 && !info[0].IsBoolean()) {
         Napi::Error::New(env, "Function accepts one optional boolean argument").ThrowAsJavaScriptException();
         return;
     }
     bool store = false;
      if (info.Length() != 0) {
          store = info[0].As<Napi::Boolean>().Value();
      }

     Function callback = info[1].As<Function>();

     CloseWalletTask* task = new CloseWalletTask(this->wallet_, store, callback);
     task->Queue();

    return;
}

Napi::Value Wallet::Address(const Napi::CallbackInfo& info) {

        auto buf = this->wallet_->address();
        auto addr = Napi::String::New(info.Env(), buf.c_str());

        return addr;
}

Napi::Value Wallet::Seed(const Napi::CallbackInfo& info) {

        auto buf = this->wallet_->seed();
        auto seed = Napi::String::New(info.Env(), buf.c_str());

        return seed;
}

void Wallet::SetSeedLanguage(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsString()) {
                Napi::TypeError::New(env, "String argument is required").ThrowAsJavaScriptException();
                return;
    }

    this->wallet_->setSeedLanguage(toStdString(info[0]));

    return;
}

void Wallet::On(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

        if (info.Length() != 2) {
            Napi::Error::New(env, "2 arguments are required").ThrowAsJavaScriptException();
            return;
        }

        if (!info[0].IsString() || !info[1].IsFunction()) {

            Napi::TypeError::New(env, "Function accepts string and function arguments").ThrowAsJavaScriptException();
            return;
        }

        this->callbacks_[toStdString(info[0])] = Napi::ThreadSafeFunction::New(
            env,
            info[1].As<Function>(),  // JavaScript function called asynchronously
            toStdString(info[0]),         // Name
            0,                       // Unlimited queue
            1,                       // Only one thread will use this initially
            []( Napi::Env ) {        // Finalizer used to clean threads up
            } );


    return;
}

void Wallet::Off(const Napi::CallbackInfo& info) {
     Napi::Env env = info.Env();
     Napi::HandleScope scope(env);

        //delete all listeners
        if (info.Length() == 0) {
            this->callbacks_.clear();
            return;
        }

        if (info.Length() != 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "Function accepts no arguments or event name").ThrowAsJavaScriptException();
            return;
        }

        this->callbacks_.erase(toStdString(info[0]));
        return;

}

void Wallet::Store(const Napi::CallbackInfo& info) {


    Function callback = info[0].As<Function>();

    StoreWalletTask* task = new StoreWalletTask(this->wallet_, callback);
    task->Queue();

    return;

}

Napi::Value Wallet::Path(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    return Napi::String::New(env, this->wallet_->path().c_str());

}

Napi::Value Wallet::NetType(const Napi::CallbackInfo& info) {

        Napi::Env env = info.Env();

        std::string nettype;
        if (!convertNettype(this->wallet_->nettype(), nettype)) {
            Napi::TypeError::New(env, "Error getting nettype").ThrowAsJavaScriptException();
            return Napi::String::New(env, "error");
        }

        return Napi::String::New(env, nettype.c_str());

}

Napi::Value Wallet::SecretViewKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, this->wallet_->secretViewKey().c_str());

}

Napi::Value Wallet::PublicViewKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, this->wallet_->publicViewKey().c_str());

}

Napi::Value Wallet::SecretSpendKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, this->wallet_->secretSpendKey().c_str());

}

Napi::Value Wallet::PublicSpendKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, this->wallet_->publicSpendKey().c_str());

}

Napi::Value Wallet::SetPassword(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

        if (info.Length() != 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "String argument is required").ThrowAsJavaScriptException();
            return env.Null();
        }

        bool res = this->wallet_->setPassword(toStdString(info[0].As<Napi::String>()));

        if (!res) {
            Napi::Error::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        }

        return Napi::Boolean::New(env, res);

}

void Wallet::SetRefreshFromBlockHeight(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

        if (info.Length() != 1 || !info[0].IsNumber()) {
            Napi::TypeError::New(env, "Integer argument is required").ThrowAsJavaScriptException();
            return;
        }

        uint32_t height = info[0].As<Napi::Number>();

        this->wallet_->setRefreshFromBlockHeight(height);

        return;

}

Napi::Value Wallet::GetRefreshFromBlockHeight(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    return Napi::Number::New(env, (uint32_t)this->wallet_->getRefreshFromBlockHeight());

}

Napi::Value Wallet::Connected(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();


        std::string status;
        switch (this->wallet_->connected()) {
        case Safex::Wallet::ConnectionStatus_Connected:
            status = "connected";
            break;
        case Safex::Wallet::ConnectionStatus_Disconnected:
            status = "disconnected";
            break;
        case Safex::Wallet::ConnectionStatus_WrongVersion:
            status = "incompatible";
            break;
        default:
            Napi::TypeError::New(env, "Error getting connected status").ThrowAsJavaScriptException();
            break;
        }

        return Napi::String::New(env, status.c_str());

}

void Wallet::SetTrustedDaemon(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();


        if (info.Length() != 1 || !info[0].IsBoolean()) {
            Napi::TypeError::New(env, "Bool argument is required").ThrowAsJavaScriptException();
            return;
        }

        this->wallet_->setTrustedDaemon(info[0].As<Napi::Boolean>().Value());

    return;

}

Napi::Value Wallet::TrustedDaemon(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    return Napi::Boolean::New(env, this->wallet_->trustedDaemon());

}

Napi::Value Wallet::Balance(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    return Napi::String::New(env, std::to_string(this->wallet_->balanceAll()).c_str());

}

Napi::Value Wallet::UnlockedBalance(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, std::to_string(this->wallet_->unlockedBalanceAll()).c_str());

}

Napi::Value Wallet::TokenBalance(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, std::to_string(this->wallet_->tokenBalanceAll()).c_str());

}

Napi::Value Wallet::UnlockedTokenBalance(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, std::to_string(this->wallet_->unlockedTokenBalanceAll()).c_str());

}

Napi::Value Wallet::StakedTokenBalance(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, std::to_string(this->wallet_->stakedTokenBalanceAll()).c_str());

}

Napi::Value Wallet::UnlockedStakedTokenBalance(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, std::to_string(this->wallet_->unlockedStakedTokenBalanceAll()).c_str());

}

Napi::Value Wallet::GetMyInterest(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

            std::vector<std::pair<uint64_t, std::pair<uint64_t, uint64_t>>> interest_per_output;

            uint64_t total_interest = this->wallet_->getMyInterest(interest_per_output);

            auto result = Napi::Array::New(env, interest_per_output.size());
            for (size_t i = 0; i < interest_per_output.size(); ++i) {
                const auto& output = interest_per_output[i];

                auto interestObj = makeInterstObject(env, output);

                result[i] = interestObj;
            }

            return result;

}

Napi::Value Wallet::GetMyFeedbackTokens(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    std::vector<std::pair<std::string, std::string>> feedback_tokens = this->wallet_->getMyFeedbacksToGive();

    auto result = Napi::Array::New(env, feedback_tokens.size());
    for (size_t i = 0; i < feedback_tokens.size(); ++i) {
        const auto& output = feedback_tokens[i];

        auto feedbackTokenObj = makeFeedbackTokenObject(env, output);

        result[i] = feedbackTokenObj;
    }

    return result;

}

Napi::Value Wallet::GetMyFeedbacks(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    std::vector<SafexNativeSafexFeedback> feedbacks = this->wallet_->getMyFeedbacksGiven();

    auto result = Napi::Array::New(env, feedbacks.size());
    for (size_t i = 0; i < feedbacks.size(); ++i) {
        const auto& output = feedbacks[i];

        auto feedbackObj = makeFeedbackObject(env, output);

        result[i] = feedbackObj;
    }

    return result;

}
Napi::Value Wallet::BlockChainHeight(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, (uint32_t)this->wallet_->blockChainHeight());
}

Napi::Value Wallet::DaemonBlockChainHeight(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, (uint32_t)this->wallet_->daemonBlockChainHeight());

}

Napi::Value Wallet::Synchronized(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Boolean::New(env, this->wallet_->synchronized());

}

Napi::Value Wallet::GenPaymentId(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::String::New(env, SafexNativeWallet::genPaymentId().c_str());

}

Napi::Value Wallet::PaymentIdValid(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

        if (info.Length() != 1 || !info[0].IsString()) {
            Napi::TypeError::New(env, "String argument is required").ThrowAsJavaScriptException();
            return env.Null();
        }

        return Napi::Boolean::New(env, SafexNativeWallet::paymentIdValid(toStdString(info[0])));

}

Napi::Value Wallet::AddressValid(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

        if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString()) {
            Napi::TypeError::New(env, "2 string arguments are required").ThrowAsJavaScriptException();
            return env.Null();
        }

        Safex::NetworkType nettype;
        if (!getNettype(toStdString(info[1]), nettype)) {
            Napi::Error::New(env, "wrong network type argument").ThrowAsJavaScriptException();
            return env.Null();
        }
        bool valid = SafexNativeWallet::addressValid(toStdString(info[0]), nettype);
        return Napi::Boolean::New(env, valid);

}

void Wallet::RescanBlockchain(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
            this->wallet_->setRefreshFromBlockHeight(0);
            this->wallet_->rescanBlockchain();
}

void Wallet::RescanBlockchainAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
            this->wallet_->setRefreshFromBlockHeight(0);
            this->wallet_->rescanBlockchainAsync();

}


Napi::Value Wallet::DefaultMixin(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    return Napi::Number::New(env, this->wallet_->defaultMixin());

}

void Wallet::SetDefaultMixin(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

        if (info.Length() != 1 || !info[0].IsNumber()) {
            Napi::TypeError::New(env, "Integer argument is required").ThrowAsJavaScriptException();
            return;
        }

        this->wallet_->setDefaultMixin((info[0].As<Number>()));

}

Napi::Value Wallet::StartRefresh(const Napi::CallbackInfo& info) {
    this->wallet_->startRefresh();

}

Napi::Value Wallet::PauseRefresh(const Napi::CallbackInfo& info) {
    this->wallet_->pauseRefresh();
}

Napi::Value Wallet::TransactionHistory(const Napi::CallbackInfo& info) {

        Napi::Env env = info.Env();

        auto history = this->wallet_->history();
        history->refresh();
        auto transactions = history->getAll();
        auto result = Napi::Array::New(env, transactions.size());
        for (size_t i = 0; i < transactions.size(); ++i) {
            const auto& transaction = transactions[i];

            if(transaction == nullptr) continue;

            auto txObj = makeTransactionInfoObject(env, transaction);

            result[i] = txObj;
        }

        return result;

}

void Wallet::CreateTransaction(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

        CreateTransactionArgs txArgs;
        std::string error = txArgs.Init(info);
        if (!error.empty()) {
            Napi::Error::New(env, error.c_str()).ThrowAsJavaScriptException();
            return;
        }

        Function callback = info[1].As<Function>();


        CreateTransactionTask* task = new CreateTransactionTask(txArgs, this->wallet_, callback);
        task->Queue();
        return;

}

void Wallet::CreateAdvancedTransaction(const Napi::CallbackInfo& info) {

        Napi::Env env = info.Env();

        CreateAdvancedTransactionArgs txArgs;
        std::string error = txArgs.Init(info);
        if (!error.empty()) {
            Napi::Error::New(env, error.c_str()).ThrowAsJavaScriptException();
            return;
        }

        Function callback = info[1].As<Function>();

        CreateAdvancedTransactionTask* task = new CreateAdvancedTransactionTask(txArgs, this->wallet_, callback);
        task->Queue();
        return;

}

Napi::Value Wallet::CreateSafexAccount(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Function accepts string argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto username = toStdString(info[0]);
    auto desc = toStdString(info[1]);

    auto description = std::vector<uint8_t>(desc.begin(),desc.end());


    auto res = this->wallet_->createSafexAccount(username, description);

    return Napi::Boolean::New(env, res);


}

Napi::Value Wallet::GetSafexAccounts(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 0) {
        Napi::TypeError::New(env, "Function accepts no arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto safexAccounts = this->wallet_->getSafexAccounts();
    auto result = Napi::Array::New(env, safexAccounts.size());

    for (size_t i = 0; i < safexAccounts.size(); ++i) {
            const auto& safexAccount = safexAccounts[i];

            auto safexAccountObj = makeSafexAccountObject(env, safexAccount);

            result[i] = safexAccountObj;

        }


    return result;


}

Napi::Value Wallet::GetSafexAccount(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function accepts string argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto username = toStdString(info[0]);

    auto safexAccount = this->wallet_->getSafexAccount(username);
    auto safexAccountObj = makeSafexAccountObject(env, safexAccount);

    return safexAccountObj;

}

Napi::Value Wallet::RecoverSafexAccount(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Function accepts string arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto username = toStdString(info[0]);
    auto secret_key = toStdString(info[1]);


    auto res = this->wallet_->recoverSafexAccount(username, secret_key);


    return Napi::Boolean::New(env, res);

}

Napi::Value Wallet::RemoveSafexAccount(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function accepts string argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto username = toStdString(info[0]);

    auto res = this->wallet_->removeSafexAccount(username);

    return Napi::Boolean::New(env, res);


}

Napi::Value Wallet::GetMySafexOffers(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

        if (info.Length() != 0) {
            Napi::TypeError::New(env, "Function accepts no arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        auto safexOffers = this->wallet_->getMySafexOffers();
        auto result = Napi::Array::New(env, safexOffers.size());

        for (size_t i = 0; i < safexOffers.size(); ++i) {
                const auto& safexOffer = safexOffers[i];

                auto safexOfferObj = makeSafexOfferObject(env, safexOffer);

                result[i] = safexOfferObj;
        }

        return result;

}

Napi::Value Wallet::ListSafexOffers(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();


        if (info.Length() != 1 || !info[0].IsBoolean()) {
            Napi::TypeError::New(env, "Function accepts boolean argument").ThrowAsJavaScriptException();
            return env.Null();
        }

        bool active = info[0].As<Napi::Boolean>().Value();


        auto safexOffers = this->wallet_->listSafexOffers(active);
        auto result = Napi::Array::New(env, safexOffers.size());

        for (size_t i = 0; i < safexOffers.size(); ++i) {
                const auto& safexOffer = safexOffers[i];

                auto safexOfferObj = makeSafexOfferObject(env, safexOffer);

                result[i] = safexOfferObj;
        }


        return result;

}

Napi::Value Wallet::GetTxKey(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function accepts string argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto txKey = this->wallet_->getTxKey(toStdString(info[0]));
    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, txKey.c_str());
}



Napi::Value Wallet::CheckTxKey(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    std::string txid;
    std::string tx_key;
    std::string address;

    auto obj = Napi::Object(env, info[0]);

    if (!getRequiredProperty<std::string>(obj, "txId", txid)) {
    	Napi::Error::New(env, "Required property not found: txId").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!getRequiredProperty<std::string>(obj, "txKey", tx_key)) {
    	Napi::Error::New(env, "Required property not found: txKey").ThrowAsJavaScriptException();
        return env.Null();    
    }

    if (!getRequiredProperty<std::string>(obj, "address", address)) {
    	Napi::Error::New(env, "Required property not found: address").ThrowAsJavaScriptException();
        return env.Null();    
    }

    uint64_t received_cash;
    uint64_t received_token;
    bool in_pool;
    uint64_t confirmations;

    bool valid = this->wallet_->checkTxKey(txid, tx_key, address, received_cash, received_token, in_pool, confirmations);

    auto result = Napi::Object::New(env);

    result.Set("receivedCash", convertAmount(env, received_cash));
    result.Set("receivedToken", convertAmount(env, received_token));
    result.Set("inPool", in_pool);
    result.Set("confirmations", confirmations);
    result.Set("valid", valid);

    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return result;
}

Napi::Value Wallet::GetTxProof(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    std::string txid;
    std::string message;
    std::string address;

    auto obj = Napi::Object(env, info[0]);

    if (!getRequiredProperty<std::string>(obj, "txId", txid)) {
    	Napi::Error::New(env, "Required property not found: txId").ThrowAsJavaScriptException();
        return env.Null();    
    }

    if (!getRequiredProperty<std::string>(obj, "address", address)) {
    	Napi::Error::New(env, "Required property not found: address").ThrowAsJavaScriptException();
        return env.Null();    
    }

    message = getOptionalProperty<std::string>(obj, "message", "");

    auto txProof = this->wallet_->getTxProof(txid, address, message);
    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, txProof.c_str());

}

Napi::Value Wallet::CheckTxProof(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();


    std::string txid;
    std::string message;
    std::string signature;
    std::string address;

    auto obj = Napi::Object(env, info[0]);

    if (!getRequiredProperty<std::string>(obj, "txId", txid)) {
    	Napi::Error::New(env, "Required property not found: txId").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!getRequiredProperty<std::string>(obj, "address", address)) {
    	Napi::Error::New(env, "Required property not found: address").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!getRequiredProperty<std::string>(obj, "signature", signature)) {
    	Napi::Error::New(env, "Required property not found: signature").ThrowAsJavaScriptException();
        return env.Null();
    }

    message = getOptionalProperty<std::string>(obj, "message", "");

    bool good;
    uint64_t received_cash;
    uint64_t received_token;
    bool in_pool; 
    uint64_t confirmations;


    auto valid = this->wallet_->checkTxProof(txid, address, message, signature, good, received_cash, received_token, in_pool, confirmations);

    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    auto result = Napi::Object::New(env);

    result.Set("receivedCash", convertAmount(env, received_cash));
    result.Set("receivedToken", convertAmount(env, received_token));
    result.Set("inPool", in_pool);
    result.Set("confirmations", confirmations);
    result.Set("valid", good);

    return result;
}



Napi::Value Wallet::GetSpendProof(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    std::string txid;
    std::string message;

    auto obj = Napi::Object(env, info[0]);

    if (!getRequiredProperty<std::string>(obj, "txId", txid)) {
    	Napi::Error::New(env, "Required property not found: txId").ThrowAsJavaScriptException();
        return env.Null();
    }

    message = getOptionalProperty<std::string>(obj, "message", "");

    auto spendProof = this->wallet_->getSpendProof(txid, message);
    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, spendProof.c_str());
}

Napi::Value Wallet::CheckSpendProof(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();


    std::string txid;
    std::string message;
    std::string signature;

    auto obj = Napi::Object(env, info[0]);

    if (!getRequiredProperty<std::string>(obj, "txId", txid)) {
    	Napi::Error::New(env, "Required property not found: txId").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!getRequiredProperty<std::string>(obj, "signature", signature)) {
    	Napi::Error::New(env, "Required property not found: signature").ThrowAsJavaScriptException();
        return env.Null(); 
	}

    message = getOptionalProperty<std::string>(obj, "message", "");

    bool good = false;

    bool valid = this->wallet_->checkSpendProof(txid, message, signature, good);

    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Boolean::New(env, good);

}

Napi::Value Wallet::GetReserveProof(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    bool all;
    uint32_t account_index;
    uint64_t amount;
    std::string message;

    auto obj = Napi::Object(env, info[0]);

    if (!getRequiredProperty<bool>(obj, "all", all)) {
    	Napi::Error::New(env, "Required property not found: all").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    if (!getRequiredProperty<uint32_t>(obj, "account_index", account_index)) {
    	Napi::Error::New(env, "Required property not found: account_index").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    std::string amount_str;
    if (!getRequiredProperty<std::string>(obj, "amount", amount_str)) {
    	Napi::Error::New(env, "Required property not found: amount").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    amount = std::stoull(amount_str);

    message = getOptionalProperty<std::string>(obj, "message", "");

    auto reserveProof = this->wallet_->getReserveProof(all, account_index, amount, message);
    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, reserveProof.c_str());
}


Napi::Value Wallet::CheckReserveProof(const Napi::CallbackInfo& info) {
	
    Napi::Env env = info.Env();

    std::string address;
    std::string message;
    std::string signature;

    auto obj = Napi::Object(env, info[0]);

    if (!getRequiredProperty<std::string>(obj, "address", address)) {
    	Napi::Error::New(env, "Required property not found: address").ThrowAsJavaScriptException();
        return env.Null(); 
	}

    if (!getRequiredProperty<std::string>(obj, "signature", signature)) {
    	Napi::Error::New(env, "Required property not found: signature").ThrowAsJavaScriptException();
        return env.Null(); 
    }

    message = getOptionalProperty<std::string>(obj, "message", "");

    bool good;
    uint64_t total_cash;
    uint64_t cash_spent;
    uint64_t total_token;
    uint64_t token_spent;


    auto valid = this->wallet_->checkReserveProof(address, message, signature, good, total_cash, cash_spent, total_token, token_spent);

    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    auto result = Napi::Object::New(env);

    result.Set("totalCash", convertAmount(env, total_cash));
    result.Set("cashSpent", convertAmount(env, cash_spent));
    result.Set("totalToken", convertAmount(env, total_token));
    result.Set("tokenSpent", convertAmount(env, token_spent));
    result.Set("valid", good);

    return result;
}


Napi::Value Wallet::SignMessage(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Function accepts string argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto signature = this->wallet_->signMessage(toStdString(info[0]));
    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, signature.c_str());

}

Napi::Value Wallet::VerifySignedMessage(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() != 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString()) {
        Napi::TypeError::New(env, "Function accepts message, Safex address and signature as string arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto message = toStdString(info[0]);
    auto address = toStdString(info[1]);
    auto signature = toStdString(info[2]);

    bool valid = this->wallet_->verifySignedMessage(message, address, signature);

    if (this->wallet_->status() != Safex::Wallet::Status_Ok) {
        Napi::TypeError::New(env, this->wallet_->errorString().c_str()).ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Boolean::New(env, valid);

}

Napi::Value Wallet::AddressBook_GetAll(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto addressBook = this->wallet_->addressBook();
        addressBook->refresh();
        auto rows = addressBook->getAll();
        auto result = Napi::Array::New(env, rows.size());
        for (size_t i = 0; i < rows.size(); ++i) {
            const auto& row = rows[i];
            auto rowObj = makeAddressBookRowInfoObject(env, row);

            result[i] = rowObj;
        }

        return result;

}

Napi::Value Wallet::AddressBook_AddRow(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

     if (info.Length() != 3  || !info[0].IsString() || !info[1].IsString() || !info[2].IsString()) {
        Napi::TypeError::New(env, "Function accepts address, payment ID and description").ThrowAsJavaScriptException();
        return env.Null();
     }

     auto address = toStdString(info[0]);
     auto paymentID = toStdString(info[1]);
     auto description = toStdString(info[2]);


     bool valid = this->wallet_->addressBook()->addRow(address, paymentID, description);

     return Napi::Boolean::New(env, valid);

}

Napi::Value Wallet::AddressBook_DeleteRow(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

         if (info.Length() != 1 || !info[0].IsNumber() ) {
            Napi::TypeError::New(env, "Function accepts rowId").ThrowAsJavaScriptException();
            return env.Null();
         }

         uint32_t rowID = info[0].As<Napi::Number>();
         bool valid = this->wallet_->addressBook()->deleteRow(rowID);
         return Napi::Boolean::New(env, valid);

}

Napi::Value Wallet::AddressBook_ErrorString(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

        auto buf = this->wallet_->addressBook()->errorString();
        auto err = Napi::String::New(env, buf.c_str());

        return err;

}

Napi::Value Wallet::AddressBook_LookupPID(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

        if (info.Length() != 1 || !info[0].IsString() ) {
            Napi::TypeError::New(env, "Function accepts paymentID").ThrowAsJavaScriptException();
            return env.Null();
        }

         auto paymentID = toStdString(info[0]);
         auto rowId = this->wallet_->addressBook()->lookupPaymentID(paymentID);

         return Napi::Number::New(env, rowId);

}


} //namespace exawallet
