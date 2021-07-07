#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<math.h>
#include	<linux/input.h>
#include	<fcntl.h>
#include	<ncurses.h>
#include	<string.h>

#define JOYDEV "/dev/input/by-path/platform-joypad-event-joystick"

#define MAINMENU 0
#define SETTINGS 1
#define UTILITY  2
#define USBMODE	 3

#define TYPE_FUNC   0
#define TYPE_MENU   1
#define TYPE_PARENT 2

int where_i_am = MAINMENU;
int item;

void retroarch()
{
	system("swaymsg workspace 2 || true");
	system("retroarch || true");
	system("swaymsg workspace 1 || true");
}

void supertuxkart()
{
	system("swaymsg workspace 2 || true");
	system("supertuxkart || true");
	system("swaymsg workspace 1 || true");
}

void kodi()
{
	system("swaymsg workspace 2 || true");
	system("kodi || true");
	system("swaymsg workspace 1 || true");
}

void minecraft()
{
	system("swaymsg workspace 2 || true");
	system("env LD_LIBRARY_PATH= LIBGL_DRIVERS_PATH=/usr/lib/arm-linux-gnueabihf/dri/:/usr/lib/aarch64-linux-gnu/dri EGL_PLATFORM=x11 QT_QPA_PLATFORM=xcb DISPLAY=:0 /home/user/mcpe/MC.AppImage > ~/.mclog 2>&1");
	system("swaymsg workspace 1 || true");
}

void re3()
{
	printf("Working on it...\n");
	if(access("/data/gta3/re3_installed", F_OK) == 0)
	{
		system("cp -r /data/re3_in/* /data/gta3/");
		system("touch /data/gta3/re3_installed");
		system("chmod a+x /data/gta3/re3");
	}
	system("swaymsg workspace 2");
	system("cd /data/gta3; /data/gta3/re3");
	system("swaymsg workspace 1");
}

void ppsspp()
{
	system("swaymsg workspace 2 || true");
	system("ppsspp-sdl || true");
	system("swaymsg workspace 1 || true");
}

void poweroff()
{
	system("sudo /sbin/poweroff");
	system("echo 0 > /sys/class/backlight/backlight/brightness");
	while(1); // wait forever
}

void update()
{
	// if we don't deinit and reinit ncurses, logs will look sorta weird
	endwin();
	
	system("sudo apt update || true");
	system("sudo apt upgrade -y || true");
	system("sudo /dtg-ng/programs/dtg-launcher/install");
	// TODO: self-overwrite doesnt work
	sleep(2); // user needs to see it succeed

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
}

void spi_erase()
{
	printf("This will take few seconds... Keep your eyes on screen.\n");
	system("sudo dd if=/dev/mtd0 of=/spi_backup bs=1M count=1 status=progress");
	system("sudo dd if=/dev/zero of=/dev/mtd0 bs=1M count=1 status=progress");
	sleep(5);
}

void firefox()
{
	system("swaymsg workspace 2 || true");
	system("firefox");
	system("swaymsg workspace 1 || true");
}

void todo()
{
	// do nothing
	;
}

void usb_mass()
{
	system("sudo /dtg-ng/programs/usb_mass_helper start");

	// TODO: global joyfd?
	int joyfd;
	struct input_event ev[64];
	joyfd = open(JOYDEV, O_RDONLY);

	read(joyfd, ev, sizeof(struct input_event) * 64);
	system("sudo /dtg-ng/programs/usb_mass_helper stop");
}

struct type_item
{
	char name[32];
	void ((*function)());
	int type;
	int child_id;
};

struct menu
{
	char name[32];
	struct type_item items[32];
	int size;
	int id;
	int parent_id;
};

int main(void)
{
	int		joyfd;
	joyfd = open(JOYDEV, O_RDONLY);

	struct		input_event ev[64];
	int		rd;

	struct menu menus[4];

	// init curses
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);


	strcpy(menus[MAINMENU].name, "Main Menu");
	menus[MAINMENU].size = 12;			// it has to be one more than highest index
	menus[MAINMENU].parent_id = MAINMENU;		// it is THE parent

	// set names of children
	strcpy(menus[MAINMENU].items[0].name, "Run Retroarch");
	strcpy(menus[MAINMENU].items[1].name, "Run STK");
	strcpy(menus[MAINMENU].items[2].name, "Run Minecraft");
	strcpy(menus[MAINMENU].items[3].name, "Run Kodi");
	strcpy(menus[MAINMENU].items[4].name, "Run Xash3D");
	strcpy(menus[MAINMENU].items[5].name, "Run PPSSPP");
	strcpy(menus[MAINMENU].items[6].name, "Run TheXTech");
	strcpy(menus[MAINMENU].items[7].name, "Run re3");
	strcpy(menus[MAINMENU].items[8].name, "Run media player");
	strcpy(menus[MAINMENU].items[9].name, "Enable USB Mass storage");
	strcpy(menus[MAINMENU].items[10].name, "Update DTG-NG");
	strcpy(menus[MAINMENU].items[11].name, "Poweroff");

	// point function pointers at functions
	menus[MAINMENU].items[0].function = &retroarch;
	menus[MAINMENU].items[1].function = &supertuxkart;
	menus[MAINMENU].items[2].function = &todo; //&minecraft;
	menus[MAINMENU].items[3].function = &kodi;
	menus[MAINMENU].items[4].function = &todo; //xash3d
	menus[MAINMENU].items[5].function = &ppsspp;
	menus[MAINMENU].items[6].function = &todo; //thextech
	menus[MAINMENU].items[7].function = &re3;
	menus[MAINMENU].items[8].function = &todo; //media player
	menus[MAINMENU].items[9].function = &usb_mass; //usb
	menus[MAINMENU].items[10].function = &todo; //update
	menus[MAINMENU].items[11].function = &poweroff;

	// set default as function type
	for(int i = 0; i < menus[MAINMENU].size; i++)
		menus[MAINMENU].items[i].type = TYPE_FUNC;

	// note: screen on ogs is 85x24
	for(int i = 0; i < menus[where_i_am].size; i++)
		mvprintw(i+1, 4, menus[where_i_am].items[i].name);

	item = 0;
	mvprintw(item+1, 2, ">");

	wrefresh(stdscr);
	
	for(;;)
	{
		rd = read(joyfd, ev, sizeof(struct input_event) * 64);
		if(rd > 0)
		{
			for(int i = 0; i < rd / sizeof(struct input_event); i++)
			{
				if(ev[i].type == EV_KEY && ev[i].value == 1)
				{
					switch(ev[i].code)
					{
						/* fall-through */
						case BTN_B: // TODO: they're swapped on ogs fix this in dts
						case BTN_DPAD_RIGHT:
							// close and open fd to avoid reading data that was put in buffer while gaming
							close(joyfd);

							switch(menus[where_i_am].items[item].type)
							{
								case TYPE_FUNC:
									endwin();
									
									menus[where_i_am].items[item].function();

									// in case something messes with curses
									initscr();
									cbreak();
									noecho();
									keypad(stdscr, TRUE);
									curs_set(0);

									for(int i = 0; i < menus[where_i_am].size; i++)
										mvprintw(i+1, 4, menus[where_i_am].items[i].name);
									break;
								case TYPE_MENU:
									// switch to child
									where_i_am = menus[where_i_am].items[item].child_id;
									item = 0;
									// now we are in child
									erase();
									for(int i = 0; i < menus[where_i_am].size; i++)
										mvprintw(i+1, 4, menus[where_i_am].items[i].name);
									break;
								case TYPE_PARENT:
									// switch to parent
									where_i_am = menus[where_i_am].parent_id;
									item = 0;
									// now we are in parent
									erase();
									for(int i = 0; i < menus[where_i_am].size; i++)
										mvprintw(i+1, 4, menus[where_i_am].items[i].name);
									break;

							}

							joyfd = open(JOYDEV, O_RDONLY);
							break;

						/* fall-through */
						case BTN_A:
						case BTN_DPAD_LEFT:
							// this switches to parent	
							where_i_am = menus[where_i_am].parent_id;
							item = 0;

							erase();

							for(int i = 0; i < menus[where_i_am].size; i++)
								mvprintw(i+1, 4, menus[where_i_am].items[i].name);

							break;
						case BTN_X:
							break;
						case BTN_Y:
							break;
						case BTN_DPAD_UP:
							if(item > 0)
							{
								mvprintw(item+1, 2, " ");
								item--;
							}
							break;
						case BTN_DPAD_DOWN:
							if(item < menus[where_i_am].size - 1)
							{
								mvprintw(item+1, 2, " ");
								item++;
							}
							break;
					}
				}
			}
		}
		mvprintw(item+1, 2, ">");
		wrefresh(stdscr);
	}

	endwin();
	printf("Goodbye\n");
	return 0;
}
