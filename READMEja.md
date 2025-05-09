format クラス   
printf に似た仕様で数値から文字列へ変換する C++ クラス
=========

[英語版](README.md)

## 概要

 C++ では、C 言語で一般的な可変引数を扱う関数は使わない文化があります。   
   
 一番の問題は、引数の引渡しはスタックを経由する点、又、スタックに引数が何個格納   
 されているのか判らない点です、これによりスタックをオーバーフローさせ、システム   
 に悪影響を与える事が出来てしまいます。   
   
 代表的な実装は「printf」関数です。   
 コンパイラはフォーマット文を解析して、引数の整合性をチェックしますが、完全には   
 チェックできません。   
 一方、「printf」は優れた柔軟性をもたらし、文字、数値を扱う事をたやすくします。   
   
 boost には、printf の柔軟性と、安全性を考慮した、format.hpp があります。   
 
 - 「%」オペレーターのオーバーロード機構を利用して、複数の引数を受け取る事ができます
 - boost::format は優れた実装ですが、iostream に依存していて組み込みマイコンでは問題があります
 - iostream を取り込むと、容量が肥大化します

```sh
    std::cout << "Hello!" << std::endl;

   text    data     bss     dec     hex filename
 508864   47644    8812  565320   8a048 hello.elf

---
    printf("Hello!\n");

   text    data     bss     dec     hex filename
  13864      48    1924   15836    3ddc hello.elf

---
    utils::format("Hello!\n");

   text    data     bss     dec     hex filename
   6700      48    2136    8884    22b4 hello.elf
```

 そこで、boost::format の仕組みをまねて、printf に代わる format クラスを実装しました。   
   
---
## 仕様

- C++17 以降のコンパイラが必須です
- 基本的な仕様は「printf」に準拠しています
- パラメーターを渡す仕様は、boost::format とほぼ同じです
- printf と異なる部分、拡張された仕様もあります
- 名前空間は「utils」です
- 文字列の出力はファンクタを定義して、テンプレートパラメーターとします
   
※標準的なファンクタ「stdout_buffered_chaout」クラスが定義されており、以下のように   
typedef されています。

```C++
    typedef basic_format<stdout_buffered_chaout<256> > format;
    typedef basic_format<stdout_chaout> nformat;
    typedef basic_format<memory_chaout> sformat;
    typedef basic_format<null_chaout> null_format;
    typedef basic_format<size_chaout> size_format;
```

通常、整形された文字列は、標準出力「stdout」に出力されます。   
※標準では、256 バイトでバッファリングされたバッファ経由です。

バッファリングを行わない場合、「nformat」を利用出来ます。

メモリー上に文字列を出力する場合「sformat」を使います。
※sprintf に相当

文字列を捨てたい場合、「null_format」を使います。

出力されるサイズを知りたい場合、「size_format」を使います。

※上記のファンクタは、format.hpp に実装があります。

組み込みマイコンで使う事を考えて、エラーに関する処理では、「例外」を送出しません。
入力変換時に起こったエラーは、エラー種別として取得する事ができます。

- 一般的に、例外を使うと多くのメモリを消費します。
- 例外を使った場合、エラーが発生して、正しい受取先が無い場合、致命的な問題を引き起こします。
- 複数の変換で、エラーが同時に発生すると、最後のエラーが残ります。
- 頑丈な事、フォーマットの不整合や、間違った使い方で、クラッシュしたり無限ループに陥らない事。

---
## 使い方

### format.hpp をインクルードします。

```C++
#include "format.hpp"
```

全ての機能を使うのに必要なヘッダーは「format.hpp」のみです。

### 名前空間は「utils」です。

### サンプル

- 標準出力に「a」の内容を表示する。

```C++
    int a = 1000;
    utils::format("%d\n") % a;
```

- 文字列「res」に「a」の内容を出力する。

```C++
    int a = 1000;
    char res[64];
    utils::sformat("%d\n", res, sizeof(res)) % a;
```

- 変換過程でのエラーを検査する。

```C++
    int a = 1000;
    auto err = (utils::format("%d\n") % a).get_error();
    if(err == utils::error::none) {
        // OK!
    } else {
        // NG!
    }
```

- 出力文字サイズのみを取得

```C++
    int a = 1000;
    auto size = (utils::size_format("%d\n") % a).size();
```

- 固定小数点表示   
組み込みマイコンでは、A/D 変換された整数を正規化して表示したい場合などが多いものです。   
そこで、固定少数点表示を拡張機能として実装してあります。   
- 又、8/16 ビットマイコンなどでは、浮動小数点を扱うと極端にメモリを消費します。   
- 以下の例では、小数点以下１０ビットの場合。
- 表示桁（下の例では、小数点以下２桁）以降は四捨五入はされますが、ビット数が足りない場合は切り捨てられた値と同等になります。   
- 小数点以下３桁の表示が必要なら、四捨五入を考慮すると、１１ビットは必要です。

```C++
    uint16_t a = 1000;
    utils::format("%3.2:10y") % a;
```

---
・バッファリングされた文字を掃き出す
文字を処理する速度を上げる為、少し前から、バッファが設けられています。   
プロトタイプでは

```C++
typedef basic_format<stdout_buffered_chaout<256> > format;
```

となっていて、256 バイトのバッファです。   
通常、"\n"（改行）コードで、バッファがフラッシュされますが、それではタイミング的に困る場合があります。
もし、バッファの中身を掃き出したい場合は、以下のように明示的に指定します。

```C++
    utils::format::chaout().flush();
```
   
---
## カスタマイズ

8/16 ビットマイコンなどリソースの限られたプロジェクトで使う場合、カスタマイズする事が出来ます。   
以下の定義をする事で、使わない機能を追い出し、コード量を最適化できます。

```C++
// float を無効にする場合（８ビット系マイコンでのメモリ節約用）
// #define NO_FLOAT_FORM

// ２進表示をサポートしない場合（メモリの節約）
// #define NO_BIN_FORM

// ８進表示をサポートしない場合（メモリの節約）
// #define NO_OCTAL_FORM
```

---
## 標準出力のカスタマイズ

通常、標準出力は、stdout ハンドルを使い、write 関数を呼び出します。   
処理系によっては、putchar 関数を使う事を強要される場合があり、その場合、以下の定義を使います。   
- コメントアウトを外して下さい。   

```C++
// 最終的な出力として putchar を使う場合有効にする（通常は write [stdout] 関数）
#define USE_PUTCHAR
```

---
## プロジェクト（全体テスト）

- format クラスは、全体テストと共に提供されます。   
- mingw64 環境で、clang++ を使ってコンパイルされます。

```sh
make
```
で全体テストがコンパイルされます。

```sh
make run
```

- 全体テストが走り、全てのテストが通過すると、正常終了となります。   
- テストに失敗すると、-1 を返します。

### 個別のテスト

- 個別のテストだけ動かす事が出来ます。
- テストは整数で指示出来ます。

例： test-1, test-5 を動かす場合

```sh
./test_format_class -1 -5
```

---

### 変換時間表示（目安）

- 文字出力として「putchar」を使った場合。

```
 % time ./test_format_class.exe -boost > list
real    0m2.043s
user    0m0.000s
sys     0m0.015s

 % time ./test_format_class.exe -printf > list
real    0m0.695s
user    0m0.015s
sys     0m0.000s

 % time ./test_format_class.exe -format > list
real    0m0.501s
user    0m0.000s
sys     0m0.015s
```

---
      
## License

[MIT](LICENSE)

```
Copyright (c) 2020, Hiramatsu Kunihito
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the <organization> nor the　names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
