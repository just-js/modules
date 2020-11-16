#ifndef JUST_SYS_H
#define JUST_SYS_H

#include <just.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <limits.h>
#include <sys/timerfd.h>

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

ssize_t process_memory_usage() {
  char buf[1024];
  const char* s = NULL;
  ssize_t n = 0;
  long val = 0;
  int fd = 0;
  int i = 0;
  do {
    fd = open("/proc/thread-self/stat", O_RDONLY);
  } while (fd == -1 && errno == EINTR);
  if (fd == -1) return (ssize_t)errno;
  do
    n = read(fd, buf, sizeof(buf) - 1);
  while (n == -1 && errno == EINTR);
  close(fd);
  if (n == -1)
    return (ssize_t)errno;
  buf[n] = '\0';
  s = strchr(buf, ' ');
  if (s == NULL)
    goto err;
  s += 1;
  if (*s != '(')
    goto err;
  s = strchr(s, ')');
  if (s == NULL)
    goto err;
  for (i = 1; i <= 22; i++) {
    s = strchr(s + 1, ' ');
    if (s == NULL)
      goto err;
  }
  errno = 0;
  val = strtol(s, NULL, 10);
  if (errno != 0)
    goto err;
  if (val < 0)
    goto err;
  return val * getpagesize();
err:
  return 0;
}

ssize_t process_memory_usage();
uint64_t hrtime();

void WaitPID(const FunctionCallbackInfo<Value> &args);
void Spawn(const FunctionCallbackInfo<Value> &args);
void HRTime(const FunctionCallbackInfo<Value> &args);
void RunMicroTasks(const FunctionCallbackInfo<Value> &args);
void EnqueueMicrotask(const FunctionCallbackInfo<Value>& args);
void Exit(const FunctionCallbackInfo<Value>& args);
void Kill(const FunctionCallbackInfo<Value>& args);
void GetrUsage(const FunctionCallbackInfo<Value> &args);
void CPUUsage(const FunctionCallbackInfo<Value> &args);
void PID(const FunctionCallbackInfo<Value> &args);
void Errno(const FunctionCallbackInfo<Value> &args);
void StrError(const FunctionCallbackInfo<Value> &args);
void Sleep(const FunctionCallbackInfo<Value> &args);
void USleep(const FunctionCallbackInfo<Value> &args);
void NanoSleep(const FunctionCallbackInfo<Value> &args);
void MemoryUsage(const FunctionCallbackInfo<Value> &args);
void SharedMemoryUsage(const FunctionCallbackInfo<Value> &args);
void HeapObjectStatistics(const FunctionCallbackInfo<Value> &args);
void HeapCodeStatistics(const FunctionCallbackInfo<Value> &args);
void HeapSpaceUsage(const FunctionCallbackInfo<Value> &args);
void Memcpy(const FunctionCallbackInfo<Value> &args);
void Calloc(const FunctionCallbackInfo<Value> &args);
void ReadString(const FunctionCallbackInfo<Value> &args);
void Utf8Length(const FunctionCallbackInfo<Value> &args);
void WriteString(const FunctionCallbackInfo<Value> &args);
void GetAddress(const FunctionCallbackInfo<Value> &args);
void Fcntl(const FunctionCallbackInfo<Value> &args);
void Cwd(const FunctionCallbackInfo<Value> &args);
void Env(const FunctionCallbackInfo<Value> &args);
void Timer(const FunctionCallbackInfo<Value> &args);
void AvailablePages(const FunctionCallbackInfo<Value> &args);
void ReadMemory(const FunctionCallbackInfo<Value> &args);
void MMap(const FunctionCallbackInfo<Value> &args);
void MUnmap(const FunctionCallbackInfo<Value> &args);
//void ShmOpen(const FunctionCallbackInfo<Value> &args);
//void ShmUnlink(const FunctionCallbackInfo<Value> &args);

void Init(Isolate* isolate, Local<ObjectTemplate> target);
}

}

extern "C" {
	void* _register_sys() {
		return (void*)just::sys::Init;
	}
}

#endif
