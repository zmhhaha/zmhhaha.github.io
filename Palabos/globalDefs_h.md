### globalDefs.h文件

#### plint

```c++
/// Integer type for Palabos
/* On some architectures, this type is larger than int.
 * Using plint instead of int ensures 64-bit compatibility of the code.*/
typedef ptrdiff_t plint;
```

>  `std::ptrdiff_t`定义于头文件`<cstddef>`
>
> `std::ptrdiff_t`是两个指针详解结果的有符号整数类型。
>
> `std::ptrdiff_t` 被用于指针算术及数组下标，若负值可行。使用其他类型，如 int 的程序，可能诸如 64 位的系统上失败，在当下标超过 `INT_MAX` 或依赖 32 位模算术时。
>
> 在用 C++ 容器库工作时，迭代器的差的准确类型是成员 `typedef difference_type` ，它常与 `std::ptrdiff_t` 相同。
>
> 只有指向同一数组元素的指针（含指向数组结尾后一位置的指针）可以相减。
>
> 若数组过大（大于`PTRDIFF_MAX`个元素，而小于`SIZE_MAX`字节），则二个指针的差可能无法以 `std::ptrdiff_t` 表示，二个这种指针相减的结果是未定义的。
>
> 对于短于 `PTRDIFF_MAX`的 char 数组， `std::ptrdiff_t` 表现为`std::size_t`的有符号对应物：它可以存储数组的大小，而且在多数平台上等同于 `std::intptr_t` 。
>
> 引用自[`std::ptrdiff_t`]( https://zh.cppreference.com/w/cpp/types/ptrdiff_t ) 



#### pluint

```c++
/// Unsigned integer type for Palabos
/* On some architectures, this type is larger than int.
 * Using fluplint instead of unsigned plint ensures 64-bit compatibility of the code.*/
typedef size_t pluint;
```

> `std::size_t`定义于头文件`<cstddef> <cstdio> <cstdlib> <cstring> <ctime>`
>
>  `std::size_t`是`sizeof`运算符还有`sizeof...`运算符和`alignof`运算符 (C++11 起)所返回的一种无符号整数类型。
>
> size_t 可以存放下理论上可能存在的对象的最大大小，该对象可以是任何类型，包括数组。大小无法以 `std::size_t` 表示的类型是病式的。 (C++14 起)在许多平台上（使用分段寻址的系统除外），`std::size_t`可以存放下任何非成员的指针，此时可以视作其与`std::uintptr_t`同义。
>
> `std::size_t`通常被用于数组索引和循环计数。使用其它类型来进行数组索引操作的程序可能会在某些情况下出错，例如在 64 位系统中使用 `unsigned int` 进行索引时，如果索引号超过`UINT_MAX`或者依赖于 32 位取模运算的话，程序就会出错。
>
> 在对诸如`std::string`、`std::vector`等 C++ 容器进行索引操作时，正确的类型是该容器的成员 `typedef size_type`，而该类型通常被定义为与`std::size_t`相同。 
>
> 引用自[`std::size_t`]( https://zh.cppreference.com/w/cpp/types/size_t )



#### IndexOrdering::OrderingT

```c++
/// Ordering of indices when a BlockXD is converted into a serial data stream.
/** Signification of constants:
 *	- forward:  Right-most index (y in 2D and z in 3D) is contiguous in memory.
 *              For non-allocated parts of the Block, the value 0 is produced 
 *              for output, and values are ignored during input.
 *  - backward: Left-most index (x) is contiguous in memory.
 *              For non-allocated parts of the Block, the value 0 is produced 
 *              for output, and values are ignored during input.
 *  - memorySaving: Ordering is forward (this respects the natural ordering in
 *                  Palabos). Non-allocated parts of the Block are neither 
 *                  written or read: memory savings in the program are 
 *                  reflected by memory savings on the disk.
 **/
namespace IndexOrdering {
    enum OrderingT {forward, backward, memorySaving};
}
```

> BlockXD数据转换为串行数据流时索引的排序。
>
> 常数的意义:
>
> -向前:最右边的索引(y在2D中，z在3D中)在内存中是连续的。对于块中未分配的部分，将为输出生成值0，并在输入期间忽略值。
>
> -向后:最左边的索引(x)在内存中是连续的。对于块中未分配的部分，将为输出生成值0，并在输入期间忽略值。
>
> -存储:排序是向前的(这尊重Palabos中的自然排序)。块中未分配的部分既不写也不读:程序中的内存节省反映在磁盘上的内存节省上。

OrderingT 为不限定作用域的枚举成员，放在IndexOrering名称空间中，限定作用域范围。