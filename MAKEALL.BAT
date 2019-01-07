@echo off

echo kernel making...
cd kernel
make
cd..
copy elara.exe host
del elara.exe

echo klib making...
cd klib
make
cd..

echo mlib making...
cd mlib
make
cd..

echo make Elara-HS
cd host
cd src

echo ulib making...
cd ulib
make
cd..

echo module-set making...
make
cd..

echo make end

