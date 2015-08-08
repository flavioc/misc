#ifndef MEM_HPP
#define MEM_HPP

//#define MEASURE_MEM

#include <stdio.h>
#include <string.h>

 int parseLine(char* line)
{
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i-3] = '\0';
    i = atoi(line);
    return i;
 }

int getMemUsage()
{
   // memory usage in KB.
   FILE* file = fopen("/proc/self/status", "r");
   if(!file)
      return -1;

   int result = -1;
   char line[128];

   while (fgets(line, 128, file) != NULL){
      if (strncmp(line, "VmSize:", 6) == 0){
         result = parseLine(line);
         break;
      }
   }
   fclose(file);
   return result;
 }

#ifdef MEASURE_MEM
static uint64_t start_mem = 0;
static uint64_t avg_memory = 0;
static uint64_t mem_count = 0;
#endif

static inline void
initMemory()
{
#ifdef MEASURE_MEM
   start_mem = getMemUsage();
#endif
}

static inline void
readMemory()
{
#ifdef MEASURE_MEM
   mem_count++;
   if(mem_count == 1) {
      avg_memory = getMemUsage() - start_mem;
   } else {
      double frac(((double)(mem_count-1)) / ((double)mem_count));
      double other(1.0 - frac);
      avg_memory = frac * ((double)avg_memory) + other * ((double)(getMemUsage() - start_mem));
   }
#endif
}

static inline void
printMemory()
{
#ifdef MEASURE_MEM
   if(!avg_memory)
      avg_memory = 1;
   std::cout << avg_memory << " " << (getMemUsage() - start_mem) << std::endl;
#endif
}

#endif
