#include "OpNode.h"
#include "LLVMGen.h"
#include "SimpleGen.h"
#include "GraphExecutor.h"

int main()
{
    OpNodePtr n1(new ConstNode("n1",{{3.,0.7}})); 
    OpNodePtr n2(new ConstNode("n2",{{2.,-0.5}})); 
    OpNodePtr add(new AddNode("add",n1,n2));
    OpNodePtr var(new VarNode("var",2));
    OpNodePtr varmul(new MulNode("varmul",var,add));
    OpNodePtr mul(new MulNode("mul",n1,add));
    OpNodePtr index1(new IndexNode("index1",add,0));
    OpNodePtr index2(new IndexNode("index2",mul,1));
    

    SimpleGen n1Gen(n1);
    n1Gen.print();
    
    SimpleGen n2Gen(n2);
    n2Gen.print();
    
    SimpleGen addGen(add);
    addGen.print();
    
    SimpleGen mulGen(mul);
    mulGen.print();
    
    SimpleGen varmulGen(varmul,{{"var",{{-2,1}}}});
    varmulGen.print();
    
    SimpleGen indexGen1(index1);
    indexGen1.print();
    
    SimpleGen indexGen2(index2);
    indexGen2.print();
    
    SimpleGen muldiffGen(mul->diff("n1diff_",{{add}}));
    muldiffGen.print();
    
    SimpleGen index1diffGen(index2->diff("n1diff_",{{n1}}));
    index1diffGen.print();
    
    return 0;
}
