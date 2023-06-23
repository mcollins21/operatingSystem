#ifndef SLEEP_H
#define SLEEP_H


//manually defined sleep function because I don't care enough to try
//to find out what POSIX version this is

void sleep(unsigned int seconds) {

	unsigned int count = 0;

	while (count < seconds) {

		unsigned int i = 0;

		while (i < 1000000)
			i++;
		
		count++;
	}
}

#endif
