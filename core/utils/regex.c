#include "utils.h"

/**
  regexAll : get strings by regex
  @regex : regular expression
  @str : input string
  @res : array of string what we get
  @num : size of @res
  @flag : 

  return the number of string what we get
**/
int regexAll(const char *regex, char *str, char **res, int num, int flag) {

  pcre *re;
  /*error msg*/
  const char *errptr;
  /*error offset*/
  int erroffset;
  int ovector[30];
  /**/
  int rc;
  
  int index;
  
  int exec_offset = 0;

  re = pcre_compile(regex, 0, &errptr, &erroffset, NULL);

  if (re == NULL) {
    printf("compilation at %d: %s\n", erroffset, errptr);
    return -1;
  }

  index = 0;
  do {
    rc = pcre_exec(re, NULL, str, strlen(str), exec_offset, 0, ovector, 30);

    if (rc > 0) {
      pcre_get_substring(str, ovector, rc, flag, &res[index++]);
      /*set offset*/
      exec_offset = ovector[1];
    }
  } while( rc > 0 && index < num);

  pcre_free(re);

  return index;
}

/**
  match : is it match?
  @regex : regular expression
  @str : input string

  return 1 for yes, 0 for no
**/
int match(char *regex, char *str) {
  pcre *re;
  /*error msg*/
  const char *errptr;
  /*error offset*/
  int erroffset;
  int ovector[30];
  
  int rc;
  

  re = pcre_compile(regex, 0, &errptr, &erroffset, NULL);

  if (re == NULL) {
    printf("compilation at %d: %s\n", erroffset, errptr);
    return -1;
  }

  
    rc = pcre_exec(re, NULL, str, strlen(str), 0, 0, ovector, 30);

    if (rc > 0) {
      /*sucess*/
      return 1;
    } else {
      return 0;
    }
}
