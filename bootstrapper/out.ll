; ModuleID = 'blam'
source_filename = "blam"

@.str.err = private unnamed_addr constant [26 x i8] c"<print unsupported type>\0A\00", align 1

define i32 @add(i32 %a, i32 %b) {
entry:
  ret i32 0
}

define i32 @negate(i32 %a) {
entry:
  ret i32 0
}

define void @log(i32 %msg) {
entry:
  %0 = call i32 @puts(ptr @.str.err)
  ret void
}

declare i32 @puts(ptr)

define i32 @id(i32 %x) {
entry:
  ret i32 0
}

define i32 @getVec() {
entry:
  ret i32 0
}

define i32 @newVec(i32 %x, i32 %y) {
entry:
  ret i32 0
}

define i32 @callChain(i32 %x) {
entry:
  ret i32 0
}

define void @main() {
entry:
  ret void
}
