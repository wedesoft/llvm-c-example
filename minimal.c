#include <stdlib.h>
#include <stdio.h>
#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Transforms/Scalar.h>


int main (int argc, char const *argv[])
{
  char *error = NULL;
  LLVMLinkInMCJIT();
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();
  LLVMInitializeNativeAsmParser();

  LLVMModuleRef mod = LLVMModuleCreateWithName("minimal_module");
  LLVMTypeRef identity_args[] = { LLVMInt32Type() };
  LLVMValueRef identity = LLVMAddFunction(mod, "identity", LLVMFunctionType(LLVMInt32Type(), identity_args, 1, 0));
  LLVMSetFunctionCallConv(identity, LLVMCCallConv);
  LLVMValueRef n = LLVMGetParam(identity, 0);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(identity, "entry");
  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMBuildRet(builder, n);

  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

  LLVMExecutionEngineRef engine;
  error = NULL;
  if(LLVMCreateJITCompilerForModule(&engine, mod, 2, &error) != 0) {
    fprintf(stderr, "%s\n", error);
    LLVMDisposeMessage(error);
    abort();
  }
  int (*fun)(int) = (int (*)(int))LLVMGetFunctionAddress(engine, "identity");

  LLVMDumpModule(mod);

  fprintf(stderr, "\n");
  fprintf(stderr, "; Running identity(42) with JIT...\n");
  fprintf(stderr, "; Result: %d\n", (*fun)(42));

  LLVMRemoveModule(engine, mod, &mod, &error);
  LLVMDisposeModule(mod);
  LLVMDisposeExecutionEngine(engine);
  LLVMDisposeBuilder(builder);
  return 0;
}
