if [ $build_type = "windows" ]
then
	make deps-win && cmake-js build --CDARCH="x86-64" --CDBUILD_WIN=ON --CDBUILD_64=ON --arch=x86_64 --CDCMAKE_TOOLCHAIN_FILE=./safexcore/contrib/depends/x86_64-w64-mingw32/share/toolchain.cmake -m
else
	make deps && cmake-js build --CDARCH='x86-64' --CDBUILD_WIN=OFF --CDBUILD_64=ON --arch=x86_64 -m;
fi