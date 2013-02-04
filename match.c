#include "match.h"
#include <fnmatch.h>
#include <string.h>
#include "config.h"

static void
readline(char *buf, char *list, int *pos, int size) {
  /* a list may be like this:
   *
   * facebook.com\r\nhttp://media-cache-*.pinterest.com/\r\n*
   *
   */

  int i = 0; 

  while (*pos < size 
	 && list[*pos] != '\n'
	 && list[*pos] != '\r') {
    buf[i] = list[*pos];
    i++; (*pos)++;
  }
  while (list[*pos] == '\n' || list[*pos] == '\r')
    (*pos)++;

  buf[i] = 0;
}

struct proxy_t *
match_list(char *url) {
  // 2KB line should be enough
  char buf[2048];
  int pos = 0, size;

  struct acllist *al = config->acl_h;
  struct acl *node = al->data;

  while (node != NULL) {
    pos = 0;
    while (1) {
      
      size = strlen(node->data);

      readline(buf, node->data, &pos, size);

      if (buf[0] == 0) break;

      if (strcasestr(url, buf) != NULL 
	  || !fnmatch(buf, url, FNM_CASEFOLD)) {

	return node->proxy;
      }
    }
    node = node->next;
  }


#ifdef DEBUG
  printf("Not Matched\n");
#endif
  return config->default_proxy;
}

