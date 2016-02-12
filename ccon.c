#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DOLOG
#define DEBUG
#include "debug/dbg.h"

/**
 * Define structs to store argument data as
 * nodes in a linked list.
 */
typedef struct _arg_node {

	int length;

	void *data;

	struct _arg_node *nextArg;

} arg_node;

/**
 * Define commands as nodes for a linked list.
 * Commands have an associated name and id,
 * as well as pointers to the next and previous
 * nodes in the linked list.
 */
typedef struct _command {

	//arg_node *firstArg;

	int size;

	int id;

	struct _command *next;

	char *name;

} command;

/**
 * A linked list for storing commands.
 */
typedef struct _command_list {

	int size;

	command *head;

	command *tail;

} command_list;

/**
 * Creates a linked list to store commands.
 */
command_list *create_command_list() {

	command_list *list = calloc(1, sizeof(command_list));

	//Verify the memory we were given.
	check_mem(list);

	list->size = 0;

	return list;

error:
	return NULL;
}

/**
 * Creates a new command node for a linked list.
 */
command *create_command(char *name) {

	check(name != NULL, "create_command() failed: Name is null.");

	debug("Execute create_command(%s).", name);

	command *node = calloc(1, sizeof(command));

	//Verify the memory we were given.
	check_mem(node);

	node->name = strdup(name);

	node->id = -1;

	node->size = sizeof(int) + sizeof(node->id) + sizeof(node->next) + sizeof(name);

	return node;

error:
	return NULL;
}

/**
 * Frees memory allocated for a command.
 */
void destroy_command(command *destroy) {

	check(destroy != NULL, "Command to destroy is null.");

	free(destroy->name);

	free(destroy);

error:
	return;
}

/**
 * Frees the memory allocated for the given command
 * list by destroying all contained commands and
 * the list itself.
 */
void destroy_command_list(command_list *list) {

	if(list->head != NULL) {

		command *prev = list->head;

		while(prev->next != NULL) {

			command *current = prev->next;

			destroy_command(prev);

			prev = current;
		}

		destroy_command(prev);
	}

	free(list);
}

/**
 * Adds the given command to the end of the given list.
 */
void add_command(command_list *list, command *node) {

	check(list != NULL, "add_command() failed, list is null.");

	check(node != NULL, "add_command() failed, node is null.");

	debug("Execute add_command(%s).", node->name);

	//If the size of the list is 0, this node will be both head and tail.
	if(!list->size) {

		node->id = 0;

		list->head = node;

		list->tail = node;

	} else {

		//Get the pointer of the head node.
		command *currentNode = list->head;

		//Count how many nodes we traverse to get to the end.
		int id = 1;

		while(currentNode->next != NULL) {

			currentNode = currentNode->next;
			id++;
		}

		node->id = id;

		currentNode->next = node;

		list->tail = node;
	}

	list->size++;

	debug("Command id %d named '%s' added; List size is %d.", node->id, node->name, list->size);

error:
	return;
}

/**
 * Loops through the given list and reassigns all command id's in order.
 */
void relink_list(command_list *list) {

	check(list != NULL, "List pointer is null.");

	//Get a reference to the head of the list.
	command *node = NULL;
	int id = 0;

	//Loop through all nodes and re-assign id's in order.
	do {
		if(node == NULL) {

			node = list->head;

			if(node == NULL) {

				log_err("List head is null.");
				list->size = 0;
				return;
			}

		} else {

			node = node->next;
		}

		//If the node's id is out of place, reassign it and log.
		if(node->id != id) {

			log_info("Command %s at incorrect id %d. Reassigning id %d.", node->name, node->id, id);

		} else {

			log_info("Command %s at correct id %d.", node->name, node->id);
		}

		node->id = id++;

	} while(node->next != NULL);

	//At the end of the loop we know the definitive length of the list.
	list->size = id;

error:
	return;
}

/**
 * Removes the named command from the given list.
 * @param *list A pointer to the Command List to remove from.
 * @param *command_name The name of the Command to remove.
 * @return A pointer to the command that was removed.
 */
command *remove_command(command_list *list, char *command_name) {

	debug("Execute remove_command(%s).", command_name);

	//Null safety check the list pointer.
	check(list != NULL, "Remove Command failed, list is null.");

	//Null safety check the command name.
	check(command_name != NULL, "Remove command failed, node is null.");

	//If the list has 0 elements, there's nothing to remove.
	check(list->size, "List has no commands. No commands removed.");

	//Hold a pointer to the "current" node.
	command *node = list->head;

	check(node != NULL, "Head node in the list is null.");

	command *last_node = NULL;

	while(node != NULL) {

		//If this node is the command to remove, remove it.
		if(!strcmp(node->name, command_name)) {

			//Check if this node is the list head.
			if(node == list->head) {

				debug("Command to remove: '%s' is the list head.", list->head->name);

				//If the next node isn't null, set it as the head.
				if(node->next != NULL) {

					list->head = node->next;
					relink_list(list);

				} else {

					//If there are no more nodes in the list, set head to null.
					list->head = NULL;
				}

				//Return the reference to the node that was removed.
				return node;

			//If this node is not the list head.
			} else {

				debug("Command to remove: '%s' was found at %d.", node->name, node->id);

				//If there are nodes following the one we remove, relink them.
				if(node->next != NULL) {

					debug("Relinking node %d named '%s' to node %d named '%s'",
							node->next->id, node->next->name, last_node->id, last_node->name);

					last_node->next = node->next;

				//If this is the last node in the list.
				} else {

					debug("Command being removed is the last node on the list.");

					last_node->next = NULL;

					list->tail = NULL;
				}

				relink_list(list);

				return node;
			}
		//If this is not the command to remove.
		} else {

			debug("Searching through commands. Passing '%s'.", node->name);

			last_node = node;

			node = node->next;
		}
	}

	//If we hit a node that was null, there was no match.
	printf("There is no command named '%s'.", command_name);
	return NULL;

error:
	printf("Failed to remove command '%s'.", command_name);
	return NULL;
}

/**
 * Prints the contents of the given list.
 */
void print_list(command_list *list) {

	debug("Execute print_list()");

	check(list != NULL, "List to print is null.");

	command *node = list->head;

	while(node != NULL) {

		printf("Command id %d named '%s'\n", node->id, node->name);

		node = node->next;
	}

error:
	return;
}

/**
 * Performs brief checks on list add/remove functionality.
 */
void test_list() {

	command_list *list = create_command_list();

	command *c1 = create_command("Apple");
	command *c2 = create_command("Banana");
	command *c3 = create_command("Cantaloupe");
	command *c4 = create_command("Dates");
	command *c5 = create_command("Eggplant");
	command *c6 = create_command("Fruit");

	add_command(list, c1);
	add_command(list, c2);
	add_command(list, c3);
	add_command(list, c4);
	add_command(list, c5);
	add_command(list, c6);

	debug("List size is %d.", list->size);

	print_list(list);

	debug("Removing 'Cantaloupe'.");

	destroy_command(remove_command(list, "Cantaloupe"));

	print_list(list);

	debug("Removing 'Apple'.");

	destroy_command(remove_command(list, "Apple"));

	print_list(list);

	destroy_command_list(list);
}

/**
 * Reads one line from standard in and null terminates it.
 * TODO Fix memory leak here.
 */
char *get_input() {

	char *line = NULL;

	size_t size;

	ssize_t chars = getline(&line, &size, stdin);

	check(line != NULL, "Line read from stdin is null.");

	//Replace newline with string terminator.
	if(line[chars-1] == '\n') {
		line[chars-1] = '\0';
	}

	return line;

error:
	return NULL;
}

int main(int argc, char *argv[]) {

	command_list *list = create_command_list();

	check(list != NULL, "List creation failed.");

	char *input = NULL;

	printf("ccon> ");

	//Begin input parsing loop.
	while((input = get_input()) != NULL) {

		debug("Received input: %s", input);

		input = strtok(input, " ");

		if(input == NULL) {

			printf("ccon> ");
			continue;
		}

		debug("Parsed command %s", input);

		//If the user entered an "add" command.
		if(!strcmp(input, "add")) {

			char *name = strtok(NULL, " ");

			if(name == NULL) {
				printf("Error reading command. Try again.\n");
				continue; }

			command *com = create_command(name);

			check(com != NULL, "Command pointer null.");
			if(com == NULL) {

			}

			add_command(list, com);

		//If the user entered a "remove" command.
		} else if(!strcmp(input, "remove") || !strcmp(input, "rm")) {

			char *name = strtok(NULL, " ");

			if(name == NULL) {
				log_info_or_printf("Command name argument is null.");
				continue;
			}

			command *to_remove = remove_command(list, name);

			if(to_remove == NULL) {
				log_info_or_printf("Command to remove: '%s' does not exist.", name);
				continue;
			}

			destroy_command(to_remove);

		//If the user entered a "list" command.
		} else if(!strcmp(input, "list") || !strcmp(input, "ls")) {

			print_list(list);

		//If the user entered a "relink" command.
		} else if(!strcmp(input, "relink")) {

			relink_list(list);

		//If the user entered a "size" command.
		} else if(!strcmp(input, "size")) {

			printf("List size is %d.\n", list->size);

		//If the user entered a "quit" command.
		} else if(!strcmp(input, "quit") || !strcmp(input, "q") ||
				!strcmp(input, "exit")) {

			break;

		} else {

			printf("Command unrecognized.\n");
		}

		printf("ccon> ");
	}

	destroy_command_list(list);

	printf("Quit\n");

	return 0;

error:

	destroy_command_list(list);
	return 1;
}
