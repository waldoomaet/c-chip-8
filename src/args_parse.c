#include "args_parse.h"

int parseArgs(int argc, char **argv, Args *args)
{
    args->romPath = NULL;
    args->debug = 0;
    args->cpuSteps = 700;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            args->debug = 1;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "Missing value after -s\n");
                return 0;
            }

            args->cpuSteps = atoi(argv[++i]);
        }
        else if (args->romPath == NULL)
        {
            args->romPath = argv[i];
        }
        else
        {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return 0;
        }
    }

    if (args->romPath == NULL)
    {
        fprintf(stderr, "ROM path required\n");
        return 0;
    }

    return 1;
}