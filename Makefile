default:
	aarch64-linux-gnu-gcc -O2 main.c -o launcher -lncurses
install:
	cp launcher /bin/dtg-launcher
	chmod a+x /bin/dtg-launcher
