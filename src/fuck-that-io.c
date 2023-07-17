/*
#include "io.h"
#include <pthread.h>

#include <stdint.h>
#include <stdio.h>

pthread_mutex_t io_data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t io_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t io_cond = PTHREAD_COND_INITIALIZER;
pthread_t io_thread;
char io_thread_data[UINT8_MAX];
uint8_t io_thread_ahead = 0;
uint8_t io_thread_head = 0;

void* io_thread_loop(void *args)
{
    
    char c;
    while ((c = getchar()) != EOF)
    {
        pthread_mutex_lock(&io_data_mutex);
        if (io_thread_ahead == UINT8_MAX)
        {
            pthread_mutex_unlock(&io_data_mutex);
            pthread_mutex_lock(&io_cond_mutex);
            pthread_cond_wait(&io_cond, &io_cond_mutex);
            pthread_mutex_unlock(&io_cond_mutex);
            pthread_mutex_lock(&io_data_mutex);
        }
        io_thread_data[io_thread_head + io_thread_ahead] = c;
        io_thread_ahead++;
        pthread_mutex_unlock(&io_data_mutex);
    }
    
    return NULL;
}

crb_error_t crb_io_init()
{
    int err;
    if ((err = pthread_create(&io_thread, NULL, io_thread_loop, NULL))) return CRB_ERR__ENEW(CRB_ERR_INNER_ERROR, "Failed to create thread!");
    return crb_error_ok;
}

crb_error_t crb_read_char(char *result)
{
    pthread_mutex_lock(&io_data_mutex);
    if (io_thread_ahead == 0)
    {
        *result = '\0';
    }
    else
    {
        *result = io_thread_data[io_thread_head + io_thread_ahead];
        io_thread_ahead--;
    }
    pthread_mutex_unlock(&io_data_mutex);

    return crb_error_ok;
}
*/