//
// Created by amarko on 29.11.18..
//

#ifndef WIN_SAFEX_NODEJS_LIBWALLET_WINPENDINGTRANSACTION_H
#define WIN_SAFEX_NODEJS_LIBWALLET_WINPENDINGTRANSACTION_H

#include <cstdint>
#include <string>
#include <wallet_api.h>

namespace Safex
{


  struct WinPendingTransaction
  {

    virtual ~WinPendingTransaction();

    virtual int status() const;

    virtual std::string errorString() const;

    // commit transaction or save to file if filename is provided.
    virtual bool commit(const std::string &filename = "", bool overwrite = false);

    virtual uint64_t amount() const;

    virtual uint64_t tokenAmount() const;

    virtual uint64_t dust() const;

    virtual uint64_t fee() const;

    virtual std::vector<std::string> txid() const;

    /*!
     * \brief txCount - number of transactions current transaction will be splitted to
     * \return
     */
    virtual uint64_t txCount() const;

    virtual std::vector<uint32_t> subaddrAccount() const;

    virtual std::vector<std::set<uint32_t>> subaddrIndices() const;
  };

}

#endif //WIN_SAFEX_NODEJS_LIBWALLET_WINPENDINGTRANSACTION_H
