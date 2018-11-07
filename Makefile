SAFEX_BRANCH?="develop"
SAFEX_BUILD_TYPE?=Debug

BOOST_VERSION=1.66.0
BOOST_DIRNAME=boost_1_66_0

PWD=${shell pwd}
BOOST_LIBS=chrono,date_time,filesystem,program_options,regex,serialization,system,thread,locale
THREADS?=6

.PHONY: all
all: binding.gyp deps
	node_modules/.bin/node-pre-gyp configure build

.PHONY: clean
clean:
	rm -rf boost
	rm -rf safexcore/build
	rm -rf ${BOOST_DIRNAME}
	rm -rf deps
	rm -rf build
	rm -rf lib

${BOOST_DIRNAME}.tar.bz2: 
	curl -L -o "${BOOST_DIRNAME}.tar.bz2" \
            http://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/${BOOST_DIRNAME}.tar.bz2/download

${BOOST_DIRNAME}: ${BOOST_DIRNAME}.tar.bz2
	tar xf ${BOOST_DIRNAME}.tar.bz2
	cd ${BOOST_DIRNAME} && ./bootstrap.sh --with-libraries=${BOOST_LIBS}

boost: ${BOOST_DIRNAME}
	cd ${BOOST_DIRNAME} && ./b2 -j4 cxxflags=-fPIC cflags=-fPIC -a link=static \
		threading=multi threadapi=pthread --prefix=${PWD}/boost install

.PHONY: deps
deps: boost safexcore/build
	mkdir -p deps
	cp boost/lib/*.a deps

safexcore:
	git clone --depth 1 --recurse-submodules -b ${SAFEX_BRANCH} https://github.com/safex/safexcore
	cp safexcore/src/wallet/api/wallet_api.h include
	
safexcore/build: boost safexcore
	mkdir -p safexcore/build
	mkdir -p deps
	cd safexcore/build && cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_GUI_DEPS=ON -DEMBEDDED_WALLET=1 \
		-DBUILD_TESTS=OFF -DSTATIC=ON -DBOOST_ROOT=${PWD}/boost \
		-DCMAKE_BUILD_TYPE=${SAFEX_BUILD_TYPE} \
		-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true \
		-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PWD}/deps \
		..

	cd safexcore/build && make -j${THREADS} wallet_merged epee easylogging lmdb unbound VERBOSE=1
	cp safexcore/build/lib/libwallet_merged.a ${PWD}/deps
