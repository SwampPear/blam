; ModuleID = 'blam'
source_filename = "blam"

@.str = private unnamed_addr constant [6 x i8] c"hello\00", align 1
@.fmt = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define double @main() {
entry:
  %0 = call i32 @puts(ptr @.str)
  %1 = call i32 (ptr, ...) @printf(ptr @.fmt, i32 42)
  ret double 0.000000e+00
}

declare i32 @puts(ptr)

declare i32 @printf(ptr, ...)
