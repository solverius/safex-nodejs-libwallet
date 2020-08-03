#include "pendingtransaction.h"

#include <cmath>

#include <wallet_api.h>
#include "wallettasks.h"

using namespace Napi;

namespace exawallet {

Napi::FunctionReference PendingTransaction::constructor;

PendingTransaction::~PendingTransaction() {
    if (transaction) {
        delete transaction;
    }
}

Napi::Object PendingTransaction::Init(Napi::Env env, Napi::Object exports) {

    Napi::HandleScope scope(env);

   Napi::Function func = DefineClass(env, "PendingTransaction",
     {InstanceMethod("commit", &PendingTransaction::Commit),
     InstanceMethod("amount", &PendingTransaction::Amount),
     InstanceMethod("dust", &PendingTransaction::Dust),
     InstanceMethod("fee", &PendingTransaction::Fee),
     InstanceMethod("transactionsIds", &PendingTransaction::TransactionsIds)});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("PendingTransaction", func);

  return exports;
}


Napi::Object PendingTransaction::NewInstance(Napi::Env env, SafexNativePendingTransaction* tx){

    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor.New({});

    PendingTransaction* pt = Napi::ObjectWrap<PendingTransaction>::Unwrap(obj);

    pt->transaction = tx;

    return scope.Escape(napi_value(obj)).ToObject();
}


void PendingTransaction::Commit(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();

    if (!info[0].IsFunction()) {

        Napi::TypeError::New(env, "Function accepts function(callback) argument").ThrowAsJavaScriptException();
        return;
    }

  Function callback = info[0].As<Function>();

  CommitTransactionTask* task = new CommitTransactionTask(this->transaction, callback);
  task->Queue();

  return;
}

Napi::Value PendingTransaction::Amount(const Napi::CallbackInfo& info) {

    Napi::Env env = info.Env();
    auto amount = std::to_string(this->transaction->amount());

    return Napi::String::New(env, amount.c_str());
}

Napi::Value PendingTransaction::Dust(const Napi::CallbackInfo& info) {

     Napi::Env env = info.Env();
     auto dust = std::to_string(this->transaction->dust());

     return Napi::String::New(env, dust.c_str());
}

Napi::Value PendingTransaction::Fee(const Napi::CallbackInfo& info) {

     Napi::Env env = info.Env();
     auto fee = std::to_string(this->transaction->fee());

     return Napi::String::New(env, fee.c_str());
}

Napi::Value PendingTransaction::TransactionsIds(const Napi::CallbackInfo& info) {

     Napi::Env env = info.Env();
     auto transactions = this->transaction->txid();
     auto result = Napi::Array::New(env, transactions.size());

     for (size_t i = 0; i < transactions.size(); ++i) {
         auto id = Napi::String::New(env, transactions[i].c_str());
         result[i] = id;
     }

     return result;
}

Napi::Value PendingTransaction::TransactionsCount(const Napi::CallbackInfo& info) {

     Napi::Env env = info.Env();
     return Napi::Number::New(env, (uint32_t)this->transaction->txCount());
}



}
