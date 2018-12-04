//
// Created by amarko on 29.11.18..
//


#include <cstdint>
#include <winwalletlistener.h>

#include <windows_wrapper.h>

namespace Safex
{


  void WinWalletListenerProxy::moneySpent(void *target, const char *txId, uint64_t amount)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->moneySpent((txId), amount);
  }

  void WinWalletListenerProxy::moneyReceived(void *target, const char *txId, uint64_t amount)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->moneyReceived((txId), amount);
  }

  void WinWalletListenerProxy::unconfirmedMoneyReceived(void *target, const char *txId, uint64_t amount)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->unconfirmedMoneyReceived(std::string(txId), amount);
  }

  void WinWalletListenerProxy::tokensSpent(void *target, const char *txId, uint64_t token_amount)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->tokensSpent(std::string(txId), token_amount);
  }

  void WinWalletListenerProxy::tokensReceived(void *target, const char *txId, uint64_t token_amount)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->tokensReceived(std::string(txId), token_amount);
  }

  void WinWalletListenerProxy::unconfirmedTokensReceived(void *target, const char *txId, uint64_t token_amount)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->unconfirmedTokensReceived(std::string(txId), token_amount);
  }

  void WinWalletListenerProxy::newBlock(void *target, uint64_t height)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->newBlock(height);
  }

  void WinWalletListenerProxy::updated(void *target)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->updated();
  }

  void WinWalletListenerProxy::refreshed(void *target)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    sink->refreshed();
  }


}
