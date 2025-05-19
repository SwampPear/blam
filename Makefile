build-bootstrapper:
	cd bootstrapper && $(MAKE) clean
	cd bootstrapper && $(MAKE)

bootstrap:
	./bootstrapper/build/main

