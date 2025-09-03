// cTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <errno.h>
#include<string.h>
#include<time.h>
#define MaxNameLength 100
#define MaxNumberOfStations 10000

struct WeatherStation
{
    char stationName[MaxNameLength];
    double maxValue;
    double minValue;
    double sum;
    int numberOfObservations;
};

void UpdateStationInfo(WeatherStation* a, double nextValue)
{
    if (a->numberOfObservations == 0)
    {
        a->sum = nextValue;
        a->minValue = nextValue;
        a->maxValue = nextValue;
    }
    else
    {
        a->sum += nextValue;
    }
    a->numberOfObservations++;
    return;
}
bool CompareStationName(WeatherStation a, char* b)
{
    for (int i = 0;i < MaxNameLength;i++)
    {
        if (a.stationName[i] != b[i])
        {
            return false;
        }
        if (a.stationName[i] == 0)
        {
            break;
        }
    }
    return true;
}

enum ReadLineStatus{error, success,endOfFile};
ReadLineStatus ReadLine(FILE* inputFile,char* stationName,double* stationValue)
{
    int nextChar;
    for (int i = 0;i < MaxNameLength;i++)
    {
        nextChar = fgetc(inputFile);
        if (i == 0 && nextChar == EOF)
        {
            return endOfFile;
        }
        if (nextChar == 59)
        {
            stationName[i] = 0;
            break;
        }
        if (nextChar == '\r' || nextChar == '\n' || nextChar == EOF)
        {
            return error;
        }
        stationName[i] = (char)nextChar;
    }
    double integerPart = 0;
    for (int i = 0;i < MaxNameLength;i++)
    {
        nextChar = fgetc(inputFile);
        if (nextChar == '.')
        {
            break;
        }
        if (nextChar == '\r')
        {
            fgetc(inputFile);
            break;
        }
        if (nextChar == '\n')
        {
            break;
        }
        if (nextChar == EOF)
        {
            break;
        }
        double nextDigit = (double)(nextChar - '0');
        integerPart = 10 * integerPart + nextDigit;
    }
    if (nextChar == '\n' || nextChar == EOF)
    {
        stationValue[0] = integerPart;
        return success;
    }
    double powerOfTen = 1.0 / 10.0;
    double fractionalPart = 0;
    for (int i = 0;i < MaxNameLength;i++)
    {
        int nextChar = fgetc(inputFile);
        if (nextChar == '.')
        {
            break;
        }
        if (nextChar == '\r')
        {
            break;
        }
        if (nextChar == '\n')
        {
            break;
        }
        if (nextChar == EOF)
        {
            break;
        }
        double nextDigit = (double)(nextChar - '0');
        fractionalPart += powerOfTen * nextDigit;
        powerOfTen /= 10;
    }
    stationValue[0] = integerPart + fractionalPart;
    return success;
}

int main()
{
    FILE* myFile;
    errno_t err = fopen_s(&myFile, "D:\\Personal\\Public Repos\\BillionRows\\cTest\\x64\\Debug\\measurements.txt", "r");
    if (myFile == NULL)
    {
        char errorMessageBuffer[100];
        strerror_s(errorMessageBuffer,100,err);
        printf(errorMessageBuffer);
        return -1;
    }
    time_t startTime = time(NULL);
    int counter = 0;
    WeatherStation* stations = (WeatherStation*)calloc(MaxNumberOfStations,sizeof(WeatherStation));
    while (true)
    {
        char nextStationName[MaxNameLength];
        double nextValue;
        ReadLineStatus status = ReadLine(myFile, nextStationName, &nextValue);
        for (int i = 0;i < MaxNumberOfStations;i++)
        {
            if (CompareStationName(stations[i], nextStationName))
            {
                UpdateStationInfo(&stations[i], nextValue);
                break;
            }
            if (stations[i].numberOfObservations == 0)
            {
                for (int j = 0;j < MaxNameLength;j++)
                {
                    stations[i].stationName[j] = nextStationName[j];
                }
                UpdateStationInfo(&stations[i], nextValue);
                break;
            }
        }
        if (status == success)
        {
            if (counter % 10000000 == 0)
            {
                printf("Current station:\n");
                printf(nextStationName);
                printf("   ");
                printf("%f", nextValue);
                printf("\n");
            }
        }
        if (status == endOfFile)
        {
            printf("Last station:\n");
            printf(nextStationName);
            printf("   ");
            printf("%f", nextValue);
            printf("\n");
            break;
        }
        if (status == error)
        {
            printf("Error!\n");
            printf(nextStationName);
            printf("\n");
            break;
        }
        counter++;
    }
    time_t endTime = time(NULL);
    for (int i = 0;i < MaxNumberOfStations;i++)
    {
        if (stations[i].numberOfObservations == 0)
        {
            break;
        }
        printf(stations[i].stationName);
        printf("   ");
        printf("Min: ");
        printf("%f", stations[i].minValue);
        printf("   ");
        printf("Max: ");
        printf("%f", stations[i].maxValue);
        printf("   ");
        printf("Average: ");
        printf("%f", stations[i].sum/(double)stations[i].numberOfObservations);
        printf("\n");
    }
    printf("Total elapsed time was %f seconds\n", difftime(endTime,startTime));
    free(stations);
    fclose(myFile);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
