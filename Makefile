default:
	aarch64-linux-gnu-gcc -O2 main.c -o dtg-launcher -lncurses
	aarch64-linux-gnu-strip dtg-launcher
install:
	cp dtg-launcher /bin/dtg-launcher
	chmod a+x /bin/dtg-launcher
