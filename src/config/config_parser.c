#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "proton.h"

/* Simple config parser - simplified version */

static void trim(char *str) {
    if (!str) return;
    
    /* Trim leading whitespace */
    char *start = str;
    while (*start && isspace(*start)) start++;
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    /* Trim trailing whitespace */
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        *end = '\0';
        end--;
    }
}

static int parse_int(const char *value, int default_value) {
    if (!value) return default_value;
    
    if (strcmp(value, "auto") == 0) {
        return 0; /* Auto-detect */
    }
    
    return atoi(value);
}

proton_config_t* proton_config_parse(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Failed to open config file: %s\n", filename);
        
        /* Return default config */
        proton_config_t *config = calloc(1, sizeof(proton_config_t));
        if (config) {
            config->worker_processes = 0; /* auto */
            config->worker_connections = 1024;
            config->listen_port = 8080;
            config->error_log = strdup("stderr");
            config->access_log = strdup("/dev/null");
            config->document_root = strdup(".");
        }
        return config;
    }
    
    proton_config_t *config = calloc(1, sizeof(proton_config_t));
    if (!config) {
        fclose(fp);
        return NULL;
    }
    
    /* Set defaults */
    config->worker_processes = 0; /* auto */
    config->worker_connections = 1024;
    config->listen_port = 8080;
    config->error_log = strdup("stderr");
    config->access_log = strdup("/dev/null");
    config->document_root = strdup(".");
    
    /* Parse line by line */
    char line[1024];
    int in_http = 0;
    int in_server = 0;
    
    while (fgets(line, sizeof(line), fp)) {
        trim(line);
        
        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#') continue;
        
        /* Parse directives */
        if (strncmp(line, "worker_processes", 16) == 0) {
            char *value = strchr(line, ' ');
            if (value) {
                value++;
                trim(value);
                /* Remove semicolon */
                char *semi = strchr(value, ';');
                if (semi) *semi = '\0';
                config->worker_processes = parse_int(value, 0);
            }
        }
        else if (strncmp(line, "worker_connections", 18) == 0) {
            char *value = strchr(line, ' ');
            if (value) {
                value++;
                trim(value);
                char *semi = strchr(value, ';');
                if (semi) *semi = '\0';
                config->worker_connections = atoi(value);
            }
        }
        else if (strncmp(line, "listen", 6) == 0 && in_server) {
            char *value = strchr(line, ' ');
            if (value) {
                value++;
                trim(value);
                char *semi = strchr(value, ';');
                if (semi) *semi = '\0';
                config->listen_port = atoi(value);
            }
        }
        else if (strncmp(line, "error_log", 9) == 0) {
            char *value = strchr(line, ' ');
            if (value) {
                value++;
                trim(value);
                char *semi = strchr(value, ';');
                if (semi) *semi = '\0';
                free(config->error_log);
                config->error_log = strdup(value);
            }
        }
        else if (strncmp(line, "root", 4) == 0 && in_server) {
            char *value = strchr(line, ' ');
            if (value) {
                value++;
                trim(value);
                char *semi = strchr(value, ';');
                if (semi) *semi = '\0';
                free(config->document_root);
                config->document_root = strdup(value);
            }
        }
        else if (strcmp(line, "http {") == 0) {
            in_http = 1;
        }
        else if (strcmp(line, "server {") == 0 && in_http) {
            in_server = 1;
        }
        else if (strcmp(line, "}") == 0) {
            if (in_server) in_server = 0;
            else if (in_http) in_http = 0;
        }
    }
    
    fclose(fp);
    return config;
}

void proton_config_destroy(proton_config_t *config) {
    if (!config) return;
    
    free(config->error_log);
    free(config->access_log);
    free(config->document_root);
    free(config);
}
