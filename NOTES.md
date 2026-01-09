# TODO
[ ] iterate on bootstrapper in tandem with coding native compiler and Bloop Beta (do not focus on grammar parser)
[ ] implement basic optimization passes on the generated IR
[ ] Once the native compiler has been created, then focus can shift to robust parsing system and more optimization passes, Blam V1.0
[ ] define grammar for Blam V1.0
[ ] replace parser system with more robust grammar parsing system from file (ideally should be predigested)
[ ] replace skip token with new grammar parsing

# Compilation/Execution
make -C bootstrapper
./bootstrapper/build/main compiler/src/main.blam blam_output.ll blam_exec
/tmp/blam_exec