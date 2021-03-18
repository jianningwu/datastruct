/*
 * Description: skip list declaration file.
 */

#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

/*The purpose of this macro is to determine the stratification probability*/
#define SKIPLIST_STRATIFICATION_PROBABILITY    0.25
#define SKIPLIST_MAX_LEVEL 32  /* Should be enough for 2^32 elements */

/***** Struct Definitions *****/
typedef struct skiplink {
        struct skiplink *prev;/* Points at the next previous in the list */
        struct skiplink *next;/* Points at the next node in the list */
}tSkipLink;

typedef struct skiplist {
        int level;/* Max level in list */
        int count;/* Number of Nodes in list */
        struct skiplink head[SKIPLIST_MAX_LEVEL];/* Header List Node */
}tSkipList;

typedef struct skipnode {
        int key;
        int value;
        struct skiplink link[0];
}tSkipNode;

/***** Struct Definitions End *****/



/***  MACRO Definition  ***/

#define SKIP_LIST_NODE_GET(ptr, type, member) \
        ((type *)((char *)(ptr) - (size_t)(&((type *)0)->member)))

#define SKIP_LIST_DYN_SCAN(pos, n, end) \
        for (n = pos->next; pos != end; pos = n, n = pos->next)

#define SKIP_LIST_LINK_INIT(link) \
    {\
        (link)->prev = (link); \
        (link)->next = (link); \
    }

#define SKIP_LIST_LINK_DELETE(link) \
    {\
        (link)->prev->next = (link)->next; \
        (link)->next->prev = (link)->prev; \
        (link)->prev = (link); \
        (link)->next = (link); \
    }

#define SKIP_LIST_NODE_ADD(link, pos, end) \
    {\
        (link)->next = (end); \
        (link)->prev = (pos); \
        (end)->prev = (link); \
        (pos)->next = (link); \
    }

#define SKIP_LIST_IS_EMPTY(link)  (link)->next == (link)

/***  MACRO Definition End ***/



/*** Application Declaration ***/

tSkipList *skipListCreate(void);
int skipListRandomLevel(void);
tSkipNode *skipListNodeCreate(int level, int key, int value);
tSkipNode *skipListInsert(tSkipList *list, int key, int value);
void skipListDump(tSkipList *list);
tSkipNode *skipListSearch(tSkipList *list, int key);
void skipListNodeRemove(tSkipList *list, tSkipNode *node, int level);
void skipListNodeDelete(tSkipList *list, int key);
void skipListDelete(tSkipList *list);


/*** Application Declaration End ***/

#endif /**** _SKIPLIST_H_ ****/
