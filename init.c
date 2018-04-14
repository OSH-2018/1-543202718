#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    /* 输入的命令行 */
    char cmd[256];
    /* 命令行拆解成的各部分，以空指针结尾 */
    char *args[128];//指针数组
	int pid=getpid();
	int pipe_fd[2];
	int n;
    pipe(pipe_fd);//管道创建
    while (1) {
        /* 提示符 */
        printf("# ");
        fflush(stdin);
        fgets(cmd, 256, stdin);
        /* 清理结尾的换行符 */
        int i,j;
        for (i = 0; cmd[i] != '\n'; i++)
            ;
        cmd[i] = '\0';
        /* 拆解命令行 */
        args[0] = cmd;
        for (i = 0; *args[i]; i++)
            for (args[i+1] = args[i] + 1; *args[i+1]; args[i+1]++)
                if (*args[i+1] == ' ') {
                    *args[i+1] = '\0';
                    while(*args[i+1]==' ') args[i+1]++;//找到第一个不是空格的字符
                    break;
                }
        args[i] = NULL;
		n=i;
		/* 管道特性 */
		i=0;
		while (args[i] != NULL) {
			if (strcmp(args[i],"|"){
				pid=fork();//分叉
				if (pid==0){
					close ( pipe_fd[0] );//关闭读端口
					close ( fileno(stdout));
					dup2 ( pipe_fd[1] , fileno(stdout));//输出重定向
					break;
				}
				else {
					wait(NULL);//等待子进程结束
					for(j=i+1;j<=n;j++)	args[j-i-1]=args[j];
					i=-1;//移动数组中的元素
					close ( pipe_fd[1] );//关闭写端口
					close ( fileno(stdin));
					dup2 ( pipe_fd[0] , fileno(stdin));//输入重定向
				}
			}
			i++;
		} 	
        /* 没有输入命令 */
        if (!args[0]){
            if (pid==0) return 0;
			else continue;
		}
        /* 内建命令 */
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] && chdir(args[1])<0)
				puts("File not found");
			if (pid==0) return 0;
			else continue;
        }
        if (strcmp(args[0], "pwd") == 0) {
            char wd[4096];
            puts(getcwd(wd, 4096));
            if (pid==0) return 0;
			else continue;
        }
        if (strcmp(args[0], "exit") == 0)
            return 0;

        /* 外部命令 */
        pid_t pid = fork();
		if (pid < 0){
			puts("Fork failed.")
			continue;
		}
        if (pid == 0) {
            /* 子进程 */
            execvp(args[0], args);
            /* execvp失败 */
			puts("Execvp failed.")
            return 255;
        }
        /* 父进程 */
        wait(NULL);
		if (pid==0) return 0;
		else continue;
		
    }
}
