## 指令流

新指令通过LSQ与外界交流。

首先修改新指令的属性，必须要是`inst->isMemRef()`，才能在`execute:issue`中push到`inFUMemInsts`。

`isMemRef()`定义在`dyn_inst.hh:259`中

~~~c++\
    bool isMemRef() const { return isInst() && staticInst->isMemRef(); }
~~~

`isMemRef`定义在`src/cpu/static_inst.hh:static_inst.hh:142`

~~~c++
isMemRef() const
{
        return flags[IsLoad] || flags[IsStore] || flags[IsAtomic];
}
~~~



属性定义在`static_inst:207:setFlag`

枚举量`Flag`的定义在`src/cpu/StaticInstFlags.py:49`

`setFlag`在`src/arch/riscv/isa/formats/amo.isa`中被调用。





先修改浮点指令，在`src/cpu/minor/fetch2.cc:423`中

~~~c++
else if (decoded_inst->isFloating())
	stats.fpInstructions++;
~~~





> ## 作废
>
> ### issue
>
> 在`execute:issue`中修改`isFloating`的分支，也送入`InFuMemInst`
>
> ~~~c++
> issued_mem_ref = inst->isMemRef() || inst->isFloating();
> ~~~
>
> ### commit
>
>
>
> pop LSQ阶段：
>
> ~~~c++
>   if (completed_inst && (inst->isMemRef() || inst->isFloating() ))
> ~~~
>
>
>
>

### 添加函数

`void SendToCustom();`









### commitInst









## gem5系统

MinorCPU

### execute.cc

在execute的构造函数中添加rtl目录下的构造函数

~~~c++
    custom(name_ + ".custom", name_ + ".custominst_port",
        cpu_, *this,

        ),

~~~

execute的include

~~~c++
#include "debug/MemObject.hh"
~~~



在execute的.hh函数中声明rtl目录下的函数

~~~c++
#include "cpu/minor/custom.hh"

//添加新函数

    /* Send To MemObject */
    void SendToCustom(MinorDynInstPtr inst)
~~~



~~~c++
    /* Custom Inst */
    Custom custom;
~~~



在execute的SConscript文件中添加

~~~python
    Source('custom.cc')

    DebugFlag('Custom', 'CustomInstPort')
~~~



>## 作废
>
>在`src/cpu/minor/BaseMinorCPU.py`中添加
>
>~~~c++
>custominst_port = RequestPort("CustomInst Port")
>~~~





> ## 作废
>
> 在`src/cpu/BaseCPU.py`中添加
>
> ~~~c++
> custominst_port = RequestPort("CustomInst Port")
>
>       _cached_ports = ["icache_port", "dcache_port", "custominst_port"]
> ~~~
>
> 在`src/cpu/base.cc`中`getPort`函数添加：
>
> ~~~c++
> else if (if_name == "custominst_port")
>   return getCustPort();
>
>
>
> getCustPort().takeOverFrom(&oldCPU->getCustPort());
> ~~~
>
> 在`src/cpu/base.hh`中添加`getCustPort()`：
>
> ~~~c++
> /** Custom*/
> virtual Port &getCustPort() = 0;
>
>
>
>
> ~~~
>
>
>
>







>## 作废
>
>在`src/cpu/kvm/base.hh`中添加
>
>~~~c++
>Port &getCustPort()  { return custPort; }
>
>
>
>/** Unused port*/
>KVMCpuPort custPort;
>~~~
>
>



貌似不用再bash里面改。

直接在minor/cpu里面加一个getPort

`src/cpu/minor/cpu.hh`

~~~c++
Port &getPort(const std::string &if_name, PortID idx=InvalidPortID) override;
~~~

.cc

~~~c++
Port &
MinorCPU::getPort(const std::string &if_name, PortID idx)
{
    if (if_name == "custominst_port")
        return getCustPort();
    else
        return getCustPort();
}
~~~





在`src/cpu/minor/cpu.hh`中添加`getCustPort()`

~~~c++
    /** Return a reference to custom*/
    Port &getCustPort() override;
~~~



在`src/cpu/minor/cpu.cc`中添加`getCustPort()`

~~~c++
Port &
MinorCPU::getCustPort()
{
    return pipeline->getCustPort();
}
~~~





在`src/cpu/minor/pipeline.cc`中添加`getCustPort()`

~~~c++
MinorCPU::MinorCPUPort &
Pipeline::getCustPort()
{
    return execute.getCustPort();
}
~~~

在`src/cpu/minor/pipeline.hh`中添加`getCustPort()`

~~~c++
    /** Return the CustomInstPort belonging to Execute for the CPU */
    MinorCPU::MinorCPUPort &getCustPort();
~~~

在`src/cpu/minor/execute.hh`中添加`getCustPort()`

~~~c++
    /** Return the CustomPort*/
    MinorCPU::MinorCPUPort &getCustPort();
~~~

在`src/cpu/minor/execute.cc`中添加`getCustPort()`

~~~c++
MinorCPU::MinorCPUPort &
Execute::getCustPort()
{
    return custom.getCustPort();
}
~~~





在`src/cpu/minor/lsq.hh`中添加`getCustPort()`

~~~c++
    /** Return the CustomInst port */
    MinorCPU::MinorCPUPort &getCustPort() { return custmoInstPort; }
~~~







### RTL

添加src/cpu/rtl文件夹

修改rtl目录下的`SConscript:50`

添加debug标志：`DebugFlag('MemObject', 'Custom inst')`

## 交叉工具链编译

交叉工具链安装

~~~sh
cd ~/Work/riscv/riscv-gnu-toolchain
./configure --prefix=/home/yangchun/Work/riscv/rv64g-linux-gnu --disable-gdb --with-arch=rv64g --with-gcc-src=/home/yangchun/Work/riscv/gcc-11.2.0 --with-binutils-src=/home/yangchun/Work/riscv/binutils-2.35 --with-glibc-src=/home/yangchun/Work/riscv/glibc-2.35
make linux -j $(nproc)
~~~

编译文件

~~~sh
/home/csx/sw/riscv/rv64g-linux-gnu/bin/riscv64-unknown-linux-gnu-gcc -static -v -o [outfile] [infile]
~~~

## 重新构建gem5

~~~bash
scons build/RISCV/gem5.opt
~~~

## 运行gem5

~~~c++
build/RISCV/gem5.opt configs/custom/riscv-floating.py
~~~

打开debug

~~~bash
build/RISCV/gem5.opt --debug-flags=CustomObj configs/custom/riscv-floating.py
~~~



运行counter

~~~bash
build/RISCV/gem5.opt --debug-flags=CustomObj configs/custom/riscv-floating-counter.py
~~~









## 运行counter



有一些编译的小问题

#### 第一个

ext/custom/SConscript里面库路径还用的绝对路径，还待解决

#### 第二个

报错

~~~bash
root@CsxDesktop:/home/csx/workland/mycode/gem5-custom# build/RISCV/gem5.opt --debug-flags=CustomObj configs/custom/riscv-floating-counter.py
build/RISCV/gem5.opt: error while loading shared libraries: libVerilatorCounter.so: cannot open shared object file: No such file or directory
~~~

找不到这个动态链接库，强行加一下：

~~~bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/csx/workland/mycode/gem5_custom/ext/custom
~~~
