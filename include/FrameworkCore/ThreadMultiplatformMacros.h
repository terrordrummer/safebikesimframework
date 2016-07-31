#ifndef _THREADMULTIPLATFORMMACROS_H
#define _THREADMULTIPLATFORMMACROS_H
#pragma once

/* Container of macro definition to have both  Windows and Linux code */

#define _MUTEX_LOCK

#ifdef WIN32
#include <windows.h>

/* Thread Identified                                                                */
#define THREADID HANDLE

/* THREAD PROCEDURE                                                                 */
#define THREADAPI_RESULT    DWORD
#define THREADPROC(arg)     THREADAPI_RESULT WINAPI arg(__in  LPVOID lpParameter)

/* STARTING, SUSPENDING AND RESUMING A THREAD                                       */
#define START_THREAD(thread_id,proc)            thread_id = CreateThread(NULL,0,&proc,NULL,0,NULL)
#define SUSPEND(thread_id,synch_var,mutex)      SuspendThread(thread_id)
#define RESUME(thread_id,synch_var,mutex)       ResumeThread(thread_id)
#define KILL_THREAD(thread_id,status)           TerminateThread(thread_id,status)
#define SETTHREADPRIORITY(thread_id,priority)   SetThreadPriority(thread_id, priority)

/* THREAD PRIORITY                                                                      */
#define CM_CALLBACK_PRIORITY           THREAD_PRIORITY_HIGHEST
#define RECONSTRUCTION_THREAD_PRIORITY THREAD_PRIORITY_NORMAL       /* priority of VL and OL threads            */
#define ADAS_THREADS_PRIORITY          THREAD_PRIORITY_ABOVE_NORMAL /* priority of SA, CW and FCW threads       */

/* MUTEX: In Windows, mutex is identified by HANDLE                                 */
#define DECLARE_MUTEX(arg)                  HANDLE arg = NULL
#define MUTEX_TYPE                          HANDLE
// LOCK PROCEDURES
#ifdef _MUTEX_LOCK
#define CREATE_INSTANCE_MUTEX(arg)         {arg = CreateMutex( NULL, FALSE,  NULL);}
#define CREATE_MUTEX(arg,name)             {arg = CreateMutex( NULL, FALSE,  (LPCSTR)name);}
#define RELEASEMUTEX(arg)                   ReleaseMutex(arg)
#endif

#ifdef _SEMAPHORE_LOCK
#define CREATE_INSTANCE_MUTEX(arg)         {arg = CreateSemaphore( NULL, 1, 1, NULL);}
#define CREATE_MUTEX(arg,name)             {arg = CreateSemaphore( NULL, 1, 1, (LPCSTR)name);}
#define RELEASEMUTEX(arg)                   ReleaseSemaphore(arg, 1, NULL)
#endif

#define DESTROY_MUTEX(arg) if (arg != NULL) { CloseHandle( arg ); arg = NULL;}

/* MUTEX SYNCHRONIZATION                                                            */
#define WAITFORMUTEX(arg,timeout)           WaitForSingleObject(arg, timeout)

#define WAITFORTHREADMUTEX(arg,timeout)     WaitForSingleObject(&arg, timeout)
#define MUTEX_TIMEOUT_CODE                  WAIT_TIMEOUT
#define MUTEX_INFINITE_TIMEOUT              INFINITE

/* EVENT SYNCHRONIZATION                                                           */
#define EVENT_TYPE  HANDLE

/* MultiThread API Error managing                                                   */
#define MTAPI_ISERROR(arg)      arg==0
#define SLEEP(arg) Sleep(arg)

/* Clock time extraction                                                            */
#define SR_TIMEVALUE DWORD
#pragma comment(lib,"winmm.lib")
#define GET_TIME timeGetTime()
/* return procedure in thread                                                       */
#define THREAD_RETURN   return(0)

#define SET_START_CLOCK_TIME(arg)   arg = windows_gettime()*1000
#define TIME_GET_SEC(arg)           (arg/1000000)
#define TIME_GET_FRAC(arg)          (arg-(arg/1000000)*1000000)
#define TIME_UNIT                   1000000.0
#define SETTIME(sec,frac)           (sec*1000000+frac)

#endif

#if defined(__linux__) || ( defined(__MACH__) && defined(__APPLE__) )
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

/* Thread Identified                                                                */
#define THREADID pthread_t

/* THREAD PROCEDURE                                                                 */
#define THREADPROC(arg) void *arg(void *ptr)

/* STARTING, SUSPENDING AND RESUMING A THREAD                                       */
#define START_THREAD(thread_id,proc) pthread_create(&thread_id,NULL,&proc,NULL)
#define SUSPEND(thread_id,synch_var,mutex) { pthread_mutex_Lock(&mutex); pthread_cond_wait(&synch_var,&mutex);  pthread_mutex_Unlock(&mutex); }
#define RESUME(thread_id,synch_var,mutex)  { pthread_mutex_Lock(&mutex); pthread_cond_signal(&synch_var); pthread_mutex_Unlock(&mutex); }

#define KILL_THREAD(thread_id,status)  pthread_cancel(thread_id)

/* SET THE THREAD PRIORITY                                                          */
static struct sched_param thread_sched_param;
#define CM_CALLBACK_PRIORITY           99  /* priority of CM Callback                  */
#define RECONSTRUCTION_THREAD_PRIORITY 75  /* priority of VL and OL threads            */
#define ADAS_THREADS_PRIORITY          50  /* priority of SA, CW and FCW threads       */
#define SETTHREADPRIORITY(thread_id,priority) {thread_sched_param.sched_priority=priority; pthread_setschedparam(thread_id,SCHED_RR,&thread_sched_param);}

/* MUTEX: In linux, mutex are pthread_mutex_t type                                  */
#define MUTEX_TYPE pthread_mutex_t
#define DECLARE_MUTEX(arg) MUTEX_TYPE arg
#define CREATE_GENERIC_MUTEX(arg) pthread_mutex_init(&arg,NULL)
#define CREATE_GENERIC_MUTEX(arg) pthread_mutex_init(&arg,NULL)
#define DESTROY_MUTEX(arg) pthread_mutex_destroy(&arg)

/* MUTEX SYNCHRONIZATION                                                            */
#define WAITFORMUTEX(arg,timeout) pthread_mutex_Lock(&arg)
#define RELEASEMUTEX(arg) pthread_mutex_Unlock(&arg)
#define WAITFORTHREADMUTEX(arg,timeout) pthread_mutex_tryLock(&arg)
#define MUTEX_TIMEOUT_CODE EBUSY

/* THREAD SYNCHRONIZATION                                                           */
#define DECLARE_SYNCH_VAR(arg) pthread_cond_t arg = PTHREAD_COND_INITIALIZER

/* MultiThread API Error managing                                                   */
#define MTAPI_ISERROR(arg) 0
#define SLEEP(arg) usleep((arg)*1000)

/* Clock time extraction                                                            */
SR_TIMEVALUE linux_gettime()
    {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (SR_TIMEVALUE)(tv.tv_sec*(time_t)1000000 + tv.tv_usec);
    }
#define GET_CLOCK_TIME(arg) arg = (linux_gettime()-StartTime)
#define SET_START_CLOCK_TIME(arg) arg = linux_gettime()
#define TIME_GET_SEC(arg) (arg/1000000)
#define TIME_GET_FRAC(arg) (arg-(arg/1000000)*1000000)
#define TIME_UNIT 1000000.0
#define SETTIME(sec,frac) (sec*1000000+frac)

/* return procedure in thread                                                       */
#define THREAD_RETURN   return NULL

#define FALSE false
#endif

#endif