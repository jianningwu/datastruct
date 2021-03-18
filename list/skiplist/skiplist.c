/*
* Description: skip list implemention code.
*/

#include "skiplist.h"

/**********************************************************************
 * Create a skiplist, the function is to record the head node of each
 * layer of the entire skiplist.
 **********************************************************************/
tSkipList *skipListCreate(void)
{
    int level = 0;;
    tSkipList *list = NULL;

    list = (tSkipList *)malloc(sizeof(*list));
    if (list != NULL)
    {
        list->level = 1;
        list->count = 0;

        for (level = 0; level < (sizeof(list->head)/sizeof(list->head[0])); level++)
        {
            SKIP_LIST_LINK_INIT(&list->head[level]);
        }
    }

    return list;
}

/**********************************************************************
 * According to the defined stratification probability, the number of
 * layers for each node is generated.
 * Algorithm idea:
 * By default, they belong to the first layer, the probability of belonging
 * to the second layer is p, the probability of belonging to the third layer
 * is p*p, and the probability of belonging to the fourth layer is p*p*p ...
 **********************************************************************/
int skipListRandomLevel(void)
{
    int level = 1;
    double p = SKIPLIST_STRATIFICATION_PROBABILITY;

    while ((random() & 0xffff) < 0xffff * p)
    {
        level++;
    }

    return level > SKIPLIST_MAX_LEVEL ? SKIPLIST_MAX_LEVEL : level;
}

/**********************************************************************
 * Create a node whose value is value and index is key.
 **********************************************************************/
tSkipNode *skipListNodeCreate(int level, int key, int value)
{
    tSkipNode *node = NULL;

    node = (tSkipNode *)malloc(sizeof(*node) + level * sizeof(tSkipLink));
    if (node != NULL)
    {
        node->key = key;
        node->value = value;
    }
    return node;
}

/**********************************************************************
 * Insert a node into the skip list.
 **********************************************************************/
tSkipNode *skipListInsert(tSkipList *list, int key, int value)
{
    /*generate a number of layers for the nodes that need to be inserted*/
    int insertNodeLevel = skipListRandomLevel();
    int level = 0;

    /*if the number of layers generated is larger than the highest layer 
     *of the current skiplist, update the number of layers of the skiplist.*/
    if (insertNodeLevel > list->level)
    {
        list->level = insertNodeLevel;
    }

    tSkipNode *node = skipListNodeCreate(insertNodeLevel, key, value);
    if (node != NULL)
    {
        level = list->level - 1;
        /*get the top-level head node.*/
        tSkipLink *pos = &(list->head[level]);
        tSkipLink *end = &(list->head[level]);

        for (; level >= 0; level--)
        {
            pos = pos->next;
            while (pos != end)
            {
                /*Get the current node based on the offset*/
                tSkipNode *nd = SKIP_LIST_NODE_GET(pos, tSkipNode, link[level]);
                if (nd->key >= key)
                {
                    end = &(nd->link[level]);
                    break;
                }

                pos = pos->next;
            }
            pos = end->prev;
            if (level < insertNodeLevel)
            {
                SKIP_LIST_NODE_ADD(&node->link[level], pos, end);
            }
            pos--;
            end--;
        }

        list->count++;
    }

    return node;
}

/**********************************************************************
 * Traverse and display the contents of the skip list.
 **********************************************************************/
void skipListDump(tSkipList *list)
{
    int level = list->level - 1;
    tSkipNode *node = NULL;
    tSkipLink *pos = &(list->head[level]);
    tSkipLink *end = &(list->head[level]);

    printf("\nTotal %d nodes: \n", list->count);
    for (; level >= 0; level--)
    {
        pos = pos->next;
        printf("level %d:\n", level + 1);

        while(pos != end)
        {
            /*Get the current node based on the offset*/
            node = SKIP_LIST_NODE_GET(pos, tSkipNode, link[level]);
            printf("key:0x%08x value:0x%08x\n",node->key, node->value);
            pos = pos->next;
        }
        pos = &list->head[level];
        pos--;
        end--;
    }
}

/**********************************************************************
 * Search the corresponding node in skiplist.
 **********************************************************************/
tSkipNode *skipListSearch(tSkipList *list, int key)
{
    int level = list->level - 1;
    tSkipNode *node = NULL;
    tSkipLink *pos = &(list->head[level]);
    tSkipLink *end = &(list->head[level]);

    for (; level >= 0; level--)
    {
        pos = pos->next;
        while (pos != end)
        {
            /*Get the current node based on the offset*/
            node = SKIP_LIST_NODE_GET(pos, tSkipNode, link[level]);
            if (node->key >= key)
            {
                end = &node->link[level];
                break;
            }

            pos = pos->next;
        }

        if (node->key == key)
        {
            return node;
        }

        pos = end->prev;
        pos--;
        end--;
    }

    return NULL;
}

/**********************************************************************
 * Delete the corresponding of node from the skiplist.
 **********************************************************************/
void skipListNodeRemove(tSkipList *list, tSkipNode *node, int level)
{
    int currentLevel = 0;

    for (currentLevel = 0; currentLevel < level; currentLevel++)
    {
        SKIP_LIST_LINK_DELETE(&(node->link[currentLevel]));
        if (SKIP_LIST_IS_EMPTY(&(list->head[currentLevel])))
        {
            list->level--;
        }
    }
    free(node);
    list->count--;
}

/**********************************************************************
 * Search and delete the corresponding of node from the skiplist.
 **********************************************************************/
void skipListNodeDelete(tSkipList *list, int key)
{
    int level = list->level - 1;
    tSkipNode *node = NULL;
    tSkipLink *n = NULL;
    tSkipLink *pos = &(list->head[level]);
    tSkipLink *end = &(list->head[level]);

    for (; level >= 0; level--)
    {
        pos = pos->next;
        SKIP_LIST_DYN_SCAN(pos, n, end)
        {
            /*Get the current node based on the offset*/
            node = SKIP_LIST_NODE_GET(pos, tSkipNode, link[level]);
            if (node->key > key)
            {
                end = &(node->link[level]);
                break;
            }
            else if (node->key == key)
            {
                skipListNodeRemove(list, node, level + 1);
            }
        }

        pos = end->prev;
        pos--;
        end--;
    }
}

/**********************************************************************
 * Delete the entire skiplist.
 **********************************************************************/
void skipListDelete(tSkipList *list)
{
    tSkipLink *n = NULL;
    tSkipLink *pos = list->head[0].next;

    SKIP_LIST_DYN_SCAN(pos, n, &(list->head[0]))
    {
        /*Get the current node based on the offset*/
        tSkipNode *node = SKIP_LIST_NODE_GET(pos, tSkipNode, link[0]);
        free(node);
    }

    free(list);
}

