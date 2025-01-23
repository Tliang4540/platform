#ifndef BOJECT_H__
#define BOJECT_H__

struct list_head
{
    struct list_head *next, *prev;
};

static inline void __list_add(struct list_head *new_node,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

static inline void list_add(struct list_head *new_node, struct list_head *head)
{
    __list_add(new_node, head, head->next);
}

static inline void list_add_tail(struct list_head *new_node, struct list_head *head)
{
    __list_add(new_node, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
}

static inline void list_replace(struct list_head *old,
				struct list_head *new_node)
{
	new_node->next = old->next;
	new_node->next->prev = new_node;
	new_node->prev = old->prev;
	new_node->prev->next = new_node;
}

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

struct object
{
    struct list_head entry;
    const char *name;
};

static inline void object_init(struct object *obj, const char *name)
{
    obj->name = name;
}

#endif
