#include <stdio.h>
#include <cstring>
#include <time.h>
#include <math.h>
#include <map>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
#include <set>
#include <iterator>
#include <unistd.h>
#include "TGraph.h"
#include "TCanvas.h"
#include "TString.h"
#include "TAxis.h"
extern char *optarg;
extern int optind, opterr, optopt;
#include <getopt.h>

static int graph = 0;
static int percentage = 5;
static int percentageError = 0;
static int id = 0;
static int verbose = 0;
int size = 0;

using namespace std;
/**
 * @brief calculate entropy
 *
 * This method calculate the entropy of the block of bytes pointed by ptr.
 *
 * @param ptr pointer to buffer of bytes to analyze
 * @return float entropy of buffer
 */
float calculateEntropy(unsigned char *ptr)
{
    int counts[256];
    float entropy = 0;

    for (int i = 0; i < 256; i++)
    {
        counts[i] = 0;
    }

    for (int i = 0; i < 32; i++)
    {
        counts[(int)ptr[i]]++;
    }

    for (int i = 0; i < 256; i++)
    {
        if (counts[i] != 0)
        {
            float p = (float)(counts[i]) / 32;
            entropy -= (float)p * (log(p) / log(256));
        }
    }

    return entropy;
}
/**
 * @brief print best values
 *
 * print on the .csv file the correct percentage of entropy-index values that  the user wants to visualize
 *
 * @param output name of .csv file
 * @param size percentage of values to show in .csv file
 * @param toSort Map with all the entropy-index values
 */
void bestValues(char output[], int size, multimap<float, int> toSort)
{
    FILE *write_ptr;
    int out = size * percentage / 100;

    if (out == 0)
        out = 1;
    fclose(fopen(output, "w+"));
    write_ptr = fopen(output, "w+");
    auto it = toSort.begin();
    for (int i = 0; i < out; i++)
    {
        fprintf(write_ptr, "%f,%d\n", -(*it).first, ((*it).second));
        it++;
    }
}
/**
 * @brief Corrupted bits with a gaussian probability
 *
 * This method gets the bits to be corrupted by averaging 300 random numbers
 *
 * @param inputFile Memory dump
 * @return char* MEmory dump corrupted
 */
char *decayData(char inputFile[])
{
    if (verbose == 1)
    {
        printf("%s ", "Resetting bit...\n");
    }

    char ch;
    FILE *source = fopen(inputFile, "rb");
    FILE *coursed = fopen("coursed.txt", "wb");
    while ((ch = fgetc(source)) != EOF)
        fputc(ch, coursed);

    fseek(coursed, 0L, SEEK_END);
    long int sizeOfFile = ftell(coursed) * 8;

    fclose(source);
    fclose(coursed);

    int times = sizeOfFile * percentageError / 100;
    set<long int> byteToDecay;
    auto it = byteToDecay.begin();

    srand(time(NULL));
    for (int i = 0; byteToDecay.size() != times; i++)
    {

        int y = 0;
        for (int j = 0; j < 300; ++j)
        {
            y += sizeOfFile * rand() / RAND_MAX;
        }
        byteToDecay.insert(byteToDecay.end(), y / 300);
    }

    it = byteToDecay.begin();
    FILE *coursedW = fopen("coursed.txt", "wb");
    FILE *coursedR = fopen(inputFile, "rb");

    fclose(fopen("coursed.txt", "wb"));
    unsigned char buffer[1];
    long int bit;
    int count = 1;

    bit = *it;
    while (!feof(coursedR))
    {
        memset(buffer, 0, 1);
        fread(buffer, sizeof(char), 1, coursedR);

        if (ferror(coursedR))
        {
            perror("Read error");
            break;
        }

        while ((ceil(bit / 8)) == count)
        {
            buffer[0] = buffer[0] ^ (1 << (int)(count * 8 - bit - 1));
            it++;
            bit = *it;
        }

        fprintf(coursedW, "%c", (char)buffer[0]);
        count++;
    }

    fclose(coursedW);
    fclose(coursedR);

    if (verbose == 1)
    {
        printf("%s ", "Resetting bit...DONE!\n");
    }

    return "coursed.txt";
}
/**
 * @brief Create an entropy graph
 *
 * Create an entropy graph entropy.png with the value of the database using ROOT
 *
 * @param file database to fill the graph
 */
void createGraph(char file[])
{

    printf("%s \n", "Drawing graph...");

    TCanvas c1;

    TGraph *myGraph = new TGraph(file);

    myGraph->SetTitle("Entropy");

    myGraph->GetXaxis()->SetTitle("Index");
    myGraph->GetYaxis()->SetTitle("Entropy");
    myGraph->SetMarkerStyle(24);
    myGraph->SetMarkerSize(0.7);
    myGraph->SetMarkerColor(kBlack);

    myGraph->Draw("AP");

    c1.Print("entropy.png", "png");

    delete myGraph;
    printf("%s \n", "Drawing graph...DONE!");
}
/**
 * @brief Initialize the dump's analysis
 *
 * Given a memory dump as input, it calls functions to obtain .csv file as output with all the entropy values
 *
 * @param in pointer to the input file
 * @param out pointer to the output file
 */
void analyzeDump(char *in, char *out)
{
    clock_t begin = clock();
    if (percentageError != 0)
        in = decayData(in);

    multimap<float, int> M;
    int bufferSize = 32;

    unsigned char buffer[bufferSize];
    FILE *ptr;

    ptr = fopen(in, "rb");
    FILE *write_ptr;
    fclose(fopen("database.txt", "w+"));
    write_ptr = fopen("database.txt", "w+");
    if (ptr == NULL)
    {
        fprintf(stderr, "Invalid input File! \n");
        fprintf(stderr, "Analysis intterupted");
        exit(0);
    }

    if (verbose == 1)
    {
        printf("%s \n", "Calculating entropy...");
    }
    while (!feof(ptr))
    {
        memset(buffer, 0, bufferSize);

        fread(buffer, sizeof(char), bufferSize, ptr);
        if (ferror(ptr))
        {
            perror("Read error");
            break;
        }

        float entropy = calculateEntropy(buffer);
        fprintf(write_ptr, "%d %f\n", id, entropy);
        M.insert({-entropy, id});
        id++;
    }

    if (verbose == 1)
    {
        printf("%s \n", "Calculating entropy...DONE!");
    }

    fclose(write_ptr);
    if (graph == 1)
    {
        createGraph("database.txt");
    }

    bestValues(out, id + 1, M);
    clock_t end = clock();

    double time_spend = (double)(end - begin) / CLOCKS_PER_SEC;

    if (verbose == 1)
    {
        printf("%s \n", "Analysis: successful");
        printf("%s ", "file ");
        printf("%s ", out);
        printf("%s \n", "created ");
        printf("%s ", "time taken:");
        printf("%f\n", time_spend);
    }
}
/**
 * @brief print help section
 *
 */
static void usage()
{
    fprintf(stderr, "Usage: DumpAnalyzer [OPTION]... INPUT-FILE OUTPUT-FILE\n"
                    "Find most important 32-byte block in MEMORY-IMAGE.\n"
                    "\n"
                    "\t-v\t\tverbose output -- prints the progress of \n"
                    "\t\t\tthe running function\n"
                    "\t-g\t\tcreate a file containing the \n"
                    "\t\t\tOFFSET/ENTROPY graph in the folder of \n"
                    "\t\t\tOUTPUT-FILE\n"
                    "\t-e\t\tsets the percentage of bit errors allowed \n"
                    "\t\t\tWe reccomended 1-5% \n"
                    "\t\t\t(default = %d)\n"
                    "\t-p\t\tsets the percentage of the best values\n"
                    "\t\t\t(default = %d)\n"
                    "\t-h\t\tdisplays this help message\n",
            0, 5);
}
/**
 * @brief main method
 *
 * @param argc
 * @param argv
 * @return int
 */
int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "gvhp:e:")) != -1)
    {
        switch (opt)
        {
        case 'g':
            graph = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case 'p':
            percentage = atoi(optarg);
            break;
        case 'e':
            percentageError = atoi(optarg);
            break;
        case 'h':
            usage();
            exit(1);
        }
    }

    if ((argc - optind) != 2)
    {
        fprintf(stderr, "Input file and/or output file arguments missing\n");
        usage();
        exit(1);
    }
    else
        analyzeDump(argv[optind], argv[optind + 1]);
    return 0;
}