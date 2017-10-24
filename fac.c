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

  LLVMModuleRef mod = LLVMModuleCreateWithName("fac_module");
  LLVMTypeRef fac_args[] = { LLVMInt32Type() };
  LLVMValueRef fac = LLVMAddFunction(mod, "fac", LLVMFunctionType(LLVMInt32Type(), fac_args, 1, 0));
  LLVMSetFunctionCallConv(fac, LLVMCCallConv);
  LLVMValueRef n = LLVMGetParam(fac, 0);

  LLVMBasicBlockRef entry = LLVMAppendBasicBlock(fac, "entry");
  LLVMBasicBlockRef iftrue = LLVMAppendBasicBlock(fac, "iftrue");
  LLVMBasicBlockRef iffalse = LLVMAppendBasicBlock(fac, "iffalse");
  LLVMBasicBlockRef end = LLVMAppendBasicBlock(fac, "end");
  LLVMBuilderRef builder = LLVMCreateBuilder();

  LLVMPositionBuilderAtEnd(builder, entry);
  LLVMValueRef If = LLVMBuildICmp(builder, LLVMIntEQ, n, LLVMConstInt(LLVMInt32Type(), 0, 0), "n == 0");
  LLVMBuildCondBr(builder, If, iftrue, iffalse);

  LLVMPositionBuilderAtEnd(builder, iftrue);
  LLVMValueRef res_iftrue = LLVMConstInt(LLVMInt32Type(), 1, 0);
  LLVMBuildBr(builder, end);

  LLVMPositionBuilderAtEnd(builder, iffalse);
  LLVMValueRef n_minus = LLVMBuildSub(builder, n, LLVMConstInt(LLVMInt32Type(), 1, 0), "n - 1");
  LLVMValueRef call_fac_args[] = {n_minus};
  LLVMValueRef call_fac = LLVMBuildCall(builder, fac, call_fac_args, 1, "fac(n - 1)");
  LLVMValueRef res_iffalse = LLVMBuildMul(builder, n, call_fac, "n * fac(n - 1)");
  LLVMBuildBr(builder, end);

  LLVMPositionBuilderAtEnd(builder, end);
  LLVMValueRef res = LLVMBuildPhi(builder, LLVMInt32Type(), "result");
  LLVMValueRef phi_vals[] = {res_iftrue, res_iffalse};
  LLVMBasicBlockRef phi_blocks[] = {iftrue, iffalse};
  LLVMAddIncoming(res, phi_vals, phi_blocks, 2);
  LLVMBuildRet(builder, res);

  LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
  LLVMDisposeMessage(error);


  LLVMExecutionEngineRef engine;
  error = NULL;
  if(LLVMCreateJITCompilerForModule(&engine, mod, 2, &error) != 0) {
    fprintf(stderr, "%s\n", error);
    LLVMDisposeMessage(error);
    abort();
  }

  LLVMPassManagerRef pass = LLVMCreatePassManager();
  LLVMAddConstantPropagationPass(pass);
  LLVMAddInstructionCombiningPass(pass);
  LLVMAddPromoteMemoryToRegisterPass(pass);
  // LLVMAddDemoteMemoryToRegisterPass(pass);
  LLVMAddGVNPass(pass);
  LLVMAddCFGSimplificationPass(pass);
  LLVMRunPassManager(pass, mod);
  LLVMDumpModule(mod);

  LLVMGenericValueRef exec_args[] = {LLVMCreateGenericValueOfInt(LLVMInt32Type(), 10, 0)};
  LLVMGenericValueRef exec_res = LLVMRunFunction(engine, fac, 1, exec_args);
  fprintf(stderr, "\n");
  fprintf(stderr, "; Running fac(10) with JIT...\n");
  fprintf(stderr, "; Result: %lld\n", LLVMGenericValueToInt(exec_res, 0));

  LLVMDisposePassManager(pass);
  LLVMDisposeBuilder(builder);
  LLVMDisposeExecutionEngine(engine);
  return 0;
}
