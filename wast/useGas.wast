(module
 (import "ethereum" "useGas" (func $useGas (param i64)))
 (memory 1)
 (func $main
       (export "main")
       (call $useGas (i64.const 666))
       )
 )
