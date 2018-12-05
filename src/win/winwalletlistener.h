//
// Created by amarko on 29.11.18..
//

#ifndef WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETLISTENER_H
#define WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETLISTENER_H

#include <cstdint>
#include <string>

#include <wallet_api.h>

namespace Safex
{

  struct WinWalletListener
  {

    /**
     * @brief moneySpent - called when money spent
     * @param txId       - transaction id
     * @param amount     - amount
     */
    virtual void moneySpent(const std::string &txId, uint64_t amount) = 0;

    /**
     * @brief moneyReceived - called when money received
     * @param txId          - transaction id
     * @param amount        - amount
     */
    virtual void moneyReceived(const std::string &txId, uint64_t amount) = 0;

    /**
     * @brief unconfirmedMoneyReceived - called when payment arrived in tx pool
     * @param txId          - transaction id
     * @param amount        - amount
     */
    virtual void unconfirmedMoneyReceived(const std::string &txId, uint64_t amount) = 0;


    /**
    * @brief tokensSpent - called when tokens are spent
    * @param txId       - transaction id
    * @param token_amount - token amount
    */
    virtual void tokensSpent(const std::string &txId, uint64_t token_amount) = 0;

    /**
     * @brief tokensReceived - called when tokens are received
     * @param txId          - transaction id
     * @param token_amount  - amount of tokens
     */
    virtual void tokensReceived(const std::string &txId, uint64_t token_amount) = 0;

    /**
     * @brief unconfirmedTokensReceived - called when token payment arrived in tx pool
     * @param txId          - transaction id
     * @param token_amount  - amount of tokens
     */
    virtual void unconfirmedTokensReceived(const std::string &txId, uint64_t token_amount) = 0;

    /**
     * @brief newBlock      - called when new block received
     * @param height        - block height
     */
    virtual void newBlock(uint64_t height) = 0;

    /**
     * @brief updated  - generic callback, called when any event (sent/received/block reveived/etc) happened with the wallet;
     */
    virtual void updated() = 0;


    /**
     * @brief refreshed - called when wallet refreshed by background thread or explicitly refreshed by calling "refresh" synchronously
     */
    virtual void refreshed() = 0;

  private:
      void* m_innerPtr;
  };

  struct WinWalletListenerProxy {
    static void moneySpent(void* target, const char* txId, uint64_t amount);
    static void moneyReceived(void* target, const char* txId, uint64_t amount);
    static void unconfirmedMoneyReceived(void* target, const char* txId, uint64_t amount);
    static void tokensSpent(void* target, const char* txId, uint64_t token_amount);
    static void tokensReceived(void* target, const char* txId, uint64_t token_amount);
    static void unconfirmedTokensReceived(void* target, const char* txId, uint64_t token_amount);
    static void newBlock(void* target, uint64_t height);
    static void updated(void* target);
    static void refreshed(void* target);

};

}

#endif //WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETLISTENER_H
