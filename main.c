// This code compiled with gnu17
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define THREAD_COUNT 32

#define BOOK_COUNT 10
#define WORD_LENGTH 50
#define BILLION 1E9
#define STOP_COUNT 851          // # of words in english stop words
#define MAX_WORD_NUM 300000     // max # of words in any book

char stopwords[STOP_COUNT][WORD_LENGTH];
char books[BOOK_COUNT][MAX_WORD_NUM][WORD_LENGTH];
int ocurrences[BOOK_COUNT][STOP_COUNT];

int main()
{
    // parsing "englishstopwords.txt"
    FILE* f_ptr;
    f_ptr = fopen("englishstopwords.txt", "r");
    if(f_ptr == NULL)
    {
        printf("File does not exist \n");
        exit(1);
    }
    char word[WORD_LENGTH];     // string in the book
    int count = 0;              // counter helps to index
    // parsing strings by spaces and line breaks
    while(fscanf(f_ptr, "%s", word) != EOF)
    {
        strcpy(stopwords[count], word);
        count++;
    }
    fclose(f_ptr);
    printf("\"englishstopwords.txt\" parsed \n");
    printf("\n");

    // parse books
    int total_words[10];        // actual word count in the books
    char book_name[10] = "";
    // loop for parsing all books
    for(int i = 0; i < BOOK_COUNT; i++)
    {
        sprintf(book_name, "book%d.txt", i+1);    // format bookname for file IO
        f_ptr = fopen(book_name, "r");
        if(f_ptr == NULL)
        {
            printf("File does not exist \n");
            exit(1);
        }
        total_words[i] = 0;         // initialize word count of the book
        // parsing strings by spaces and line breaks
        while(fscanf(f_ptr, "%s", word) != EOF)
        {
            strcpy(books[i][total_words[i]], word);
            total_words[i]++;
        }
        fclose(f_ptr);
        printf("\"book%d.txt\" parsed\n", i+1);
    }
    printf("\n");

    /* thread function for counting words
    takes the pointer of the thread number as an argument (0, 1, 2, .. etc.)
    each thread processes one book
    returns nothing
    */
    void* count_matches(void* args)
    {
        int thread_number = *((int*) args);
        free(args);     // to prevent memory leaks
        // loop for books
        for(int j = 0; j < BOOK_COUNT; j++)    // j: book index
        {
            /* each thread processes portion amount in each books */
            int portion = total_words[j] / THREAD_COUNT;
            int head = thread_number * portion;             // start index
            int tail = (thread_number+1) * portion;         // end index
            // loop for stopwords
            for(int k = 0; k < STOP_COUNT; k++)     // k: stop words index
            {
                /* loop for the comparison
                l: index of the compared word in the book */
                for(int l = head; l < tail; l++)
                {
                    if(strcmp(stopwords[k], books[j][l]) == 0) // if words equal
                    {
                        ocurrences[j][k]++;
                    }
                }
            }
        }
        pthread_exit(NULL);
    }

    pthread_t threads[THREAD_COUNT];    // declare threads

    struct timespec requestStart, requestEnd;
	clock_gettime(CLOCK_REALTIME, &requestStart);      // start timer

    int *thread_number;
    // loop for creating threads
    printf("STATUS : Creating %d threads\n", THREAD_COUNT);
    printf("\n");
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        thread_number = malloc(sizeof(int));
        *thread_number = i;
        pthread_create(&threads[i], NULL, count_matches, thread_number);
    }
    // join all threads
    for(int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_REALTIME, &requestEnd);     // end timer

    // calculate execution time of the threads
    double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;
	printf("Total time spent %lf, for %d threads\n", accum, THREAD_COUNT);
    printf("\n");
    // loop for printing frequencies
    for(int i = 0; i < 3; i++)
    {
        // frequency of the 1st 11th end 21st word
        printf("frequency of the word %s in all books:\n", stopwords[i*10]);
        // loop for iterating books
        for(int j = 0; j < 10; j++)
        {
            printf("%d-", ocurrences[j][i*10]);
        }
        printf("\n");
        printf("\n");
    }
    printf("Program finished successfully!\n");
    exit(0);
}
