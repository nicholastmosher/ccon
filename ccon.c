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

	list->size = 0;

	return list;
}

/**
 * Creates a new command node for a linked list.
 */
command *create_command(char *name) {

	if(name == NULL) {

		log_warn("create_command() failed: Name is null.");

		return NULL;
	}

	debug("Execute create_command(\"%s\").", name);

	command *node = calloc(1, sizeof(command));

	node->name = strdup(name);

	node->id = -1;

	node->size = sizeof(int) + sizeof(node->id) + sizeof(node->next) + sizeof(name);

	return node;
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

	if(list == NULL) {

		log_warn("Add Command failed, list is null!");

		return;
	}

	if(node == NULL) {

		log_warn("Add Command failed, node is null!");

		return;
	}

	debug("Execute add_command(\"%s\").", node->name);

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

	debug("Command id %d named \"%s\" added; List size is %d.", node->id, node->name, list->size);
}

/**
 * Removes the named command from the given list.
 * @param *list A pointer to the Command List to remove from.
 * @param *removeCommand The name of the Command to remove.
 * @return A pointer to the command that was removed.
 */
command *remove_command(commandList *list, char *removeCommand) {

	debug("Execute remove_command(\"%s\").", removeCommand);

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

			debug("Removing command id %d named \"%s\".", node->id, node->name);

			list->head = NULL;

			list->tail = NULL;

			list->size = 0;

			return node;
		}

		debug("No command with id %d named \"%s\" in list.", node->id, node->name);

	//If the size of the list is not 0 or 1.
	} else {

		////Hold a pointer to the "current" node.
		//command *node = list->head;

		////While this command is not the command to remove and the next isn't NULL.
		//while((strcmp(node->name, removeCommand)) && node->next != NULL) {

		//	debug("Searching for node to remove: id %d named \"%s\".", node->id, node->name);

		//	//Increment node.
		//	node = node->next;
		//}

		//debug("Found node to remove: id %d named \"%s\".", node->id, node->name);

		////If the next node is NULL, we hit the end of the list with no match.
		//if(node->next == NULL) {

		//	log_warn("Command to remove does not exist!");

		//	return NULL;
		//}

		//debug("Node to remove is id %d in list.", node->id);

		////Otherwise, the next node is the one to delete.
		//command *removeNode = node->next;

		////If there's no node after the one to remove, the node becomes tail.
		//if(removeNode->next == NULL) {

		//	list->tail = node;

		////If there IS a node after, repair the list links.
		//} else {

		//	node->next = removeNode->next;

		//	//Update the id's of all next commands.
		//	int currentId = node->id;

		//	while(node->next != NULL) {

		//		//Increment current node and working id.
		//		node = node->next;

		//		//Set the node's id to one more than the last node's id.
		//		node->id = ++currentId;
		//	}
		//}

		////Since we've removed a node, the list is one element smaller.
		//list->size--;

		//debug("List size after removal is %d.", list->size);

		//return removeNode;
///////////////////////////////////////////////////////////////////////////////
		//Hold a pointer to the "current" node.
		debug("stuff and things.");
		command *node = list->head;

		debug("Sifting for node to remove: id %d named \"%s\".", node->id, node->name);

		//Check if the first node is the one to remove.
		if(!strcmp(node->name, removeCommand)) {

			//If the next node isn't null, set it as the head.
			if(node->next != NULL) {

				//Set the second node as the new head.
				list->head = node->next;
			}

			//Return the reference to the node that was removed.
			return node;
		}

		//While the next node isn't the one to remove, loop.
		while(node->next != NULL && strcmp(node->next->name, removeCommand)) {

			debug("Sifting for node to remove: id %d named \"%s\".", node->next->id, node->next->name);

			//Increment node.
			node = node->next;
		}

		command *node_to_remove = node->next;

		debug("Found command \"%s\" to remove at id %d.", node_to_remove->name, node_to_remove->id);

		return node;
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

		log_info("Command id %d named \"%s\".", node->id, node->name);

		//Increment node.
		node = node->next;
	}

error:
	return;
}

void test_list();

int main(int argc, char *argv[]) {

	test_list();

	return 0;
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

	remove_command(list, "Command Three");

	print_list(list);

	destroy_command_list(list);
}
