const safex = require('safex-nodejs-libwallet');
const path = require('path');

const fs = require('fs');
if(fs.existsSync('index.log'))
	fs.unlinkSync('index.log')

safex.setupLog(3, "index.log");

var wallet;
var sent = false;
const wallet_path = path.join(__dirname, 'test-wallet');

var args = {
	'path': wallet_path,
	'password': '123',
	'network': "testnet",
	'daemonAddress': '127.0.0.1:29393',
	'restoreHeight': 195000,
	'addressString':  'SFXtzS7mTDsA9Sgp8D9EfDJq1bqQtc6TVckF8QP94QH6XkUKs8WZq7D6fcR6DtvoCdUch3y5FdxT1NH3gnE2symR7mvo5aYXTkp',
	'viewKeyString':  '15593dfcbd182ce6640700593ae2a9fa722ff1edd2acaba14be13b2c16916c0a',
	'spendKeyString':  '1d60513005ed051dec31aa8d22dbd8f2c1bd6171d504aa485a487940ef49a10b'
}

if (!safex.walletExists(wallet_path)) {
	console.log("wallet doesn't exist. creating new one: " + wallet_path);
	if(args.mnemonic)
		promise = safex.recoveryWallet(args);
	else if(args.addressString)
		promise = safex.createWalletFromKeys(args);
	else
		promise = safex.createWallet(args);
} else {
	console.log("wallet already exists. opening: " + wallet_path);
	promise = safex.openWallet(args);
}

const nextTick = () => {
    if (wallet) {
		console.log("address: " + wallet.address());
		console.log("balance: " + wallet.balance());
		console.log("unlocked balance: " + wallet.unlockedBalance());
        console.log("token balance: " + wallet.tokenBalance());
        console.log("unlocked token balance: " + wallet.unlockedTokenBalance());
		console.log("seed: " + wallet.seed());
		console.log("secret view key: " + wallet.secretViewKey());
		console.log("secret spend key: " + wallet.secretSpendKey());
		console.log("public view key: " + wallet.publicViewKey());
		console.log("public spend key: " + wallet.publicSpendKey());
    }

    setTimeout(nextTick, 10000);
}
var lastHeight = 0;
promise
	.then((w) => {
		console.log("address: " + w.address());
		console.log("seed: " + w.seed());

		wallet = w;
		wallet.on('newBlock', function (height) {
			if(height-lastHeight>1000) {
				console.log("blockchain updated, height: " + height);
				lastHeight = height;
			}
		});

		wallet.on('refreshed', function () {
			console.log("wallet refreshed");

			wallet.store()
				.then(() => {console.log("wallet stored")})
				.catch((e) => {console.log("unable to store wallet: " + e)})

			if (!sent) {
				sent = true;

				var r = wallet.createSafexAccount("test", "Test account",args["password"]);

				if(r)
					console.log("Created acccount test");
				else
					console.log("Didn't create account test");

				wallet.createAdvancedTransaction({
					'address': 'SFXtzS7mTDsA9Sgp8D9EfDJq1bqQtc6TVckF8QP94QH6XkUKs8WZq7D6fcR6DtvoCdUch3y5FdxT1NH3gnE2symR7mvo5aYXTkp', // testnet
					'amount': '1', //safex atomic units as string
					'tx_type': '6',
					'safex_username': 'test',
				}).then((tx) => {
					console.log("transaction created: " + tx.transactionsIds());
					
					tx.commit().then(() => {
						console.log("transaction commited successfully");
					}).catch((e) => {
						console.log("error on commiting transaction: " + e);
                    });
                    return wallet;
				}).catch((e) => {
					sent = false;
					console.log("couldn't create transaction: " + e);
				})
			}
		});

		wallet.on('updated', function () {
			console.log("updated");
		});

		wallet.on('unconfirmedMoneyReceived', function(tx, amount) {
			console.log("unconfirmed money received. tx: " + tx + ", amount: " + amount);
		});

		wallet.on('moneyReceived', function(tx, amount) {
			console.log("money received. tx: " + tx + ", amount: " + amount);
		});

		wallet.on('moneySpent', function(tx, amount) {
			console.log("money spent. tx: " + tx + ", amount: " + amount);
		});

        wallet.on('unconfirmedTokensReceived', function(tx, token_amount) {
            console.log("unconfirmed tokens received. tx: " + tx + ", token amount: " + token_amount);
        });

        wallet.on('tokensReceived', function(tx, token_amount) {
            console.log("tokens received. tx: " + tx + ", token amount: " + token_amount);
        });

        wallet.on('tokensSpent', function(tx, token_amount) {
            console.log("tokens spent. tx: " + tx + ", token amount: " + token_amount);
        });

		nextTick();
	})
	.catch((e) => {
		console.log("no luck tonight: " + e);
	});
