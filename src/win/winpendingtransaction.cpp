//
// Created by amarko on 29.11.18..
//

#include <wallet_api.h>
#include <windows_wrapper.h>

#include "winpendingtransaction.h"

namespace Safex
{

    WinPendingTransaction::WinPendingTransaction(void* ptr) : m_innerPtr(ptr) {

  }

  WinPendingTransaction::~WinPendingTransaction()
  {
    ::win_pt_delete(m_innerPtr);
  }

  int WinPendingTransaction::status() const
  {
    return ::win_pt_status(m_innerPtr);
  }

  std::string WinPendingTransaction::errorString() const
  {
    return std::string(win_pt_errorString(m_innerPtr));
  }

  bool WinPendingTransaction::commit(const std::string &filename, bool overwrite)
  {
    return static_cast<bool>(::win_pt_commit(m_innerPtr));
  }

  uint64_t WinPendingTransaction::amount() const
  {
    return ::win_pt_amount(m_innerPtr);
  }

  uint64_t WinPendingTransaction::tokenAmount() const
  {
    return ::win_pt_tokenAmount(m_innerPtr);
  }

  uint64_t WinPendingTransaction::dust() const
  {
    return ::win_pt_dust(m_innerPtr);
  }

  uint64_t WinPendingTransaction::fee() const
  {
    return ::win_pt_fee(m_innerPtr);
  }

  std::vector<std::string> WinPendingTransaction::txid() const
  {
    char** results = ::win_pt_txid(m_innerPtr);
    char* temp = results[0];
    std::vector<std::string> ret;
    while(temp != nullptr) {
      ret.push_back(temp);
      delete temp;
      temp++;
    }
    delete results;

    return ret;
  }

  uint64_t WinPendingTransaction::txCount() const
  {
    return ::win_pt_txCount(m_innerPtr);
  }

  std::vector<uint32_t> WinPendingTransaction::subaddrAccount() const
  {
    throw std::string("Not implemented yet!!");
    return {};
  }

  std::vector<std::set<uint32_t>> WinPendingTransaction::subaddrIndices() const
  {
    throw std::string("Not implemented yet!!");
    return {};
  }

}