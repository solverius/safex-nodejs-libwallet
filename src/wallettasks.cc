#include "wallettasks.h"

#include "wallet.h"
#include "pendingtransaction.h"


#include "safexnativewallet.h"


namespace exawallet {

using namespace Napi;

void CreateWalletTask::Execute() {

    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    if (manager->walletExists(args_.path)) {
        SetError("Wallet already exists: " + args_.path);
    }

    wallet_ = manager->createWallet(args_.path, args_.password, args_.language, args_.nettype);

    if (!wallet_) {
        SetError( "WalletManager returned null wallet pointer");
    }

    if (!wallet_->errorString().empty()) {
        SetError( wallet_->errorString());
    }

    if (!wallet_->init(args_.daemonAddress)) {
        SetError("Couldn't init wallet");
    }

    wallet_->setTrustedDaemon(true);
    wallet_->startRefresh();

return;
}

void CreateWalletTask::OnOK() {
    Callback().Call({Env().Null(), Wallet::NewInstance(Env(),wallet_)});
    return;
}

void CreateWalletFromKeysTask::Execute() {
  auto manager = SafexNativeWalletManagerFactory::getWalletManager();
  if (manager->walletExists(args_.path)) {
      SetError( "Wallet already exists: " + args_.path);
  }

  wallet_ = manager->createWalletFromKeys(args_.path, args_.password, args_.language, args_.nettype, args_.restoreHeight, args_.addressString, args_.viewKeyString, args_.spendKeyString);


  if (!wallet_) {
      SetError( "WalletManager returned null wallet pointer");
  }

  if (!wallet_->errorString().empty()) {
      SetError( wallet_->errorString());
  }

  if (!wallet_->init(args_.daemonAddress)) {
      SetError( "Couldn't init wallet");
  }

  wallet_->setTrustedDaemon(true);
  wallet_->startRefresh();

return;
}

void CreateWalletFromKeysTask::OnOK() {
    Callback().Call({Env().Null(), Wallet::NewInstance(Env(),wallet_)});
    return;
}

void OpenWalletTask::Execute() {
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    if (!manager->walletExists(args_.path)) {
        SetError( "wallet does not exist: " + args_.path);
    }

    wallet_ = manager->openWallet(args_.path, args_.password, args_.nettype);

    if (!wallet_) {
        SetError( "WalletManager returned null wallet pointer");
    }

    if (!wallet_->errorString().empty()) {
        SetError( wallet_->errorString());
    }

    if (!wallet_->init(args_.daemonAddress)) {
        SetError( "Couldn't init wallet");
    }

    //set refresh height as latest block wallet has seen - 1 day
    uint64_t currentHeight = wallet_->blockChainHeight();
    wallet_->setRefreshFromBlockHeight(currentHeight - 720 > 0 ? currentHeight - 720 : 0);

    wallet_->setTrustedDaemon(true);
    wallet_->startRefresh();
return;
}

void OpenWalletTask::OnOK() {
    Callback().Call({Env().Null(), Wallet::NewInstance(Env(),wallet_)});
    return;
}

void CloseWalletTask::Execute() {
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();
    manager->closeWallet(wallet_, store_);
    return;
}

void RecoveryWalletTask::Execute() {
    auto manager = SafexNativeWalletManagerFactory::getWalletManager();

    wallet_ = manager->recoveryWallet(args_.path,
                                       args_.password,
                                       args_.mnemonic,
                                       args_.nettype,
                                       args_.restoreHeight);

    if (!wallet_) {
        SetError( "WalletManager returned null wallet pointer");
    }

    if (!wallet_->errorString().empty()) {
        SetError( wallet_->errorString());
    }
    
    if (!wallet_->init(args_.daemonAddress)) {
        SetError( "Couldn't init wallet");
    }

    wallet_->setTrustedDaemon(true);
    wallet_->startRefresh();
}

void RecoveryWalletTask::OnOK() {
    Callback().Call({Env().Null(), Wallet::NewInstance(Env(),wallet_)});
    return;
}


void StoreWalletTask::Execute() {
    if (!wallet_->store(wallet_->path())) {
        std::cout << "Error storing wallet path:" << wallet_->path() << std::endl;
        SetError( "Couldn't store wallet");
    }

    return;
}


void CreateTransactionTask::Execute() {
    //std::cout << "CreateTransactionTask::doWork amount=" <<  args_.amount << " tx_type=" << static_cast<int>(args_.tx_type) << std::endl;

    transaction_ = wallet_->createTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, args_.tx_type);
    if (!wallet_->errorString().empty()) {
        SetError( wallet_->errorString());
    }

    return;
}

void CreateTransactionTask::OnOK() {
    Callback().Call({Env().Null(), PendingTransaction::NewInstance(Env(), transaction_)});
    return;
}

void CreateAdvancedTransactionTask::Execute() {
    //std::cout << "CreateTransactionTask::doWork amount=" <<  args_.amount << " tx_type=" << static_cast<int>(args_.tx_type) << std::endl;

    if(args_.tx_type==Safex::TransactionType::CreateAccountTransaction){
        Safex::CreateAccountCommand s{args_.safexUsername};

        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::EditAccountTransaction){
       Safex::EditAccountCommand s{args_.safexUsername, args_.safexData};

       transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::CreateOfferTransaction){
       Safex::CreateOfferCommand s{args_.safexUsername, args_.safexOfferTitle, args_.safexOfferPrice, args_.safexOfferQuantity, args_.safexOfferDescription,args_.safexOfferPricePegUsed,
                                   args_.safexOfferPricePegId, args_.safexOfferMinSfxPrice};
       transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::EditOfferTransaction){
        Safex::EditOfferCommand s{args_.safexOfferId, args_.safexUsername, args_.safexOfferActive, args_.safexOfferTitle, args_.safexOfferPrice, args_.safexOfferQuantity, args_.safexOfferDescription, args_.safexOfferPricePegUsed,
                                    args_.safexOfferPricePegId, args_.safexOfferMinSfxPrice};
        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::StakeTokenTransaction){
        Safex::StakeTokenCommand s{args_.address,args_.amount};
        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::UnstakeTokenTransaction){
        Safex::UnstakeTokenCommand s{args_.address,args_.amount};
        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::CreatePricePegTransaction){
        Safex::CreatePricePegCommand s{args_.safexPricePegTitle,args_.safexPricePegCreator,args_.safexPricePegDescription,args_.safexPricePegCurrency,args_.safexPricePegRate};
        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::UpdatePricePegTransaction){
        Safex::UpdatePricePegCommand s{args_.safexPricePegId,args_.safexPricePegTitle,args_.safexPricePegCreator,args_.safexPricePegDescription,args_.safexPricePegCurrency,args_.safexPricePegRate};
        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::PurchaseTransaction){
        Safex::PurchaseCommand s{args_.safexOfferId, args_.safexPurchaseQuantity};
        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);

    } else if(args_.tx_type==Safex::TransactionType::FeedbackTransaction){
        Safex::FeedbackCommand s{args_.safexOfferId, args_.safexFeedbackStarsGiven, args_.safexFeedbackComment};
        transaction_ = wallet_->createAdvancedTransaction(args_.address, args_.paymentId, args_.amount, args_.mixin, args_.priority, 0 /*subaddr account*/,{} /*subaddr indices*/, s);
    } else
        SetError( "Bad tx type given");

    if (!wallet_->errorString().empty()) {
        SetError( wallet_->errorString());
    }

    return;
}

void CreateAdvancedTransactionTask::OnOK() {
    Callback().Call({Env().Null(), PendingTransaction::NewInstance(Env(),transaction_)});
    return;
}

void CommitTransactionTask::Execute() {
    if (!transaction_->commit()) {
        SetError( "Couldn't commit transaction: " + transaction_->errorString());
    }

    return;
}


}
