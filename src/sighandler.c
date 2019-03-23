#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>

/*lazy*/
#define attrib(a) __attribute__((a))                                           
#define nonnull(...) attrib(nonnull(__VA_ARGS__))

typedef void (*on_sig)(int);
typedef void (*on_sigact)(int, siginfo_t *, void *);

struct list_node {
	on_sig sig_handler;
	on_sigact sigact_handler;
	struct list_node *next;
};

struct tree_node {
	int sig_nr;
	struct sigaction *old_act;
	struct list_node *handlers;
	struct tree_node *left;
	struct tree_node *right;
}attrib(packed);

static struct tree_node *root;
static pthread_mutex_t mtx;
static struct sigaction action;

static struct tree_node nonnull(1) **find_sig_node(struct tree_node **n,
                int sig_nr)
{
	struct tree_node **node_ptr = n;
	while (*node_ptr) {
		if ((*node_ptr)->sig_nr == sig_nr)
			return node_ptr;
		if (sig_nr > (*node_ptr)->sig_nr)
			node_ptr = &(*node_ptr)->right;
		else
			node_ptr = &(*node_ptr)->left;
	}
	return NULL;
}

static void _os_sig_handler(int sig_nr, siginfo_t *info, void *e)
{
	pthread_mutex_lock(&mtx);
	struct tree_node **n = find_sig_node(&root, sig_nr);
	if (n) {
		struct list_node *ln = (*n)->handlers;
		while (ln) {
			if (info && e)
				ln->sigact_handler(sig_nr, info, e);
			else
				ln->sig_handler(sig_nr);
			ln = ln->next;
		}
	}
	pthread_mutex_unlock(&mtx);
}

static void os_sig_handler(int sig_nr)
{
	_os_sig_handler(sig_nr, NULL, NULL);
}

static void os_sig_acthandler(int sig_nr, siginfo_t *info, void *e)
{
	_os_sig_handler(sig_nr, info, e);
}

static void attrib(constructor) construct()
{
	pthread_mutex_init(&mtx, NULL);
}

static void free_tree(struct tree_node **n)
{
	if (!*n)
		return;
	free_tree(&(*n)->left);
	free_tree(&(*n)->right);

	/*free the list*/
	struct list_node *ln = (*n)->handlers;
	while(ln) {
		struct list_node *nn = ln->next;
		free(ln);
		ln = nn;
	}
	/*restore old action*/
	sigaction((*n)->sig_nr, (*n)->old_act, NULL);

	/*free the tree node*/
	free(*n);
}

static void nonnull(1) del_node(struct tree_node **node, int sig_nr)
{

	if (!node)
		return;
	if ((*node)->left && (*node)->right) {
		struct tree_node **follow = &(*node)->right;
		while (1) {
			if (!(*follow)->left)
				break;
			follow = &(*follow)->left;
		}
		(*node)->sig_nr = (*follow)->sig_nr;
		free(*follow);
		*follow = NULL;
	} else if ((*node)->left || (*node)->right) {
		struct tree_node *cp =
		                (*node)->left ? (*node)->left : (*node)->right;
		free(*node);
		(*node) = NULL;
		*node = cp;
	} else {
		free(*node);
		*node = NULL;
	}
}

static void attrib(destructor) destruct()
{
	free_tree(&root);
	pthread_mutex_destroy(&mtx);
}

static void add_handler(struct list_node **head, on_sig sig_handler,
                on_sigact sig_acthandler)
{
	struct list_node **cur = head;
	while (*cur)
		cur = &((*cur)->next);

	*cur = malloc(sizeof(struct list_node));

	if (sig_handler)
		(*cur)->sig_handler = sig_handler;
	if (sig_acthandler)
		(*cur)->sigact_handler = sig_acthandler;

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

static int nonnull(1) add_node(struct tree_node **n, int sig_nr,
                on_sig sig_handler, on_sigact sig_acthandler, int blck_mask,
                int flags)
{
	struct tree_node **node_ptr = n;

	while (*node_ptr) {
		if (sig_nr > (*node_ptr)->sig_nr)
			node_ptr = &(*node_ptr)->right;
		else
			node_ptr = &(*node_ptr)->left;
	}

	*node_ptr = malloc(sizeof(struct tree_node));
	(*node_ptr)->sig_nr = sig_nr;
	(*node_ptr)->left = NULL;
	(*node_ptr)->right = NULL;

	sigaddset(&action.sa_mask, blck_mask);
	action.sa_flags |= flags;


	if (sig_acthandler)
		action.sa_sigaction = os_sig_acthandler;
	if (sig_handler)
		action.sa_handler = os_sig_handler;

	if (sigaction(sig_nr, &action, (*node_ptr)->old_act))
		return errno;

	add_handler(&(*node_ptr)->handlers, sig_handler, sig_acthandler);

	return 1;
}



static void remove_list_node(struct list_node **head, on_sig sahadler,
                on_sigact sig_acthandler)
{
	struct list_node **cur = head;
	while (*cur) {
		if ((*cur)->sig_handler && (*cur)->sig_handler == sahadler)
			(*cur)->sig_handler = NULL;

		if ((*cur)->sigact_handler
		                && (*cur)->sigact_handler == sig_acthandler)
			(*cur)->sigact_handler = NULL;

		if (!(*cur)->sig_handler && !(*cur)->sigact_handler) {
			struct list_node *n = (*cur)->next;
			free((*cur));
			*cur = n;
			break;
		}
	}
}

/*public function*/
int _reg_sig(int sig_nr, on_sig sig_handler, on_sigact sigact_handler,
                int blck_mask, int flags)
{
	if (!sig_nr || sig_nr == SIGKILL || sig_nr == SIGSTOP || (!sig_handler && !sigact_handler))
		return 0;

	pthread_mutex_lock(&mtx);
	struct tree_node **sig_node = find_sig_node(&root, sig_nr);
	int reged = 0;
	if (sig_node != NULL) {
		if (!find_handler((*sig_node)->handlers, sigact_handler,
		                sig_handler))
			add_handler(&(*sig_node)->handlers, sig_handler,
			                sigact_handler);
		reged = 1;
	} else {
		reged = add_node(&root, sig_nr, sig_handler, sigact_handler,
		                blck_mask, flags);
	}
	pthread_mutex_unlock(&mtx);

	return reged;
}

void _unreg_sig(int sig_num, on_sig sig_handler,
                on_sigact sig_acthandler)
{
	pthread_mutex_lock(&mtx);
	struct tree_node **n = find_sig_node(&root, sig_num);
	if (n) {
		remove_list_node(&(*n)->handlers, sig_handler, sig_acthandler);
		if (!(*n)->handlers)
			del_node(n, sig_num);
	}
	pthread_mutex_unlock(&mtx);
}
