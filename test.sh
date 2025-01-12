#!/usr/bin/env bash

#
# Copyright (C) 2025  tete
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

OUTPUT_BINARY="temp_test_temp"  # Name of the compiled binary
LOG_FILE="valgrind_output.log" # Log file for Valgrind output
OID_LIB_PATH=$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${PWD}/build:$LD_LIBRARY_PATH

function cli_exit() {
    LD_LIBRARY_PATH=$OID_LIB_PATH
    exit $1
}

# Compile the source code
echo "Compiling test code"
gcc -g -std=c99 -O0 -L./build -I./include -lsafetynet -o "$OUTPUT_BINARY" -x c - <<EOF
#include <stdio.h>
#include <stdlib.h>

#define __SN_WIP_CALLS__
#include "libsafetynet.h"
void sn_set_last_error(sn_error_codes_e er_code);
#include <time.h>
#include <stddef.h>
#include <stdint.h>

int main()
{
    for (volatile size_t i = 0; i < 400; i++)
    {
        sn_malloc(sizeof(size_t));
    }

    int32_t* buff = sn_malloc(sizeof(int32_t) * 10);

    if (!buff)
    {
        printf("ERROR: %s", sn_get_error_msg(sn_get_last_error()));
        exit(1);
    }

    size_t buff_size = SN_GET_ARR_SIZE(sn_query_size(buff), sizeof(int32_t));

    clock_t start, end;
    double cpu_time_used;

    // Start the clock
    start = clock();

    printf("Allocated buffer of %lu\n", sn_query_size(buff));

    // End the clock
    end = clock();

    // Calculate elapsed time
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Size query Time: %f seconds\n", cpu_time_used);

    for (size_t i = 0; i < buff_size; i++)
    {
        buff[i] = rand();
    }

    for (size_t i = 0; i < buff_size; i++)
    {
        printf("buff[%lu] = %i\n", i, buff[i]);
    }

    sn_set_last_error(SN_WARN_DUB_FREE);
    printf("ERROR: %s", sn_get_error_msg(sn_get_last_error()));

    const sn_mem_metadata_t* const metadata = sn_query_metadata(buff);

    printf("\n\nmetadata\n");
    printf("data: %p\n", metadata->data);
    printf("size: %lu\n", metadata->size);
    printf("rsize: %lu\n", SN_GET_ARR_SIZE(metadata->size, sizeof(int32_t)));
    printf("tid: %lu\n", metadata->tid);

    return 0;
}
EOF

# Check if compilation was successful
if [ $? -ne 0 ]; then
  echo "Compilation failed! Exiting."
  cli_exit 44
fi

# Run the program with Valgrind and log errors if any
echo "Running Valgrind on $OUTPUT_BINARY..."
valgrind --leak-check=full --track-origins=yes ./"$OUTPUT_BINARY" 2>&1 | tee "$LOG_FILE"

if [ $? -ne 0 ]; then
    echo "errors in test code"
fi

# Check if Valgrind reported any errors
if grep -i "ERROR SUMMARY: 0 errors" "$LOG_FILE" > /dev/null; then
  echo "No memory errors detected."
  ecode=0
else
  echo "Memory errors found! Check $LOG_FILE for details."
  ecode=1
fi


rm -f ${OUTPUT_BINARY}
cli_exit $ecode