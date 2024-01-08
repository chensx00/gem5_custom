### todo

接下来看gem5的packet结构体：能否传递riscv的vec指令。不行的话考虑写一个它的派生类。

gem5里riscv的向量相关的上下文怎么更新？考虑把向量寄存器放在RTL里面，如果这样，load和store指令该怎么实现。

minor中针对新指令的执行流还很粗糙。

