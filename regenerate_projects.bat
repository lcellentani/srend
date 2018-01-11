rmdir /S /Q .build
mkdir .build
cd .build
cmake .. -G "Visual Studio 15"
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv" srend.sln
cd ..
pause