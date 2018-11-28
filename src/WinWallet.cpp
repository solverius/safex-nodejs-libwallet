//
// Created by amarko on 28.11.18..
//

#include "WinWallet.h"


namespace Safex {

  WinWallet::~WinWallet()
  {

  }

  std::string WinWallet::seed() const
  {
    return std::__cxx11::string();
  }

  Wallet *WinWallet::createWallet(const std::string &path, const std::string &password, const std::string &language, NetworkType nettype)
  {
    return nullptr;
  }

  std::string WinWallet::address(uint32_t accountIndex, uint32_t addressIndex) const
  {
    return std::__cxx11::string();
  }

  std::string WinWallet::path() const
  {
    return std::__cxx11::string();
  }

  NetworkType WinWallet::nettype() const
  {
    return STAGENET;
  }

  std::string WinWallet::secretViewKey() const
  {
    return std::__cxx11::string();
  }

  std::string WinWallet::publicViewKey() const
  {
    return std::__cxx11::string();
  }

  std::string WinWallet::secretSpendKey() const
  {
    return std::__cxx11::string();
  }

  std::string WinWallet::publicSpendKey() const
  {
    return std::__cxx11::string();
  }

  bool WinWallet::setPassword(const std::string &password)
  {
    return false;
  }

  std::string WinWallet::errorString() const
  {
    return std::__cxx11::string();
  }

  void WinWallet::setRefreshFromBlockHeight(uint64_t refresh_from_block_height)
  {

  }

  Safex::Wallet::ConnectionStatus WinWallet::connected() const
  {
    return ConnectionStatus_WrongVersion;
  }

  void WinWallet::setTrustedDaemon(bool arg)
  {

  }

  uint64_t WinWallet::balanceAll() const
  {
    return 0;
  }

  uint64_t WinWallet::unlockedBalanceAll() const
  {
    return 0;
  }

  uint64_t WinWallet::tokenBalanceAll() const
  {
    return 0;
  }

  uint64_t WinWallet::unlockedTokenBalanceAll() const
  {
    return 0;
  }


}
