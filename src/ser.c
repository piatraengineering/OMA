    #include <stdio.h>   /* Standard input/output definitions */
    #include <string.h>  /* String function definitions */
    #include <unistd.h>  /* UNIX standard function definitions */
    #include <fcntl.h>   /* File control definitions */
    #include <errno.h>   /* Error number definitions */
    #include <termios.h> /* POSIX terminal control definitions */

    /*
     * 'open_port()' - Open serial port 1.
     *
     * Returns the file descriptor on success or -1 on error.
     */
int main()
{
      int fd; /* File descriptor for the port */
	  int n=2;
	  char ch_array[256],c;
	  struct termios options;


      //fd = open("/dev/cu.USA28X1b1P1.1", O_RDWR | O_NOCTTY | O_NDELAY);
	  fd = open("/dev/tty.USA28X1b1P1.1", O_RDWR | O_NOCTTY | O_NDELAY);
      if (fd == -1)
      {
       /*
        * Could not open the port.
        */

        printf("open_port: Unable to open /dev/tty.USA28X1b1P1.1 - \n");
		return 0;
      }
      else{
        //fcntl(fd, F_SETFL, FNDELAY);
		fcntl(fd, F_SETFL, 0);
	 }

    /*
     * Get the current options for the port...
     */

    tcgetattr(fd, &options);

    /*
     * Set the baud 
     */

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    /*
     * Enable the receiver and set local mode...
     */

    options.c_cflag |= (CLOCAL | CREAD);

    /*
     * Set the new options for the port...
     */
	options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    options.c_cflag |= CS8;    /* Select 8 data bits */
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
    //options.c_cflag &= ~CNEW_RTSCTS;
	
    tcsetattr(fd, TCSANOW, &options);

	while(n >1){
		printf("type string: ");
		c=getc(stdin);
		n = 0;
		while(c != '\n'){
			//printf("%c\n",c);
			ch_array[n] = c;
			c=getc(stdin);
			n++;
		}
		ch_array[n] = 13;
		ch_array[n+1] = 0;
		n = write(fd, ch_array, strlen(ch_array));
		printf("%d bytes written\n%s",n,ch_array);
	}

	 close(fd);
	  
	 return (fd);
}

