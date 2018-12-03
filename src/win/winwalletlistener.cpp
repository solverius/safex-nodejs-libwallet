//
// Created by amarko on 29.11.18..
//


#include <cstdint>
#include <winwalletlistener.h>

#include <windows_wrapper.h>

namespace Safex
{
  WinWalletListener::WinWalletListener() {
   m_innerPtr = ::win_lstn_Create(static_cast<void*>(this));
    ::win_lstn_setMoneySpent(m_innerPtr, &WinWalletListenerProxy::moneySpent);
    ::win_lstn_setMoneyReceived(m_innerPtr, &WinWalletListenerProxy::moneyReceived);
    ::win_lstn_setUnconfirmedMoneyReceived(m_innerPtr, &WinWalletListenerProxy::unconfirmedMoneyReceived);
    ::win_lstn_setTokensSpent(m_innerPtr, &WinWalletListenerProxy::tokensSpent);
    ::win_lstn_setTokenReceived(m_innerPtr, &WinWalletListenerProxy::tokensReceived);
    ::win_lstn_setUnconfirmedTokenReceived(m_innerPtr, &WinWalletListenerProxy::unconfirmedTokensReceived);
    ::win_lstn_setNewBlock(m_innerPtr, &WinWalletListenerProxy::newBlock);
    ::win_lstn_setUpdated(m_innerPtr, &WinWalletListenerProxy::updated);
    ::win_lstn_setRefreshed(m_innerPtr, &WinWalletListenerProxy::refreshed);
  }

  WinWalletListener::~WinWalletListener()
  {

  }


  void WinWalletListener::moneySpent(const std::string &txId, uint64_t amount)
  {

  }

  void WinWalletListener::moneyReceived(const std::string &txId, uint64_t amount)
  {

  }

  void WinWalletListener::unconfirmedMoneyReceived(const std::string &txId, uint64_t amount)
  {

  }

  void WinWalletListener::tokensSpent(const std::string &txId, uint64_t token_amount)
  {

  }

  void WinWalletListener::tokensReceived(const std::string &txId, uint64_t token_amount)
  {

  }

  void WinWalletListener::unconfirmedTokensReceived(const std::string &txId, uint64_t token_amount)
  {

  }

  void WinWalletListener::newBlock(uint64_t height)
  {

  }

  void WinWalletListener::updated()
  {

  }

  void WinWalletListener::refreshed()
  {

  }

    void WinWalletListenerProxy::moneySpent(void* target, const char* txId, uint64_t amount) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->moneySpent((txId), amount);
    }

    void WinWalletListenerProxy::moneyReceived(void* target, const char* txId, uint64_t amount) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->moneyReceived((txId), amount);
    }

    void WinWalletListenerProxy::unconfirmedMoneyReceived(void* target, const char* txId, uint64_t amount) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->unconfirmedMoneyReceived(std::string(txId), amount);
    }

    void WinWalletListenerProxy::tokensSpent(void* target, const char* txId, uint64_t token_amount) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->tokensSpent(std::string(txId), token_amount);
    }

    void WinWalletListenerProxy::tokensReceived(void* target, const char* txId, uint64_t token_amount) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->tokensReceived(std::string(txId), token_amount);
    }

    void WinWalletListenerProxy::unconfirmedTokensReceived(void* target, const char* txId, uint64_t token_amount) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->unconfirmedTokensReceived(std::string(txId), token_amount);
    }

    void WinWalletListenerProxy::newBlock(void* target, uint64_t height) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->newBlock(height);
    }

    void WinWalletListenerProxy::updated(void* target) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->updated();
    }

    void WinWalletListenerProxy::refreshed(void* target) {
      WinWalletListener* sink = static_cast<WinWalletListener*>(target);
      sink->refreshed();
    }


}