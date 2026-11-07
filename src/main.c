#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>

#include "globals.h"
#include "actions.h"
#include "tty_handle.h"

#define MAX_FILESYSTEM_OBJECTS 256
#define PRIOBJ_NO_SELECTOR -1
#define OBJECT_DISPLAY_FRAME_SIZE 30

struct termios original_configuration = {0};
struct termios modified_configuration = {0};
char user_input_buffer[USER_INPUT_BUFFER_SIZE];

struct filesystem_object_t filesystem_object[MAX_FILESYSTEM_OBJECTS] = {0};
struct filesystem_object_t selected_object = {0};
size_t object_stack_size = 0;
ssize_t selector = 0;

/*i don't know in what other way i should do this*/
int regular_file(const struct dirent *ent) {
	return ent->d_type == DT_REG;
}
int regular_directory(const struct dirent *ent) {
	return ent->d_type == DT_DIR;
}
int file_socket(const struct dirent *ent) {
	return ent->d_type == DT_SOCK;
}
int file_fifo(const struct dirent *ent) {
	return ent->d_type == DT_FIFO;
}
int file_chr(const struct dirent *ent) {
	return ent->d_type == DT_CHR;
}
int file_blk(const struct dirent *ent) {
	return ent->d_type == DT_BLK;
}
int file_lnk(const struct dirent *ent) {
	return ent->d_type == DT_LNK;
}

size_t add_objects(int (*sel)(const struct dirent *), size_t pm_object_stack) {
	struct dirent **entries;

	size_t fn_total_obj = scandir(".",&entries,sel,alphasort);

	size_t true_object = 0;
	for (size_t i = 0; i < fn_total_obj; i++) {
		if ( (strcmp(entries[i]->d_name,"." ) != 0) && (strcmp(entries[i]->d_name,".." ) != 0) ){

			strcpy(filesystem_object[true_object + pm_object_stack].name,entries[i]->d_name);
			filesystem_object[true_object + pm_object_stack].type = entries[i]->d_type;

			free(entries[i]);
			true_object++;
		}
	}
	free(entries);

	return true_object;
}

void update_working_directory(void) {
	getcwd(current_working_directory, sizeof(current_working_directory));
}

void reset_object_stack(void) {
	struct filesystem_object_t fsys_blank = {0};
	for (size_t i = 0; i < MAX_FILESYSTEM_OBJECTS; i++) {
		filesystem_object[i] = fsys_blank;
	}
	object_stack_size = 0;
}

void default_item_update(void) {
	reset_object_stack();
	update_working_directory();
	/*normally this could be solved by making a for loop and making a lookup
	table of functions so that they get executed by index according to the for loop.
	But i gotta do more research to know if that's true or not, Because this looks simple.*/
	object_stack_size += add_objects(regular_file,object_stack_size);
	object_stack_size += add_objects(regular_directory,object_stack_size);
	object_stack_size += add_objects(file_socket,object_stack_size);
	object_stack_size += add_objects(file_fifo,object_stack_size);
	object_stack_size += add_objects(file_chr,object_stack_size);
	object_stack_size += add_objects(file_blk,object_stack_size);
	object_stack_size += add_objects(file_lnk,object_stack_size);
}

void print_objects(ssize_t selector_pm, size_t object_stack_size_pm) {
	printf("< %s > :\n",current_working_directory);
	size_t i = 0;
	size_t frame;
	if (object_stack_size_pm > OBJECT_DISPLAY_FRAME_SIZE) {
		i = selector;
		frame = i + OBJECT_DISPLAY_FRAME_SIZE;
	}
	for (i; i < object_stack_size_pm; i++) {
		if (i > frame) {
			break;
		}
		switch(filesystem_object[i].type) {
			/*Efficient? Or Inneficient??*/
			case DT_REG:
				printf("- %s",filesystem_object[i].name);
				break;
			case DT_DIR:
				printf("/ %s",filesystem_object[i].name);
				break;
			case DT_SOCK:
				printf("*Socket: %s",filesystem_object[i].name);
				break;
			case DT_FIFO:
				printf("*FIFO: %s",filesystem_object[i].name);
				break;
			case DT_CHR:
				printf("*Character Device: %s",filesystem_object[i].name);
				break;
			case DT_BLK:
				printf("*Block Device: %s",filesystem_object[i].name);
				break;
			case DT_LNK:
				printf(">>Link: %s",filesystem_object[i].name);
				break;
			/*I don't understand why gcc marks these as undefined if dirent.h has definitions of it*/
			/* case DT_MQ:
				printf("*Message Queue: %s",filesystem_object[i].name);
				break;
			case DT_SEM:
				printf("*Semaphore: %s",filesystem_object[i].name);
				break;
			case DT_SHM:
				printf("*Shared Memory Object: %s",filesystem_object[i].name);
				break;
			case DT_TMO:
				printf("*Typed Memory Object: %s",filesystem_object[i].name);
				break;*/
			default:
				printf("Unknown file type\n");
				break;
		}
		if (i == selector_pm && i >= selector_pm) {
			printf(" <\n");
		} else {
			printf("\n");
		}
	}
	if (i == 0) {
		printf("[No Filesystem objects in this directory]\n");
	}
}

int main(void) {
	tcgetattr(STDIN_FILENO,&original_configuration);
	tty_setup();
	default_item_update();
	print_objects(selector,object_stack_size);
	printf("\033[2J\033[H");

	while (1) {
		read(STDIN_FILENO,user_input_buffer,sizeof(user_input_buffer));
		tty_identify(user_input_buffer,sizeof(user_input_buffer),filesystem_object[selector]);
		default_item_update();

		if (selector < 0) {
			selector = 0;
		} else if (selector >= object_stack_size) {
			selector = object_stack_size - 1;
		}
		selected_object = filesystem_object[selector];
		printf("\033[2J\033[H");
		print_objects(selector,object_stack_size);
	}

	tty_reset();

	return 0;
}
