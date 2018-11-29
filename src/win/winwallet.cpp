//
// Created by amarko on 28.11.18..
//

#include <cstdint>
#include "winwallet.h"


namespace Safex {

  WinTransactionInfo::~WinTransactionInfo()
  {

  }

  int WinTransactionInfo::direction() const
  {
    return 0;
  }

  bool WinTransactionInfo::isPending() const
  {
    return false;
  }

  bool WinTransactionInfo::isFailed() const
  {
    return false;
  }

  uint64_t WinTransactionInfo::amount() const
  {
    return 0;
  }

  uint64_t WinTransactionInfo::fee() const
  {
    return 0;
  }

  uint64_t WinTransactionInfo::blockHeight() const
  {
    return 0;
  }

  std::set<uint32_t> WinTransactionInfo::subaddrIndex() const
  {
    return std::set<uint32_t>();
  }

  uint32_t WinTransactionInfo::subaddrAccount() const
  {
    return 0;
  }

  std::string WinTransactionInfo::label() const
  {
    return std::string();
  }

  uint64_t WinTransactionInfo::confirmations() const
  {
    return 0;
  }

  uint64_t WinTransactionInfo::unlockTime() const
  {
    return 0;
  }

  std::string WinTransactionInfo::hash() const
  {
    return std::string();
  }

  std::time_t WinTransactionInfo::timestamp() const
  {
    return 0;
  }

  std::string WinTransactionInfo::paymentId() const
  {
    return std::string();
  }

  const std::vector<Safex::TransactionInfo::Transfer> &WinTransactionInfo::transfers() const
  {
    return {};
  }

  TransactionType WinTransactionInfo::transactionType() const
  {
    return Safex::TransactionType::CashTransaction;
  }


  WinTransactionHistory::~WinTransactionHistory()
  {

  }

  int WinTransactionHistory::count() const
  {
    return 0;
  }

  WinTransactionInfo *WinTransactionHistory::transaction(int index) const
  {
    return nullptr;
  }

  WinTransactionInfo *WinTransactionHistory::transaction(const std::string &id) const
  {
    return nullptr;
  }

  std::vector<WinTransactionInfo *> WinTransactionHistory::getAll() const
  {
    return std::vector<WinTransactionInfo *>();
  }

  void WinTransactionHistory::refresh()
  {

  }




  WinWallet::~WinWallet()
  {

  }

  std::string WinWallet::seed() const
  {
    return std::string();
  }

  Wallet *WinWallet::createWallet(const std::string &path, const std::string &password, const std::string &language, NetworkType nettype)
  {
    return nullptr;
  }

  std::string WinWallet::address(uint32_t accountIndex, uint32_t addressIndex) const
  {
    return std::string();
  }

  std::string WinWallet::path() const
  {
    return std::string();
  }

  NetworkType WinWallet::nettype() const
  {
    return STAGENET;
  }

  std::string WinWallet::secretViewKey() const
  {
    return std::string();
  }

  std::string WinWallet::publicViewKey() const
  {
    return std::string();
  }

  std::string WinWallet::secretSpendKey() const
  {
    return std::string();
  }

  std::string WinWallet::publicSpendKey() const
  {
    return std::string();
  }

  bool WinWallet::setPassword(const std::string &password)
  {
    return false;
  }

  std::string WinWallet::errorString() const
  {
    return std::string();
  }

  bool WinWallet::init(const std::string &daemon_address, uint64_t upper_transaction_size_limit, const std::string &daemon_username, const std::string &daemon_password, bool use_ssl, bool lightWallet)
  {
    return false;
  }

  bool WinWallet::store(const std::string &path)
  {
    return false;
  }

  void WinWallet::segregatePreForkOutputs(bool segregate)
  {

  }

  void WinWallet::keyReuseMitigation2(bool mitigation)
  {

  }

  uint64_t WinWallet::getRefreshFromBlockHeight() const
  {
    return 0;
  }

  bool WinWallet::trustedDaemon() const
  {
    return false;
  }

  std::string WinWallet::genPaymentId()
  {
    return std::string();
  }

  uint64_t WinWallet::daemonBlockChainHeight() const
  {
    return 0;
  }

  bool WinWallet::synchronized() const
  {
    return false;
  }

  bool WinWallet::paymentIdValid(const std::string &paiment_id)
  {
    return false;
  }

  bool WinWallet::addressValid(const std::string &str, NetworkType nettype)
  {
    return false;
  }

  uint32_t WinWallet::defaultMixin() const
  {
    return 0;
  }

  WinTransactionHistory *WinWallet::history()
  {
    return nullptr;
  }




  Safex::Wallet::ConnectionStatus WinWallet::connected() const
  {
    return Safex::Wallet::ConnectionStatus::ConnectionStatus_WrongVersion;
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


  void WinWallet::setDefaultMixin(uint32_t arg)
  {

  }

  std::string WinWallet::signMessage(const std::string &message)
  {
    return std::string();
  }

  int WinWallet::status() const
  {
    return 0;
  }

  bool WinWallet::verifySignedMessage(const std::string &message, const std::string &addres, const std::string &signature) const
  {
    return false;
  }

  uint64_t WinWallet::blockChainHeight() const
  {
    return 0;
  }

  void WinWallet::setListener(WinWalletListener *)
  {

  }

  WinPendingTransaction *WinWallet::createTransaction(const std::string &dst_addr, const std::string &payment_id, optional<uint64_t> value_amount, uint32_t mixin_count,
          PendingTransaction::Priority, uint32_t subaddr_account, std::set<uint32_t> subaddr_indices, const TransactionType tx_type)
  {
    return nullptr;
  }
  
  void WinWallet::startRefresh()
  {

  }

  void WinWallet::pauseRefresh()
  {

  }

  bool WinWallet::refresh()
  {
    return false;
  }

  void WinWallet::refreshAsync()
  {

  }

  void WinWallet::setRefreshFromBlockHeight(uint64_t refresh_from_block_height)
  {

  }


}
