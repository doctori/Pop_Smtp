#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

void  str_echo(int sockfd)
{
	ssize_t n;
	char buf[100],final[150];
	
do
	while((n=read(sockfd,buf,100))>0)
	{
		buf[n]='\0';
		printf("envoi dans socket de %s\n",buf);
		write(sockfd,buf,strlen(buf)+1);
	}

while(n<0 && errno==EINTR);
}

main(int argc,char **argv)
{
	int lfd,cfd,port;
	pid_t childpid;
	socklen_t	clilen;
	struct sockaddr_in cliaddr,servaddr;
	void sig_chld(int);
port=25;
lfd=socket(AF_INET,SOCK_STREAM,0);
bzero(&servaddr,sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
servaddr.sin_port=htons(port);

bind(lfd,(struct sockaddr *) &servaddr, sizeof(servaddr));	
listen(lfd,10);
signal(SIGCHLD,sig_chld);   

for(;;)
{
	clilen=sizeof(cliaddr);
	if((cfd=accept(lfd,(struct sockaddr *) &cliaddr,&clilen))<0)
	{
		if(errno==EINTR)
		{
			continue;
		}
		else
		{
			exit(-1);
		}
	}
	if((childpid=fork())==0)
	{	
		close(lfd);
		str_echo(cfd);
		exit(0);
	}
	close(cfd);
}

}

void sig_chld(int s)
{
	pid_t pid;
	
	while((pid=waitpid(-1,NULL,WNOHANG))>0)
	{return;}
}
