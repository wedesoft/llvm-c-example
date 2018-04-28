# llvm-c-example [![](https://travis-ci.org/wedesoft/llvm-c-example.svg?branch=master)](https://travis-ci.org/wedesoft/llvm-c-example/)

An example program showing off usage of LLVM-3.9's C bindings.

This is a github repo of @ynd's [example](http://npcontemplation.blogspot.com/2008/06/secret-of-llvm-c-bindings.html)
usage of LLVM's C bindings. It has been cleaned up to compile with later versions of LLVM and includes a Makefile.

fac.c builds up a representation of a factorial function piece-by-piece, adds
optimization passes to the function, then executes it with LLVM's JIT.

Here is the example output:
```
; ModuleID = 'fac_module'
source_filename = "fac_module"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"

define i32 @fac(i32) {
entry:
  %"n * fac(n - 1).reg2mem" = alloca i32
  %result.reg2mem = alloca i32
  %"n == 0" = icmp eq i32 %0, 0
  br i1 %"n == 0", label %iftrue, label %iffalse

iftrue:                                           ; preds = %entry
  store i32 1, i32* %result.reg2mem
  br label %end

iffalse:                                          ; preds = %entry
  %"n - 1" = add i32 %0, -1
  %"fac(n - 1)" = call i32 @fac(i32 %"n - 1")
  %"n * fac(n - 1)" = mul i32 %"fac(n - 1)", %0
  store i32 %"n * fac(n - 1)", i32* %"n * fac(n - 1).reg2mem"
  store i32 %"n * fac(n - 1)", i32* %result.reg2mem
  br label %end

end:                                              ; preds = %iffalse, %iftrue
  %result.reload = phi i32 [ %"n * fac(n - 1)", %iffalse ], [ 1, %iftrue ]
  ret i32 %result.reload
}

; Running fac(10) with JIT...
; Result: 3628800
```

## External links
* [wickedchicken/llvm-c-example](https://github.com/wickedchicken/llvm-c-example/)
* [Using ORC with LLCM-C API](https://www.doof.me.uk/2017/05/11/using-orc-with-llvms-c-api/)
* [bamboo/libguile-llvm](https://github.com/bamboo/libguile-llvm/)
* [Using LUA and LLVM](https://msm.runhello.com/p/1003)
* [Adding loops](http://www.duskborn.com/adding-loops-mpc-llvm-for-the-neil-language-5/) (also see [neilc.c](https://github.com/sheredom/neil/blob/master/neilc.cpp))
