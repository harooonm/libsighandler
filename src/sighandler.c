#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

/*lazy*/
#define attrib(a) __attribute__((a))                                           
#define nonNULL(...) attrib(nonNULL(__VA_ARGS__))

typedef void (*on_sig)(int);
typedef void (*on_sigact)(int, siginfo_t *, void *);

enum handler_type{
	SA_HANDLER,
	SIGACT_HANDLER
};

struct list_node{
	on_sig sig_handler;
	on_sigact sigact_handler;
	struct list_node *next;
};

struct tree_node{
	int sig_nr;
	struct sigaction *old_act;
	struct list_node *handlers;
	struct tree_node *left;
	struct tree_node *right;
}attrib(packed);

static struct tree_node *root;
static pthread_mutex_t mtx;

static struct tree_node nonNULL(1) **find_sig_node(struct tree_node **n,
                int sig_nr)
{
        struct tree_node **node_ptr  = n;
        while(*node_ptr){
                if ((*node_ptr)->sig_nr == sig_nr)
                        return node_ptr;
                if (sig_nr > (*node_ptr)->sig_nr)
                        node_ptr = &(*node_ptr)->right;
                else
                        node_ptr = &(*node_ptr)->left;
        }
        return NULL;
}



static void attrib(constructor) construct()
{
	root->old_act = NULL;
	root->handlers = NULL;
	root->left = NULL;
	root->right = NULL;
	pthread_mutex_init(&mtx, NULL);
}

static void free_tree(struct tree_node **n)
{
	if (!*n)
		return;
	free_tree(&(*n)->left);
	free_tree(&(*n)->right);

	/*free the list*/

	/*free the tree node*/
	free(*n);
}

static void nonNULL(1) del_node(struct tree_node **root, int sig_nr)
{
	struct tree_node **node = find_sig_node(root, sig_nr);
	if (!node)
		return;
	if ((*node)->left && (*node)->right){
		struct tree_node **follow = &(*node)->right;
		while(1){
			if (!(*follow)->left)
				break;
			follow = &(*follow)->left;
		}
		(*node)->sig_nr = (*follow)->sig_nr;
		free(*follow);
		*follow = NULL;
	}else if ((*node)->left || (*node)->right){
		struct tree_node *cp = (*node)->left ? (*node)->left : (*node)->right;
		free(*node);
		(*node)= NULL;
		*node = cp;
	}else{
		free(*node);
		*node = NULL;
	}
}

static void attrib(destructor) destruct()
{
	free_tree(&root);
	pthread_mutex_destroy(&mtx);
}


static void add_handler(struct list_node **head, on_sig sig_handler, on_sigact act_handler,
		int type)
{
	struct list_node **cur = head;
	while (*cur)
		cur = &((*cur)->next);
	*cur = malloc(sizeof(struct list_node));
	switch (type) {
	case SA_HANDLER:
		(*cur)->sig_handler = sig_handler;
		break;
	case SIGACT_HANDLER:
		(*cur)->sigact_handler = act_handler;
		break;
	}
	(*cur)->next = NULL;
}

static int find_handler(struct list_node *n, on_sigact act, on_sig sahandler)
{
	while (n) {
		if (n->sig_handler && n->sig_handler == sahandler)
			return 1;
		if (n->sigact_handler && n->sigact_handler == act)
			return 1;
		n = n->next;
	}
	return 0;
}

static void nonNULL(1) add_node(struct tree_node **n, int sig_nr, on_sig sig_handler, on_sigact sig_acthandler, int type)
{
        struct tree_node **node_ptr  = n;
        while(*node_ptr){
                if (sig_nr > (*node_ptr)->sig_nr)
                        node_ptr = &(*node_ptr)->right;
                else
                        node_ptr = &(*node_ptr)->left;
        }
        *node_ptr = malloc(sizeof(struct tree_node));
        (*node_ptr)->sig_nr = sig_nr;
        (*node_ptr)->left = NULL;
        (*node_ptr)->right = NULL;
        add_handler(&(*node_ptr)->handlers, sig_handler, sig_acthandler, type);
}

static int _reg_sig(int sig_nr, on_sigact act_hanlder, on_sig sig_handler,
		int blck_mask, int flags, int type)
{
	if (sig_nr == SIGKILL || sig_nr == SIGSTOP)
		return EINVAL;

	pthread_mutex_lock(&mtx);
	int reged = 0;
	struct tree_node **sig_node = find_sig_node(sig_nr);
	if (sig_node != NULL){
		if (!find_handler((*sig_node)->handlers, act_hanlder, sig_handler))
			add_handler(&(*sig_node)->handlers, sig_handler, act_hanlder, type);
	}else {
		add_node(&root, sig_nr, sig_handler, act_hanlder, type);
	}
	pthread_mutex_unlock(&mtx);

	return reged;
}

int reg_sig(int sig_nr, on_sig handler , int blck_mask, int flags)
{
	return _reg_sig(sig_nr, NULL, handler, blck_mask, flags,
			SA_HANDLER);
}

int reg_sigaction(int sig_nr, on_sigact handler, int blck_mask, int flags)
{
	return _reg_sig(sig_nr,  handler, NULL, blck_mask, flags,
			SIGACT_HANDLER);
}
