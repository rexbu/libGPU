/**
 * file :	object_test.cpp
 * author :	bushaofeng
 * create :	2016-08-25 13:36
 * func : 
 * history:
 */

#include <stdio.h>
#include "bs_object.h"
#include "bs_string.h"

typedef struct s{
	object_t object;
	int* a;
}s;

void* s_init(void* ss){
	s* so = (s*)ss;
	so->a = (int*)malloc(sizeof(int));
	*so->a = 10;
	return so;
}
void s_destroy(void* ss){
	s* so = (s*)ss;
	free(so->a);
	free(so);
}

int main(){
	string_t* str = new(string);
	string_append(str, "aaaaa");
	string_append(str, "bbbbb");
	string_append(str, "cccccc");
	printf("%d/%d: %s\n", str->len, str->size, string_char(str));
	delete(str);
	return 0;
}