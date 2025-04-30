package main 

/*
#cgo CFLAGS: -I./../api/include
#cgo LDFLAGS: -L./../api/lib -lchronix
#include "chronix_c_api.h"

// 明确定义Handle类型为void*
typedef void* ChronixHandle;
*/
import "C"

func main() {
    // 使用显式类型转换
    handle := C.chronix_scheduler_create(4)
    if handle == nil {
        fmt.Println("创建调度器失败")
        return
    }
    defer func() {
        C.chronix_scheduler_destroy(handle)
    }()
    fmt.Println("调度器创建成功")
}