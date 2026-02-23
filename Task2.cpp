#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <pthread.h>
using namespace std;


// Structure used to pass data to each thread
struct ThreadData {
    vector<string>* words;
    int start;
    int end;
    map<string,int>* localMap;
};

// Thread function, counts words in its assigned range
void* countWords(void* arg) {

    ThreadData* data = (ThreadData*) arg;

    for (int i = data->start; i < data->end; i++) {
        string w = (*(data->words))[i];
        (*(data->localMap))[w]++;
    }

    pthread_exit(NULL);
}

int main() {

    // Open input file and check if it is successfuk
    ifstream file("words.txt");

    if (!file) {
        cout << "File not found." << endl;
        return 1;
    }

    // Read all words from file and store into vector
    vector<string> words;
    string word;

    while (file >> word) {
        words.push_back(word);
    }

    file.close();

    // Get number of threads from the user
    int N;
    cout << "Enter the number of threads: ";
    cin >> N;

    // Create thread array, thread data array, and local maps (based on previous N)
    pthread_t threads[N];
    ThreadData threadData[N];
    map<string,int> localMaps[N];

    // Calculate total words and divide per thread, give word ranges and create threads
    int totalWords = words.size();
    int partSize = totalWords / N;

    for (int i = 0; i < N; i++) {

        int start = i * partSize;
        int end;

        if (i == N - 1)
            end = totalWords;
        else
            end = start + partSize;

        threadData[i].words = &words;
        threadData[i].start = start;
        threadData[i].end = end;
        threadData[i].localMap = &localMaps[i];

        pthread_create(&threads[i], NULL, countWords, &threadData[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print intermediate results from each thread
    for (int i = 0; i < N; i++) {
        cout << "\nIntermediate result from thread " << i+1 << ":\n";
        for (map<string,int>::iterator it = localMaps[i].begin(); it != localMaps[i].end(); it++) {
            cout << it->first << " : " << it->second << endl;
        }
    }

    // Merging all the local maps into final result
    map<string,int> finalMap;

    for (int i = 0; i < N; i++) {
        for (map<string,int>::iterator it = localMaps[i].begin(); it != localMaps[i].end(); it++) {
            finalMap[it->first] += it->second;
        }
    }

    // Print final words count
    cout << "\nFinal Words Count:\n";

    for (map<string,int>::iterator it = finalMap.begin(); it != finalMap.end(); it++) {
        cout << it->first << " : " << it->second << endl;
    }

    return 0;
}