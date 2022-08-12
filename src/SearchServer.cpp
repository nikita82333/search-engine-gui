#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

std::vector<std::vector<RelativeIndex>> SearchServer::Search(const std::vector<std::string> &queriesInput) {
    ConverterJSON converterJson;
    size_t responsesLimit = converterJson.GetResponsesLimit();
    std::vector<std::vector<RelativeIndex>> relativeIndex;
    for (const auto& request : queriesInput) {
        std::string currentWord;
        std::unordered_set<std::string> wordSet;
        std::istringstream sString {request};
        while (sString >> currentWord) { //split text to words
            wordSet.insert(currentWord);
        }
        std::map<size_t, std::vector<std::pair<std::string, size_t>>> docWordsCount; //doc -> {word, count}
        for (const auto& word : wordSet) {
            for (const auto& [docId, count] : _index.GetWordCount(word)) {
                if (count != 0) {
                    if (docWordsCount.count(docId) == 0) {
                        docWordsCount[docId] = std::vector{std::pair{word, count}};
                    } else {
                        docWordsCount[docId].emplace_back(std::pair{word, count});
                    }
                }
            }
        }
        std::vector<RelativeIndex> currentRelativeIndex;
        size_t maxRelevance = 0;
        for (const auto& [docId, wordsCount] : docWordsCount) {
            size_t relevance = 0;
            for (const auto& [word, count] : wordsCount) {
                relevance += count;
            }
            if (relevance > maxRelevance) maxRelevance = relevance;
            currentRelativeIndex.emplace_back(RelativeIndex{docId, static_cast<float>(relevance)});
        }
        for (auto& [docId, rank] : currentRelativeIndex) {
            rank /= static_cast<float>(maxRelevance);
        }
        std::stable_sort(currentRelativeIndex.begin(), currentRelativeIndex.end(),
                         [](const RelativeIndex& a, const RelativeIndex& b) {
                             return b.rank < a.rank;
                         });

        if (currentRelativeIndex.size() > responsesLimit) currentRelativeIndex.resize(responsesLimit);
        relativeIndex.emplace_back(currentRelativeIndex);
    }

    return relativeIndex;
}
