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
  LLVMTypeRef abs_args[] = { LLVMDoubleType() };
  LLVMValueRef abs = LLVMAddFunction(mod, "abs", LLVMFunctionType(LLVMDoubleType(), abs_args, 1, 0));
  LLVMSetFunctionCallConv(abs, LLVMCCallConv);
  LLVMValueRef n = LLVMGetParam(abs, 0);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(abs, "entry");
  LLVMBasicBlockRef else_block = LLVMAppendBasicBlock(abs, "else");
  LLVMBasicBlockRef endif_block = LLVMAppendBasicBlock(abs, "endif");

  LLVMBuilderRef builder = LLVMCreateBuilder();
  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMValueRef zero = LLVMConstNull(LLVMDoubleType());
  LLVMValueRef cond = LLVMBuildFCmp(builder, LLVMRealOGT, n, zero, "ogt");
  LLVMBuildCondBr(builder, cond, endif_block, else_block);

  LLVMPositionBuilderAtEnd(builder, else_block);
  LLVMValueRef m = LLVMBuildFNeg(builder, n, "negate");
  LLVMBuildBr(builder, endif_block);

  LLVMPositionBuilderAtEnd(builder, endif_block);
  LLVMValueRef result = LLVMBuildPhi(builder, LLVMDoubleType(), "result");
  LLVMAddIncoming(result, &n, &entry, 1);
  LLVMAddIncoming(result, &m, &else_block, 1);
  LLVMBuildRet(builder, result);

  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);

  LLVMExecutionEngineRef engine;
  error = NULL;
  if(LLVMCreateJITCompilerForModule(&engine, mod, 2, &error) != 0) {
    fprintf(stderr, "%s\n", error);
    LLVMDisposeMessage(error);
    abort();
  };
  double (*fun)(double) = (double (*)(double))LLVMGetFunctionAddress(engine, "abs");

  LLVMDumpModule(mod);
  fprintf(stderr, "\n");
  fprintf(stderr, "; Running abs(%f) with JIT...\n", 1.25);
  fprintf(stderr, "; Result: %f\n", (*fun)(1.25));
  fprintf(stderr, "; Running abs(%f) with JIT...\n", -1.25);
  fprintf(stderr, "; Result: %f\n", (*fun)(-1.25));

  LLVMRemoveModule(engine, mod, &mod, &error);
  LLVMDisposeModule(mod);
  LLVMDisposeExecutionEngine(engine);
  LLVMDisposeBuilder(builder);
  return 0;
}
