default:
	${CC} -O2 main.c -o dtg-launcher -lncurses
	strip dtg-launcher
install:
	cp dtg-launcher /bin/dtg-launcher
	chmod a+x /bin/dtg-launcher
