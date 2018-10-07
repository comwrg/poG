#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <stdlib.h>

struct node {
    int value; // save value
    struct node *next_father; // link list, save father of this node

    int depth; // save depth of node during graph
    struct node *next_longest; // link list, save longest children of this node
};

struct node **nodelist;
struct node **memlist;
int memlist_len = 0;

struct node *getnode() { // return node address
    struct node *nd = malloc(sizeof(struct node));
    memset(nd, 0, sizeof(struct node));
    memlist[memlist_len++] = nd; // add address to memlist for free
    return nd;
}


int isin(int src, int dst) { // check src is in dst
    int ary[10]; // save per digit of dst
    memset(ary, 0, sizeof(ary));
    for (; dst; dst /= 10) { // separate number
        ary[dst % 10] = 1;
    }

    for (; src; src /= 10) {
        if (!ary[src % 10]) { // calc per digit， if not in ary, return 0;
            return 0;
        }
    }

    return 1;
}

void pointto(int src, int dst) { // point src to dst
    struct node *p = nodelist[src];
    for (; p->next_father; p = p->next_father) { // insert sort, O(n)
        if (dst == p->next_father->value) // if dst exists, return
            return;
        if (dst < p->next_father->value) // if dst < value, break loop
            break;
    }

    // insert node
    struct node *new = getnode();
    new->value = dst;

    new->next_father = p->next_father;
    p->next_father = new;

}

void build(int n) { // build graph
    for (int i = 1; i <= sqrt(n); ++i) {
        if (n % i == 0) { // can be divided
            int f = n / i;
            if (nodelist[i] == NULL) { // malloc memory
                nodelist[i] = getnode();
                nodelist[i]->value = i;
            }
            if (nodelist[f] == NULL) { // malloc memory
                nodelist[f] = getnode();
                nodelist[f]->value = f;
            }

            // factor are i and n/i
            if (i < n) {
                if (isin(i, n)) {
                    // set i point to n
                    pointto(i, n);
                }
                build(i);
            }

            if (i != f && // if i == f, only build one time enough, e.g. n = 121, i = 11, f = 11
                f < n) {
                if (isin(f, n)) {
                    // set f point to n
                    pointto(f, n);
                }
                build(f);
            }

        }
    }
}

int calc_longest(int i) { // calculate longest
    int longest = 0;

    for (struct node *nd = nodelist[i]->next_father; nd; nd = nd->next_father) {
        if (nodelist[i]->depth + 1 < nodelist[nd->value]->depth) // check depth
            continue;
        if (nodelist[i]->depth + 1 > nodelist[nd->value]->depth) // reset next_longest
            nodelist[nd->value]->next_longest = NULL;
        nodelist[nd->value]->depth = nodelist[i]->depth + 1;
        struct node *p = nodelist[nd->value];
        for (; p->next_longest; p = p->next_longest) { // insert sort, O(n)
            if (i == p->next_longest->value) // if i exists, continue;
                goto ct;
            if (i < p->next_longest->value) // if i < value, insert
                break;
        }

        // insert node
        struct node *new = getnode();
        new->value = i;

        new->next_longest = p->next_longest;
        p->next_longest = new;


        // save longest
        if (nodelist[nd->value]->depth > longest)
            longest = nodelist[nd->value]->depth;

        ct:
        continue;
    }

    return longest;
}

void printlongest(int n) {
    static int *stk = NULL;
    static int len;
    if (!stk) {
        stk = malloc(1024 * sizeof(int)); // stack
        len = 0; // stack length
        stk[len++] = n;
    }
    struct node *p = nodelist[n]->next_longest;
    if (!p) { // p == NULL, print stack
        for (; len;) {
            printf("%d", stk[--len]);
            if (len > 0)
                printf(" < ");
        }
        printf("\n");
        len = 1;
        return;
    }
    for (; p; p = p->next_longest) {
        stk[len++] = p->value; // push p->value to stack
        printlongest(p->value); // recursive
    }
    if (stk[0] == n) {
        free(stk), stk = NULL;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("no input.");
        return 0;
    }
    int n = 0;
    char *p = argv[1];
    for (; *p; ++p) { // calculate number p from string
        n = 10*n + *p-'0';
    }
    // malloc memory
    size_t nodelistsize = ((n+1) * sizeof(struct node *));
    nodelist = malloc(nodelistsize);
    memset(nodelist, 0, nodelistsize);
    memlist = malloc((n < 128 ? 128 : n) * sizeof(struct node *));
    int longest = 0;

    build(n);

    printf("Partial order:\n");
    // O(n)
    for (int i = 1; i <= n; ++i) { // print partial order
        if (nodelist[i] == NULL)
            continue;
        printf("%d:", i);
        struct node *p = nodelist[i]->next_father;
        for (; p; p = p->next_father) {
            printf(" %d", p->value);
        }
        printf("\n");


        int l = calc_longest(i); // calculate longest path
        if (l > longest)
            longest = l;
    }

    printf("\nLongest monotonically increasing sequences:\n");

    for (int i = 1; i <= n; ++i) {
        if (!nodelist[i])
            continue;
        if (nodelist[i]->depth < longest)
            continue;
        printlongest(i); // print longest path
    }


    // free memory
    for (; memlist_len;) {
        free(memlist[--memlist_len]);
    }
    free(memlist);
    free(nodelist);

    return 0;
}