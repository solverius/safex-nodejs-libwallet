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
    return;
  }

  void WinWalletListenerProxy::moneyReceived(void *target, const char *txId, uint64_t amount)
  {
    return;
  }

  void WinWalletListenerProxy::unconfirmedMoneyReceived(void *target, const char *txId, uint64_t amount)
  {
    return;
  }

  void WinWalletListenerProxy::tokensSpent(void *target, const char *txId, uint64_t token_amount)
  {
    return;
  }

  void WinWalletListenerProxy::tokensReceived(void *target, const char *txId, uint64_t token_amount)
  {
   return;
  }

  void WinWalletListenerProxy::unconfirmedTokensReceived(void *target, const char *txId, uint64_t token_amount)
  {
    return;
  }

  void WinWalletListenerProxy::newBlock(void *target, uint64_t height)
  {
    //During syncing, this callback is called 100000 times and it messes with JS engine
    //We will disable it
    return;

//    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
//    sink->newBlock(height);
  }

  void WinWalletListenerProxy::updated(void *target)
  {
   return;
  }

  void WinWalletListenerProxy::refreshed(void *target)
  {
    WinWalletListener *sink = static_cast<WinWalletListener *>(target);
    if(sink != nullptr) {
      std::cout << "sink is not nullptr" << std::endl;
    }
    else {
      std::cout << "sink is  nullptr" << std::endl;
    }
    sink->refreshed();
  }


}
