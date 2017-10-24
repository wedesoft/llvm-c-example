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

  LLVMDumpModule(mod);

  LLVMGenericValueRef exec_args[] = {LLVMCreateGenericValueOfInt(LLVMInt32Type(), 42, 0)};
  LLVMGenericValueRef exec_res = LLVMRunFunction(engine, identity, 1, exec_args);
  fprintf(stderr, "\n");
  fprintf(stderr, "; Running identity(42) with JIT...\n");
  fprintf(stderr, "; Result: %lld\n", LLVMGenericValueToInt(exec_res, 0));

  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
  return 0;
}
