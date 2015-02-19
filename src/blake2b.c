/***
  This file is part of libblake2

  Copyright 2015 Mirco Tischler

  libblake2 is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  libblake2 is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with libblake2; If not, see <http://www.gnu.org/licenses/>.
 ***/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include <blake2b.h>

int main(int argc, char** argv) {
    int i, r;
    blake2b *b = blake2b_new();
    if (!b) {
        printf("Could not create blake2b object}n", argv[i]);
        return 1;
    }

    r = blake2b_set_digest_length(b, 64);
    if (r != 0) {
        printf("Could not set digest length\n");
        blake2b_delete(b);
        return 1;
    }

    for (i = 1; i < argc; ++i) {
        int fd;
        struct stat s;
        const char *data;
        uint8_t hash[64];
        char hex[129];

        fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            printf("Could not open file %s: %s\n", argv[i], strerror(errno));
            continue;
        }

        r = fstat(fd, &s);
        if (r != 0) {
            printf("Could not stat file %s: %s\n", argv[i], strerror(errno));
            continue;
        }

        if(!S_ISREG(s.st_mode)){
            printf("File is not a regular file: %s\n", argv[i]);
            continue;
        }
        
        data = mmap(NULL, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if (!data) {
            printf("Could not mmap file %s: %s\n", argv[i], strerror(errno));
            continue;
        }

        r = blake2b_hash(b, data, s.st_size, hash);
        if (r != 0) {
            printf("Could not hash file %s\n", argv[i]);
            continue;
        }

        r = blake2b_hash_to_hex(hash, 64, hex);
        if (r != 0) {
            printf("Could not convert to hex: %s\n", argv[i]);
            continue;
        }

        printf("%s  %s\n", hex, argv[i]);
        close(fd);
    }

    blake2b_delete(b);

    return 0;
}
