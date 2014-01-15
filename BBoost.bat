rem �����ߣ���־�� great3779@sina.com 20120301
rem @echo off
rem �ȱ���bjam
start bootstrap.bat

rem �ȴ�һ���Ӵ�bjam������ɣ�����������ܹ���������õȴ�ʱ�����һЩ��
SET SLEEP=ping 127.0.0.1 -n
%SLEEP% 60 > nul

rem ����bjam����boost��
rem �ڴˣ����Ǳ���vs2008��vs2010�µ�x86��x64��4��boost���ļ�

rem �������ļ���
rem vs2008 win32���ļ���
md stage\lib\win32\vs9_0\

rem vs2008 x64���ļ���
md stage\lib\x64\vs9_0\

rem vs2010 win32���ļ���
md stage\lib\win32\vs10_0\

rem vs2010 x64���ļ���
md stage\lib\x64\vs10_0\

rem ******************************************************************
rem ��ɾ��lib�������ļ�����ɾ���ļ��У�
del stage\lib\*.* /Q

rem ����vs2008 win32���ļ�
bjam --toolset=msvc-9.0 architecture=x86 --with-system --with-thread --with-date_time --with-filesystem --with-serialization

rem ������\stage\lib\win32\vs9_0
copy stage\lib\*.* stage\lib\win32\vs9_0\*.* /Y
rem ##################################################################

rem ******************************************************************
rem ��ɾ��lib�������ļ�����ɾ���ļ��У�
del stage\lib\*.* /Q

rem ����vs2008 x64���ļ�
bjam --toolset=msvc-9.0 architecture=x86 address-model=64 --with-system --with-thread --with-date_time --with-filesystem --with-serialization

rem ������\stage\lib\x64\vs9_0
copy stage\lib\*.* stage\lib\x64\vs9_0\*.* /Y
rem ##################################################################

rem ******************************************************************
rem ��ɾ��lib�������ļ�����ɾ���ļ��У�
del stage\lib\*.* /Q

rem ����vs2010 win32���ļ�
bjam stage --toolset=msvc-10.0 architecture=x86 --with-system --with-thread --with-date_time --with-filesystem --with-serialization --with-regex --with-chrono --with-locale --with-timer

rem bjam stage --toolset=msvc-10.0 architecture=x86 --without-graph --without-graph_parallel 
rem --without-math --without-mpi --without-python --without-serialization --without-wave link=static 
rem runtime-link=shared runtime-link=static threading=multi variant=debug,release

rem ������\stage\lib\win32\vs10_0
copy stage\lib\*.* stage\lib\win32\vs10_0\*.* /Y
rem ##################################################################

rem ******************************************************************
rem ��ɾ��lib�������ļ�����ɾ���ļ��У�
del stage\lib\*.* /Q

rem ����vs2010 x64���ļ�
bjam --toolset=msvc-10.0 architecture=x86 address-model=64 --with-system --with-thread --with-date_time --with-filesystem --with-serialization

rem ������\stage\lib\x64\vs10_0
copy stage\lib\*.* stage\lib\x64\vs10_0\*.* /Y
rem ##################################################################

rem ɾ��lib�������ļ�����ɾ���ļ��У�
del stage\lib\*.* /Q

rem boost_1_49_0����21������Ҫ����ʹ�ã��ֱ���chrono, date_time, exception, filesystem, graph, graph_parallel, iostreams, locale, math, 
rem mpi, program_options, python, random, regex, serialization, signals, system, test, thread, timer, wave��
rem �ҽ�ѡ�����Լ����õļ��������ϱ���ʾ��������ʹ���߿��Ը����Լ�������ѡ����롣ȫ������boost�����Ҫ1��Сʱ���ϣ��ӻ������ܣ�
rem ȫ������boost���������£�bjam --toolset=msvc-9.0 --build-type=complete stage