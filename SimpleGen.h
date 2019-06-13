#ifndef _SIMPLEGEN_H_
#define _SIMPLEGEN_H_

#include "OpNode.h"
#include "GenInterface.h"


class SimpleGen:
    public GenInterface
{
    protected:
        const OpNodePtr resultNode_;
        const VarMap varMap_;
        
        std::vector<double> result_;
        
        virtual void process(const VarNode& node)
        {
            const auto& it = varMap_.find(node.name());
            if (it==varMap_.end())
            {
                std::cout<<"Error - cannot find element: "<<node.name()<<std::endl;
            }
            result_ = it->second;
        }
        
        virtual void process(const ConstNode& node)
        {
            result_ = node.values();
        }
        
        virtual void process(const AddNode& node)
        {
            SimpleGen lhsGen(node.lhs(),varMap_);
            SimpleGen rhsGen(node.rhs(),varMap_);
            
            result_ = lhsGen.eval();
            std::vector<double> rhsResult = rhsGen.eval();
            
            for (unsigned int i = 0; i < result_.size(); ++i)
            {
                result_[i] += rhsResult[i];
            }
        }
        
        virtual void process(const MulNode& node)
        {
            SimpleGen lhsGen(node.lhs(),varMap_);
            SimpleGen rhsGen(node.rhs(),varMap_);
            
            result_ = lhsGen.eval();
            std::vector<double> rhsResult = rhsGen.eval();
            
            for (unsigned int i = 0; i < result_.size(); ++i)
            {
                result_[i] *= rhsResult[i];
            }
        }
        
        virtual void process(const IndexNode& node)
        {
            SimpleGen nodeGen(node.node(),varMap_);
            
            result_ = std::vector<double>{{nodeGen.eval()[node.index()]}};
        }
        
    public:
        SimpleGen(const OpNodePtr& result, const VarMap& varMap={{}}):
            resultNode_(result),
            varMap_(varMap)
        {   
        }
        
        virtual std::vector<double> eval()
        {
            resultNode_->generate(*this);
            return result_;
        }
        
        virtual void print()
        {
            std::vector<double> result(this->eval());
            std::cout<<resultNode_->name()<<": ";
            for (unsigned int i = 0; i < result.size(); ++i)
            {
                std::cout<<result[i]<<", ";
            }
            std::cout<<std::endl;
        }
        
};

#endif
