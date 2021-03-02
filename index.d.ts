export = monero;
export as namespace monero;

declare namespace monero {
  type Language =
    | 'English'
    | 'Nederlands'
    | 'Français'
    | 'Español'
    | 'Português'
    | '日本語'
    | 'Italiano'
    | 'Deutsch'
    | 'русский язык'
    | '简体中文 (中国)'
    | 'Esperanto'
    | 'Lojban';

  type WalletEvent =
    | 'newBlock'
    | 'refreshed'
    | 'updated'
    | 'unconfirmedMoneyReceived'
    | 'moneyReceived'
    | 'moneySpent'
    | 'unconfirmedTokensReceived'
    | 'tokensReceived'
    | 'tokensSpent';

  type Network =
    | 'mainnet'
    | 'stagenet'
    | 'testnet';

  type MultisigState = {
    isMultisig: boolean,
    isReady: boolean,
    threshold: number,
    total: number,
  };

  type SafexAccount = {
    username: string,
    data: string,
    publicKey: string,
    privateKey: string,
    status: number
  };

  type SafexOffer = {
    title: string,
    quantity: string,
    price: string,
    minSfxPrice: string,
    pricePegUsed: boolean,
    pricePegID: string,
    description: string,
    seller: string,
    active: boolean,
    offerID: string,
    currency: string
  };

  type Transaction = {
    commit(): Promise<void>;
    amount(): string;
    tokenAmount(): string;
    fee(): string;
    transactionsCount(): number;
    transactionsIds(): string[];
    multisigSignData(): string;
    signMultisigTransaction(): void;
    signersKeys(): string[];
  };

  type Transfer = {
    amount: string;
    token_amount: string;
    address: string;
  };

  type TransactionInfo = {
    id: string;
    direction: 'in' | 'out';
    pending: boolean;
    failed: boolean;
    amount: string;
    fee: string;
    blockHeight: number;
    subAddresses: string;
    subAddrAccount: string;
    label: string;
    confirmations: number;
    unlockTime: number;
    timestamp: number;
    paymentId: number;
    transfers: Transfer[];
    transactionType: number;
  };

  type InterestInfo = {
    tokenStaked: string;
    collectedInterest: string;
    blockHeight: string;
  };

  type FeedbackToken = {
    offerID: string;
    offerTitle: string;
  };

  type Feedback = {
    offerID: string;
    offerTitle: string;
    ratingGiven: number;
    comment: string;
  };

  interface Wallet {
    address(): string;
    seed(): string;
    on(event: 'newBlock', callback: (height: number) => void): Wallet;
    on(
      event: 'unconfirmedMoneyReceived' | 'moneyReceived' | 'moneySpent',
      callback: (tx: string, amount: string) => void,
    ): Wallet;
    on(
        event: 'unconfirmedTokensReceived' | 'tokensReceived' | 'tokensSpent',
        callback: (tx: string, token_amount: string) => void,
    ): Wallet;
    on(event: 'refreshed' | 'updated', callback: () => void): Wallet;
    off(event?: WalletEvent): Wallet;
    store(): Promise<void>;
    createSafexAccount(username: string,
                       description: string): boolean;
    getSafexAccounts(): SafexAccount[];
    getSafexAccount(username: string): SafexAccount;
    recoverSafexAccount(username: string,
                        secretKey: string): boolean;
    removeSafexAccount(username: string): boolean;
    getMySafexOffers(): SafexOffer[];
    listSafexOffers(active: boolean): SafexOffer[];
    createTransaction(options: {
      address: string,
      amount: string,
      paymentId?: string,
      mixin?: number,
      priority?:number,
      tx_type?: number
    }): Promise<Transaction>;
    createAdvancedTransaction(options: {
      address?: string,
      amount?: string,
      paymentId?: string,
      mixin?: number,
      priority?:number,
      tx_type?: number,
      safex_username?: string,
      safex_data?: string,
      safex_offer_title?: string,
      safex_offer_price?: string,
      safex_offer_quantity?: string,
      safex_offer_description?: string,
      safex_offer_price_peg_used?: number,
      safex_offer_price_peg_id?: string,
      safex_offer_min_sfx_price?: string,
      safex_offer_active?: number,
      safex_offer_id?: string,
      safex_price_peg_title?: string,
      safex_price_peg_creator?: string,
      safex_price_peg_description?: string,
      safex_price_peg_currency?: string,
      safex_price_peg_rate?: number,
      safex_price_peg_id?: string,
      safex_purchase_quantity?: number,
      safex_feedback_stars_given?: number,
      safex_feedback_comment?: string,
      safex_staked_token_height?: number,
    }): Promise<Transaction>;
    getMyStake(): InterestInfo[];
    getMyFeedbackTokens(): FeedbackToken[];
    getMyFeedbacks(): Feedback[];
    history(): TransactionInfo[];
    path(): string;
    network(): Network;
    secretViewKey(): string;
    publicViewKey(): string;
    secretSpendKey(): string;
    publicSpendKey(): string;
    publicMultisigSignerKey(): string;
    setPassword(password: string): Wallet;
    setRefreshFromBlockHeight(height: number): Wallet;
    getRefreshFromBlockHeight(): string;
    connected(): boolean;
    setTrustedDaemon(value: boolean): Wallet;
    trustedDaemon(): boolean;
    balance(): string;
    unlockedBalance(): string;
    blockchainHeight(): number;
    daemonBlockchainHeight(): number;
    synchronized(): boolean;
    pauseRefresh(): void;
    startRefresh(): void;
    multisigState(): MultisigState;
    verifyMessageWithPublicKey(message: string, key: string, signature: string): boolean;
    getMultisigInfo(): string;
    makeMultisig(multisigInfos: string[], walletsNumber: number): string;
    finalizeMultisig(multisigInfos: string[]): string;
    exportMultisigImages(): string;
    importMultisigImages(keys: string[]): void;
    restoreMultisigTransaction(sign: string): Promise<Transaction>;
    close(storeWallet?: boolean): Promise<void>;
    // Personal wallet methods
    getTxKey(txId: string): string;
    checkTxKey(options: {
      txId: string, 
      txKey: string, 
      address: string,
      }): TxKeyCheck;
    getTxProof(options: {
      txId: string,
      address: string,
      message?: string,
      }): string;
    checkTxProof(options: {
      txId: string, 
      address: string, 
      signature: string, 
      message?: string,
      }): TxProofCheck;
    getSpendProof(options: {
      txId: string, 
      message?: string,
      }): string;
    checkSpendProof(options: {
      txId: string, 
      message?: string, 
      signature: string,
      }): boolean;
    getReserveProof(options: {
      all: boolean, 
      accountIndex: number, 
      amount: string, 
      message?: string,
    }): string;
    checkReserveProof(options: {
      address: string, 
      signature: string, 
      message?: string,
    }): reserveProofCheck;
    signMessage(message: string): string;
    verifySignedMessage(message: string, address: string, signature: string): boolean;
    // Multisig wallet methods
    signMultisigParticipant(message: string): string;
  }

  function setupLog(level: 0 | 1 | 2 | 3 | 4, output?: string): void;
  function createWallet(options: {
    path: string,
    password: string,
    network?: Network,
    daemonAddress: string,
    language?: Language,
  }): Promise<Wallet>;
  function createWalletFromKeys(options: {
    path: string,
    password: string,
    network?: Network,
    daemonAddress: string,
    language?: Language,
  }): Promise<Wallet>;
  function openWallet(options: {
    path: string,
    password: string,
    network?: Network,
    daemonAddress: string,
    language?: Language,
  }): Promise<Wallet>;
  function recoveryWallet(options: {
    path: string,
    password: string,
    network?: Network,
    daemonAddress: string,
    mnemonic: string,
    restoreHeight?: number,
  }): Promise<Wallet>;
  function walletExists(path: string): boolean;
  function genPaymentId(): string;
  function paymentIdValid(paymentId: string): boolean;
  function addressValid(address: string): boolean;
}
