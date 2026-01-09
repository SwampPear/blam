build-bootstrapper:
	cd bootstrapper && $(MAKE) clean
	cd bootstrapper && $(MAKE)

bootstrap:
	mkdir -p build
	./bootstrapper/build/main compiler/src/main.blam build/output.ll build/blam_exec

run:
	./build/blam_exec

clean:
	cd bootstrapper && $(MAKE) clean
