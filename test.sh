#!/bin/bash

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

# Set the source file and output binary names
OUTPUT_BINARY="temp_test_temp"  # Name of the compiled binary
LOG_FILE="valgrind_output.log" # Log file for Valgrind output

# Compile the source code
echo "Compiling test code"
gcc -g -std=gnu99 -O0 -L./build -lsafetynet -o "$OUTPUT_BINARY" -x c - <<EOF
#include <stdio.h>
#include <stdlib.h>
#include <libsafetynet.h>


#include <stddef.h>
#include <stdint.h>

int main()
{
    int32_t* buff = sn_malloc(sizeof(int32_t) * 10);

    if (!buff)
    {
        printf("ERROR: %s", sn_get_error_msg(sn_get_last_error()));
        exit(1);
    }

    size_t buff_size = SN_GET_ARR_SIZE(sn_query_size(buff), sizeof(int32_t));

    printf("Allocated buffer of %lu\n", sn_query_size(buff));

    for (size_t i = 0; i < buff_size; i++)
    {
        buff[i] = rand();
    }

    for (size_t i = 0; i < buff_size; i++)
    {
        printf("buff[%lu] = %i\n", i, buff[i]);
    }

    return 0;
}
EOF

# Check if compilation was successful
if [ $? -ne 0 ]; then
  echo "Compilation failed! Exiting."
  exit 44
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
exit $ecode