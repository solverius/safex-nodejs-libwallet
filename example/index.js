const safex = require('safex-nodejs-libwallet');
const path = require('path');

const fs = require('fs');
if(fs.existsSync('index.log')) 
	fs.unlinkSync('index.log')
	
safex.setupLog(3, "index.log");

var wallet;
var sent = false;
const wallet_path = path.join(__dirname, 'test-wallet');
// const wallet_path = "C:/Users/marko/balkaneum/temp/test-wallet";
// const wallet_path = "test-wallet";

// var args = {
//     'path': wallet_path,
// 	'password': '123',
// 	'network': 'testnet',
// 	'daemonAddress': '192.168.1.194:29393',
// 	'restoreHeight': 0,
// 	'mnemonic' : 'nifty inflamed against focus gasp ethics spying gulp tiger cogs evicted cohesive woken nylon erosion tell saved fatal alkaline acquire lemon maps hull imitate saved'
// }


var args = {
    'path': wallet_path,
 	'password': '123',
	'network': "testnet",
 	'daemonAddress': '127.0.0.1:38001',
 	'restoreHeight': 0,
    'addressString':  'SFXtzR3hzrNfCpTAgJFfQyAoHHLhLhw53DLuWYSk3pz2adF7WQqdYJURUCptBkrR8WRmdsY1oVZX7j2QXerkynJ2iDzPsu68q9V',
    'viewKeyString':  '9fde8d863a3040ff67ccc07c49b55ee4746d4db410fb18bdde7dbd7ccba4180e',
    'spendKeyString':  'e6887bea1e8126e8160ceef01ec35c81dd3e86e9d0e7e3c47087c113731ae508'
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
                console.log("block chain height: " + wallet.blockchainHeight());
                console.log("daemon block chain height: " + wallet.daemonBlockchainHeight());

        var offrs = wallet.listSafexOffers(true);
		for (i in offrs){
				console.log("Safex offer " +i+" title: " + offrs[i].title);
				console.log("Safex offer description: "  + offrs[i].description);
				console.log("Safex offer quantity: " + offrs[i].quantity);
				console.log("Safex offer price: " + offrs[i].price);
				console.log("Safex offer minSfxPrice: " + offrs[i].minSfxPrice);
				console.log("Safex offer pricePegUsed: " + offrs[i].pricePegUsed);
				console.log("Safex offer pricePegID: " + offrs[i].pricePegID);
				console.log("Safex offer seller: " + offrs[i].seller);
				console.log("Safex offer active: " + offrs[i].active);
				console.log("Safex offer offerID: " + offrs[i].offerID);
				console.log("Safex offer currency: " + offrs[i].currency);
			}


    }

    setTimeout(nextTick, 10000);
}
var lastHeight = 0;
promise
	.then((w) => {
		console.log("address: " + w.address());
		console.log("seed: " + w.seed());

		wallet = w;

		if (wallet) {
			var r = wallet.createSafexAccount("test", "Test account");

			if(r)
				console.log("Created acccount test");
			else
				console.log("Didn't create account test");



			r = wallet.recoverSafexAccount("igor1","0d6fc00c0d3321eec2e15710046c6e95a581ebbe5866580d6a07f818649c5b0d");
			if(r)
				console.log("Safex account recovered");
			else
				console.log("Didn't recover safex account");


			r = wallet.removeSafexAccount("test");
			if(r)
				console.log("Safex account removed");
			else
				console.log("Didn't remove safex account");



		}

		wallet.on('newBlock', function (height) {
			if(height-lastHeight>100) {
				console.log("blockchain updated, height: " + height);
				lastHeight = height;
			}
		});

		wallet.on('refreshed', function () {
			console.log("wallet refreshed");

			accs = wallet.getSafexAccounts();

			for (i in accs){
				console.log("Safex account " +i+" username: " + accs[i].username);
				console.log("Safex account data: " 		   + accs[i].data);
				console.log("Safex account public key: " + accs[i].publicKey);
				console.log("Safex account secret key: " + accs[i].privateKey);
				console.log("Safex account status: " + accs[i].status);
			}

			wallet.store()
				.then(() => {console.log("wallet stored")})
				.catch((e) => {console.log("unable to store wallet: " + e)})

			if (!sent) {
				sent = true;
				// //cash
                // wallet.createTransaction({
                //     'address': 'SFXtzT37s8jWtjUx8kfWD24PU2mMLqYkt7DQ3KzJKC7B3pp67XFpFJhiEvwTe1DX9gT7nWcYZQRt7UWnEoWDcjmLdegfWoLVZwY',
                //     'amount': '21300000000', //send 2.13 cash
                // }).then((tx) => {
                //     console.log("cash transaction created: " + tx.transactionsIds());
                //
                //     tx.commit().then(() => {
                //         console.log("cash transaction commited successfully");
                //     }).catch((e) => {
                //         console.log("error on commiting transaction: " + e);
                //     });
                // }).catch((e) => {
                //     sent = false;
                //     console.log("couldn't create transaction: " + e);
                // });
                //
                //token
				// wallet.createTransaction({
				// 	'address': 'SFXtzT37s8jWtjUx8kfWD24PU2mMLqYkt7DQ3KzJKC7B3pp67XFpFJhiEvwTe1DX9gT7nWcYZQRt7UWnEoWDcjmLdegfWoLVZwY',
				// 	'amount': '30000000000', //send 3 tokens
				// 	'tx_type': 1 //token transaction
				// }).then((tx) => {
				// 	console.log("token transaction created: " + tx.transactionsIds());
				//
				// 	tx.commit().then(() => {
				// 		console.log("transaction commited successfully");
				// 	}).catch((e) => {
				// 		console.log("error on commiting transaction: " + e);
				// 	});
				// }).catch((e) => {
				// 	sent = false;
				// 	console.log("couldn't create transaction: " + e);
				// });
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
