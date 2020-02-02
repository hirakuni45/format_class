format クラス   
printf に似た仕様で文字列から数値へ変換する C++ クラス
=========

## 概要

 C++ では、C 言語で一般的な可変引数を扱う関数は使わない文化があります。   
   
 一番の問題は、引数の引渡しはスタックを経由する点、又、スタックに引数が何個格納   
 されているのか判らない点です、これによりスタックをオーバーロードさせ、システム   
 に悪影響を与える事が出来てしまいます。   
   
 代表的な実装は「printf」関数です。   
 コンパイラはフォーマット文を解析して、引数の整合性をチェックしますが、完全には   
 チェックできません。   
 一方、「printf」は優れた柔軟性をもたらし、文字、数値を扱う事をたやすくします。   
   
 boost には、printf の柔軟性と、安全性を考慮した、format.hpp があります。   
 ※「%」オペレーターのオーバーロード機構を利用して、複数の引数を受け取る事ができます。   
 ※boost::format は優れた実装ですが、iostream に依存していて組み込みマイコンでは問題があります。    
 ※iostream を取り込むと、容量が肥大化します。

 そこで、boost::format の仕組みをまねて、printf に代わる format クラスを実装しました。   
   
---
## 仕様

 - 基本的な仕様は「printf」に準拠しています。
 - パラメーターを渡す仕様は、boost::format とほぼ同じです。
 - printf と異なる部分、拡張された仕様もあります。
 - 名前空間は「utils」です。
 - 文字列の出力はファンクタを定義して、テンプレートパラメーターとします。   
   ※標準的なファンクタ「stdout_buffered_chaout」クラスが定義されており、以下のように   
   typedef されています。   

```
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

---
## 使い方

### format.hpp をインクルードします。

```
#include "format.hpp"
```

全ての機能を使うのに必要なヘッダーは「format.hpp」のみです。

### 名前空間は「utils」です。

### サンプル







---
## プロジェクト（全体テスト）

format クラスは、全体テストと共に提供されます。

```
make
```
で全体テストがコンパイルされます。

```
make run
```

全体テストが走り、全てのテストが通過すると、正常終了となります。   
※テストに失敗すると、-1 を返します。

---
      
License
----

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
