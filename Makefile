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
	rm -rf ${BOOST_DIRNAME}
	rm -rf deps
	rm -rf build
	rm -rf lib


${BOOST_DIRNAME}:
	git clone --depth 1 -b boost-1.66.0 \
	--recurse-submodules=libs/chrono \
	--recurse-submodules=libs/date_time \
	--recurse-submodules=libs/filesystem \
	--recurse-submodules=libs/program_options \
	--recurse-submodules=libs/regex \
	--recurse-submodules=libs/serialization \
	--recurse-submodules=libs/system \
	--recurse-submodules=libs/thread \
	--recurse-submodules=libs/locale \
	--recurse-submodules=libs/config \
	--recurse-submodules=tools \
	https://github.com/boostorg/boost.git ${BOOST_DIRNAME}
	cd ${BOOST_DIRNAME} && ./bootstrap.sh --with-libraries=${BOOST_LIBS}

boost: ${BOOST_DIRNAME}
	cd ${BOOST_DIRNAME} && ./b2 -j4 cxxflags=-fPIC cflags=-fPIC -a link=static \
		threading=multi threadapi=pthread --prefix=${PWD}/boost install


.PHONY: deps
deps: boost safexcore/build
	mkdir -p deps
	cp boost/lib/*.a deps

safexcore:
	git clone --depth 1 -b develop --recurse-submodules https://github.com/VanGrx/safexcore
	cp safexcore/src/wallet/api/wallet_api.h include
	
ifeq ($(OS),Windows_NT)
#windows
safexcore/build: boost safexcore
	mkdir -p safexcore/build
	mkdir -p deps
	cp safexcore/src/wallet/api/win_wrapper/windows_wrapper.h include
	cd safexcore/build && cmake -G "MSYS Makefiles" -DBUILD_TAG="win-x64" -DCMAKE_TOOLCHAIN_FILE=../cmake/64-bit-toolchain.cmake -DMSYS2_FOLDER=c:/msys64 -DARCH="x86-64" \
	 -DBUILD_64=ON  -DBUILD_SHARED_LIBS=OFF -DBUILD_GUI_DEPS=ON -DBUILD_TESTS=OFF -DSTATIC=ON -DBOOST_ROOT=${PWD}/boost -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true \
	  -DBUILD_WIN_WALLET_WRAPPER=ON -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PWD}/deps ..
	cd safexcore/build && make -j${THREADS}
	cp safexcore/build/src/wallet/api/win_wrapper/libwin_wallet_wrapper.* ${PWD}/deps
	cd deps &&  '${PWD}/lib.exe' /machine:x64 /def:libwin_wallet_wrapper.def
		
else
#linux, mac
safexcore/build: boost safexcore
	mkdir -p safexcore/build
	mkdir -p deps
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

endif

