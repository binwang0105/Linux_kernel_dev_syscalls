#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "prinfo.h"

void print_prinfo(struct prinfo* p){
	if(p == NULL){
		return;
	}
	printf("%s,%d,%ld,%d,%d,%d,%ld\n", p->comm, p->pid, p->state,
	p->parent_pid, p->first_child_pid, p->next_sibling_pid, p->uid);
}


int main()
{

	int i;
	int j;
	int actual_num;
	int copied_num;
	int current_tabs;

	struct prinfo* buf = NULL;
	
	int buf_size = 64;
	
	//open a buff
	buf = malloc(buf_size*sizeof(struct prinfo));

	//if malloc fail
	if(buf == NULL){
		printf("error: %s\n", strerror(errno));
		return -1;
	}

	copied_num = buf_size;
	actual_num = syscall(223,buf,&copied_num);

	//if get wrong actual_num of the process tree
	if(actual_num < 0){
		free(buf);
		return -1;
	}

	while(actual_num != copied_num){
		
		//need more space, realloc
		buf_size = buf_size * 2;
		copied_num = buf_size;
		buf = realloc(buf, copied_num);
		
		if(buf == NULL){
			printf("error: %s\n", strerror(errno));
			return -1;
		}

		actual_num = syscall(223,buf,&copied_num);
		if(actual_num < 0){
			free(buf);
			return -1;
		}
	}
	
	int pid_array[actual_num];
	pid_array[0] = 0;
	current_tabs = 0;

	for(i=0;i<actual_num;i++){
		while (buf[i].parent_pid != pid_array[current_tabs]){
			current_tabs = current_tabs - 1;
		}
		for(j=0;j<current_tabs;j++){
			printf("\t");
		}
		print_prinfo(&(buf[i]));
		current_tabs = current_tabs + 1;
		pid_array[current_tabs] = buf[i].pid;
	}

	free(buf);
    return 0;
}
