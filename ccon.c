#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dbg.h"

/**
 * Define structs to store argument data as
 * nodes in a linked list.
 */
typedef struct _argNode {

	int length;

	void *data;

	struct _argNode *nextArg;

} argNode;

/**
 * Define commands as nodes for a linked list.
 * Commands have an associated name and id,
 * as well as pointers to the next and previous
 * nodes in the linked list.
 */
typedef struct _command {

	//argNode *firstArg;

	int size;

	int id;

	struct _command *next;

	char *name;

} command;

/**
 * A linked list for storing commands.
 */
typedef struct _commandList {

	int size;

	command *head;

	command *tail;

} commandList;

/**
 * Creates a linked list to store commands.
 */
commandList *create_command_list() {

	commandList *list = calloc(1, sizeof(commandList));

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

	free(destroy->name);

	free(destroy);
}

/**
 * Frees the memory allocated for the given command
 * list by destroying all contained commands and
 * the list itself.
 */
void destroy_command_list(commandList *list) {

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
void add_command(commandList *list, command *node) {

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
void relink_list(commandList *list) {

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

			log_info("Command %s at correct id   %d.", node->name, node->id);
		}

		node->id = id++;

	} while(node->next != NULL);

	//At the end of the loop we know the difinitive length of the list.
	list->size = id;

error:
	return;
}

/**
 * Removes the named command from the given list.
 * @param *list A pointer to the Command List to remove from.
 * @param *removeCommand The name of the Command to remove.
 * @return A pointer to the command that was removed.
 */
command *remove_command(commandList *list, char *removeCommand) {

	debug("Execute remove_command(%s).", removeCommand);

	//Null safety check the list pointer.
	check(list != NULL, "Remove Command failed, list is null.");

	//Null safety check the command name.
	check(removeCommand != NULL, "Remove command failed, node is null.");

	//If the list has 0 elements, there's nothing to remove.
	if(!list->size) {

		log_warn("List has no commands! No commands removed.");

		return NULL;

	//If the size of the list is 1, there's only one element.
	} else if(list->size == 1) {

		command *node = list->head;

		//If this command is indeed the one to remove, remove it.
		if(!strcmp(node->name, removeCommand)) {

			debug("Removing command id %d named '%s'.", node->id, node->name);

			list->head = NULL;

			list->tail = NULL;

			list->size = 0;

			return node;
		}

		debug("No command with id %d named '%s' in list.", node->id, node->name);

		return NULL;

	//If the size of the list is not 0 or 1.
	} else {

		//Hold a pointer to the "current" node.
		command *node = list->head;

		debug("Sifting for node to remove: id %d named '%s'.", node->id, node->name);

		//Check if the first node is the one to remove.
		if(!strcmp(node->name, removeCommand)) {

			//If the next node isn't null, set it as the head.
			if(node->next != NULL) {

				//Set the second node as the new head.
				list->head = node->next;
			}

			relink_list(list);

			//Return the reference to the node that was removed.
			return node;
		}

		//While the next node isn't the one to remove, loop.
		while(node->next != NULL && strcmp(node->next->name, removeCommand)) {

			debug("Sifting for node to remove: id %d named '%s'.", node->next->id, node->next->name);

			//Increment node.
			node = node->next;
		}

		command *node_to_remove = node->next;

		debug("Found command '%s' to remove at id %d.", node_to_remove->name, node_to_remove->id);

		//Fix linked list by taking the removed node's next and attach it to node.
		if(node_to_remove->next != NULL) {

			debug("Relinking node id %d named '%s' to node id %d named '%s'.",
					node_to_remove->next->id, node_to_remove->next->name, node->id, node->name);

			node->next = node_to_remove->next;

		} else {

			//Set the last node as the tail.
			list->tail = node;

			node->next = NULL;
		}

		relink_list(list);

		return node_to_remove;
	}

error:
	return NULL;
}

void print_list(commandList *list) {

	debug("Execute print_list()");

	check(list != NULL, "List to print is null.");

	command *node = list->head;

	check(node != NULL, "Head node is null.");

	while(node->next != NULL) {

		printf("Command id %d named '%s'.\n", node->id, node->name);

		//Increment node.
		node = node->next;
	}

	printf("Command id %d named '%s'.\n", node->id, node->name);

error:
	return;
}

void test_list() {

	commandList *list = create_command_list();

	command *c1 = create_command("Command One");
	command *c2 = create_command("Command Two");
	command *c3 = create_command("Command Three");
	command *c4 = create_command("Command Four");
	command *c5 = create_command("Command Five");
	command *c6 = create_command("Command Six");

	add_command(list, c1);
	add_command(list, c2);
	add_command(list, c3);
	add_command(list, c4);
	add_command(list, c5);
	add_command(list, c6);

	debug("List size is %d.", list->size);

	print_list(list);

	destroy_command(remove_command(list, "Command Three"));

	print_list(list);

	destroy_command(remove_command(list, "Command Five"));

	print_list(list);

	destroy_command_list(list);
}

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

	commandList *list = create_command_list();

	char *input = NULL;

	printf("ccon> ");

	//Begin input parsing loop.
	while((input = get_input()) != NULL) {

		debug("Received input: %s", input);

		input = strtok(input, " ");

		check(input != NULL, "Command token is null.");

		debug("Parsed command %s", input);

		//If the user entered an "add" command.
		if(!strcmp(input, "add") || !strcmp(input, "a")) {

			char *name = strtok(NULL, " ");

			check(name != NULL, "Command name argument is null.");

			command *com = create_command(name);

			check(com != NULL, "Command pointer null.");

			add_command(list, com);

		//If the user entered a "remove" command.
		} else if(!strcmp(input, "remove") || !strcmp(input, "r")) {

			char *name = strtok(NULL, " ");

			check(name != NULL, "Command name argument is null.");

			destroy_command(remove_command(list, name));

		//If the user entered a "list" command.
		} else if(!strcmp(input, "list") || !strcmp(input, "l")) {

			print_list(list);

		} else if(!strcmp(input, "relink")) {

			relink_list(list);

		} else if(!strcmp(input, "size")) {

			printf("List size is %d.", list->size);

		//If the user entered a "quit" command.
		} else if(!strcmp(input, "quit") || !strcmp(input, "q") ||
				!strcmp(input, "exit")) {

			return 0;

		} else {

			printf("Command unrecognized.\n");
		}

		printf("ccon> ");
	}

	destroy_command_list(list);

	printf("Quit\n");

	return 0;

error:
	return 1;
}
