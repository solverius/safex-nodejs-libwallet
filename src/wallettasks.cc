#include "wallettasks.h"

#include "wallet.h"
#include "pendingtransaction.h"


#include "safexnativewallet.h"


namespace exawallet {

using namespace v8;

std::string CreateWalletTask::doWork() {
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    if (manager->walletExists(args_.path)) {
        return "Wallet already exists: " + args_.path;
    }

    wallet_ = manager->createWallet(args_.path, args_.password, args_.language, args_.nettype);

    if (!wallet_) {
        return "WalletManager returned null wallet pointer";
    }

    if (!wallet_->errorString().empty()) {
        return wallet_->errorString();
    }

    if (!wallet_->init(args_.daemonAddress)) {
        return "Couldn't init wallet";
    }

    wallet_->setTrustedDaemon(true);
    wallet_->startRefresh();
    return {};
}

Local<Value> CreateWalletTask::afterWork(std::string& error) {
    return Wallet::NewInstance(wallet_);
}

std::string CreateWalletFromKeysTask::doWork() {
  auto manager = SafexNativeWalletManagerFactory::getWalletManager();
  if (manager->walletExists(args_.path)) {
    return "Wallet already exists: " + args_.path;
  }

  wallet_ = manager->createWalletFromKeys(args_.path, args_.password, args_.language, args_.nettype, args_.restoreHeight, args_.addressString, args_.viewKeyString, args_.spendKeyString);


  if (!wallet_) {
    return "WalletManager returned null wallet pointer";
  }

  if (!wallet_->errorString().empty()) {
    return wallet_->errorString();
  }

  if (!wallet_->init(args_.daemonAddress)) {
    return "Couldn't init wallet";
  }

  wallet_->setTrustedDaemon(true);
  wallet_->startRefresh();
  return {};
}

Local<Value> CreateWalletFromKeysTask::afterWork(std::string& error) {
  return Wallet::NewInstance(wallet_);
}

std::string OpenWalletTask::doWork() {
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    if (!manager->walletExists(args_.path)) {
        return "wallet does not exist: " + args_.path;
    }

    wallet_ = manager->openWallet(args_.path, args_.password, args_.nettype);

    if (!wallet_) {
        return "WalletManager returned null wallet pointer";
    }

    if (!wallet_->errorString().empty()) {
        return wallet_->errorString();
    }

    if (!wallet_->init(args_.daemonAddress)) {
        return "Couldn't init wallet";
    }

    //set refresh height as latest block wallet has seen - 1 day
    uint64_t currentHeight = wallet_->blockChainHeight();
    wallet_->setRefreshFromBlockHeight(currentHeight - 720 > 0 ? currentHeight - 720 : 0);

    wallet_->setTrustedDaemon(true);
    wallet_->startRefresh();
    return {};
}

Local<Value> OpenWalletTask::afterWork(std::string& error) {
    return Wallet::NewInstance(wallet_);
}

std::string CloseWalletTask::doWork() {
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    manager->closeWallet(wallet_, store_);
    return {};
}

Local<Value> CloseWalletTask::afterWork(std::string& error) {
    return Nan::Undefined();
}

std::string RecoveryWalletTask::doWork() {
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();

    wallet_ = manager->recoveryWallet(args_.path,
                                       args_.password,
                                       args_.mnemonic,
                                       args_.nettype,
                                       args_.restoreHeight);

    if (!wallet_) {
        return "WalletManager returned null wallet pointer";
    }

    if (!wallet_->errorString().empty()) {
        return wallet_->errorString();
    }
    
    if (!wallet_->init(args_.daemonAddress)) {
        return "Couldn't init wallet";
    }

    wallet_->setTrustedDaemon(true);
    wallet_->startRefresh();
    return {};
}

Local<Value> RecoveryWalletTask::afterWork(std::string& error) {
    return Wallet::NewInstance(wallet_);
}

std::string StoreWalletTask::doWork() {
    if (!wallet_->store(wallet_->path())) {
        std::cout << "Error storing wallet path:" << wallet_->path() << std::endl;
        return "Couldn't store wallet";
    }

    return {};
}
Local<Value> StoreWalletTask::afterWork(std::string& error) {
    return Nan::Undefined();
}

std::string CreateTransactionTask::doWork() {
    //std::cout << "CreateTransactionTask::doWork amount=" <<  args_.amount << " tx_type=" << static_cast<int>(args_.tx_type) << std::endl;

    transaction_ = wallet_->createTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, args_.tx_type);
    if (!wallet_->errorString().empty()) {
        return wallet_->errorString();
    }

    return {};
}

Local<Value> CreateTransactionTask::afterWork(std::string& error) {
    return PendingTransaction::NewInstance(transaction_);
}

std::string CreateAdvancedTransactionTask::doWork() {
    //std::cout << "CreateTransactionTask::doWork amount=" <<  args_.amount << " tx_type=" << static_cast<int>(args_.tx_type) << std::endl;

    if(args_.tx_type==Safex::TransactionType::CreateAccountTransaction){
        Safex::CreateAccountCommand s{args_.safexUsername};

        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);
            if (!wallet_->errorString().empty()) {
                return wallet_->errorString();
            }
    } else if(args_.tx_type==Safex::TransactionType::EditAccountTransaction){
       Safex::EditAccountCommand s{args_.safexUsername, args_.safexData};

       transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);
           if (!wallet_->errorString().empty()) {
               return wallet_->errorString();
           }
    } else
        return "Bad tx type given";

    return {};
}

Local<Value> CreateAdvancedTransactionTask::afterWork(std::string& error) {
    return PendingTransaction::NewInstance(transaction_);
}

std::string CommitTransactionTask::doWork() {
    if (!transaction_->commit()) {
        return "Couldn't commit transaction: " + transaction_->errorString();
    }

    return {};
}

Local<Value> CommitTransactionTask::afterWork(std::string& error) {
    return Nan::Undefined();
}

}
