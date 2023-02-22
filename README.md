# Linux-System-Information-Tool
Anusha Karkhanis CSCB09 Winter 2023
A software tool designed to gather data from a Linux system. 
Displays information such as program memory usage, device memory usage, CPU utilization,
and all users logged onto the system.

Along with running the program without any additional flags, the following arguments are also accepted: 
--system or -s
  Displays system information only.
--user or -u
  Displays user information only.
--sequential or -q
  Displays the information in a sequential output for each sample, useful for conversion
  into a .txt file.
--tdelay or -t or --tdelay=N 
  To indicate the number of seconds to wait between the collection of each sample.
  If either of the first 2 is used, the default value for tdelay will be 1 second. 
  If the third option is used, tdelay will be set to N seconds.
-- samples or -m or --samples=N
  To indicate the number of times the information is to be refreshed.
  If either of the first 2 is used, the default value for samples will be 10. 
  If the third option is used, samples will be set to N samples
--graphics or --g
  Displays a graphical representation of CPU Usage.
  
Positional arguments without any flags are also accepted in the order samples tdelay
If only one positional argument is given, it will be read as a value for samples.
Positional arguments must precede all other arguments.
