rem 创建者：黄志丹 great3779@sina.com 20120301
rem @echo off
rem 先编译bjam
start bootstrap.bat

rem 等待一分钟待bjam编译完成（如果电脑性能过差，可以设置等待时间更长一些）
SET SLEEP=ping 127.0.0.1 -n
%SLEEP% 60 > nul

rem 利用bjam编译boost库
rem 在此，我们编译vs2008和vs2010下的x86及x64共4类boost库文件

rem 建立库文件夹
rem vs2008 win32库文件夹
md stage\lib\win32\vs9_0\

rem vs2008 x64库文件夹
md stage\lib\x64\vs9_0\

rem vs2010 win32库文件夹
md stage\lib\win32\vs10_0\

rem vs2010 x64库文件夹
md stage\lib\x64\vs10_0\

rem ******************************************************************
rem 先删除lib下所有文件（不删除文件夹）
del stage\lib\*.* /Q

rem 编译vs2008 win32库文件
bjam --toolset=msvc-9.0 architecture=x86 --with-system --with-thread --with-date_time --with-filesystem --with-serialization

rem 拷贝至\stage\lib\win32\vs9_0
copy stage\lib\*.* stage\lib\win32\vs9_0\*.* /Y
rem ##################################################################

rem ******************************************************************
rem 先删除lib下所有文件（不删除文件夹）
del stage\lib\*.* /Q

rem 编译vs2008 x64库文件
bjam --toolset=msvc-9.0 architecture=x86 address-model=64 --with-system --with-thread --with-date_time --with-filesystem --with-serialization

rem 拷贝至\stage\lib\x64\vs9_0
copy stage\lib\*.* stage\lib\x64\vs9_0\*.* /Y
rem ##################################################################

rem ******************************************************************
rem 先删除lib下所有文件（不删除文件夹）
del stage\lib\*.* /Q

rem 编译vs2010 win32库文件
bjam stage --toolset=msvc-10.0 architecture=x86 --with-system --with-thread --with-date_time --with-filesystem --with-serialization --with-regex --with-chrono --with-locale --with-timer

rem bjam stage --toolset=msvc-10.0 architecture=x86 --without-graph --without-graph_parallel 
rem --without-math --without-mpi --without-python --without-serialization --without-wave link=static 
rem runtime-link=shared runtime-link=static threading=multi variant=debug,release

rem 拷贝至\stage\lib\win32\vs10_0
copy stage\lib\*.* stage\lib\win32\vs10_0\*.* /Y
rem ##################################################################

rem ******************************************************************
rem 先删除lib下所有文件（不删除文件夹）
del stage\lib\*.* /Q

rem 编译vs2010 x64库文件
bjam --toolset=msvc-10.0 architecture=x86 address-model=64 --with-system --with-thread --with-date_time --with-filesystem --with-serialization

rem 拷贝至\stage\lib\x64\vs10_0
copy stage\lib\*.* stage\lib\x64\vs10_0\*.* /Y
rem ##################################################################

rem 删除lib下所有文件（不删除文件夹）
del stage\lib\*.* /Q

rem boost_1_49_0共有21个库需要编译使用，分别是chrono, date_time, exception, filesystem, graph, graph_parallel, iostreams, locale, math, 
rem mpi, program_options, python, random, regex, serialization, signals, system, test, thread, timer, wave。
rem 我仅选用了自己常用的几个做以上编译示例，其他使用者可以根据自己的需求选择编译。全部编译boost大概需要1个小时以上（视机器性能）
rem 全部编译boost的命令如下：bjam --toolset=msvc-9.0 --build-type=complete stage