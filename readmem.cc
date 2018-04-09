#include <stdio.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <iostream>
#include <iostream>
#include <iomanip>


typedef unsigned char byte;


using namespace std;


/* The hexDump function was taken from: https://stackoverflow.com/a/7776146
 */
void hexDump(std::string desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
	std::cout << desc << std::endl;

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}


int readmem(int pid, byte *buf, unsigned long start_address, int nr_bytes) {
    const iovec remote_mem_addr_space = {(void*)start_address, static_cast<size_t>(nr_bytes)};
    const iovec local_mem_addr_space = {buf, static_cast<size_t>(nr_bytes)};
    int nr_read_bytes = process_vm_readv(pid,
                                         &local_mem_addr_space, 1,
                                         &remote_mem_addr_space, 1,
                                         0);
    std::cout << "Read " << nr_read_bytes << " bytes." << std::endl;
    return nr_read_bytes;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cout << "Invalid arguments. Usage: exename <pid>, <nr_bytes>" << std::endl;
        return 1;
    }

    auto pid = atoi(argv[1]);
    auto start_address = strtoul(argv[2], nullptr, 10);
    auto nr_bytes = strtoul(argv[3], nullptr, 10);

    byte buf[nr_bytes];
    memset(&buf[0], 0, sizeof(buf));

    std::cout << "Attempting to read memory of PID " << pid << " from " << start_address << " for "
              << nr_bytes << " bytes..." << std::endl;
    auto nr_read_bytes = readmem(pid, &buf[0], start_address, nr_bytes);
    if (nr_read_bytes <= 0) {
        std:cerr << "Could not read memory of process\n";
        return -1;
    }
	hexDump("Read mem:", buf, nr_bytes);

    return 0;
}
