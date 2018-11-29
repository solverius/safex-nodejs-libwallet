//
// Created by amarko on 29.11.18..
//

#ifndef WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETLISTENER_H
#define WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETLISTENER_H

#include <cstdint>
#include <string>

#include <wallet_api.h>
#include <windows_wrapper.h>

namespace Safex
{

  struct WinWalletListener
  {
    virtual ~WinWalletListener();

    /**
     * @brief moneySpent - called when money spent
     * @param txId       - transaction id
     * @param amount     - amount
     */
    virtual void moneySpent(const std::string &txId, uint64_t amount);

    /**
     * @brief moneyReceived - called when money received
     * @param txId          - transaction id
     * @param amount        - amount
     */
    virtual void moneyReceived(const std::string &txId, uint64_t amount);

    /**
     * @brief unconfirmedMoneyReceived - called when payment arrived in tx pool
     * @param txId          - transaction id
     * @param amount        - amount
     */
    virtual void unconfirmedMoneyReceived(const std::string &txId, uint64_t amount);


    /**
    * @brief tokensSpent - called when tokens are spent
    * @param txId       - transaction id
    * @param token_amount - token amount
    */
    virtual void tokensSpent(const std::string &txId, uint64_t token_amount);

    /**
     * @brief tokensReceived - called when tokens are received
     * @param txId          - transaction id
     * @param token_amount  - amount of tokens
     */
    virtual void tokensReceived(const std::string &txId, uint64_t token_amount);

    /**
     * @brief unconfirmedTokensReceived - called when token payment arrived in tx pool
     * @param txId          - transaction id
     * @param token_amount  - amount of tokens
     */
    virtual void unconfirmedTokensReceived(const std::string &txId, uint64_t token_amount);

    /**
     * @brief newBlock      - called when new block received
     * @param height        - block height
     */
    virtual void newBlock(uint64_t height);

    /**
     * @brief updated  - generic callback, called when any event (sent/received/block reveived/etc) happened with the wallet;
     */
    virtual void updated();


    /**
     * @brief refreshed - called when wallet refreshed by background thread or explicitly refreshed by calling "refresh" synchronously
     */
    virtual void refreshed();
  };

}

#endif //WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETLISTENER_H
