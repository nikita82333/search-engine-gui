#ifndef SEARCH_ENGINE_SEARCHSERVER_H
#define SEARCH_ENGINE_SEARCHSERVER_H


struct RelativeIndex {
    size_t docId;
    float rank;
    bool operator ==(const RelativeIndex& other) const {
        return (docId == other.docId && rank == other.rank);
    }
};

class SearchServer {
public:
/**
* @param idx в конструктор класса передаётся ссылка на класс InvertedIndex,
* чтобы SearchServer мог узнать частоту слов встречаемых в запросе
*/
    SearchServer(InvertedIndex& idx) : _index(idx) { };
/**
* Метод обработки поисковых запросов
* @param queriesInput поисковые запросы взятые из файла requests.json
* @return возвращает отсортированный список релевантных ответов для заданных запросов
*/
    std::vector<std::vector<RelativeIndex>> Search(const std::vector<std::string>& queriesInput);

private:
    InvertedIndex _index;
};


#endif //SEARCH_ENGINE_SEARCHSERVER_H
