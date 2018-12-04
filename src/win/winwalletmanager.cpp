//
// Created by amarko on 29.11.18..
//

#include <string>
#include <vector>
#include <tuple>

#include <winwalletmanager.h>

#include <windows_wrapper.h>



namespace Safex
{

  SafexNativeWallet *WinWalletManager::createWallet(const std::string &path, const std::string &password, const std::string &language, NetworkType nettype)
  {
    void *self = nullptr;
    win_mng_createWallet(self, path.c_str(), password.c_str(), language.c_str(), nettype);
    SafexNativeWallet *wlt = new WinWallet(self);
    return wlt;
  }

  SafexNativeWallet *WinWalletManager::openWallet(const std::string &path, const std::string &password, NetworkType nettype)
  {
    WinWallet *nativeWallet = new WinWallet(win_mng_openWallet(m_innerPtr, path.c_str(), password.c_str(), static_cast<uint32_t>(nettype)));
    return nativeWallet;
  }

  SafexNativeWallet *WinWalletManager::recoveryWallet(const std::string &path, const std::string &password, const std::string &mnemonic, NetworkType nettype, uint64_t restoreHeight)
  {

    WinWallet *nativeWallet = new WinWallet(win_mng_recoveryWallet(m_innerPtr, path.c_str(), password.c_str(), mnemonic.c_str(), static_cast<uint32_t>(nettype), restoreHeight));
    return nativeWallet;
  }

  SafexNativeWallet *WinWalletManager::recoveryWallet(const std::string &path, const std::string &mnemonic, NetworkType nettype, uint64_t restoreHeight)
  {
    return nullptr;
  }

  SafexNativeWallet *WinWalletManager::createWalletFromKeys(const std::string &path, const std::string &password, const std::string &language, NetworkType nettype, uint64_t restoreHeight,
          const std::string &addressString, const std::string &viewKeyString, const std::string &spendKeyString)
  {
    WinWallet *nativeWallet = new WinWallet(win_mng_createWalletFromKeys(m_innerPtr, path.c_str(), password.c_str(), language.c_str(), static_cast<uint32_t>(nettype), restoreHeight,
                                                                         addressString.c_str(), viewKeyString.c_str(), spendKeyString.c_str()));

    return nativeWallet;
  }

  SafexNativeWallet *WinWalletManager::createWalletFromKeys(const std::string &path, const std::string &language, NetworkType nettype, uint64_t restoreHeight, const std::string &addressString, const std::string &viewKeyString, const std::string &spendKeyString)
  {
    return nullptr;
  }

  bool WinWalletManager::closeWallet(SafexNativeWallet *wallet, bool store)
  {
    return false;
  }

  bool WinWalletManager::walletExists(const std::string &path)
  {
    return win_mng_walletExists(m_innerPtr, path.c_str());
  }

  bool WinWalletManager::verifyWalletPassword(const std::string &keys_file_name, const std::string &password, bool no_spend_key) const
  {
    return false;
  }

  std::vector<std::string> WinWalletManager::findWallets(const std::string &path)
  {
    return std::vector<std::string>();
  }

  std::string WinWalletManager::errorString() const
  {
    return std::string();
  }

  void WinWalletManager::setDaemonAddress(const std::string &address)
  {

  }

  bool WinWalletManager::connected(uint32_t *version)
  {
    return false;
  }

  uint64_t WinWalletManager::blockchainHeight()
  {
    return 0;
  }

  uint64_t WinWalletManager::blockchainTargetHeight()
  {
    return 0;
  }

  uint64_t WinWalletManager::networkDifficulty()
  {
    return 0;
  }

  double WinWalletManager::miningHashRate()
  {
    return 0;
  }

  uint64_t WinWalletManager::blockTarget()
  {
    return 0;
  }

  bool WinWalletManager::isMining()
  {
    return false;
  }

  bool WinWalletManager::startMining(const std::string &address, uint32_t threads, bool background_mining, bool ignore_battery)
  {
    return false;
  }

  bool WinWalletManager::stopMining()
  {
    return false;
  }

  std::string WinWalletManager::resolveOpenAlias(const std::string &address, bool &dnssec_valid) const
  {
    return std::string();
  }

  std::tuple<bool, std::string, std::string, std::string, std::string> WinWalletManager::checkUpdates(const std::string &software, std::string subdir)
  {
    return std::tuple<bool, std::string, std::string, std::string, std::string>();
  }

  WinWalletManager *WinWalletManagerFactory::getWalletManager()
  {
    WinWalletManager *winWalletManager = new WinWalletManager(::win_mngf_getWalletManager());
    WinWalletManagerFactory::setLogLevel(LogLevel::LogLevel_0); //by default mlog it prints on stdout on Windows
    return winWalletManager;
  }

  void WinWalletManagerFactory::setLogLevel(int level)
  {
    win_mlog_set_log_levelI(level);
  }

  void WinWalletManagerFactory::setLogCategories(const std::string &categories)
  {
    win_mlog_set_log_levelCPtr(categories.c_str());
  }
}
