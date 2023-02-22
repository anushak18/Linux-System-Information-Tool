#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <getopt.h>
#include<sys/utsname.h>
#include<utmp.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#define USER_PROCESS  7
/*
CSCB09 Winter 2023
Anusha Karkhanis

The program serves as a system monitoring tool designed to read and print data 
on a Linux machine. Features include displaying memory usage, CPU utilization,
other users logged onto the machine, and basic system information including 
the machine name and architecture.
*/

/*
The following function reads system information from sys/utsname.h using fields
of the struct utsname including sysname, nodename, release, version, and machine.
It then prints the information in the corresponding order.
*/
void system_info(){
    struct utsname uname_pointer;
    uname(&uname_pointer);
    printf("\t-----System Information:-----\n");
    printf("System Name:  %s\n", uname_pointer.sysname);
    printf("Machine Name: %s\n", uname_pointer.nodename);
    printf("Release:      %s\n", uname_pointer.release);
    printf("Version:      %s\n", uname_pointer.version);
    printf("Architecture: %s\n", uname_pointer.machine);
}

/*
The following function reads information from utmp.h and utilizes the fields of 
struct utmp including ut_user, ut_line, and ut_host. These display the username,
hostname, and device name of all users logged on the system at that time.
*/
void user_info(){
    struct utmp *ut;
    setutent();
    printf("\t-----User Information:-----\n");

    while ((ut = getutent()) != NULL ) {
        //using the constant defined USER_PROCESS to ensure only users are being printed
        if (ut->ut_type == USER_PROCESS) {
            printf("%s\t%s\t%s\n", ut->ut_user, ut->ut_line, ut->ut_host);
        }
        
    }
    endutent();
    
  
}

/*
The following function reads data from sys/sysinfo.h and uses fields of struct 
sysinfo including totalram, totalswap, freeram, and freeswap. It calculates 
the total and used physical memory and virtual memory on the system and converts
it to GB, which is then printed out.
*/
void memory_usage(){
    
    struct sysinfo memInfo;
    sysinfo(&memInfo);
    //getting memory information
    float totalPhysicalMem = memInfo.totalram;
    float totalVirtualMem = memInfo.totalram + memInfo.totalswap;
    float usedPhysicalMem = memInfo.totalram - memInfo.freeram;
    float usedVirtualMem = memInfo.totalram + memInfo.totalswap - memInfo.freeswap - memInfo.freeram;
    
    //Conversion to GB
    totalPhysicalMem = totalPhysicalMem/(1024*1024*1024);
    totalVirtualMem = totalVirtualMem/(1024*1024*1024);
    usedPhysicalMem = usedPhysicalMem/(1024*1024*1024);
    usedVirtualMem = usedVirtualMem/(1024*1024*1024);
    printf("Physical Memory Usage (GB): Used/Total = %.2f/%.2f\n", usedPhysicalMem, totalPhysicalMem);
    printf("\t\tVirtual Memory Usage (GB): Used/Total = %.2f/%.2f\n", usedVirtualMem, totalVirtualMem);
    
}

/*
The following function calculates and prints the memory used by this program
using struct rusage from sys/resource.h. 
*/
void memory_used_by_program(){
    struct rusage rusage_ptr;
    printf("\t-----Memory Used by Program:-----\n");
    getrusage(RUSAGE_SELF, &rusage_ptr); 
    printf("%ld kB\n", rusage_ptr.ru_maxrss);
}

/*
The following function returns the number of cores found on the machine using
unistd.h. 
*/
int number_of_cores() {
    return sysconf(_SC_NPROCESSORS_ONLN);
}


/*
The following function reads data from /proc/stat and calculates total CPU
usage by using 2 samples from different points in time and comparing the change
in CPU relative to the first. It obtains the data and then converts it to a percentage
to print out.
*/
void cpu_usage(){
    FILE* file;
    char *ptr;
    char buffer[1024];
    int user, nice, system, idle, iowait, irq, softirq, steal, total, idle_difference;
    int idle_previous=0, total_previous=0;
    float cpu_use;

    //read data first time, store those values
    file = fopen("/proc/stat", "r");
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    ptr = buffer;
    sscanf(ptr, "%*s %d %d %d %d %d %d %d %d", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    //storing and adding up values from first line of /proc/stat, not including the word "cpu"
    total_previous = user+nice+system+idle+iowait+irq+softirq+steal;
    idle_previous = idle;
    //wait 1 second to generate appropriate gap between samples, ensure there is a difference btwn them
    sleep(1);
    //read data second time, store those values
    file = fopen("/proc/stat", "r");
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    ptr = buffer;
    sscanf(ptr, "%*s %d %d %d %d %d %d %d %d", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    total = user+nice+system+idle+iowait+irq+softirq+steal;
    idle_difference = idle - idle_previous;
    total=total-total_previous;
    cpu_use = ((((float)(total - idle_difference))/total) * 100);

    printf("Total CPU Utilization: %.2f%%\n", cpu_use);
  
}

/*
The following function performs the same tasks as cpu_usage as detailed above,
except it also includes an additional graphical representation of the CPU usage
to visually see how much or how little CPU is being used from sample to sample.
*/
void cpu_usage_graphics(){
    FILE* file;
    char *ptr;
    char buffer[1024];
    int user, nice, system, idle, iowait, irq, softirq, steal, total, idle_difference;
    int idle_previous=0, total_previous=0;
    float cpu_use;

    //read data first time, store those values
    file = fopen("/proc/stat", "r");
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    ptr = buffer;
    sscanf(ptr, "%*s %d %d %d %d %d %d %d %d", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    //storing and adding up values from first line of /proc/stat, not including the word "cpu"
    total_previous = user+nice+system+idle+iowait+irq+softirq+steal;
    idle_previous = idle;
    //wait 1 second to generate appropriate gap between samples, ensure there is a difference btwn them
    sleep(1);
    //read data second time, store those values
    file = fopen("/proc/stat", "r");
    fgets(buffer, sizeof(buffer), file);
    fclose(file);

    ptr = buffer;
    sscanf(ptr, "%*s %d %d %d %d %d %d %d %d", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
    total = user+nice+system+idle+iowait+irq+softirq+steal;
    idle_difference = idle - idle_previous;
    total=total-total_previous;
    cpu_use = ((((float)(total - idle_difference))/total) * 100);

    //everything above this line is the same as cpu_usage()
    printf("Total CPU utilization: %.2f%%\t", cpu_use);

    //graphic representation of CPU usage using '-' for values less than 1.00% and '|' for values
    //greater than or equal to 1.00%
    if (cpu_use == 0.00){
        printf("\n");
    }
    else if (0.000001 <= cpu_use && cpu_use <= 0.500000){
        printf("-\n");
    }
    else if (0.500001 <= cpu_use && cpu_use <= 0.800000){
        printf("--\n");
    }
    else if (0.800001 <= cpu_use && cpu_use <= 0.999999){
        printf("---\n");
    }
    else {
        for (int i = 0; i < (int)cpu_use; i++){
            printf("|");
        }
        printf("\n");
    }
  

}


/*
In main, the entire program is put together to allow it to perform cohesively.
Getopt.h is used to parse the command line arguments and allow the program
to be run with different commands, such as choosing for only system information
to be displayed, or choosing to include a graphical representation for data.
Positional arguments are accepted as the first 1 or 2 arguments following the
name of the program itself. Data is refreshed for each sample requested.
*/
int main(int argc, char *argv[]){
    int c = 0, samples = 10, tdelay = 1, system = 0, user = 0, sequential = 0, graphics = 0;
    char *end_pointer;
    int x = 2*samples;

    // call program with no additional arguments: ./SystemMonitorTool
    // all data is displayed: memory used by program, memory usage of the system,
    // CPU usage, list of users on the same machine, and system information
    if (argc == 1){
        memory_used_by_program();
        printf("\t-----Memory Usage:-----\n");
        for (int j = 0; j < 2*samples; j++){
            printf("\n");
        } 
        printf("\t-----CPU Usage:-----\n");
        int cores = number_of_cores();
        printf("Number of Cores: %d\n", cores);
        //escape codes used to refresh output of CPU and memory usage simultaneously
        for (int b = 0; b < samples; b++){
            printf("\x1B[%dA", ((2*samples)-(2*(b+1)))+4);
            printf("Sample #%d:\t", b+1);
            memory_usage();
            printf("\033[%dB", ((2*samples)-(2*(b+1)))+2);
            cpu_usage();
            printf("\x1B[A");
            sleep(tdelay);
        }
        printf("\n");
        user_info();
        system_info(); 

    }

    // program called with at least 1 other argument: samples/system/user/tdelay/graphics/sequential etc.
    else{
        //using getopt_long to handle 'long' arguments with flags
        static struct option long_options[] = {
            {"system",      no_argument,       0, 's'},
            {"user",        no_argument,       0, 'u' },
            {"sequential",  no_argument,       0, 'q' },
            {"graphics",    no_argument,       0, 'g' },
            {"tdelay",      optional_argument, 0, 't'},
            {"samples",     optional_argument, 0, 'm'},
            {0,             0,                 0,  0 }
        }; 
        while ((c=getopt_long(argc, argv, "suqtm:", long_options, NULL)) != -1){
            switch (c) {
                case 0:
                    samples = atoi(argv[c]);
                    break;
                case 1:
                    tdelay = atoi(argv[c]);
                    break;
                case 's':
                    system = 1;
                    break;
                case 'g':
                    graphics = 1;
                    break;
                case 'u':
                    user = 1;
                    break;
                case 'q':
                    sequential = 1;
                    break;
                
                //handling optional arguments of tdelay and samples
                case 't':
                    if (optarg){
                        tdelay = atoi(optarg); 
                    }
                    break; 
                case 'm':
                    if (optarg){
                        samples = atoi(optarg);
                    }
                    break;
            }

        }
        //getting positional arguments for samples and tdelay
        if (optind < argc) {
            //converting the first positional argument to a number to be stored into samples
            samples = strtol(argv[optind], &end_pointer, 10);
            //checking if input is valid
            if (*end_pointer != '\0') {
                return 1;
            }
            //advancing optind to check if the second positional argument, tdelay, is present
            optind++;
        }
        if (optind < argc) {
            tdelay = strtol(argv[optind], &end_pointer, 10);
            //checking if input is valid
            if (*end_pointer != '\0') {
                return 1;
            }
        }
        //sequential flag is not used, refreshing output using escape sequences and loops
        if(!sequential){
            memory_used_by_program();
            if (system || !user){
                 printf("\t-----Memory Usage:-----\n");
                 //allocating space for memory samples
                for (int j = 0; j < 2*samples; j++){
                    printf("\n");
                } 
                printf("\t-----CPU Usage:-----\n");
                int cores = number_of_cores();
                printf("Number of Cores: %d\n", cores); 
                if (!graphics){
                    //using escape codes to print memory sample then CPU usage simultaneously back and forth
                    for (int b = 0; b < samples; b++){
                    printf("\x1B[%dA", ((2*samples)-(2*(b+1)))+4);
                    printf("Sample #%d:\t", b+1);
                    memory_usage();
                    printf("\033[%dB", ((2*samples)-(2*(b+1)))+2);
                    cpu_usage();
                    //this escape code deletes the previous CPU usage number and reprints it in its place
                    printf("\x1B[A");
                    sleep(tdelay);
                    }
                
                    printf("\n");

                }
                if (graphics){
                    //same code as above except a graphical representation of CPU usage is used instead
                    for (int b = 0; b < samples; b++){
                        printf("\x1B[%dA", ((2*samples) - (b-2)));
                        printf("Sample #%d:\t", b+1);
                        memory_usage();
                        printf("\033[%dB", ((2*samples) - b));
                        cpu_usage_graphics();
                        //no longer have the escape code to replace the CPU usage, this time we print
                        //it on separate lines as in the video demo
                        sleep(tdelay);
                    }
                
                    
                }
                
            }
            if (user || !system) {
              
                user_info();
                
            }
            system_info();

        }
        if (sequential){
            //output displayed in a sequential manner, in order of each sample
            int counter = 0;
            for (int i = 0; i < samples; i++) {
                memory_used_by_program();
                if (system || !user) {
                    //printing empty lines before and/or after the current sample
                    //to indicate the position of the current sample
                    printf("\t-----Memory Usage:-----\n");
                    if (i == 0){
                        printf("Sample #1:\t");
                        memory_usage();
                        for(int j = 0; j < (2*samples)-2; j++){
                            printf("\n");
                        }
                    }
                    if (i>0 && counter>0){
                        for (int i = 0; i < 2*counter; i++){
                            printf("\n");
                        }
                        printf("Sample #%d:\t", i+1);
                        memory_usage();
                        for(int j = 0; j < ((2*samples)-2*(i+1)); j++){
                            printf("\n");
                        }

                    }
                    //incrementing counter to get number of blank lines to display
                    counter++;
                    if (!graphics){
                        printf("\t-----CPU Usage:-----\n");
                        int cores = number_of_cores();
                        printf("Number of Cores: %d\n", cores);
                        cpu_usage();
                    }
                    if (graphics){
                        //same code as above except including graphical representation of CPU usage
                        printf("\t-----CPU Usage:-----\n");
                        int cores = number_of_cores();
                        printf("Number of Cores: %d\n", cores);
                        cpu_usage_graphics();
                    }
                    
                }
                if (user || !system){
                    user_info();
                }
                //sleeping the program for tdelay seconds
                sleep(tdelay);
            }
            system_info();
        }
    } 
    return 0;
    
}