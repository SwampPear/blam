; ModuleID = 'SyscallModule'
source_filename = "SyscallModule"

@0 = private unnamed_addr constant [76 x i8] c"Information provided is for educational purposes and not financial advice.\0A\00", align 1

define i32 @main() {
entry:
  call i64 (i64, ...) @syscall(i64 1, i64 1, ptr @0, i64 75)
  ret i32 0
}

declare i64 @syscall(i64, ...)
