/*
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sig_handler(int signum) {
        printf("Received Signal %d\n", signum);
}       

void vram_print(int c) {
	uint16_t *vram = (uint16_t*)0xb8000;
	vram[0] = 0x0700 | c;
}

int main() {

        signal(SIGSEGV, sig_handler);
        while(1) {
                sleep(10);
        }       
        return 0;
} 
*/

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

struct sigaction act;

void sighandler(int signum, siginfo_t *info, void *ptr)
{
    printf("Received signal %d\n", signum);
    printf("Signal originates from process %lu\n",
        (unsigned long)info->si_pid);
}

int main()
{
    printf("I am %lu\n", (unsigned long)getpid());

    memset(&act, 0, sizeof(act));

    act.sa_sigaction = sighandler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &act, NULL);

    // Waiting for CTRL+C...
    sleep(100);

    return 0;
}
