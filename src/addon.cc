#include <napi.h>
#include <uv.h>
#include "wallet.h"
#include "pendingtransaction.h"

extern void mlog_configure(const std::string &filename_base, bool console, const std::size_t max_log_file_size = 104850000);
extern void mlog_set_log(const char *log);


namespace exawallet {

Napi::Value SetupLog(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (info.Length() == 0 || !info[0].IsNumber()) {
        Napi::Error::New(env, "Log level arument must be 0 - 4 integer").ThrowAsJavaScriptException();
        return env.Null();
    }

    const char* configureString = nullptr;
    auto level = info[0].As<Napi::Number>();
    switch (level.ToNumber().Uint32Value()) {
    case 0:
        configureString = "0";
        break;
    case 1:
        configureString = "1";
        break;
    case 2:
        configureString = "2";
        break;
    case 3:
        configureString = "3";
        break;
    case 4:
        configureString = "4";
        break;
    default:
        Napi::Error::New(env, "Log level arument must be 0 - 4 integer").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (info.Length() == 1) {
        mlog_configure("", true);
        mlog_set_log(configureString);
        return Napi::Value();
    }


    if (!info[1].IsString()) {
        Napi::Error::New(env, "Filename argument is expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string filename = info[1].As<Napi::String>();
    mlog_configure(filename, false);
    mlog_set_log(configureString);
}



Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Wallet::Init(env, exports);
    PendingTransaction::Init(env, exports);


    //Safex libwallet generates too much logs in stdout by default
    mlog_set_log("0");
    mlog_configure("", false);

    exports.Set(Napi::String::New(env, "createWallet"), Napi::Function::New(env, Wallet::CreateWallet));
    exports.Set(Napi::String::New(env, "createWalletFromKeys"), Napi::Function::New(env, Wallet::CreateWalletFromKeys));
    exports.Set(Napi::String::New(env, "walletExists"), Napi::Function::New(env, Wallet::WalletExists));
    exports.Set(Napi::String::New(env, "openWallet"), Napi::Function::New(env, Wallet::OpenWallet));
    exports.Set(Napi::String::New(env, "recoveryWallet"), Napi::Function::New(env, Wallet::RecoveryWallet));
    exports.Set(Napi::String::New(env, "genPaymentId"), Napi::Function::New(env, Wallet::GenPaymentId));
    exports.Set(Napi::String::New(env, "paymentIdValid"), Napi::Function::New(env, Wallet::PaymentIdValid));
    exports.Set(Napi::String::New(env, "addressValid"), Napi::Function::New(env, Wallet::AddressValid));
    exports.Set(Napi::String::New(env, "setupLog"), Napi::Function::New(env, SetupLog));

     return exports;
}


NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)

} //namespace exawallet
