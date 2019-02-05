// Copyright (c) 2018, The Safex Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers
// Parts of this file are originally copyright (c) 2014-2018 The Monero Project

#ifndef SAFEX_NODEJS_LIBWALLET_WINWALLET_H
#define SAFEX_NODEJS_LIBWALLET_WINWALLET_H



#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <set>
#include <ctime>


#include <wallet_api.h>
#include "safexnativewallet.h"


//  Public interface for libwallet library
namespace Safex
{

  struct WinTransactionInfo
  {
    struct Transfer {
      Transfer(uint64_t _amount, uint64_t _token_amount, const std::string &address);
      const uint64_t amount;
      const uint64_t token_amount;
      const std::string address;
    };

    virtual ~WinTransactionInfo();

    WinTransactionInfo(void *innerPtr) : m_innerPtr(innerPtr)
    {}

    virtual int direction() const;

    virtual bool isPending() const;

    virtual bool isFailed() const;

    virtual uint64_t amount() const;

    virtual uint64_t fee() const;

    virtual uint64_t blockHeight() const;

    virtual std::set<uint32_t> subaddrIndex() const;

    virtual uint32_t subaddrAccount() const;

    virtual std::string label() const;

    virtual uint64_t confirmations() const;

    virtual uint64_t unlockTime() const;

    //! transaction_id
    virtual std::string hash() const;

    virtual std::time_t timestamp() const;

    virtual std::string paymentId() const;

    //! only applicable for output transactions
    virtual const std::vector<WinTransactionInfo::Transfer> &transfers() const;

    virtual TransactionType transactionType() const;

  private:
    void *m_innerPtr;
  };


  struct WinTransactionHistory
  {
    WinTransactionHistory(void* innerPtr) : m_innerPtr(innerPtr) {}
    virtual ~WinTransactionHistory();
    virtual int count() const;
    virtual WinTransactionInfo * transaction(int index)  const;
    virtual WinTransactionInfo * transaction(const std::string &id) const;
    virtual std::vector<WinTransactionInfo*> getAll() const;
    virtual void refresh();
    
    private:
      void* m_innerPtr;
  };


  struct WinWallet
  {
    friend class WinWalletManager;

    WinWallet(void *self_):m_innerPtr{self_} {}

    virtual ~WinWallet();
    virtual std::string seed() const;
    virtual Wallet * createWallet(const std::string &path, const std::string &password, const std::string &language, NetworkType nettype);
    virtual std::string address(uint32_t accountIndex = 0, uint32_t addressIndex = 0) const;
    virtual std::string path() const;
    virtual NetworkType nettype() const;
    virtual std::string secretViewKey() const;
    virtual std::string publicViewKey() const;
    virtual std::string secretSpendKey() const;
    virtual std::string publicSpendKey() const;
    virtual bool store(const std::string &path);
    virtual bool setPassword(const std::string &password);
    virtual std::string errorString() const;
    virtual bool init(const std::string &daemon_address, uint64_t upper_transaction_size_limit = 0, const std::string &daemon_username = "", const std::string &daemon_password = "", bool use_ssl = false, bool lightWallet = false);
    virtual void setRefreshFromBlockHeight(uint64_t refresh_from_block_height);
    virtual uint64_t getRefreshFromBlockHeight() const;
    virtual bool trustedDaemon() const;
    virtual Safex::Wallet::ConnectionStatus connected() const;
    virtual void startRefresh();
    virtual void pauseRefresh();
    virtual bool refresh();
    virtual void refreshAsync();
    virtual void setTrustedDaemon(bool arg);
    virtual void setListener(WinWalletListener *);
    virtual WinPendingTransaction * createTransaction(const std::string &dst_addr, const std::string &payment_id,
                                                   optional<uint64_t> value_amount, uint32_t mixin_count,
                                                   PendingTransaction::Priority = PendingTransaction::Priority_Low,
                                                   uint32_t subaddr_account = 0,
                                                   std::set<uint32_t> subaddr_indices = {},
                                                   const TransactionType tx_type = TransactionType::CashTransaction);
    uint64_t balanceAll() const;
    uint64_t unlockedBalanceAll() const;
    uint64_t tokenBalanceAll() const;
    uint64_t unlockedTokenBalanceAll() const;
    virtual void segregatePreForkOutputs(bool segregate);
    virtual void keyReuseMitigation2(bool mitigation);

    static std::string genPaymentId();
    virtual uint64_t daemonBlockChainHeight() const;
    virtual bool synchronized() const;
    static bool paymentIdValid(const std::string &paiment_id);
    static bool addressValid(const std::string &str, NetworkType nettype);
    static bool addressValid(const std::string &str, bool testnet)          // deprecated
    {
      return addressValid(str, testnet ? TESTNET : MAINNET);
    }
    virtual uint32_t defaultMixin() const;
    virtual WinTransactionHistory  *history();
    virtual uint64_t blockChainHeight() const;

    virtual void setDefaultMixin(uint32_t arg);
    virtual std::string signMessage(const std::string &message);
    virtual int status() const;
    virtual bool verifySignedMessage(const std::string &message, const std::string &addres, const std::string &signature) const;
    virtual void setAutoRefreshInterval(int millis);

    virtual bool rescanBlockchain();
    virtual void rescanBlockchainAsync();

    virtual void setSeedLanguage(const std::string &seedLanguage);

  private:
    void *m_innerPtr;
    void *m_nativeListenerPtr;
  };

}

#endif //SAFEX_NODEJS_LIBWALLET_WINWALLET_H
