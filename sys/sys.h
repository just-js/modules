#ifndef JUST_SYS_H
#define JUST_SYS_H

#include <just.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <sys/timerfd.h>
#include <sys/reboot.h>
#include <sys/syscall.h>
#include <termios.h>
#ifndef STATIC
#include <dlfcn.h>
#endif

namespace just {

namespace sys {

struct pstat {
    long unsigned int utime_ticks;
    long int cutime_ticks;
    long unsigned int stime_ticks;
    long int cstime_ticks;
    long unsigned int vsize; // virtual memory size in bytes
    long unsigned int rss; //Resident  Set  Size in bytes
    long unsigned int cpu_total_time;
};

// from here: https://raw.githubusercontent.com/fho/code_snippets/master/c/getusage.c
/*
 * read /proc data into the passed struct pstat
 * returns 0 on success, -1 on error
*/
int get_usage(const pid_t pid, struct pstat* result) {
    //convert  pid to string
    char pid_s[20];
    snprintf(pid_s, sizeof(pid_s), "%d", pid);
    char stat_filepath[30] = "/proc/"; strncat(stat_filepath, pid_s,
            sizeof(stat_filepath) - strlen(stat_filepath) -1);
    strncat(stat_filepath, "/stat", sizeof(stat_filepath) -
            strlen(stat_filepath) -1);

    FILE *fpstat = fopen(stat_filepath, "r");
    if (fpstat == NULL) {
        perror("FOPEN ERROR ");
        return -1;
    }

    FILE *fstat = fopen("/proc/stat", "r");
    if (fstat == NULL) {
        perror("FOPEN ERROR ");
        fclose(fstat);
        return -1;
    }

    //read values from /proc/pid/stat
    bzero(result, sizeof(struct pstat));
    long int rss;
    if (fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
                "%lu %ld %ld %*d %*d %*d %*d %*u %lu %ld",
                &result->utime_ticks, &result->stime_ticks,
                &result->cutime_ticks, &result->cstime_ticks, &result->vsize,
                &rss) == EOF) {
        fclose(fpstat);
        return -1;
    }
    fclose(fpstat);
    result->rss = rss * getpagesize();

    //read+calc cpu total time from /proc/stat
    long unsigned int cpu_time[10];
    bzero(cpu_time, sizeof(cpu_time));
    if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3],
                &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7],
                &cpu_time[8], &cpu_time[9]) == EOF) {
        fclose(fstat);
        return -1;
    }

    fclose(fstat);

    for(int i=0; i < 10;i++)
        result->cpu_total_time += cpu_time[i];

    return 0;
}

uint64_t hrtime();

void WaitPID(const FunctionCallbackInfo<Value> &args);
void Spawn(const FunctionCallbackInfo<Value> &args);
void RunMicroTasks(const FunctionCallbackInfo<Value> &args);
void EnqueueMicrotask(const FunctionCallbackInfo<Value>& args);
void Exit(const FunctionCallbackInfo<Value>& args);
void Kill(const FunctionCallbackInfo<Value>& args);
void GetrUsage(const FunctionCallbackInfo<Value> &args);
void CPUUsage(const FunctionCallbackInfo<Value> &args);
void PID(const FunctionCallbackInfo<Value> &args);
void PPID(const FunctionCallbackInfo<Value> &args);
void TID(const FunctionCallbackInfo<Value> &args);
void Fork(const FunctionCallbackInfo<Value> &args);
void Ioctl(const FunctionCallbackInfo<Value> &args);
void Exec(const FunctionCallbackInfo<Value> &args);
void GetSid(const FunctionCallbackInfo<Value> &args);
void SetSid(const FunctionCallbackInfo<Value> &args);
void Errno(const FunctionCallbackInfo<Value> &args);
void StrError(const FunctionCallbackInfo<Value> &args);
void Sleep(const FunctionCallbackInfo<Value> &args);
void Getchar(const FunctionCallbackInfo<Value> &args);
void Putchar(const FunctionCallbackInfo<Value> &args);
void SetTerminalFlags(const FunctionCallbackInfo<Value> &args);
void GetTerminalFlags(const FunctionCallbackInfo<Value> &args);
void ShmOpen(const FunctionCallbackInfo<Value> &args);
void ShmUnlink(const FunctionCallbackInfo<Value> &args);
void USleep(const FunctionCallbackInfo<Value> &args);
void NanoSleep(const FunctionCallbackInfo<Value> &args);
void SharedMemoryUsage(const FunctionCallbackInfo<Value> &args);
void HeapObjectStatistics(const FunctionCallbackInfo<Value> &args);
void HeapCodeStatistics(const FunctionCallbackInfo<Value> &args);
void HeapSpaceUsage(const FunctionCallbackInfo<Value> &args);
void Memcpy(const FunctionCallbackInfo<Value> &args);
void Calloc(const FunctionCallbackInfo<Value> &args);
void BufferInfo(const FunctionCallbackInfo<Value> &args);
void ReadString(const FunctionCallbackInfo<Value> &args);
void Utf8Length(const FunctionCallbackInfo<Value> &args);
void WriteString(const FunctionCallbackInfo<Value> &args);
void WriteCString(const FunctionCallbackInfo<Value> &args);
void GetAddress(const FunctionCallbackInfo<Value> &args);
void Fcntl(const FunctionCallbackInfo<Value> &args);
void Cwd(const FunctionCallbackInfo<Value> &args);
void Env(const FunctionCallbackInfo<Value> &args);
void Timer(const FunctionCallbackInfo<Value> &args);
void AvailablePages(const FunctionCallbackInfo<Value> &args);
void ReadMemory(const FunctionCallbackInfo<Value> &args);
void WritePointer(const FunctionCallbackInfo<Value> &args);
void MMap(const FunctionCallbackInfo<Value> &args);
void MUnmap(const FunctionCallbackInfo<Value> &args);
void Reboot(const FunctionCallbackInfo<Value> &args);
void Getenv(const FunctionCallbackInfo<Value> &args);
void Setenv(const FunctionCallbackInfo<Value> &args);
void Unsetenv(const FunctionCallbackInfo<Value> &args);
void MemFdCreate(const FunctionCallbackInfo<Value> &args);

void GetPgrp(const FunctionCallbackInfo<Value> &args);
void SetPgid(const FunctionCallbackInfo<Value> &args);

void GetUid(const FunctionCallbackInfo<Value> &args);
void SetUid(const FunctionCallbackInfo<Value> &args);
void GetGid(const FunctionCallbackInfo<Value> &args);
void SetGid(const FunctionCallbackInfo<Value> &args);

#ifndef STATIC
void DLOpen(const FunctionCallbackInfo<Value> &args);
void DLSym(const FunctionCallbackInfo<Value> &args);
void DLClose(const FunctionCallbackInfo<Value> &args);
void DLError(const FunctionCallbackInfo<Value> &args);
#endif

void FExecVE(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_sys() {
		return (void*)just::sys::Init;
	}
}

#endif
