# llvm-c-example[![](https://travis-ci.org/wedesoft/llvm-c-example.svg?branch=master)](https://travis-ci.org/wedesoft/llvm-c-example/)

An example program showing off usage of LLVM's C bindings.

This is a github repo of @ynd's [example](http://npcontemplation.blogspot.com/2008/06/secret-of-llvm-c-bindings.html)
usage of LLVM's C bindings. It has been cleaned up to compile with later versions of LLVM and includes a Makefile.

fac.c builds up a representation of a factorial function piece-by-piece, adds
optimization passes to the function, then executes it with LLVM's JIT.

## External links
* [wickedchicken/llvm-c-example](https://github.com/wickedchicken/llvm-c-example/)
* [Using ORC with LLCM-C API](https://www.doof.me.uk/2017/05/11/using-orc-with-llvms-c-api/)
* [bamboo/libguile-llvm](https://github.com/bamboo/libguile-llvm/)
* [Using LUA and LLVM](https://msm.runhello.com/p/1003)
