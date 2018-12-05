//
// Created by amarko on 29.11.18..
//

#ifndef WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETMANAGER_H
#define WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETMANAGER_H

#include <cstdint>
#include <string>
#include <tuple>

#include <wallet_api.h>
#include <winwallet.h>

namespace Safex
{


/**
 * @brief WinWalletManager - wrapper on windows for WalletManager
 */
  struct WinWalletManager
  {

    WinWalletManager(void *nativeWalletPtr): m_innerPtr(nativeWalletPtr) {}

    /*!
     * \brief  Creates new wallet
     * \param  path           Name of wallet file
     * \param  password       Password of wallet file
     * \param  language       Language to be used to generate electrum seed mnemonic
     * \param  nettype        Network type
     * \return                SafexNativeWallet instance (SafexNativeWallet::status() needs to be called to check if created successfully)
     */
    virtual SafexNativeWallet *createWallet(const std::string &path, const std::string &password, const std::string &language, NetworkType nettype);

    SafexNativeWallet *createWallet(const std::string &path, const std::string &password, const std::string &language, bool testnet = false)      // deprecated
    {
      return createWallet(path, password, language, testnet ? TESTNET : MAINNET);
    }

    /*!
     * \brief  Opens existing wallet
     * \param  path           Name of wallet file
     * \param  password       Password of wallet file
     * \param  nettype        Network type
     * \return                SafexNativeWallet instance (SafexNativeWallet::status() needs to be called to check if opened successfully)
     */
    virtual SafexNativeWallet *openWallet(const std::string &path, const std::string &password, NetworkType nettype);

    SafexNativeWallet *openWallet(const std::string &path, const std::string &password, bool testnet = false)     // deprecated
    {
      return openWallet(path, password, testnet ? TESTNET : MAINNET);
    }

    /*!
     * \brief  recovers existing wallet using mnemonic (electrum seed)
     * \param  path           Name of wallet file to be created
     * \param  password       Password of wallet file
     * \param  mnemonic       mnemonic (25 words electrum seed)
     * \param  nettype        Network type
     * \param  restoreHeight  restore from start height
     * \return                SafexNativeWallet instance (SafexNativeWallet::status() needs to be called to check if recovered successfully)
     */
    virtual SafexNativeWallet *recoveryWallet(const std::string &path, const std::string &password, const std::string &mnemonic,
                                   NetworkType nettype = MAINNET, uint64_t restoreHeight = 0);

    SafexNativeWallet *recoveryWallet(const std::string &path, const std::string &password, const std::string &mnemonic,
                           bool testnet = false, uint64_t restoreHeight = 0)           // deprecated
    {
      return recoveryWallet(path, password, mnemonic, testnet ? TESTNET : MAINNET, restoreHeight);
    }

    /*!
     * \deprecated this method creates a wallet WITHOUT a passphrase, use the alternate recoverWallet() method
     * \brief  recovers existing wallet using mnemonic (electrum seed)
     * \param  path           Name of wallet file to be created
     * \param  mnemonic       mnemonic (25 words electrum seed)
     * \param  nettype        Network type
     * \param  restoreHeight  restore from start height
     * \return                SafexNativeWallet instance (SafexNativeWallet::status() needs to be called to check if recovered successfully)
     */
    virtual SafexNativeWallet *recoveryWallet(const std::string &path, const std::string &mnemonic, NetworkType nettype, uint64_t restoreHeight = 0);

    SafexNativeWallet *recoveryWallet(const std::string &path, const std::string &mnemonic, bool testnet = false, uint64_t restoreHeight = 0)         // deprecated
    {
      return recoveryWallet(path, mnemonic, testnet ? TESTNET : MAINNET, restoreHeight);
    }

    /*!
     * \brief  recovers existing wallet using keys. Creates a view only wallet if spend key is omitted
     * \param  path           Name of wallet file to be created
     * \param  password       Password of wallet file
     * \param  language       language
     * \param  nettype        Network type
     * \param  restoreHeight  restore from start height
     * \param  addressString  public address
     * \param  viewKeyString  view key
     * \param  spendKeyString spend key (optional)
     * \return                SafexNativeWallet instance (SafexNativeWallet::status() needs to be called to check if recovered successfully)
     */
    virtual SafexNativeWallet *createWalletFromKeys(const std::string &path,
                                         const std::string &password,
                                         const std::string &language,
                                         NetworkType nettype,
                                         uint64_t restoreHeight,
                                         const std::string &addressString,
                                         const std::string &viewKeyString,
                                         const std::string &spendKeyString = "");

    SafexNativeWallet *createWalletFromKeys(const std::string &path,
                                 const std::string &password,
                                 const std::string &language,
                                 bool testnet,
                                 uint64_t restoreHeight,
                                 const std::string &addressString,
                                 const std::string &viewKeyString,
                                 const std::string &spendKeyString = "")       // deprecated
    {
      return createWalletFromKeys(path, password, language, testnet ? TESTNET : MAINNET, restoreHeight, addressString, viewKeyString, spendKeyString);
    }

    /*!
     * \deprecated this method creates a wallet WITHOUT a passphrase, use createWalletFromKeys(..., password, ...) instead
     * \brief  recovers existing wallet using keys. Creates a view only wallet if spend key is omitted
     * \param  path           Name of wallet file to be created
     * \param  language       language
     * \param  nettype        Network type
     * \param  restoreHeight  restore from start height
     * \param  addressString  public address
     * \param  viewKeyString  view key
     * \param  spendKeyString spend key (optional)
     * \return                SafexNativeWallet instance (SafexNativeWallet::status() needs to be called to check if recovered successfully)
     */
    virtual SafexNativeWallet *createWalletFromKeys(const std::string &path,
                                         const std::string &language,
                                         NetworkType nettype,
                                         uint64_t restoreHeight,
                                         const std::string &addressString,
                                         const std::string &viewKeyString,
                                         const std::string &spendKeyString = "");

    SafexNativeWallet *createWalletFromKeys(const std::string &path,
                                 const std::string &language,
                                 bool testnet,
                                 uint64_t restoreHeight,
                                 const std::string &addressString,
                                 const std::string &viewKeyString,
                                 const std::string &spendKeyString = "")           // deprecated
    {
      return createWalletFromKeys(path, language, testnet ? TESTNET : MAINNET, restoreHeight, addressString, viewKeyString, spendKeyString);
    }

    /*!
     * \brief Closes wallet. In case operation succeeded, wallet object deleted. in case operation failed, wallet object not deleted
     * \param wallet        previously opened / created wallet instance
     * \return              None
     */
    virtual bool closeWallet(SafexNativeWallet *wallet, bool store = true);

    /*
     * ! checks if wallet with the given name already exists
     */

    /*!
     * @brief TODO: delme walletExists - check if the given filename is the wallet
     * @param path - filename
     * @return - true if wallet exists
     */
    virtual bool walletExists(const std::string &path);

    /*!
     * @brief verifyWalletPassword - check if the given filename is the wallet
     * @param keys_file_name - location of keys file
     * @param password - password to verify
     * @param no_spend_key - verify only view keys?
     * @return - true if password is correct
     */
    virtual bool verifyWalletPassword(const std::string &keys_file_name, const std::string &password, bool no_spend_key) const;

    /*!
     * \brief findWallets - searches for the wallet files by given path name recursively
     * \param path - starting point to search
     * \return - list of strings with found wallets (absolute paths);
     */
    virtual std::vector<std::string> findWallets(const std::string &path);

    //! returns verbose error string regarding last error;
    virtual std::string errorString() const;

    //! set the daemon address (hostname and port)
    virtual void setDaemonAddress(const std::string &address);

    //! returns whether the daemon can be reached, and its version number
    virtual bool connected(uint32_t *version = NULL);

    //! returns current blockchain height
    virtual uint64_t blockchainHeight();

    //! returns current blockchain target height
    virtual uint64_t blockchainTargetHeight();

    //! returns current network difficulty
    virtual uint64_t networkDifficulty();

    //! returns current mining hash rate (0 if not mining)
    virtual double miningHashRate();

    //! returns current block target
    virtual uint64_t blockTarget();

    //! returns true iff mining
    virtual bool isMining();

    //! starts mining with the set number of threads
    virtual bool startMining(const std::string &address, uint32_t threads = 1, bool background_mining = false, bool ignore_battery = true);

    //! stops mining
    virtual bool stopMining();

    //! resolves an OpenAlias address to a monero address
    virtual std::string resolveOpenAlias(const std::string &address, bool &dnssec_valid) const;

    //! checks for an update and returns version, hash and url
    static std::tuple<bool, std::string, std::string, std::string, std::string> checkUpdates(const std::string &software, std::string subdir);

  private:
    void* m_innerPtr;

  };


  struct WinWalletManagerFactory
  {
    // logging levels for underlying library
    enum LogLevel
    {
      LogLevel_Silent = -1,
      LogLevel_0 = 0,
      LogLevel_1 = 1,
      LogLevel_2 = 2,
      LogLevel_3 = 3,
      LogLevel_4 = 4,
      LogLevel_Min = LogLevel_Silent,
      LogLevel_Max = LogLevel_4
    };

    static WinWalletManager *getWalletManager();

    static void setLogLevel(int level);

    static void setLogCategories(const std::string &categories);
  };


}
#endif //WIN_SAFEX_NODEJS_LIBWALLET_WINWALLETMANAGER_H
