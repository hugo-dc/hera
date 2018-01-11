;; 
(module
 (import "ethereum" "return" (func $return (param i32) (param i32)))
 (memory 1)
 (func $main
       (export "main")
       (call $return (i32.const 10) (i32.const 100))))

