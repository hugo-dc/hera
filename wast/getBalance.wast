(module
 (import "ethereum" "getBalance" (func $getBalance (param i32) (param i32)))
 (memory 1)
 (func $main
       (export "main")
       (call $getBalance (i32.const 0) (i32.const 0))))
