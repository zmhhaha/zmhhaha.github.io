plbDebug.h 文件

```C++
#include <cassert>

#ifdef PLB_DEBUG

    #define PLB_ASSERT( COND )        assert( COND );
    #define PLB_PRECONDITION( COND )  assert( COND );
    #define PLB_POSTCONDITION( COND ) assert( COND );
    #define PLB_STATECHECK( A,B )     assert( (A) == (B) );

#else

    #define PLB_ASSERT( COND )
    #define PLB_PRECONDITION( COND )
    #define PLB_POSTCONDITION( COND )
    #define PLB_STATECHECK( A,B )

#endif  // PLB_DEBUG
```

>  assert宏的原型定义在<assert.h>中，其作用是如果它的条件返回错误，则终止程序执行。 

此处封装为宏命令，用于在条件COND错误时中断错误运行。