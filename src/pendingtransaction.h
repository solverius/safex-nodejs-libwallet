#pragma once

#include <napi.h>

#include "safexnativewallet.h"

namespace exawallet {

class PendingTransaction : public Napi::ObjectWrap<PendingTransaction> {
public:

    explicit PendingTransaction(SafexNativePendingTransaction* tx, const Napi::CallbackInfo& info): Napi::ObjectWrap<PendingTransaction>(info), transaction(tx) {}
    explicit PendingTransaction(const Napi::CallbackInfo& info): transaction(nullptr), Napi::ObjectWrap<PendingTransaction>(info) {}

    ~PendingTransaction();

    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, SafexNativePendingTransaction* tx);

private:

     void Commit(const Napi::CallbackInfo& info);
     Napi::Value Amount(const Napi::CallbackInfo& info);
     Napi::Value Dust(const Napi::CallbackInfo& info);
     Napi::Value Fee(const Napi::CallbackInfo& info);
     Napi::Value TransactionsIds(const Napi::CallbackInfo& info);
     Napi::Value TransactionsCount(const Napi::CallbackInfo& info);

     static Napi::FunctionReference constructor;

     SafexNativePendingTransaction* transaction;
};

}
