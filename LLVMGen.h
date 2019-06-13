#ifndef _LLVMGEN_H_
#define _LLVMGEN_H_

#include "OpNode.h"
#include "GenInterface.h"

class LLVMGen:
    public GenInterface
{
    protected:
        virtual void process(const VarNode& node)
        {
        }
    
        virtual void process(const ConstNode& node)
        {
        }
        
        virtual void process(const AddNode& node)
        {
        }
        
        virtual void process(const MulNode& node)
        {
        }
        
        virtual void process(const IndexNode& node)
        {
        }
        
    public:
        LLVMGen(const OpNodePtr& result)
        {
            result->generate(*this);
        }
        
        virtual std::vector<double> eval()
        {
            return std::vector<double>();
        }
        
        
};

#endif
