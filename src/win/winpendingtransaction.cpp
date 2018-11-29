//
// Created by amarko on 29.11.18..
//

#include <wallet_api.h>
#include <windows_wrapper.h>

#include "winpendingtransaction.h"

namespace Safex
{


  WinPendingTransaction::~WinPendingTransaction()
  {

  }

  int WinPendingTransaction::status() const
  {
    return 0;
  }

  std::string WinPendingTransaction::errorString() const
  {
    return nullptr;
  }

  bool WinPendingTransaction::commit(const std::string &filename, bool overwrite)
  {
    return false;
  }

  uint64_t WinPendingTransaction::amount() const
  {
    return 0;
  }

  uint64_t WinPendingTransaction::tokenAmount() const
  {
    return 0;
  }

  uint64_t WinPendingTransaction::dust() const
  {
    return 0;
  }

  uint64_t WinPendingTransaction::fee() const
  {
    return 0;
  }

  std::vector<std::string> WinPendingTransaction::txid() const
  {
    return {};
  }

  uint64_t WinPendingTransaction::txCount() const
  {
    return 0;
  }

  std::vector<uint32_t> WinPendingTransaction::subaddrAccount() const
  {
    return {};
  }

  std::vector<std::set<uint32_t>> WinPendingTransaction::subaddrIndices() const
  {
    return {};
  }

}