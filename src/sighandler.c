#include <pthread.h>
#include <stdlib.h>
#include "sighandler.h"

struct list_node{
	on_signal m_handler;
	struct list_node *m_next;
};

struct tree_node{
	int m_sig_num;
	struct sigaction  *m_old_act;
	struct list_node *m_handlers;
	struct tree_node *m_left;
	struct tree_node *m_right;
}ATTRIB(packed);

static pthread_mutex_t g_mtx;
static struct tree_node *g_root = NULL;
static struct sigaction g_action;

static struct tree_node **find_sig(struct tree_node **root, int sig_num)
{
	if (!*root) 
		return NULL;

	if (sig_num > (*root)->m_sig_num)
		return find_sig(&((*root)->m_right), sig_num);

	if ( sig_num < (*root)->m_sig_num)
		return find_sig(&((*root)->m_left), sig_num);

	return root;
	
}

static void os_sig_handler(int num)
{
	pthread_mutex_lock(&g_mtx);
	struct tree_node **sig = find_sig(&g_root, num);	
	if (!sig) {
		pthread_mutex_unlock(&g_mtx);
		return;
	}

	struct list_node *n = (*sig)->m_handlers;
	while(n) {
		n->m_handler(num);
		n = n->m_next;
	}
	pthread_mutex_unlock(&g_mtx);
}

ATTRIB(constructor) static void  construct()
{
	g_action.sa_handler = os_sig_handler;
	pthread_mutex_init(&g_mtx, NULL);
}

static void free_tree(struct tree_node **node)
{
	if (!*node)
		return;
	free_tree(&(*node)->m_left);
	free_tree(&(*node)->m_right);
	struct list_node *n = (*node)->m_handlers;
	while(n) {
		struct list_node *nn = n->m_next;
		free(n);
		n = nn;
	}
	sigaction((*node)->m_sig_num, (*node)->m_old_act, NULL);
	free((*node));
}

ATTRIB(destructor) static void  destruct()
{
	free_tree(&g_root);
	pthread_mutex_destroy(&g_mtx);
}

static void add_sig_handler(struct list_node **head, on_signal handler)
{
	struct list_node **cur = head;
	while(*cur)
		cur = &((*cur)->m_next);
	*cur = malloc(sizeof(struct list_node));
	(*cur)->m_handler = handler;
	(*cur)->m_next = NULL;
}


static bool add_sig(struct tree_node **root, int sig_num, on_signal handler)
{
	if (!*root) {
		*root = malloc(sizeof(struct tree_node));

		if(sigaction(sig_num, &g_action, (*root)->m_old_act))
			return false;
		(*root)->m_sig_num = sig_num;
		add_sig_handler(&(*root)->m_handlers, handler);
		return true;
	}

	if (sig_num  > (*root)->m_sig_num)
		return add_sig(&((*root)->m_right), sig_num, handler);

	if ( sig_num < (*root)->m_sig_num)
		return add_sig(&((*root)->m_left), sig_num, handler);
	return true;
}


static bool find_callback(struct list_node *head, on_signal handler)
{
	while(head) {
		if (head->m_handler == handler)
			return true;
		head = head->m_next;
	}
	return false;
}


bool reg_handler(int sig_num, on_signal handler)
{
	pthread_mutex_lock(&g_mtx);
	bool reged = true;
	struct tree_node **sig_node = find_sig(&g_root, sig_num);
	if (sig_node) {
		if (!find_callback((*sig_node)->m_handlers, handler))
			add_sig_handler(&(*sig_node)->m_handlers, handler);
	}else{
		reged = add_sig(&g_root, sig_num, handler);
	}
	pthread_mutex_unlock(&g_mtx);
	return reged;
}

void remove_handler(on_signal handler, struct list_node **head)
{
	struct list_node **cur = head;
	while(*cur) {
		if ((*cur)->m_handler != handler)
			continue;
		struct list_node *n = (*cur)->m_next;
		free((*cur));
		*cur = n;
	}
}

void unreg_handler(int sig_num, on_signal handler)
{
	pthread_mutex_lock(&g_mtx);
	struct tree_node **sig_node = find_sig(&g_root, sig_num);

	if (!sig_node) {
		pthread_mutex_unlock(&g_mtx);
		return;
	}

	remove_handler(handler, &((*sig_node)->m_handlers));
	if (!(*sig_node)->m_handlers)
		sigaction((*sig_node)->m_sig_num, (*sig_node)->m_old_act, NULL);
	pthread_mutex_unlock(&g_mtx);
}
