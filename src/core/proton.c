#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include "proton.h"

/* Global state */
volatile sig_atomic_t proton_quit = 0;
volatile sig_atomic_t proton_reload = 0;
pid_t proton_pid;

/* Signal handlers */
static void signal_handler(int signo) {
    switch (signo) {
        case SIGINT:
        case SIGTERM:
            proton_quit = 1;
            break;
        case SIGHUP:
            proton_reload = 1;
            break;
        case SIGCHLD:
            /* Child process terminated */
            break;
    }
}

static void setup_signals(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGCHLD, &sa, NULL);
    
    /* Ignore SIGPIPE */
    signal(SIGPIPE, SIG_IGN);
}

static void usage(const char *prog) {
    fprintf(stderr, "Proton Web Server v%s\n", PROTON_VERSION);
    fprintf(stderr, "Usage: %s [-c config_file] [-h]\n", prog);
    fprintf(stderr, "  -c config_file  Specify configuration file\n");
    fprintf(stderr, "  -h              Show this help message\n");
}

int main(int argc, char *argv[]) {
    const char *config_file = "proton.conf";
    int opt;

    /* Parse command line arguments */
    while ((opt = getopt(argc, argv, "c:h")) != -1) {
        switch (opt) {
            case 'c':
                config_file = optarg;
                break;
            case 'h':
                usage(argv[0]);
                return 0;
            default:
                usage(argv[0]);
                return 1;
        }
    }

    proton_pid = getpid();

    printf("Proton Web Server v%s starting...\n", PROTON_VERSION);
    printf("Configuration file: %s\n", config_file);

    /* Parse configuration */
    proton_config_t *config = proton_config_parse(config_file);
    if (!config) {
        fprintf(stderr, "Failed to parse configuration file\n");
        return 1;
    }

    /* Initialize logging */
    proton_log_init(config->error_log, LOG_INFO);
    proton_log(LOG_INFO, "Proton v%s starting (pid=%d)", PROTON_VERSION, proton_pid);

    /* Setup signal handlers */
    setup_signals();

    /* Start master process */
    int ret = proton_master_process(config);

    /* Cleanup */
    proton_log(LOG_INFO, "Proton shutting down");
    proton_log_close();
    proton_config_destroy(config);

    return ret;
}
