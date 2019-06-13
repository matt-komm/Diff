#ifndef _GENINTERFACE_H_
#define _GENINTERFACE_H_

#include <unordered_map>
#include <vector>
#include <iostream>

class VarNode;
class ConstNode;
class AddNode;
class MulNode;
class IndexNode;

typedef std::unordered_map<std::string,std::vector<double>> VarMap;

class GenInterface
{
    friend class VarNode;
    friend class ConstNode;
    friend class AddNode;
    friend class MulNode;
    friend class IndexNode;

    protected:
        virtual void process(const VarNode& node) = 0;
        virtual void process(const ConstNode& node) = 0;
        virtual void process(const AddNode& node) = 0;
        virtual void process(const MulNode& node) = 0;
        virtual void process(const IndexNode& node) = 0;
        
    public:
        virtual std::vector<double> eval() = 0;
        
        
};

#endif
