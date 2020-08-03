SAFEX_BRANCH?="node_wallet"
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
	rm -rf deps
	rm -rf build
	rm -rf lib


${BOOST_DIRNAME}:
	git clone --depth 1 -b boost-1.66.0 \
	--recurse-submodules --shallow-submodules \
	https://github.com/boostorg/boost.git ${BOOST_DIRNAME}
	cd ${BOOST_DIRNAME}/tools/build && git apply ../../../mac_fix.patch

boost: ${BOOST_DIRNAME}
	cd ${BOOST_DIRNAME} && ./bootstrap.sh --prefix=${PWD}/boost
	cd ${BOOST_DIRNAME} && ./b2 headers
	cd ${BOOST_DIRNAME} && ./b2 -j4 cxxflags=-fPIC cflags=-fPIC -a link=static \
		threading=multi threadapi=pthread install
	mkdir -p boost/include && cp -L -R ${BOOST_DIRNAME}/boost/ boost/include/  

libnode-win:
	mkdir -p deps
	cd libnode && wget https://nodejs.org/dist/v13.8.0/win-x64/node.exe
	cd libnode && x86_64-w64-mingw32-dlltool -d node.def -y libnode.a
	cp libnode/libnode.a ${PWD}/deps

.PHONY: deps
deps: boost safexcore-build
	mkdir -p deps
	cp boost/lib/*.a deps

deps-win: libnode-win safexcore-win-build
	mkdir -p deps

safexcore:
	git clone --depth 1 -b develop --recurse-submodules https://github.com/VanGrx/safexcore
	cp safexcore/src/wallet/api/wallet_api.h include


#windows
safexcore-win-build: libnode-win safexcore
	mkdir -p safexcore/build
	cd safexcore/contrib/depends && $(MAKE) HOST=x86_64-w64-mingw32
	cd safexcore/build && cmake -DARCH="x86-64" \
	 -DBUILD_64=ON  -DBUILD_SHARED_LIBS=OFF -DBUILD_GUI_DEPS=OFF -DBUILD_TESTS=OFF -DSTATIC=ON -DBOOST_ROOT=${PWD}/boost -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true \
	  -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=TRUE -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PWD}/deps -DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/safexcore/contrib/depends/x86_64-w64-mingw32/share/toolchain.cmake ..
	cd safexcore/build && make wallet_api -j${THREADS}
	cp safexcore/build/lib/libwallet_api.dll.a ${PWD}/deps
	cp safexcore/build/src/wallet/api/libwallet_api.dll ${PWD}/deps	

#linux, mac
safexcore-build: boost safexcore
	mkdir -p safexcore/build
	cd safexcore/build && cmake -DBUILD_SHARED_LIBS=OFF -DBUILD_GUI_DEPS=ON \
		-DBUILD_TESTS=OFF -DSTATIC=ON -DBOOST_ROOT=${PWD}/boost \
		-DARCH="x86-64" -D \
		-DBUILD_64=ON -D \
		-DCMAKE_BUILD_TYPE=release \
		-DCMAKE_BUILD_TYPE=${SAFEX_BUILD_TYPE} \
		-DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true \
		-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PWD}/deps \
		..

	cd safexcore/build && make -j${THREADS} wallet_merged epee easylogging lmdb unbound VERBOSE=1
	cp safexcore/build/lib/libwallet_merged.a ${PWD}/deps



