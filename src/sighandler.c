#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include "libbtree.h"
#include "linked_list.h"
typedef void (*on_sig)(int);
typedef void (*on_sigact)(int, siginfo_t *, void *);

struct list_data {
	on_sig sig_handler;
	on_sigact sigact_handler;
	struct list_node *next;
};

struct tree_data {
	int sig_nr;
	int blck_mask;
	int flags;
	struct sigaction *old_act;
	struct linked_list_node *handlers;
};

struct sig_handler_args {
	int sig_nr;
	siginfo_t *info;
	void *extra;
};

static btree_t *root = NULL;
static pthread_mutex_t mtx;
static struct sigaction action;

#define lock {\
	pthread_mutex_lock(&mtx);

#define unlock pthread_mutex_unlock(&mtx);\
	}

int comp_sig(void *old, void *new)
{
	struct tree_data *oldsig = (struct tree_data *) old;
	struct tree_data *newsig = (struct tree_data *) new;
	if (oldsig->sig_nr == newsig->sig_nr)
		return 0;
	if (newsig->sig_nr > oldsig->sig_nr)
		return 1;
	return -1;
}

int comp_handler(void *old, void *new)
{
	struct list_data *oldnode = (struct list_data *) old;
	struct list_data *newnode = (struct list_data *) new;
	if (oldnode->sig_handler == newnode->sig_handler
	                && oldnode->sigact_handler == newnode->sigact_handler)
		return 1;
	return 0;
}

void free_node_data(void *node)
{
	struct tree_data *n = (struct tree_data *) node;
	if (n){
		linked_list_free(n->handlers, 1);
		free(n);
	}
}

void for_each_list_node(struct linked_list_node *n, void *extra)
{
	struct list_data *ld = (struct list_data *) n->data;
	struct sig_handler_args *a = (struct sig_handler_args *) extra;

	if (ld->sigact_handler && a->info && a->extra)
		ld->sigact_handler(a->sig_nr, a->info, a->extra);
	else if (ld->sig_handler && !a->info && !a->extra)
		ld->sig_handler(a->sig_nr);
}

static void _os_sig_handler(int sig_nr, siginfo_t *info, void *e)
{
	lock
	btree_t *n = find_tree_node(&root, &(struct tree_data){sig_nr, 0, 0, 0, 0}, comp_sig);
	if (n)
		linked_list_itr(((struct tree_data *) n->data)->handlers,
		                for_each_list_node, &(struct sig_handler_args) {sig_nr, info, e});
	unlock
}

static void os_sig_handler(int sig_nr)
{
	_os_sig_handler(sig_nr, NULL, NULL);
}

static void os_sig_acthandler(int sig_nr, siginfo_t *info, void *e)
{
	_os_sig_handler(sig_nr, info, e);
}

static void __attribute__((constructor)) construct()
{
	pthread_mutex_init(&mtx, NULL);
}

static void __attribute__((destructor)) destruct()
{
	free_tree(&root, free_node_data);
	pthread_mutex_destroy(&mtx);
}

static int _reg_sig(int sig_nr, on_sig sig_handler, on_sigact sigact_handler,
                int blck_mask, int flags)
{
	if (!sig_nr || sig_nr == SIGKILL || sig_nr == SIGSTOP
	                || (!sig_handler && !sigact_handler))
		return 0;

	int reged = 0;

	lock
	btree_t *sig_node = find_tree_node(&root, &((struct tree_data){sig_nr, 0, 0, 0, 0}), comp_sig);

	struct list_data *d = calloc(1, sizeof(struct list_data));
	d->sig_handler = sig_handler;
	d->sigact_handler = sigact_handler;

	if (sig_node) {
		struct tree_data *t = (struct tree_data *) sig_node->data;
		if (!linked_list_find(t->handlers, &((struct list_data){sig_handler, sigact_handler, NULL}), comp_handler))
			linked_list_append(&(t->handlers), d);
		reged = 1;
	} else {
		struct tree_data *t = (struct tree_data *)calloc(1, sizeof(struct tree_data));
		t->blck_mask = blck_mask;
		t->sig_nr = sig_nr;
		t->flags = flags;

		sigaddset(&action.sa_mask, blck_mask);
		action.sa_flags |= flags;
		if (sigact_handler)
			action.sa_sigaction = os_sig_acthandler;
		if (sig_handler)
			action.sa_handler = os_sig_handler;

		if (sigaction(sig_nr, &action, t->old_act)){
			free(t);
			free(d);
			goto ret;
		}

		linked_list_append(&(t->handlers), d);
		add_tree_node(&root, t, comp_sig);
		reged = 1;
	}
ret:
	unlock
	return reged;
}

static void _unreg_sig(int sig_num, on_sig sig_handler,
                on_sigact sig_acthandler)
{
	lock
	del_tree_node(&root, (void *) &(struct tree_data){sig_num, 0, 0, 0, 0 }, comp_sig, free_node_data);
	unlock
}

/*public functions*/
int reg_sig(const int sig_nr, const void (*sahandler)(const int),
                const int mask, const int flags)
{
	return _reg_sig(sig_nr, sahandler, NULL, mask, flags);
}

int reg_sigaction(const int sig_nr,
                const void (*sigact_handler)(int, siginfo_t *, void *),
                const int mask, const int flags)
{
	return _reg_sig(sig_nr, NULL, sigact_handler, mask, flags);
}

void unreg_sig(const int sig_nr, const void (*sahandler)(const int))
{
	return _unreg_sig(sig_nr, sahandler, NULL);
}

void unreg_sigaction(const int sig_nr,
                const void (*sigact_handler)(int, siginfo_t *, void *))
{
	return _unreg_sig(sig_nr, NULL, sigact_handler);
}
