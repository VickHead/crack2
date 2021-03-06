#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "md5.h"

const int PASS_LEN=20;        // Maximum any password will be
const int HASH_LEN=33;        // Length of MD5 hash strings

// Given a hash and a plaintext guess, return 1 if
// the hash of the guess matches the given hash.
// That is, return 1 if the guess is correct.
int tryguess(char *hash, char *guess)
{
    // Hash the guess using MD5
    char *g = md5(guess, strlen(guess));
    
    // Compare the two hashes
    if (strcmp(hash, g) == 0)
    {
        // Free any malloc'd memory
        free(g);
        return 1;
    }
    // Free any malloc'd memory
    free(g);
    return 0;
}

// Read in the dictionary file and return the array of strings
// and store the length of the array in size.
// This function is responsible for opening the dictionary file,
// reading from it, building the data structure, and closing the
// file.
char **read_dictionary(char *filename, int *size)
{
    int len = 0;
    struct stat info;
    if (stat(filename, &info) != -1)
    {
        len = info.st_size;
    }
    
    char *contents = malloc(len);
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        printf("can't open %s for reading\n", filename);
        exit(1);
    }
    fread(contents, 1, len, f);
    fclose(f);
    
    int count = 0;
    for (int i = 0; i < len; i++)
    {
        if (contents[i] == '\n')
        {
            count++;
            contents[i] = '\0';
        }
    }
    
    char **arr = malloc(len * sizeof(char *));
    arr[0] = contents;
    int j = 1;
    for (int i = 0; i < len - 1; i++)
    {
        if (contents[i] == '\0')
        {
            arr[j] = &contents[i+1];
            j++;
        }
    }
    
    *size = count;
    return arr;
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }

    // Read the dictionary file into an array of strings.
    int dlen;
    char **dict = read_dictionary(argv[2], &dlen);

    // Open the hash file for reading.
    FILE *h = fopen(argv[1], "r");
    if (!h)
    {
        printf("Couldn't open %s for reading\n", argv[1]);
        exit(1);
    }

    // For each hash, try every entry in the dictionary.
    // Print the matching dictionary entry.
    // Need two nested loops.
    char line[HASH_LEN];
    while (fgets(line, HASH_LEN, h) != NULL)
    {
        for (int i = 0; i < dlen; i++)
        {
            if (tryguess(line, dict[i]) == 1)
            {
                printf("%s ", line);
                puts(dict[i]);
                break;
            }
        }
    }
    
    free(dict[0]);
    free(dict);
    fclose(h);
}