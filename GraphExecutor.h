#ifndef _GRAPHEXECUTOR_H_
#define _GRAPHEXECUTOR_H_

#include "OpNode.h"
#include "GenInterface.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class GraphExecutor
{
    protected:
        const OpNodePtr resultNode_;
        const VarMap varMap_;
        
        
        
        class NodeRunner
        {
            protected:
                enum Type
                {
                    ADD,MUL,INDEX,CONST
                };
                
            public:
                class Result
                {
                    protected:
                        std::vector<double> result_;
                    public:
                        Result():
                            result_(0),
                        {
                        }
                        
                        inline void setResult(std::vector<double>&& result)
                        {
                            result_ = result;
                        }
                        
                        inline const std::vector<double>& getResult() const
                        {
                            return result_;
                        }
                };
            
                NodeRunner()
                {
                }
                
                std::shared_ptr<Result> produce()
                {
                    
                }
        };
        
        class GraphBuilder:
            public GenInterface
        {
            public:
                std::vector<NodeRunner> runOrder_; 
                std::unordered_map<std::string,NodeRunnerPtr> runMap_;
                
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
        };
        
        
    public:
        GraphExecutor(const OpNodePtr& result, const VarMap& varMap={{}}):
            resultNode_(result),
            varMap_(varMap)
        {   
        }
        
        virtual std::vector<double> eval()
        {
            return std::vector<double>();
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
