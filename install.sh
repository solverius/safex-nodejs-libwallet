if [ $build_type = "windows" ]
then
	make deps-win && cmake-js build --CDARCH="x86-64" --CDBUILD_WIN=ON --CDBUILD_64=ON --arch=x86_64 --CDCMAKE_TOOLCHAIN_FILE=./safexcore/contrib/depends/x86_64-w64-mingw32/share/toolchain.cmake -m;
	mkdir -p lib/binding;
	cp build/safex.node lib/binding/;
	cp deps/libwallet_api.dll lib/binding/
	cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll lib/binding/
	cp /usr/lib/gcc/x86_64-w64-mingw32/7.3-posix/libgcc_s_seh-1.dll lib/binding/
	cp /usr/lib/gcc/x86_64-w64-mingw32/7.3-posix/libstdc++-6.dll lib/binding/
else
	make deps && cmake-js build --CDARCH='x86-64' --CDBUILD_WIN=OFF --CDBUILD_64=ON --arch=x86_64 -m;
	mkdir -p lib/binding;
	cp build/Release/safex.node lib/binding/;
fi