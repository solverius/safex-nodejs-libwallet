// Copyright (c) 2018, The Safex Project
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers
// Parts of this file are originally copyright (c) 2014-2018 The Monero Project

#ifndef SAFEX_NODEJS_LIBWALLET_WINWALLET_H
#define SAFEX_NODEJS_LIBWALLET_WINWALLET_H


#pragma once


#include <string>
#include <vector>
#include <list>
#include <set>
#include <ctime>
#include <iostream>

#include <wallet_api.h>
#include <windows_wrapper.h>

//  Public interface for libwallet library
namespace Safex
{


/**
 * @brief Interface for wallet operations.
 *        TODO: check if /include/IWallet.h is still actual
 */
  struct WinWallet
  {

    virtual ~WinWallet();
    virtual std::string seed() const;
    virtual Wallet * createWallet(const std::string &path, const std::string &password, const std::string &language, NetworkType nettype);
    virtual std::string address(uint32_t accountIndex = 0, uint32_t addressIndex = 0) const;
    virtual std::string path() const;
    virtual NetworkType nettype() const;
    virtual std::string secretViewKey() const;
    virtual std::string publicViewKey() const;
    virtual std::string secretSpendKey() const;
    virtual std::string publicSpendKey() const;
    virtual bool setPassword(const std::string &password);
    virtual std::string errorString() const;
    virtual void setRefreshFromBlockHeight(uint64_t refresh_from_block_height);
    virtual Safex::Wallet::ConnectionStatus connected() const;
    virtual void setTrustedDaemon(bool arg);
    uint64_t balanceAll() const;
    uint64_t unlockedBalanceAll() const;
    uint64_t tokenBalanceAll() const;
    uint64_t unlockedTokenBalanceAll() const;


  };

}

#endif //SAFEX_NODEJS_LIBWALLET_WINWALLET_H
