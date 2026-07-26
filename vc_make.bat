@echo off
rem Visual Studio でのビルド用バッチファイル

rem 対応するコンパイラのバージョン
rem  - Visual Studio 2022

rem ----------------------------------------------------------------
rem 共通設定
echo vc_make.bat - Auriga makefile for Visual C++
set __BITTYPE__=x64

rem ----------------------------------------------------------------
rem パケット定義
rem 2020-02-05aRagexeRE: 20200205:
rem 2025-03-19_Ragexe_1742361965: 20250319
rem 2025-06-04_Ragexe_1748494356: 20250604
rem 2025-07-16_Ragexe_175220998: 20250716
rem 2026-01-07_Ragexe_1767686776: 20260107
set __PACKETDEF__=/D "PACKETVER=20250716" /D "NEW_006b"

rem ----------------------------------------------------------------
rem コンパイラ設定

rem ---- Visual Studio 2022 64bitコンパイル の設定
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set __BITTYPE__=x64
rem ---- Visual Studio 2022 64bitコンパイル の設定ここまで

rem ----------------------------------------------------------------
rem SQL の設定 / 必要ならコメントアウトをはずす
rem set INCLUDE=C:\Program Files\MySQL\MySQL Server 5.5\include;%INCLUDE%
rem set LIB=C:\Program Files\MySQL\MySQL Server 5.5\lib;%LIB%

rem ----------------------------------------------------------------
rem ビルドオプションの選択

rem データ保存方法の選択 ： SQL にするならコメントアウトする
set __TXT_MODE__=/D "TXT_ONLY"

rem データ保存方法が TXT の時、ジャーナルを使うならコメントアウトをはずす
rem set __TXT_MODE__=/D "TXT_ONLY" /D "TXT_JOURNAL"

rem データ保存方法が SQL の時、txt-converter が不要ならコメントアウトをはずす
rem set __TXTCONVERTER__=SKIP

rem zlib.dllをコンパイルするならコメントアウトをはずす
set __ZLIB__=/D "LOCALZLIB"

rem login_id2 や IP で AUTHFIFO を比較する場合はコメントアウトをはずす
rem set __CMP_AFL2__=/D "CMP_AUTHFIFO_LOGIN2"
rem set __CMP_AFIP__=/D "CMP_AUTHFIFO_IP"

rem httpd を完全に無効にする場合コメントアウトをはずす
rem set __NO_HTTPD__=/D "NO_HTTPD"

rem httpd で外部 CGI を使う場合はコメントアウトする
set __NO_HTTPD_CGI__=/D "NO_HTTPD_CGI"

rem csvdb のスクリプトからの利用を無効にする場合コメントアウトをはずす
rem set __NO_CSVDB_SCRIPT__=/D "NO_CSVDB_SCRIPT"

rem R化前のシステムを使う場合はコメントアウトをはずす
rem set __PRE_RENEWAL__=/D "PRE_RENEWAL"

rem MB を使う場合はコメントアウトをはずす
rem set __EXCLASS__=/D "CLASS_MB"

rem 動的にMOBの sc_data を確保する場合はコメントアウトをはずす
set __DYNAMIC_STATUS_CHANGE__=/D "DYNAMIC_SC_DATA"

rem キャラの削除にメールアドレスを使う場合はコメントアウトをはずす
rem set __AC_MAIL__=/D "AC_MAIL"

rem キャラの削除に誕生日を使う場合はコメントアウトをはずす
rem set __AC_BIRTHDATE__=/D "AC_BIRTHDATE"

rem ステータス異常データの保存を無効にする場合はコメントアウトをはずす
rem set __NO_SCDATA_SAVING__=/D "NO_SCDATA_SAVING"

rem タイマーをキャッシュするならコメントアウトをはずす
rem set __TIMER_CACHE__=/D "TIMER_CACHE=256"

rem ---------------------------
rem コンパイルオプション設定

@rem CPU最適化スイッチ(By Nameless)
@rem 以下の例を参考にスイッチ名を記入してください。
set _model_=EM64T

@rem 最適化なし
if "%_model_%"=="NOOPTIMIZE" set __cpu__=/c /W3 /Od /Zi

@rem CPUアーキテクチャ32BitCPU/64BitCPU
if "%_model_%"=="x32" set __cpu__=/c /W3 /O2 %__OPT_OP__% /GA /TC /Zi
if "%_model_%"=="x64" set __cpu__=/c /arch:SSE2 /W3 /O2 %__OPT_OP__% /GA /TC /Zi

@rem メモリー1024以上搭載の32bitCPU/64bitCPU
if "%_model_%"=="HiMemL" set __cpu__=/c /bigobj /W3 /O2 %__OPT_OP__% /GA /TC /Zi
if "%_model_%"=="HiMemH" set __cpu__=/c /bigobj /arch:SSE2 /W3 /O2 %__OPT_OP__% /GA /TC /Zi

@rem スタック制御をコンパイラで行う場合
if "%_model_%"=="Stac32" set __cpu__=/c /F4096 /W3 /O2 %__OPT_OP__% /GA /TC /Zi
if "%_model_%"=="Stac64" set __cpu__=/c /F4096 /arch:SSE2 /W3 /O2 %__OPT_OP__% /GA /TC /Zi
@rem AMD系64bitCPU用
if "%_model_%"=="A64x2" set __cpu__=/c /favor:blend /W3 /O2 %__OPT_OP__% /GA /TC /Zi
if "%_model_%"=="A64x1" set __cpu__=/c /favor:AMD64 /W3 /O2 %__OPT_OP__% /GA /TC /Zi

@rem Intel系64bitCPU用
if "%_model_%"=="EM64T" set __cpu__=/c /favor:EM64T /W3 /O2 %__OPT_OP__% /GA /TC /Zi


@rem 以下実験段階(人柱求む by Nameless)
@rem 暴走風味…32BitCPU最高速モード
if "%_model_%"=="mode01" set __cpu__=/c /fp:fast /F4096 /bigobj /W3 /Ox /GA /TC /Zi
@rem 暴走風味…64BitCPU最高速モード
if "%_model_%"=="mode02" set __cpu__=/c /arch:SSE2 /fp:fast /F4096 /bigobj /W3 /Ox /Gr /GA /TC /Zi
@rem 暴走風味…AMD 64x2 & FX系最適化・最高速
if "%_model_%"=="mode03" set __cpu__=/c /arch:SSE2 /fp:fast /F4096 /bigobj /favor:AMD64 /W3 /Ox /Gr /GA /TC /Zi
if "%_model_%"=="mode04" set __cpu__=/c /arch:SSE2 /fp:fast /F4096 /bigobj /favor:blend /W3 /Ox /Gr /GA /TC /Zi
@rem 暴走風味…Intel 64bitCPU用最適化・最高速
if "%_model_%"=="mode05" set __cpu__=/c /arch:SSE2 /fp:fast /F4096 /bigobj /favor:EM64T /W3 /Ox /Gr /GA /TC /Zi
@rem 以下リザーブ
if "%_model_%"=="mode06" set __cpu__=/c /W3 /Ox /Gr /GA /TC /Zi
if "%_model_%"=="mode07" set __cpu__=/c /W3 /Ox /Gr /GA /TC /Zi
if "%_model_%"=="mode08" set __cpu__=/c /W3 /Ox /Gr /GA /TC /Zi
if "%_model_%"=="mode09" set __cpu__=/c /W3 /Ox /Gr /GA /TC /Zi



rem ----------------------------------------------------------------
rem 最終的なビルドオプションを生成
if "%__ZLIB__%"=="" goto NOZLIB1
set __LINKZLIB__=../common/zlib/*.obj
:NOZLIB1

if "%__BITTYPE__%"=="x32" set __BITOPTION__=/D "WIN32" /D "_WIN32" /D "_WIN32_WINDOWS"
if "%__BITTYPE__%"=="x64" set __BITOPTION__=/D "WIN64" /D "_WIN64"

set __opt1__=/D "FD_SETSIZE=4096" /D "NDEBUG" /D "_CONSOLE" /D "_CRT_SECURE_NO_DEPRECATE" /D "WINDOWS" %__MULTIBUILD__% %__BITOPTION__% %__PACKETDEF__% %__TXT_MODE__% %__ZLIB__% %__CMP_AFL2__% %__CMP_AFIP__% %__NO_HTTPD__% %__NO_HTTPD_CGI__% %__NO_CSVDB_SCRIPT__% %__PRE_RENEWAL__% %__EXCLASS__% %__DYNAMIC_STATUS_CHANGE__% %__AC_MAIL__% %__AC_BIRTHDATE__% %__NO_SCDATA_SAVING__% %__TIMER_CACHE__%
set __opt2__=/DEBUG %__FIXOPT2__% user32.lib %__LINKZLIB__% ../common/lua/*.lib ../common/*.obj *.obj
set __include__=/I "../common/zlib/" /I "../common/lua/" /I "../common/"

if "%__TXT_MODE__%"=="" (set __dbmode__=sql) else (set __dbmode__=txt)

rem ----------------------------------------------------------------
rem 警告の抑制
rem   C4819 : 表示できない文字を含んでいます
set __warning__=/wd4819

rem ----------------------------------------------------------------
rem コンパイルオプションの表示

echo ■コンパイル情報表示■
echo ◆───────────────────────────────◆
echo [BITTYPE = %__BITTYPE__%]
echo [model = %_model_%]
echo [CompileOption = %__opt1__%]
echo ◆───────────────────────────────◆

rem ビルド作業本体

rem 共通コンポーネントのコンパイル
cd src\common\zlib
if "%__ZLIB__%"=="" goto NOZLIB2
echo zlibのコンパイル
cl %__warning__% %__cpu__% %__opt1__% %__include__% *.c

:NOZLIB2
echo luaのコンパイル
cd ..\lua
cl %__BITOPTION__% /D "_LIB" /c *.c
del lua.obj luac.obj
lib /out:lualib.lib *.obj

echo 共通コンポーネントのコンパイル
cd ..\
cl %__warning__% %__cpu__% %__opt1__% %__include__% *.c

rem サーバー本体のビルド
echo ログインサーバーコンパイル
cd ..\login
cl %__warning__% %__cpu__% %__opt1__% %__include__% *.c .\%__dbmode__%\*.c
link %__opt2__% /out:"../../login-server.exe"

echo キャラクターサーバーコンパイル
cd ..\char
cl %__warning__% %__cpu__% %__opt1__% %__include__% *.c .\%__dbmode__%\*.c
link %__opt2__% /out:"../../char-server.exe"

echo マップサーバーコンパイル
cd ..\map
cl %__warning__% %__cpu__% %__opt1__% %__include__% *.c .\%__dbmode__%\*.c
link %__opt2__% /out:"../../map-server.exe"

rem 必要なら txt-converter をビルド
if NOT "%__TXT_MODE__%"=="" goto NOCONVERTER1
if "%__TXTCONVERTER__%"=="SKIP" goto NOCONVERTER1

echo コンバーターコンパイル
cd ..\converter
cl %__warning__% %__cpu__% %__opt1__% %__include__% *.c
link %__opt2__% /out:"../../txt-converter.exe"
:NOCONVERTER1

cd ..\..\

rem 不必要なファイルを削除
echo オブジェクトファイル等のクリーンアップ
if "%__ZLIB__%"=="" goto NOZLIB3
del src\common\zlib\*.obj
:NOZLIB3
del src\common\lua\*.obj
del src\common\lua\*.lib
del src\common\*.obj
del src\char\*.obj
del src\login\*.obj
del src\map\*.obj
if NOT "%__TXT_MODE__%"=="" goto NOCONVERTER2
if "%__TXTCONVERTER__%"=="SKIP" goto NOCONVERTER2
del src\converter\*.obj
:NOCONVERTER2

rem 結果確認用の一時停止
pause
