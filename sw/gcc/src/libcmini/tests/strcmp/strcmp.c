#include <stdio.h>
#include <string.h>
#include <assert.h>

int testcmp(const char* s1, const char* s2)
{
    int cmp = strcmp(s1, s2);
    if (cmp>0) {
        printf("'%s' > '%s'\n", s1, s2);
    } else if (cmp<0) {
        printf("'%s' < '%s'\n", s1, s2);
    } else {
        printf("'%s' = '%s'\n", s1, s2);
    }
    return cmp;
}

void testmemchr(void)
{
    char tb[] = "abcdef\0ghijkl";

    assert(memchr(tb, 'a', 10) == tb);
    assert(memchr(tb, 'f', 10) != tb);
}

int main(void)
{
	int cmp;

    cmp = testcmp("abc", "def");
    assert(cmp < 0);
    cmp = testcmp("def", "def");
    assert(cmp == 0);
    cmp = testcmp("def", "abc");
    assert(cmp > 0);
    
    cmp = testcmp("abc", "");
    assert(cmp > 0);
    cmp = testcmp("abc", "ab");
    assert(cmp > 0);
    cmp = testcmp("", "abc");
    assert(cmp < 0);
    cmp = testcmp("ab", "abc");
    assert(cmp < 0);
    cmp = testcmp("ab\204d", "abcd");
    assert(cmp > 0);
    cmp = testcmp("ab\204", "ab");
    assert(cmp > 0);
    cmp = testcmp("ab", "ab\204");
    assert(cmp < 0);
    cmp = testcmp("abcd", "ab\204d");
    assert(cmp < 0);
    cmp = testcmp("ab\204d", "ab\204d");
    assert(cmp == 0);

    testmemchr();

    return 0;
}
