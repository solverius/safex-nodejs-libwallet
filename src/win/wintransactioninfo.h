//
// Created by stefan on 3.12.18..
//

#ifndef SAFEX_NODEJS_LIBWALLET_WINTRANSACTIONINFO_H
#define SAFEX_NODEJS_LIBWALLET_WINTRANSACTIONINFO_H

#include <wallet_api.h>

namespace Safex {
    struct WinTransactionInfo {

        WinTransactionInfo(void*);
        virtual ~WinTransactionInfo() = 0;

        virtual int direction();

        virtual bool isPending();

        virtual bool isFailed();

        virtual uint64_t amount();

        virtual uint64_t fee();

        virtual uint64_t blockHeight();

        virtual std::set <uint32_t> subaddrIndex();

        virtual uint32_t subaddrAccount();

        virtual std::string label();

        virtual uint64_t confirmations();

        virtual uint64_t unlockTime();

        //! transaction_id
        virtual std::string hash();

        virtual std::time_t timestamp();

        virtual std::string paymentId();

        //! only applicable for output transactions
        virtual const std::vector <Transfer> &transfers();

        virtual TransactionType transactionType();
    };
}
#endif //SAFEX_NODEJS_LIBWALLET_WINTRANSACTIONINFO_H
