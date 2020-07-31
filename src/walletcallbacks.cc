#include "wallet.h"


struct TransactionAmount {
    std::string tx;
    std::string amount;
};

namespace exawallet {

void Wallet::newBlock(uint64_t height) {

    auto it = callbacks_.find("newBlock");
    if (it == callbacks_.end()) {
        return;
    }

    uint64_t* h = new uint64_t(height);

    auto callback = []( Napi::Env env, Napi::Function jsCallback, uint64_t* height ) {
        uint64_t h = *height;
        delete height;
        jsCallback.Call( {Napi::String::New(env, std::to_string(h).c_str())} );
    };

    it->second.NonBlockingCall(h, callback);
    return;

}

void Wallet::refreshed() {

    auto it = callbacks_.find("refreshed");
    if (it == callbacks_.end()) {
        return;
    }

    it->second.NonBlockingCall();
    return;
}

void Wallet::moneySpent(const std::string& tx, uint64_t amount) {

    auto it = callbacks_.find("moneySpent");
    if (it == callbacks_.end()) {
        return;
    }

    TransactionAmount* ta = new TransactionAmount{tx,std::to_string(amount)};

    auto callback = []( Napi::Env env, Napi::Function jsCallback, TransactionAmount* tm ) {
        TransactionAmount ta = *tm;
        delete tm;

        Napi::Object obj = Napi::Object::New(env);

        obj.Set("tx", ta.tx);
        obj.Set("amount", ta.amount);

        jsCallback.Call( {obj} );
    };

    it->second.NonBlockingCall(ta, callback);
    return;
}

void Wallet::moneyReceived(const std::string& tx, uint64_t amount) {
    auto it = callbacks_.find("moneyReceived");
    if (it == callbacks_.end()) {
        return;
    }

    TransactionAmount* ta = new TransactionAmount{tx,std::to_string(amount)};

    auto callback = []( Napi::Env env, Napi::Function jsCallback, TransactionAmount* tm ) {
        TransactionAmount ta = *tm;
        delete tm;

        Napi::Object obj = Napi::Object::New(env);

        obj.Set("tx", ta.tx);
        obj.Set("amount", ta.amount);

        jsCallback.Call( {obj} );
    };

    it->second.NonBlockingCall(ta, callback);
    return;
}

void Wallet::unconfirmedMoneyReceived(const std::string& tx, uint64_t amount) {
    auto it = callbacks_.find("unconfirmedMoneyReceived");
    if (it == callbacks_.end()) {
        return;
    }

    TransactionAmount* ta = new TransactionAmount{tx,std::to_string(amount)};

    auto callback = []( Napi::Env env, Napi::Function jsCallback, TransactionAmount* tm ) {
        TransactionAmount ta = *tm;
        delete tm;

        Napi::Object obj = Napi::Object::New(env);

        obj.Set("tx", ta.tx);
        obj.Set("amount", ta.amount);

        jsCallback.Call( {obj} );
    };

    it->second.NonBlockingCall(ta, callback);
    return;
}

void Wallet::tokensSpent(const std::string& tx, uint64_t token_amount) {
    auto it = callbacks_.find("tokensSpent");
    if (it == callbacks_.end()) {
        return;
    }

    TransactionAmount* ta = new TransactionAmount{tx,std::to_string(token_amount)};

    auto callback = []( Napi::Env env, Napi::Function jsCallback, TransactionAmount* tm ) {
        TransactionAmount ta = *tm;
        delete tm;

        Napi::Object obj = Napi::Object::New(env);

        obj.Set("tx", ta.tx);
        obj.Set("amount", ta.amount);

        jsCallback.Call( {obj} );
    };

    it->second.NonBlockingCall(ta, callback);
    return;
}

void Wallet::tokensReceived(const std::string &tx, uint64_t token_amount) {
    auto it = callbacks_.find("tokensReceived");
    if (it == callbacks_.end()) {
        return;
    }

    TransactionAmount* ta = new TransactionAmount{tx,std::to_string(token_amount)};

    auto callback = []( Napi::Env env, Napi::Function jsCallback, TransactionAmount* tm ) {
        TransactionAmount ta = *tm;
        delete tm;

        Napi::Object obj = Napi::Object::New(env);

        obj.Set("tx", ta.tx);
        obj.Set("amount", ta.amount);

        jsCallback.Call( {obj} );
    };

    it->second.NonBlockingCall(ta, callback);
    return;
}

void Wallet::unconfirmedTokensReceived(const std::string &tx, uint64_t token_amount) {
    auto it = callbacks_.find("unconfirmedTokensReceived");
    if (it == callbacks_.end()) {
        return;
    }

    TransactionAmount* ta = new TransactionAmount{tx,std::to_string(token_amount)};

    auto callback = []( Napi::Env env, Napi::Function jsCallback, TransactionAmount* tm ) {
        TransactionAmount ta = *tm;
        delete tm;

        Napi::Object obj = Napi::Object::New(env);

        obj.Set("tx", ta.tx);
        obj.Set("amount", ta.amount);

        jsCallback.Call( {obj} );
    };

    it->second.NonBlockingCall(ta, callback);
    return;
}

void Wallet::advancedReceived(const std::string &txId, const uint8_t output_type) {

    auto it = callbacks_.find("advancedReceived");
    if (it == callbacks_.end()) {
        return;
    }

    TransactionAmount* ta = new TransactionAmount{txId,std::to_string(output_type)};

    auto callback = []( Napi::Env env, Napi::Function jsCallback, TransactionAmount* tm ) {
        TransactionAmount ta = *tm;
        delete tm;

        Napi::Object obj = Napi::Object::New(env);

        obj.Set("tx", ta.tx);
        obj.Set("amount", ta.amount);

        jsCallback.Call( {obj} );
    };

    it->second.NonBlockingCall(ta, callback);
    return;
}

void Wallet::updated() {

    auto it = callbacks_.find("updated");
    if (it == callbacks_.end()) {
        return;
    }

    it->second.NonBlockingCall();
    return;
}

} //namespace exawallet
