@echo off
rem Visual Studio でのビルド用バッチファイル
rem C++ によるデスクトップ開発 をインストールしていればエディションは問いません

rem 対応するコンパイラのバージョン
rem  - Visual Studio 2026
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
set __PACKETDEF__=/D "PACKETVER=20260107" /D "NEW_006b"

rem ----------------------------------------------------------------
rem コンパイラ設定

rem ---- Visual Studio 2026 / 2022 64bitコンパイル の設定 (全検索対応)
set "VCVARS_PATH="
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

rem 1. vswhere.exe で最新の C++ コンパイラ搭載環境を検索
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        if exist "%%i\VC\Auxiliary\Build\vcvars64.bat" set "VCVARS_PATH=%%i\VC\Auxiliary\Build\vcvars64.bat"
    )
)

rem 2. vswhere で見つからない場合、標準フォルダ配下（Preview / BuildTools含む）を直接探索
if "%VCVARS_PATH%"=="" (
    for /f "delims=" %%f in ('dir /b /s "%ProgramFiles%\Microsoft Visual Studio\vcvars64.bat" 2^>nul') do (
        set "VCVARS_PATH=%%f"
    )
)

rem 3. x86 Program Files 側も念のため探索
if "%VCVARS_PATH%"=="" (
    for /f "delims=" %%f in ('dir /b /s "%ProgramFiles(x86)%\Microsoft Visual Studio\vcvars64.bat" 2^>nul') do (
        set "VCVARS_PATH=%%f"
    )
)

if "%VCVARS_PATH%"=="" (
    echo [ERROR] PC内に vcvars64.bat が1つも見つかりませんでした。
    echo Visual Studio Installer の「変更」から「C++ によるデスクトップ開発」を追加してください。
    pause
    exit /b 1
)

echo Visual Studio 環境をロード中: %VCVARS_PATH%
call "%VCVARS_PATH%"
set __BITTYPE__=x64
rem ---- Visual Studio 設定ここまで

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

rem モード選択:
rem   MODERN : 現代の Intel / AMD CPU 両対応（通常・本番ビルド用）
rem   DEV    : 高速コンパイル＆厳格チェック（AI開発・テスト用）
set _model_=MODERN

rem 現代の Intel / AMD 64bit CPU 共通最適化（AVX2有効）
if "%_model_%"=="MODERN" set __cpu__=/c /O2 /arch:AVX2 /MP /W3 /TC /Zi

rem AI開発・高速ビルド用（最適化なし・警告強化）
if "%_model_%"=="DEV" set __cpu__=/c /Od /MP /W4 /TC /Zi

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