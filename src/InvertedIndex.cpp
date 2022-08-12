#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include "InvertedIndex.h"

void InvertedIndex::UpdateDocumentBase(const std::vector<std::string>& inputDocs) {
    _docs = inputDocs;
    _freqDictionary.clear();
    std::mutex freqDictionaryAccess;
    std::vector<std::thread> threads;
    size_t docId = 0;
    for (const auto& doc : _docs) {
        threads.emplace_back(std::thread([this, &doc, &freqDictionaryAccess, docId]() {
            std::map<std::string, size_t> wordsMap;
            std::string currentWord;
            std::istringstream sString {doc};
            while (sString >> currentWord) {
                if (wordsMap.count(currentWord) == 0) { wordsMap[currentWord] = 1; }
                else { wordsMap[currentWord] += 1; }
            }

            freqDictionaryAccess.lock();
            for (const auto& [word, count] : wordsMap) {
                if (_freqDictionary.count(word) == 0) {
                    _freqDictionary.emplace(word, std::vector<Entry>{{docId, count}});
                } else {
                    _freqDictionary[word].emplace_back(Entry{docId, count});
                }
            }
            freqDictionaryAccess.unlock();
        }));
        ++docId;
    }

    for (auto& thread : threads) {
        thread.join();
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    auto it = _freqDictionary.find(word);
    if (it != _freqDictionary.end())
        return it->second;
    else
        return {};
}

