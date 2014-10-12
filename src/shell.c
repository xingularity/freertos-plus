#include "shell.h"
#include <stddef.h>
#include <stdlib.h>
#include "clib.h"
#include <string.h>
#include "fio.h"
#include "filesystem.h"

#include "FreeRTOS.h"
#include "task.h"
#include "host.h"

typedef struct {
	const char *name;
	cmdfunc *fptr;
	const char *desc;
} cmdlist;


int myatoi(char* str);
void ls_command(int, char **);
void man_command(int, char **);
void cat_command(int, char **);
void ps_command(int, char **);
void host_command(int, char **);
void help_command(int, char **);
void host_command(int, char **);
void mmtest_command(int, char **);
void test_command(int, char **);
void helloworld_command(int, char **);
int filedump(char* filename);
void filedump_command(int, char **);
void fibonacci_command(int, char**);

#define MKCL(n, d) {.name=#n, .fptr=n ## _command, .desc=d}

cmdlist cl[]={
	MKCL(ls, "List directory"),
	MKCL(man, "Show the manual of the command"),
	MKCL(cat, "Concatenate files and print on the stdout"),
	MKCL(ps, "Report a snapshot of the current processes"),
	MKCL(host, "Run command on host"),
	MKCL(mmtest, "heap memory allocation test"),
	MKCL(help, "help"),
	MKCL(test, "test new function"),
              MKCL(helloworld, "print hello world"),
              MKCL(fibonacci, "fibonacci"),
              MKCL(filedump, "filedump")
};

int myatoi(char* str){
    int num = 0;
    while(*str != '\0'){
        num = num*10 + *str - '0';
        ++str;
    }
    return num;
}

int parse_command(char *str, char *argv[]){
	int b_quote=0, b_dbquote=0;
	int i;
	int count=0, p=0;
	for(i=0; str[i]; ++i){
		if(str[i]=='\'')
			++b_quote;
		if(str[i]=='"')
			++b_dbquote;
		if(str[i]==' '&&b_quote%2==0&&b_dbquote%2==0){
			str[i]='\0';
			argv[count++]=&str[p];
			p=i+1;
		}
	}
	/* last one */
	argv[count++]=&str[p];

	return count;
}

void ls_command(int n, char *argv[]){
    fio_printf(1, "\r\n ls command\r \n"); 
}

int filedump(char* filename){
    char buf[128];

    int fd=fs_open(filename, 0, O_RDONLY);

    if(fd==OPENFAIL)
        return 0;

    fio_printf(1, "\r\n");

    int count;
    while((count=fio_read(fd, buf, sizeof(buf)))>0){
        fio_write(1, buf, count);
    }

    fio_close(fd);
    return 1;
}

void filedump_command(int n, char *argv[]){
	char buf[128];
              if (n == 1)
                return ;

	int fd=fs_open(argv[1], 0, O_RDONLY);

	if(fd==OPENFAIL)
		return ;

	fio_printf(1, "\r\n");

	int count;
	while((count=fio_read(fd, buf, sizeof(buf)))>0){
		fio_write(1, buf, count);
	}

	fio_close(fd);
	return ;
}

void ps_command(int n, char *argv[]){
	signed char buf[1024];
	vTaskList(buf);
        fio_printf(1, "\n\rName          State   Priority  Stack  Num\n\r");
        fio_printf(1, "*******************************************\n\r");
	fio_printf(1, "%s\r\n", buf + 2);	
}

void cat_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: cat <filename>\r\n");
		return;
	}

	if(!filedump(argv[1]))
		fio_printf(2, "\r\n%s no such file or directory.\r\n", argv[1]);
}

void man_command(int n, char *argv[]){
	if(n==1){
		fio_printf(2, "\r\nUsage: man <command>\r\n");
		return;
	}

	char buf[128]="/romfs/manual/";
	strcat(buf, argv[1]);

	if(!filedump(buf))
		fio_printf(2, "\r\nManual not available.\r\n");
}

void host_command(int n, char *argv[]){
    int i, len = 0, rnt;
    char command[128] = {0};

    if(n>1){
        for(i = 1; i < n; i++) {
            memcpy(&command[len], argv[i], strlen(argv[i]));
            len += (strlen(argv[i]) + 1);
            command[len - 1] = ' ';
        }
        command[len - 1] = '\0';
        rnt=host_action(SYS_SYSTEM, command);
        fio_printf(1, "\r\nfinish with exit code %d.\r\n", rnt);
    } 
    else {
        fio_printf(2, "\r\nUsage: host 'command'\r\n");
    }
}

void help_command(int n, char * argv[]){
	int i;
	fio_printf(1, "\r\n");
	for(i=0;i<sizeof(cl)/sizeof(cl[0]); ++i){
		fio_printf(1, "%s - %s\r\n", cl[i].name, cl[i].desc);
	}
}

void helloworld_command(int n , char* argv[]){
    fio_printf(1,"\r\nHello World\r\n");
}

int fib(int x){
    if (x == 0)
        return 1;
    if (x == 1)
        return 1;
    return fib(x-1) + fib(x-2);
}

void fibonacci_command(int n , char* argv[]){
    
    if (n < 2){
        fio_printf(2, "\r\n please give a number \r\n");    
        return;
    }
    int x = myatoi(argv[1]);
    
    fio_printf(1, "\r\n %d \r\n", fib(x));
}

void test_command(int n, char *argv[]) {
    int handle;
    int error;

    fio_printf(1, "\r\n");

    handle = host_action(SYS_OPEN, "output/syslog", 8);
    if(handle == -1) {
        fio_printf(1, "Open file error!\n\r");
        return;
    }
    
    char *buffer = "Test host_write function which can write data to output/syslog\n";
    error = host_action(SYS_WRITE, handle, (void *)buffer, strlen(buffer));
    if(error != 0) {
        fio_printf(1, "Write file error! Remain %d bytes didn't write in the file.\n\r", error);
        host_action(SYS_CLOSE, handle);
        return;
    }
    
    if ((n > 1) && (n <3)){
        char ansstr[30];
        sprintf(ansstr, "The answer of fibonacci is %d.\r\n", fib(myatoi(argv[1])));
        error = host_action(SYS_WRITE, handle, (void *)ansstr, strlen(ansstr));
    }
    host_action(SYS_CLOSE, handle);
}

cmdfunc *do_command(const char *cmd){

	int i;

	for(i=0; i<sizeof(cl)/sizeof(cl[0]); ++i){
		if(strcmp(cl[i].name, cmd)==0)
			return cl[i].fptr;
	}
	return NULL;	
}
