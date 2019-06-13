#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"

#include "llvm/IR/Verifier.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/TargetRegistry.h"

#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"

#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"

#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"

#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/Error.h"

#include <vector>
#include <iostream>
#include <memory>
#include <functional>

llvm::orc::ThreadSafeModule createDemoModule() 
{
    using namespace llvm;
    using namespace llvm::orc;
    
    auto Context = llvm::make_unique<LLVMContext>();
    auto M = make_unique<Module>("test", *Context);

    // Create the add1 function entry and insert this entry into module M.  The
    // function will have a return type of "int" and take an argument of "int".
    Function *Add1F =
      Function::Create(FunctionType::get(Type::getInt32Ty(*Context),
                                         {Type::getInt32Ty(*Context)}, false),
                       Function::ExternalLinkage, "add1", M.get());

    // Add a basic block to the function. As before, it automatically inserts
    // because of the last argument.
    BasicBlock *BB = BasicBlock::Create(*Context, "EntryBlock", Add1F);

    // Create a basic block builder with default parameters.  The builder will
    // automatically append instructions to the basic block `BB'.
    IRBuilder<> builder(BB);

    // Get pointers to the constant `1'.
    Value *One = builder.getInt32(1);

    // Get pointers to the integer argument of the add1 function...
    assert(Add1F->arg_begin() != Add1F->arg_end()); // Make sure there's an arg
    Argument *ArgX = &*Add1F->arg_begin();          // Get the arg
    ArgX->setName("AnArg"); // Give it a nice symbolic name for fun.

    // Create the add instruction, inserting it into the end of BB.
    Value *Add = builder.CreateAdd(One, ArgX);

    // Create the return instruction and add it to the basic block
    builder.CreateRet(Add);

    return ThreadSafeModule(std::move(M), std::move(Context));
}

int main()
{
    using namespace llvm;
    std::unique_ptr<LLVMContext> context(new LLVMContext());
    
    
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    //InitializeNativeTarget();
    //InitializeNativeTargetAsmPrinter();
    
    std::string error;
    raw_os_ostream osstream(std::cout);
    
    const std::string targetTriple = sys::getProcessTriple(); //for JIT
    std::cout<<"Triple: "<<targetTriple<<std::endl;
    
  
    const Target* target = TargetRegistry::lookupTarget(targetTriple, error);
    if (!target)
    {
        std::cout<<"Error: "<<error<<std::endl;
        return 1;
    }
    TargetOptions opt;
    Optional<Reloc::Model> rm;
    CodeModel::Model cm;
	CodeGenOpt::Level ol = CodeGenOpt::Default;
    TargetMachine* targetMachine = target->createTargetMachine(
        targetTriple,
        "generic",
        "",
        opt,
        rm,
        cm,
        ol
    );
    
    std::unique_ptr<Module> module(new Module("test",*context));
    module->setTargetTriple(targetTriple);
    module->setDataLayout(targetMachine->createDataLayout());
    
    IRBuilder<> builder(*context);
    
    std::vector<Type*> fctArgs{{builder.getInt32Ty()}};
    Type* fctResult = builder.getInt32Ty();
    FunctionType* fctType = FunctionType::get(
        fctResult,
        fctArgs,
        false
    );
    Function* myFct = Function::Create(
        fctType,
        Function::ExternalLinkage,
        "myFct",
        module.get()
    );
    
    BasicBlock* fctBlock = BasicBlock::Create(*context,"entry",myFct);
    builder.SetInsertPoint(fctBlock);
    Value* arg = myFct->arg_begin();
    Value* res = builder.CreateNeg(arg,"make_neg");
    builder.CreateRet(res);
    
    //verifyModule(*module);
    
    verifyFunction(*myFct,&osstream);
    module->print(osstream, nullptr);
    
    ExitOnError exitOnErr;
    

    orc::ExecutionSession sess;

 	
    orc::RTDyldObjectLinkingLayer objectLinkLayer(sess,[]() { return llvm::make_unique<SectionMemoryManager>(); });
    orc::SimpleCompiler compiler(*targetMachine);
    orc::IRCompileLayer compileLayer(sess,objectLinkLayer,compiler);
    
    orc::IRTransformLayer optimizeLayer(
        sess,
        compileLayer,
        [](orc::ThreadSafeModule m, const orc::MaterializationResponsibility &mr)
        {
            //TODO: optimization pass here
            return m;
        }
    );
    
    auto moduleKey = sess.allocateVModule();
    
    exitOnErr(optimizeLayer.add(
        sess.getMainJITDylib(),
        orc::ThreadSafeModule(std::move(module),std::move(context))
    ));
    
    auto fctPtr = (int(*)(int))exitOnErr(sess.lookup(ArrayRef<orc::JITDylib*>({{&sess.getMainJITDylib()}}),"myFct")).getAddress();
    std::cout<<fctPtr(4)<<std::endl;
    
    
    return 0;
    
}

