#ifndef _OPNODE_H_
#define _OPNODE_H_

#include "GenInterface.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>

class OpNode;

typedef std::shared_ptr<OpNode> OpNodePtr;

class OpNode
{
    protected:
        const std::string name_;
        
    public:
        OpNode(const std::string& name):
            name_(name)
        {
        }
        
        inline const std::string& name() const
        {
            return name_;
        }
        
        virtual unsigned int size() const = 0;
        
        virtual void generate(GenInterface& generator) const = 0;
        
        virtual OpNodePtr diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const;
        
        //virtual OpNodePtr taylor(const OpNodePtr& node, unsigned int order) const = 0;
};

class VarNode:
    public OpNode
{
    protected:
        const unsigned int size_;
    public:
        VarNode(const std::string& name, unsigned int size):
            OpNode(name),
            size_(size)
        {
        }
        
        unsigned int size() const
        {
            return size_;
        }
        
        virtual void generate(GenInterface& generator) const 
        {
            generator.process(*this);
        }
        
        virtual OpNodePtr diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const;
};

class UnaryOpNode:
    public OpNode
{
    protected:
        OpNodePtr node_;
    public:
        UnaryOpNode(const std::string& name, const OpNodePtr& node):
            OpNode(name),
            node_(node)
        {
        }
        
        inline const OpNodePtr& node() const
        {
            return node_;
        }
};

class BinaryOpNode:
    public OpNode
{
    protected:
        const OpNodePtr lhs_;
        const OpNodePtr rhs_;
    public:
        BinaryOpNode(const std::string& name, const OpNodePtr& lhs, const OpNodePtr& rhs):
            OpNode(name),
            lhs_(lhs),
            rhs_(rhs)
        {
        }
        
        inline const OpNodePtr& lhs() const
        {
            return lhs_;
        }
        
        inline const OpNodePtr& rhs() const
        {
            return rhs_;
        }
};

class ConstNode:
    public OpNode
{
    protected:
        std::vector<double> values_;
    public:
        ConstNode(const std::string& name, const std::vector<double>& values):
            OpNode(name),
            values_(values)
        {
        }
        
        inline const std::vector<double>& values() const
        {
            return values_;
        }
        
        virtual unsigned int size() const
        {
            return values_.size();
        }
        
        virtual void generate(GenInterface& generator) const
        {
            generator.process(*this);
        }
        
        virtual OpNodePtr diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const;
};

class AddNode:
    public BinaryOpNode
{
    public:
        using BinaryOpNode::BinaryOpNode;
        
        virtual unsigned int size() const
        {
            return lhs_->size();
        }
        
        virtual void generate(GenInterface& generator) const
        {
            generator.process(*this);
        }
        
        virtual OpNodePtr diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const;
};

class MulNode:
    public BinaryOpNode
{
    protected:
        
    public:
        using BinaryOpNode::BinaryOpNode;
        
        virtual unsigned int size() const
        {
            return lhs_->size();
        }
        
        virtual void generate(GenInterface& generator) const
        {
            generator.process(*this);
        }
        
        virtual OpNodePtr diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const;
};

class IndexNode:
    public UnaryOpNode
{
    protected:
        const unsigned int index_;
    public:
        IndexNode(const std::string& name, const OpNodePtr& node, unsigned int index):
            UnaryOpNode(name, node),
            index_(index)
        {
        }
        
        virtual unsigned int size() const
        {
            return 1;
        }
        
        inline unsigned int index() const
        {
            return index_;
        }
        
        virtual void generate(GenInterface& generator) const
        {
            generator.process(*this);
        }
        
        virtual OpNodePtr diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const;
};

OpNodePtr OpNode::diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const
{
    bool found = false;
    for (const auto& node: nodes)
    {
        found |= node->name()==this->name();
        if (found) break;
    }
    if (found)
    {
        return OpNodePtr(new ConstNode(prefix+this->name()+"grad",std::vector<double>(this->size(),1.)));
    }
    return nullptr;
}

OpNodePtr VarNode::diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const
{
    OpNodePtr nodeDiff = OpNode::diff(prefix,nodes);
    if (nodeDiff) return nodeDiff;
    
    return OpNodePtr(new ConstNode(prefix+this->name()+"grad",std::vector<double>(this->size(),0.)));
}

OpNodePtr ConstNode::diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const
{
    OpNodePtr nodeDiff = OpNode::diff(prefix,nodes);
    if (nodeDiff) return nodeDiff;
    
    return OpNodePtr(new ConstNode(prefix+this->name()+"grad",std::vector<double>(this->size(),0.)));
}

OpNodePtr AddNode::diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const
{
    OpNodePtr nodeDiff = OpNode::diff(prefix,nodes);
    if (nodeDiff) return nodeDiff;
    
    return OpNodePtr(new AddNode(prefix+this->name()+"_grad",lhs_->diff(prefix,nodes),rhs_->diff(prefix,nodes)));
}

OpNodePtr MulNode::diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const
{
    OpNodePtr nodeDiff = OpNode::diff(prefix,nodes);
    if (nodeDiff) return nodeDiff;
    
    OpNodePtr mul1(new MulNode(prefix+this->name()+"_grad_mul1",lhs_->diff(prefix,nodes),rhs_));
    OpNodePtr mul2(new MulNode(prefix+this->name()+"_grad_mul2",lhs_,rhs_->diff(prefix,nodes)));
    return OpNodePtr(new AddNode(prefix+this->name()+"_grad",mul1,mul2));
}

OpNodePtr IndexNode::diff(const std::string& prefix, const std::vector<OpNodePtr>& nodes) const
{
    OpNodePtr nodeDiff = OpNode::diff(prefix,nodes);
    if (nodeDiff) return nodeDiff;
    
    return OpNodePtr(new IndexNode(prefix+this->name()+"_grad",node()->diff(prefix,nodes),index()));
}


#endif
