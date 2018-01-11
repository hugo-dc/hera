(module
  (import "ethereum" "storageStore" (func $sstore (param i32 i32 i32)))
  (memory 1)
  (export "main" (func $main))
  (export "memory" (memory 0))
  (func $main
    ;; should roundtrip store and load a value from storage
    (i64.store (i32.const 0) (i64.const 173553719826446289))
    (call $sstore (i32.const 64) (i32.const 0) (i32.const 1))
  )
)