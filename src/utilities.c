#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#if defined(_WIN32) || defined(WIN32)
#include <winsock2.h>
#include <iphlpapi.h>
#endif
#include "init.h"
#include "thread.h"

#define get_byte(d,i) ( ( ( d ) >> ( ( i ) * 8 ) ) & 0xff )

char *LOG_PATH = "./Bulltricker_Data/Logs/";

char *get_local_time() {
  CURRENT_TIME = time(NULL);
  struct tm *time_info = localtime(&CURRENT_TIME);
  return trim(asctime(time_info));
}

void debug_log(Bool show_call, int line, const char *file, char *format, ...) {
  SDL_LockMutex(log_mutex);
  char *path = malloc(100);
  char *time = malloc(100);
  itoa(START_TIME, time, 10);
  char *name = malloc(105);
  name = strcpy(name, "Game_");
  strcat(name, time); strcat(name, ".log");
  FILE *fptr = fopen(strcat(strcpy(path, LOG_PATH), name), "a");
  va_list args;
  if (show_call == true)
    fprintf(fptr, "File \"%s\", line %d:\n", file, line);
  va_start(args, format);
  vfprintf(fptr, format, args);
  va_end(args);
  fclose(fptr);
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

char *ip_to_string(const __int32 ip_address) {
  char *ip_string = malloc(16);
  sprintf(ip_string, "%i.%i.%i.%i", get_byte(ip_address, 0), get_byte(ip_address, 1), get_byte(ip_address, 2), get_byte(ip_address, 3));
  return ip_string;
}

#if defined(_WIN32) || defined(WIN32)
char *get_win_ip() {
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
    printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    if (pAdapterInfo)
      free(pAdapterInfo);
    return FALSE;
  }
}
#endif
