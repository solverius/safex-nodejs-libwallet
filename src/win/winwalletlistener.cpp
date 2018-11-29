//
// Created by amarko on 29.11.18..
//


#include <cstdint>
#include <winwalletlistener.h>

#include <windows_wrapper.h>

namespace Safex
{


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

}