#include	<stdio.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<math.h>
#include	<linux/input.h>
#include	<fcntl.h>
#include	<curses.h>
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

void poweroff()
{
	system("systemctl poweroff -i || true");
}

void update_rootfs()
{
	// if we don't deinit and reinit ncurses, logs will look sorta weird
	endwin();
	
	system("sudo apt update || true");
	system("sudo apt upgrade -y || true");
	sleep(2); // user needs to see it succeed

	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);
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
	menus[MAINMENU].size = 7;			// it has to be one more than highest index
	menus[MAINMENU].parent_id = MAINMENU;		// it is THE parent

	// set names of children
	strcpy(menus[MAINMENU].items[0].name, "Launch Retroarch");
	strcpy(menus[MAINMENU].items[1].name, "Launch STK");
	strcpy(menus[MAINMENU].items[2].name, "Launch Minecraft");
	strcpy(menus[MAINMENU].items[3].name, "Launch Kodi");
	strcpy(menus[MAINMENU].items[4].name, "Settings");
	strcpy(menus[MAINMENU].items[5].name, "Utilities");
	strcpy(menus[MAINMENU].items[6].name, "Poweroff");

	// point function pointers at functions
	menus[MAINMENU].items[0].function = &retroarch;
	menus[MAINMENU].items[1].function = &supertuxkart;
	//menus[MAINMENU].items[2].function = &minecraft;
	//menus[MAINMENU].items[3].function = &kodi;
	//menus[MAINMENU].items[4].function = NONE
	//menus[MAINMENU].items[5].function = NONE
	menus[MAINMENU].items[6].function = &poweroff;

	// set default as function type
	for(int i = 0; i < menus[MAINMENU].size; i++)
		menus[MAINMENU].items[i].type = TYPE_FUNC;

	// and override it later for special items
	menus[MAINMENU].items[4].type = TYPE_MENU;
	menus[MAINMENU].items[4].child_id = SETTINGS;

	menus[MAINMENU].items[5].type = TYPE_MENU;
	menus[MAINMENU].items[5].child_id = UTILITY;
	
	strcpy(menus[SETTINGS].name, "Settings");
	menus[SETTINGS].size = 3;
	menus[SETTINGS].parent_id = MAINMENU;

	strcpy(menus[SETTINGS].items[0].name, "SSH Access:");
	strcpy(menus[SETTINGS].items[1].name, "WiFi Settings");
	strcpy(menus[SETTINGS].items[2].name, "Go back to main menu");

	//menus[SETTINGS].items[0].function = &ssh_toggle;
	//menus[SETTINGS].items[0].function = &wifi_set;
	//menus[SETTINGS].items[0].function = NONE

	for(int i = 0; i < menus[SETTINGS].size; i++)
		menus[SETTINGS].items[i].type = TYPE_FUNC;

	menus[SETTINGS].items[2].type = TYPE_PARENT;

	strcpy(menus[UTILITY].name, "Utilities");
	menus[UTILITY].size = 7;
	menus[UTILITY].parent_id = MAINMENU;

	strcpy(menus[UTILITY].items[0].name, "Update rootfs");
	strcpy(menus[UTILITY].items[1].name, "Update kernel");
	strcpy(menus[UTILITY].items[2].name, "USB Modes");
	strcpy(menus[UTILITY].items[3].name, "File manager");
	strcpy(menus[UTILITY].items[4].name, "Music player");
	strcpy(menus[UTILITY].items[5].name, "Launch Weston");
	strcpy(menus[UTILITY].items[6].name, "Go back to main menu");

	menus[UTILITY].items[0].function = &update_rootfs;
	//menus[UTILITY].items[1].function = &update_kernel;
	//menus[UTILITY].items[2].function = NONE
	//menus[UTILITY].items[3].function = &filemanager;
	//menus[UTILITY].items[4].function = &music;
	//menus[UTILITY].items[5].function = &weston;
	//menus[UTILITY].items[6].function = NONE

	for(int i = 0; i < menus[MAINMENU].size; i++)
		menus[UTILITY].items[i].type = TYPE_FUNC;

	menus[UTILITY].items[2].type = TYPE_MENU;
	menus[UTILITY].items[2].child_id = USBMODE;

	menus[UTILITY].items[6].type = TYPE_PARENT;

	strcpy(menus[USBMODE].name, "USB Mode");
	menus[USBMODE].size = 4;
	menus[USBMODE].parent_id = UTILITY;

	strcpy(menus[USBMODE].items[0].name, "Restore Host USB mode");
	strcpy(menus[USBMODE].items[1].name, "Switch to USB Storage mode");
	strcpy(menus[USBMODE].items[2].name, "Switch to USB Gamepad mode");
	strcpy(menus[USBMODE].items[3].name, "Go back to utilities menu");

	for(int i = 0; i < menus[MAINMENU].size; i++)
		menus[USBMODE].items[i].type = TYPE_FUNC;

	menus[USBMODE].items[3].type = TYPE_PARENT;


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
