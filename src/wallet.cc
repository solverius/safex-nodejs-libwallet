#include "wallet.h"

#include <iostream>
#include <algorithm>

#include "deferredtask.h"
#include "walletargs.h"
#include "wallettasks.h"


using namespace v8;

namespace exawallet {

namespace {



std::string toStdString(const Local<Value>& val) {
    Nan::Utf8String nanStr(val);
    return std::string (*nanStr);
}

bool toVectorString(Local<Value> args, std::vector<std::string>& append) {
    Local<Array> items = Local<Array>::Cast(args);
    append.reserve(append.size() + items->Length());
    for (uint32_t i = 0; i < items->Length(); ++i) {
        auto msigInfo = items->Get(Nan::GetCurrentContext(), i).ToLocalChecked();
        if (!msigInfo->IsString()) {
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

Local<String> convertAmount(uint64_t amount) {
    return Nan::New(std::to_string(amount).c_str()).ToLocalChecked();
}

Local<Object> makeAddressBookRowInfoObject( SafexNativeAddressBookRow* row) {
    auto result = Nan::New<Object>();
    result->Set(Nan::GetCurrentContext(),
                    Nan::New("rowID").ToLocalChecked(),
                    Nan::New((uint32_t)row->getRowId())
                    );

    result->Set(Nan::GetCurrentContext(),
                        Nan::New("address").ToLocalChecked(),
                        Nan::New(row->getAddress().c_str()).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                        Nan::New("description").ToLocalChecked(),
                        Nan::New(row->getDescription().c_str()).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                        Nan::New("paymentId").ToLocalChecked(),
                        Nan::New(row->getPaymentId().c_str()).ToLocalChecked());

    return result;
}

Local<Object> makeTransactionInfoObject(const SafexNativeTransactionInfo* transaction) {

    // auto transfersNative = transaction->transfers();
    // auto transfers = Nan::New<Array>(transfersNative.size());

    // for (size_t i = 0; i < transfersNative.size(); ++i) {
    //     const auto& transfer = transfersNative[i];

    //     auto trObj = Nan::New<Object>();
    //     trObj->Set(Nan::GetCurrentContext(),
    //                Nan::New("amount").ToLocalChecked(),
    //                convertAmount(transfer.amount));

    //     trObj->Set(Nan::GetCurrentContext(),
    //                Nan::New("tokenAmount").ToLocalChecked(),
    //                convertAmount(transfer.token_amount));

    //     transfers->Set(Nan::GetCurrentContext(), i, trObj);
    // }

    auto result = Nan::New<Object>();
    // result->Set(Nan::GetCurrentContext(),
    //             Nan::New("transfers").ToLocalChecked(),
    //             transfers);


    const char* direction = transaction->direction() == Safex::TransactionInfo::Direction_In ? "in" : "out";
    result->Set(Nan::GetCurrentContext(),
                Nan::New("direction").ToLocalChecked(),
                Nan::New(direction).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                Nan::New("pending").ToLocalChecked(),
                Nan::New(transaction->isPending()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("failed").ToLocalChecked(),
                Nan::New(transaction->isFailed()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("amount").ToLocalChecked(),
                convertAmount(transaction->amount()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("tokenAmount").ToLocalChecked(),
                convertAmount(transaction->token_amount()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("fee").ToLocalChecked(),
                convertAmount(transaction->fee()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("blockHeight").ToLocalChecked(),
                Nan::New((uint32_t)transaction->blockHeight()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("subAddrAccount").ToLocalChecked(),
                Nan::New((uint32_t)transaction->subaddrAccount()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("label").ToLocalChecked(),
                Nan::New(transaction->label().c_str()).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                Nan::New("confirmations").ToLocalChecked(),
                Nan::New((uint32_t)transaction->confirmations()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("unlockTime").ToLocalChecked(),
                Nan::New((uint32_t)transaction->unlockTime()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("id").ToLocalChecked(),
                Nan::New(transaction->hash().c_str()).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                Nan::New("timestamp").ToLocalChecked(),
                Nan::New((uint32_t)transaction->timestamp()));

    result->Set(Nan::GetCurrentContext(),
                Nan::New("paymentId").ToLocalChecked(),
                Nan::New(transaction->paymentId().c_str()).ToLocalChecked());

    return result;
}

Local<Object> makeSafexAccountObject(const SafexNativeSafexAccount& safexAccount) {

    auto result = Nan::New<Object>();

    result->Set(Nan::GetCurrentContext(),
                Nan::New("username").ToLocalChecked(),
                Nan::New(safexAccount.getUsername()).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                Nan::New("data").ToLocalChecked(),
                Nan::New(safexAccount.getData()).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                Nan::New("publicKey").ToLocalChecked(),
                Nan::New(safexAccount.getPubKey()).ToLocalChecked());

    result->Set(Nan::GetCurrentContext(),
                Nan::New("privateKey").ToLocalChecked(),
                Nan::New(safexAccount.getSecKey()).ToLocalChecked());

    return result;
}

}

Nan::Persistent<v8::Function> Wallet::constructor;

Wallet::~Wallet() {
    if (wallet_) {
        delete wallet_;
    }
}

NAN_METHOD(Wallet::WalletExists) {

    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Function accepts path to wallet");
        return;
    }

    std::string path = toStdString(info[0]);
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    if (!manager) {
        Nan::ThrowTypeError("Wallet manager could not be instantiated!");
        return;
    }
    bool exists = manager->walletExists(path);
    info.GetReturnValue().Set(Nan::New(exists));
}

NAN_METHOD(Wallet::CreateWallet) {
    CreateWalletArgs walletArgs;
    std::string error = walletArgs.Init(info);
    if (!error.empty()) {
        Nan::ThrowError(error.c_str());
        return;
    }

    CreateWalletTask* task = new CreateWalletTask(walletArgs);
    auto promise = task->Enqueue();
    info.GetReturnValue().Set(promise);
}

NAN_METHOD(Wallet::CreateWalletFromKeys) {
        CreateWalletFromKeysArgs walletArgs;
        std::string error = walletArgs.Init(info);
        if (!error.empty()) {
          Nan::ThrowError(error.c_str());
          return;
        }

        CreateWalletFromKeysTask* task = new CreateWalletFromKeysTask(walletArgs);
        auto promise = task->Enqueue();
        info.GetReturnValue().Set(promise);
}

NAN_METHOD(Wallet::OpenWallet) {
    OpenWalletArgs walletArgs;
    std::string error = walletArgs.Init(info);
    if (!error.empty()) {
        Nan::ThrowError(error.c_str());
        return;
    }

    OpenWalletTask* task = new OpenWalletTask(walletArgs);
    auto promise = task->Enqueue();
    info.GetReturnValue().Set(promise);
}

NAN_METHOD(Wallet::RecoveryWallet) {
    RecoveryWalletArgs walletArgs;
    std::string error = walletArgs.Init(info);
    if (!error.empty()) {
        Nan::ThrowError(error.c_str());
        return;
    }

    RecoveryWalletTask* task = new RecoveryWalletTask(walletArgs);
    auto promise = task->Enqueue();
    info.GetReturnValue().Set(promise);
}

MaybeLocal<Function> Wallet::FindCallback(const std::string& name) {
    auto it = callbacks_.find(name);
    if (it == callbacks_.end()) {
        return MaybeLocal<Function>();
    }
    return Nan::New(it->second);
}

NAN_MODULE_INIT(Wallet::Init) {
    struct FunctionRegisterInfo {
        const char* name;
        Nan::FunctionCallback func;
    };

    static std::vector<FunctionRegisterInfo> walletFunctions = {
        {"close", Close},
        {"address", Address},
        {"seed", Seed},
        {"setSeedLanguage", SetSeedLanguage},
        {"on", On},
        {"off", Off},
        {"store", Store},
        {"path", Path},
        {"network", NetType},
        {"secretViewKey", SecretViewKey},
        {"publicViewKey", PublicViewKey},
        {"secretSpendKey", SecretSpendKey},
        {"publicSpendKey", PublicSpendKey},
        {"setPassword", SetPassword},
        {"setRefreshFromBlockHeight", SetRefreshFromBlockHeight},
        {"getRefreshFromBlockHeight", GetRefreshFromBlockHeight},
        {"rescanBlockchain", RescanBlockchain},
        {"rescanBlockchainAsync", RescanBlockchainAsync},
        {"connected", Connected},
        {"setTrustedDaemon", SetTrustedDaemon},
        {"trustedDaemon", TrustedDaemon},
        {"balance", Balance},
        {"unlockedBalance", UnlockedBalance},
        {"tokenBalance", TokenBalance},
        {"unlockedTokenBalance", UnlockedTokenBalance},
        {"blockchainHeight", BlockChainHeight},
        {"daemonBlockchainHeight", DaemonBlockChainHeight},
        {"synchronized", Synchronized},
        {"defaultMixin", DefaultMixin},
        {"setDefaultMixin", SetDefaultMixin},
        {"startRefresh", StartRefresh},
        {"pauseRefresh", PauseRefresh},
        {"createTransaction", CreateTransaction},
        {"createSafexAccount", CreateSafexAccount},
        {"getSafexAccounts", GetSafexAccounts},
        {"signMessage", SignMessage},
        {"verifySignedMessage", VerifySignedMessage},
        {"history", TransactionHistory},
        {"addressBook_GetAll", AddressBook_GetAll},
        {"addressBook_AddRow", AddressBook_AddRow},
        {"addressBook_DeleteRow", AddressBook_DeleteRow},
        {"addressBook_ErrorString", AddressBook_ErrorString},
        {"addressBook_LookupPID", AddressBook_LookupPID}
    };

    std::cout << "Wallet::Init" << std::endl;

    auto tpl = Nan::New<FunctionTemplate>(Wallet::New);
    tpl->SetClassName(Nan::New("Wallet").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(walletFunctions.size());

    for (const auto& info: walletFunctions) {
        Nan::SetPrototypeMethod(tpl, info.name, info.func);
    }
    constructor.Reset(tpl->GetFunction(Nan::GetCurrentContext()).ToLocalChecked());
}

v8::Local<v8::Object> Wallet::NewInstance(SafexNativeWallet *wallet) {
    const unsigned argc = 0;
    Local<Value> argv[1] = { Nan::Null() };
    Local<Function> cons = Nan::New(constructor);
    Local<Context> context = Nan::GetCurrentContext();
    Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();

    Wallet* w = new Wallet(wallet);
    wallet->setListener(w);
    //TODO: make it configurable
    wallet->segregatePreForkOutputs(false);
    wallet->keyReuseMitigation2(false);

    w->Wrap(instance);
    return instance;
}

NAN_METHOD(Wallet::New) {

  if (info.IsConstructCall()) {
    Wallet* obj = new Wallet(nullptr);
    obj->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  } else {
    const int argc = 0;
    Local<Value> argv[1] = { Nan::Null()};
    Local<Function> cons = Nan::New(constructor);
    Local<Context> context = Nan::GetCurrentContext();

    Local<Object> instance = cons->NewInstance(context, argc, argv).ToLocalChecked();
    info.GetReturnValue().Set(instance);
  }
}

NAN_METHOD(Wallet::Close)  {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    if (info.Length() > 1 && !info[0]->IsBoolean()) {
        Nan::ThrowError("Function accepts one optional boolean argument");
        return;
    }
    bool store = false;
         if (info.Length() != 0) {
             store = Nan::To<bool>(info[0]).FromJust();
         }

    CloseWalletTask* task = new CloseWalletTask(obj->wallet_, store);
    auto promise = task->Enqueue();
    info.GetReturnValue().Set(promise);
}

NAN_METHOD(Wallet::Address) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    auto buf = obj->wallet_->address();
    auto addr = Nan::New(buf.c_str()).ToLocalChecked();

    info.GetReturnValue().Set(addr);
}

NAN_METHOD(Wallet::Seed) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    auto buf = obj->wallet_->seed();
    auto seed = Nan::New(buf.c_str()).ToLocalChecked();

    info.GetReturnValue().Set(seed);
}

NAN_METHOD(Wallet::SetSeedLanguage) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    if (info.Length() != 1 || !info[0]->IsString()) {
            Nan::ThrowTypeError("String argument is required");
            return;
        }

    obj->wallet_->setSeedLanguage(toStdString(Nan::To<v8::String>(info[0]).ToLocalChecked()));

    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Wallet::On) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    if (info.Length() != 2) {
        Nan::ThrowError("2 arguments are required");
        return;
    }

    if (!info[0]->IsString() || !info[1]->IsFunction()) {

        Nan::ThrowTypeError("Function accepts string and function arguments");
        return;
    }

    obj->callbacks_[toStdString(info[0])] = CopyablePersistentFunction(info.GetIsolate(), Local<Function>::Cast(info[1]));
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Wallet::Off) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    //delete all listeners
    if (info.Length() == 0) {
        obj->callbacks_.clear();
        info.GetReturnValue().Set(info.Holder());
        return;
    }

    if (info.Length() != 1 || !info[0]->IsString()) {

        Nan::ThrowTypeError("Function accepts no arguments or event name");
        return;
    }

    obj->callbacks_.erase(toStdString(info[0]));
    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Wallet::Store) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    StoreWalletTask* task = new StoreWalletTask(obj->wallet_);
    auto promise = task->Enqueue();
    info.GetReturnValue().Set(promise);
}

NAN_METHOD(Wallet::Path) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->path().c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::NetType) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    std::string nettype;
    if (!convertNettype(obj->wallet_->nettype(), nettype)) {
        assert(0);
    }

    info.GetReturnValue().Set(Nan::New(nettype.c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::SecretViewKey) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->secretViewKey().c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::PublicViewKey) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->publicViewKey().c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::SecretSpendKey) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->secretSpendKey().c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::PublicSpendKey) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->publicSpendKey().c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::SetPassword) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("String argument is required");
        return;
    }

    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    if (!obj->wallet_->setPassword(toStdString(Nan::To<v8::String>(info[0]).ToLocalChecked()))) {
        Nan::ThrowError(obj->wallet_->errorString().c_str());
        return;
    }

    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Wallet::SetRefreshFromBlockHeight) {
    if (info.Length() != 1 || !info[0]->IsInt32()) {
        Nan::ThrowTypeError("Integer argument is required");
        return;
    }

    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    obj->wallet_->setRefreshFromBlockHeight(info[0]->Uint32Value(Nan::GetCurrentContext()).ToChecked());

    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Wallet::GetRefreshFromBlockHeight) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New((uint32_t)obj->wallet_->getRefreshFromBlockHeight()));
}

NAN_METHOD(Wallet::Connected) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    std::string status;
    switch (obj->wallet_->connected()) {
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
        assert(0);
        break;
    }

    info.GetReturnValue().Set(Nan::New(status.c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::SetTrustedDaemon) {
    if (info.Length() != 1 || !info[0]->IsBoolean()) {
        Nan::ThrowTypeError("Bool argument is required");
        return;
    }

    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    obj->wallet_->setTrustedDaemon(Nan::To<bool>(info[0]).FromJust());

    info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Wallet::TrustedDaemon) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->trustedDaemon()));
}

NAN_METHOD(Wallet::Balance) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    // it seems v8 doesn't have uint64
    info.GetReturnValue().Set(Nan::New(std::to_string(obj->wallet_->balanceAll()).c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::UnlockedBalance) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(std::to_string(obj->wallet_->unlockedBalanceAll()).c_str()).ToLocalChecked());
}

  NAN_METHOD(Wallet::TokenBalance) {
          Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

          // it seems v8 doesn't have uint64
          info.GetReturnValue().Set(Nan::New(std::to_string(obj->wallet_->tokenBalanceAll()).c_str()).ToLocalChecked());
  }

  NAN_METHOD(Wallet::UnlockedTokenBalance) {
          Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

          info.GetReturnValue().Set(Nan::New(std::to_string(obj->wallet_->unlockedTokenBalanceAll()).c_str()).ToLocalChecked());
  }

NAN_METHOD(Wallet::BlockChainHeight) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    info.GetReturnValue().Set(Nan::New((uint32_t)obj->wallet_->blockChainHeight()));
}

NAN_METHOD(Wallet::DaemonBlockChainHeight) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New((uint32_t)obj->wallet_->daemonBlockChainHeight()));
}

NAN_METHOD(Wallet::Synchronized) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->synchronized()));
}

NAN_METHOD(Wallet::GenPaymentId) {
    info.GetReturnValue().Set(Nan::New(SafexNativeWallet::genPaymentId().c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::PaymentIdValid) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("String argument is required");
        return;
    }

    info.GetReturnValue().Set(Nan::New(SafexNativeWallet::paymentIdValid(toStdString(info[0]))));
}

NAN_METHOD(Wallet::AddressValid) {
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsString()) {
        Nan::ThrowTypeError("2 string arguments are required");
        return;
    }

    Safex::NetworkType nettype;
    if (!getNettype(toStdString(info[1]), nettype)) {
        Nan::ThrowError("wrong network type argument");
        return;
    }
    bool valid = SafexNativeWallet::addressValid(toStdString(info[0]), nettype);
    info.GetReturnValue().Set(Nan::New(valid));
}

NAN_METHOD(Wallet::RescanBlockchain) {
        Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
        obj->wallet_->setRefreshFromBlockHeight(0);
        obj->wallet_->rescanBlockchain();
}

NAN_METHOD(Wallet::RescanBlockchainAsync) {
        Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
        obj->wallet_->setRefreshFromBlockHeight(0);
        obj->wallet_->rescanBlockchainAsync();
}


NAN_METHOD(Wallet::DefaultMixin) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    info.GetReturnValue().Set(Nan::New(obj->wallet_->defaultMixin()));
}

NAN_METHOD(Wallet::SetDefaultMixin) {
    if (info.Length() != 1 || !info[0]->IsInt32()) {
        Nan::ThrowTypeError("Integer argument is required");
        return;
    }

    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    obj->wallet_->setDefaultMixin(Nan::To<v8::Int32>(info[0]).ToLocalChecked()->Value());
}

NAN_METHOD(Wallet::StartRefresh) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    obj->wallet_->startRefresh();
}

NAN_METHOD(Wallet::PauseRefresh) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    obj->wallet_->pauseRefresh();
}

NAN_METHOD(Wallet::TransactionHistory) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    auto history = obj->wallet_->history();
    history->refresh();
    auto transactions = history->getAll();
    auto result = Nan::New<Array>(transactions.size());
    for (size_t i = 0; i < transactions.size(); ++i) {
        const auto& transaction = transactions[i];

        if(transaction == nullptr) continue;
        
        auto txObj = makeTransactionInfoObject(transaction);

        if (result->Set(Nan::GetCurrentContext(), i, txObj).IsNothing()) {
            Nan::ThrowError("Couldn't make transaction info list: unknown error");
            return;
        }
    }

    info.GetReturnValue().Set(result);
}

NAN_METHOD(Wallet::CreateTransaction) {
    CreateTransactionArgs txArgs;
    std::string error = txArgs.Init(info);
    if (!error.empty()) {
        Nan::ThrowError(error.c_str());
        return;
    }
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    CreateTransactionTask* task = new CreateTransactionTask(txArgs, obj->wallet_);
    auto promise = task->Enqueue();
    info.GetReturnValue().Set(promise);
}

NAN_METHOD(Wallet::CreateSafexAccount) {

    if (info.Length() != 3 || !info[0]->IsString() || !info[1]->IsString() || !info[2]->IsString()) {
        Nan::ThrowTypeError("Function accepts string argument");
        return;
    }

    auto username = toStdString(info[0]);
    auto desc = toStdString(info[1]);
    auto password = toStdString(info[2]);

    auto description = std::vector<uint8_t>(desc.begin(),desc.end());

     Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

     auto res = obj->wallet_->createSafexAccount(username, description, password);


    info.GetReturnValue().Set(Nan::New(res));

}

NAN_METHOD(Wallet::GetSafexAccounts) {

    if (info.Length() != 0) {
        Nan::ThrowTypeError("Function accepts no arguments");
        return;
    }

     Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    auto safexAccounts = obj->wallet_->getSafexAccounts();
    auto result = Nan::New<Array>(safexAccounts.size());

    for (size_t i = 0; i < safexAccounts.size(); ++i) {
            const auto& safexAccount = safexAccounts[i];

            auto safexAccountObj = makeSafexAccountObject(safexAccount);

            if (result->Set(Nan::GetCurrentContext(), i, safexAccountObj).IsNothing()) {
                Nan::ThrowError("Couldn't make safexAccount info list: unknown error");
                return;
            }
        }


    info.GetReturnValue().Set(result);

}


NAN_METHOD(Wallet::SignMessage) {

    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Function accepts string argument");
        return;
    }

    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    auto signature = obj->wallet_->signMessage(toStdString(info[0]));
    if (obj->wallet_->status() != Safex::Wallet::Status_Ok) {
        Nan::ThrowTypeError(obj->wallet_->errorString().c_str());
        return;
    }

    info.GetReturnValue().Set(Nan::New(signature.c_str()).ToLocalChecked());
}

NAN_METHOD(Wallet::VerifySignedMessage) {

    if (info.Length() != 3 || !info[0]->IsString() || !info[1]->IsString() || !info[2]->IsString()) {
        Nan::ThrowTypeError("Function accepts message, monero address and signature as string arguments");
        return;
    }

    auto message = toStdString(info[0]);
    auto address = toStdString(info[1]);
    auto signature = toStdString(info[2]);

    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    bool valid = obj->wallet_->verifySignedMessage(message, address, signature);

    if (obj->wallet_->status() != Safex::Wallet::Status_Ok) {
        Nan::ThrowTypeError(obj->wallet_->errorString().c_str());
        return;
    }

    info.GetReturnValue().Set(valid);
}

NAN_METHOD(Wallet::AddressBook_GetAll) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
    auto addressBook = obj->wallet_->addressBook();
    addressBook->refresh();
    auto rows = addressBook->getAll();
    auto result = Nan::New<Array>(rows.size());
    for (size_t i = 0; i < rows.size(); ++i) {
        const auto& row = rows[i];
        auto rowObj = makeAddressBookRowInfoObject(row);

        if (result->Set(Nan::GetCurrentContext(), i, rowObj).IsNothing()) {
            Nan::ThrowError("Couldn't make row info list: unknown error");
            return;
        }
    }

    info.GetReturnValue().Set(result);
}

NAN_METHOD(Wallet::AddressBook_AddRow) {
     if (info.Length() != 3  || !info[0]->IsString() || !info[1]->IsString() || !info[2]->IsString()) {
        Nan::ThrowTypeError("Function accepts address, payment ID and description");
        return;
     }

     auto address = toStdString(info[0]);
     auto paymentID = toStdString(info[1]);
     auto description = toStdString(info[2]);

     Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

     bool valid = obj->wallet_->addressBook()->addRow(address, paymentID, description);

     info.GetReturnValue().Set(valid);
}

NAN_METHOD(Wallet::AddressBook_DeleteRow) {
     if (info.Length() != 1 || !info[0]->IsInt32() ) {
        Nan::ThrowTypeError("Function accepts rowId");
        return;
     }

     auto rowID = Nan::To<v8::Int32>(info[0]).ToLocalChecked()->Value();
     Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
     bool valid = obj->wallet_->addressBook()->deleteRow(rowID);
     info.GetReturnValue().Set(valid);
}

NAN_METHOD(Wallet::AddressBook_ErrorString) {
    Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());

    auto buf = obj->wallet_->addressBook()->errorString();
    auto err = Nan::New(buf.c_str()).ToLocalChecked();

    info.GetReturnValue().Set(err);
}

NAN_METHOD(Wallet::AddressBook_LookupPID) {
    if (info.Length() != 1 || !info[0]->IsString() ) {
        Nan::ThrowTypeError("Function accepts paymentID");
        return;
    }

     auto paymentID = toStdString(info[0]);
     Wallet* obj = ObjectWrap::Unwrap<Wallet>(info.Holder());
     auto rowId = obj->wallet_->addressBook()->lookupPaymentID(paymentID);

    info.GetReturnValue().Set(rowId);
}


} //namespace exawallet
