#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#if defined __unix__ || defined __APPLE__
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#elif defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#include <iphlpapi.h>
#endif
#include "init.h"
#include "thread.h"

#define get_byte(d,i) ( ( ( d ) >> ( ( i ) * 8 ) ) & 0xff )

char *LOG_PATH = "./Bulltricker_Data/Logs/";

void reverse(char s[]) {
  int i, j;
  char c;
  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

void my_itoa(int n, char s[]) {
  int i, sign;
  if ((sign = n) < 0)
    n = -n;
  i = 0;
  do {
    s[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);
  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}

char *get_local_time() {
  CURRENT_TIME = time(NULL);
  struct tm *time_info = localtime(&CURRENT_TIME);
  return trim(asctime(time_info));
}

void debug_log(Bool trace_call, int line, const char *file, char *format, ...) {
  SDL_LockMutex(log_mutex);
  char *path = malloc(100);
  char *time = malloc(100);
  my_itoa(START_TIME, time);
  char *name = malloc(105);
  name = strcpy(name, "Game_");
  strcat(name, time); strcat(name, ".log");
  FILE *fptr = fopen(strcat(strcpy(path, LOG_PATH), name), "a");
  if (fptr != NULL) {
    va_list args;
    if (trace_call == true)
      fprintf(fptr, "File \"%s\", line %d:\n", file, line);
    va_start(args, format);
    vfprintf(fptr, format, args);
    va_end(args);
    fclose(fptr);
  }
  free(path); free(time); free(name);
  SDL_UnlockMutex(log_mutex);
}

char *trim(char *s) {
  char *ptr;
  if (!s)
    return NULL;
  if (!*s)
    return s;
  for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
  ptr[1] = '\0';
  return s;
}

char *pop_back(char *s) {
  if (!s)
    return NULL;
  if (!*s)
    return s;
  s[strlen(s)-1] = '\0';
  return s;
}

int mirror_position(int index) {
  return get_board_size()-1-(index/column_count)*column_count-(index%column_count);
}

char *get_current_ip() {
  #if defined __unix__ || defined __APPLE__
    struct ifaddrs *addresses;
    if (getifaddrs(&addresses) == -1)
      return NULL;
    struct ifaddrs *address = addresses;
    while(address) {
      int family = address->ifa_addr->sa_family;
      if (family == AF_INET) {
        char *ap = malloc(16);
        getnameinfo(address->ifa_addr, sizeof(struct sockaddr_in), ap, 16, 0, 0, NI_NUMERICHOST);
        if (strstr(ap, "0.0.0.0") == NULL && strstr(ap, "127.0.0.1") == NULL)
          return ap;
      }
      address = address->ifa_next;
    }
    freeifaddrs(addresses);
    return NULL;
  #elif defined(_WIN32) || defined(WIN32)
    UINT i = 0;
    DWORD dwRetVal = 0;
    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
      return NULL;
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
      free(pAdapterInfo);
      pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
      if (pAdapterInfo == NULL)
        return NULL;
    }
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
      pAdapter = pAdapterInfo;
      while (pAdapter) {
        if (strstr(pAdapter->Description, "Virtual") == NULL && strstr(pAdapter->IpAddressList.IpAddress.String, "0.0.0.0") == NULL) {
          if (pAdapterInfo)
            free(pAdapterInfo);
          return pAdapter->IpAddressList.IpAddress.String;
        }
        pAdapter = pAdapter->Next;
      }
      if (pAdapterInfo)
        free(pAdapterInfo);
      return NULL;
    }
    else {
      if (pAdapterInfo)
        free(pAdapterInfo);
      return NULL;
    }
  #endif
}
