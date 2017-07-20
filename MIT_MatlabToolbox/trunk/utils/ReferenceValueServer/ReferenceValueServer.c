//File: ReferenceValueServer.c
/*
* AUTHOR Fabian Riether
* CREATE DATE 2015/09/04
* PURPOSE This program provides a server functionality to enable flying the drone with a keyboard. It reads pressed keyboard buttons and sends them to the drone via a socket.
* SPECIAL NOTES
* ===============================
* Change History
* 2015/09/04 created
* ==================================
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>

#include <unistd.h>
#include <termios.h>

static double roll_ref = 0;
static double pitch_ref = 0;
static double alt_ref = -1.0;
static double yaw_ref = 0;





char getch() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}


int main(int argc, char *argv[])
{

int listenfd = 0, connfd = 0;
struct sockaddr_in serv_addr;

char sendBuff[1025];
char recvBuff[100];
char keybch;
static int 	  yes = 1;

static double SATU_angle   = 0.5;
static double SATU_alt_min = 0.0;
static double SATU_alt_max = -1.1;
int runcmd = 1;

double roll_ref_RS, pitch_ref_RS;
int count = 0;
int mission_sw = 0;

/*
 * PROGRAM
 */

listenfd = socket(AF_INET, SOCK_STREAM, 0);
setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

memset(&serv_addr, '0', sizeof(serv_addr));
memset(sendBuff, '0', sizeof(sendBuff));

serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
serv_addr.sin_port = htons(12345);

if  (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr))<0)
	{
 	printf("Socket error (bind), closing all open ReferenceValueServers!\n");
	system("pkill -f DroneKeyboardPilot*");
	system("pkill -f ReferenceValueServer*");	
	exit(1);
	}

printf("Waiting for connection to drone...\n");
if  (listen(listenfd, 10)<0) 	
	{
	printf("Socket error (listen), closing all open ReferenceValueServers!\n");
	system("pkill -f DroneKeyboardPilot*");
	system("pkill -f ReferenceValueServer*");	
	exit(1);
	}

connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
setsockopt(connfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
system("clear");
printf("Fly drone with\n w-s-a-d :: i-k-j-l ::\n r(eset angles) :: 1(Mission 2 Start) :: e(xit)! \n");


while(runcmd==1)
	{
  //memset(sendBuff, '\0', sizeof(sendBuff));
  //recv(connfd, recvBuff, sizeof(recvBuff),O_NONBLOCK);
  //send(sockfd, sendBuff, strlen(sendBuff), 0);
  //scanf(" %c", keybch,1); //whitespace before c!!!

	if(mission_sw==1)
	{
		//system("clear");
		//alt_ref=-1.0;
		//pitch_ref = -0.1;
		//if(alt_ref <= -1.0)
		//alt_ref = -1.0;
		count++;
		//if(count == 5)
		//	pitch_ref = -0.3;
		//printf("count : %d\n",count);
		usleep(200);
		if(count>50000){
			mission_sw++;
			count++;
		}
		continue;
	}
	else if(mission_sw ==2){
		//pitch_ref -= 0.01;
		//roll_ref -= 0.01;
		alt_ref += 0.15;
		
		//count++;
		usleep(500000);
		if(alt_ref >= -0.1){
			mission_sw = 0;
			alt_ref += 0.05;
			count = 0;
			runcmd = 0;
		}
	}
	printf("alt_ref : %g\n",alt_ref);
  if(mission_sw == 0)
		keybch = getch();
  switch (keybch)
  {
  	  case 'd': roll_ref += 0.02; break;
  	  case 'a': roll_ref -= 0.02; break;
  	  case 'w': pitch_ref -= 0.02; break;
  	  case 's': pitch_ref += 0.02; break;
  	  case 'i': alt_ref -= 0.3; break;
  	  case 'y': alt_ref -= 0.6; break;
  	  case 'k': alt_ref += 0.3; break;
  	  case 'h': alt_ref += 0.6; break;
  	  case 'j': yaw_ref -= 0.2; break;
  	  case 'l': yaw_ref += 0.2; break;
	  case 'r': pitch_ref = 0.0; roll_ref = 0.0; alt_ref = -1.0; break;
  	  case 'e': runcmd = 0; break;
			case '1': mission_sw = 1;	break;
			case '2':
								printf("Drone is Wating Mode\n");
								alt_ref = 0.0;	roll_ref = 0.0;	pitch_ref = 0.0; yaw_ref = 0.0;
								break;	
			default : break;
  }
	
	/*switch(mission_sw){
		case 1: 
			alt_ref = -1.0;
			count++;
			printf("count : %d\n",count);
			if(count > 50)
				mission_sw++;
			break;
		case 2:
			alt_ref += 0.2;
			break;
	}*/
  	  //Saturation
  	  roll_ref_RS = roll_ref;
  	  pitch_ref_RS = pitch_ref;

	  if (roll_ref_RS > SATU_angle) roll_ref_RS = SATU_angle;
	  if (roll_ref_RS < -SATU_angle) roll_ref_RS = -SATU_angle;
	  if (pitch_ref_RS > SATU_angle) pitch_ref_RS = SATU_angle;
	  if (pitch_ref_RS < -SATU_angle) pitch_ref_RS = -SATU_angle;

	  if (alt_ref > SATU_alt_min) alt_ref = SATU_alt_min;
	  if (alt_ref < SATU_alt_max) alt_ref = SATU_alt_max;

		printf("send Command\n");
		//printf("pitch_ref_RS : %g\n",pitch_ref_RS);
		//printf("roll_ref_RS  : %g\n",roll_ref_RS);
		//printf("yaw_ref      : %g\n",yaw_ref);
		//printf("alt_reg      : %g\n",alt_ref);
	  sprintf(sendBuff,"%i %i %i %i %i",runcmd,
				(int)(pitch_ref_RS*1000 + 10000),
				(int)(roll_ref_RS*1000 + 10000), 
				(int)(yaw_ref*1000 + 10000), 
				(int)(alt_ref*100.0));
	  send(connfd, sendBuff, strlen(sendBuff),0);
		
		pitch_ref = 0;
		roll_ref = 0;
		yaw_ref = 0;
	  keybch = '\0';
	  usleep(1000);
}




if (shutdown(listenfd,2) || shutdown(connfd,2) || close(listenfd) || close(connfd)  )
		printf("ERROR: Shutdown Socket\n");
else
	printf("Shutdown Socket successful \n");

return 0;

}