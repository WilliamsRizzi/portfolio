/*
#################################################################################
#									##											#
#			Williams Rizzi			##			Alessandro Bacchiega			#
#			Mat. 	165098			##			Mat. 	      167391			#
#									##											#
#################################################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "client.h"
#include "server.h"

int main(int argc, char** argv) {

    int c, max = -1, win = -1;

    static struct option long_options[] = {
        {"max", 1, 0, 'm'},
        {"win", 1, 0, 'w'},
    };

    int option_index = 0;
    while ((c = getopt_long(argc, argv, "abc:d:012",
            long_options, &option_index)) != -1) {
        switch (c) {
            case 'm':
                max = atoi(optarg);
                break;
            case 'w':
                win = atoi(optarg);
                break;
        }
    }

    if (optind < argc) {
        while (optind < argc) {
            if (strcmp(argv[optind], "client") == 0) {
                optind += 1;
                client();
            } else if (strcmp(argv[optind], "server") == 0) {
                optind += 1;
                server(max, win);
            }
        }
    } else printf("Usage: <executable> <mode> --max <#> --win <#>\n");

    return 0;
}
