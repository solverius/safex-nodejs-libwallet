//
// Created by amarko on 29.11.18..
//

#ifndef WIN_SAFEX_NODEJS_LIBWALLET_NATIVESAFEXWALLET_H
#define WIN_SAFEX_NODEJS_LIBWALLET_NATIVESAFEXWALLET_H

#include <wallet_api.h>

#if _MSC_VER //windows node-gyp build
namespace Safex {
  struct WinWallet;
  struct WinWalletListener;
  struct WinWalletManager;
  struct WinWalletManagerFactory;
  struct WinPendingTransaction;
  struct WinTransactionInfo;
  struct WinTransactionHistory;
  struct WinAddressBookRow;
}

#endif



//SafexNativeWallet is a type that represents OS agnostic interface
#if _MSC_VER //windows node-gyp build

using SafexNativeWallet                 = Safex::WinWallet;
using SafexNativeWalletListener         = Safex::WinWalletListener;
using SafexNativeWalletManager          = Safex::WinWalletManager;
using SafexNativeWalletManagerFactory   = Safex::WinWalletManagerFactory;
using SafexNativePendingTransaction     = Safex::WinPendingTransaction;
using SafexNativeTransactionInfo        = Safex::WinTransactionInfo;
using SafexNativeTransactionHistory     = Safex::WinTransactionHistory;
using SafexNativeAddressBookRow         = Safex::WinAddressBookRow;
#else

using SafexNativeWallet                 = Safex::Wallet;
using SafexNativeWalletListener         = Safex::WalletListener;
using SafexNativeWalletManager          = Safex::WalletManager;
using SafexNativeWalletManagerFactory   = Safex::WalletManagerFactory;
using SafexNativePendingTransaction     = Safex::PendingTransaction;
using SafexNativeTransactionInfo        = Safex::TransactionInfo;
using SafexNativeTransactionHistory     = Safex::TransactionHistory;
using SafexNativeAddressBook            = Safex::AddressBook;
using SafexNativeAddressBookRow         = Safex::AddressBookRow;

#endif

#endif //WIN_SAFEX_NODEJS_LIBWALLET_NATIVESAFEXWALLET_H
