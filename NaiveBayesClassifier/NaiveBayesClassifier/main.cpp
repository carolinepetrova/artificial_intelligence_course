//
//  main.cpp
//  NaiveBayesClassifier
//
//  Created by Karolina Koleva on 16.12.20.
//

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <limits>
#include <random>

using std::vector;
using std::unordered_map;
using std::string;
using std::fstream;

struct FeaturePoint {
    int _numOfYes;
    int _numOfNo;
    int _numOfUnknown;

    FeaturePoint() {
        _numOfNo = 0;
        _numOfYes = 0;
    }
    
    int getTotal() const {
        return _numOfYes + _numOfNo;
    }
};


struct FeatureVector {
    string className;
    vector<string> featureValues;
    FeatureVector() {}
    FeatureVector(int size) : featureValues(vector<string>(size)) {}
};

class FrequencyTable {
private:
    int numOfRepublicans;
    int numOfDemocrats;
    int totalNum;
    unordered_map<string, vector<FeaturePoint>> _frequencyTable;
public:
    unordered_map<string, vector<FeaturePoint>> & getTable()  {
        return _frequencyTable;
    }
    
    void setNumOfRepublicansAndDemocrats(vector<FeatureVector> data) {
        numOfRepublicans = 0;
        numOfDemocrats = 0;
        totalNum = 0;
        for (FeatureVector featureVector : data) {
            if (featureVector.className == "republican")
                numOfRepublicans++;
            else if(featureVector.className == "democrat")
                numOfDemocrats++;
        }
        totalNum = numOfDemocrats + numOfRepublicans;
    }
    
    int getNumOfRepublicans() const { return numOfRepublicans; }
    int getNumOfDemocrats() const {return numOfDemocrats; }
    int getAll() const { return totalNum;}

    bool containsKey(string key) {
        std::unordered_map<std::string,vector<FeaturePoint>>::const_iterator got = _frequencyTable.find(key);
        return (got == _frequencyTable.end());
    }
};

class NaiveBayesClassifier {
public:
    NaiveBayesClassifier(const char *filename) {
        readFromFile(filename);
    }

    vector<string> getClasses(vector<FeatureVector> data) {
        vector<string> classesName;
        for (FeatureVector featureVector : data) {
            if(std::find(classesName.begin(), classesName.end(), featureVector.className) == classesName.end()) {
                classesName.push_back(featureVector.className);
            }
        }
        return classesName;
    }

    FrequencyTable train(vector<FeatureVector> trainingData) {
        FrequencyTable table;
        table.setNumOfRepublicansAndDemocrats(trainingData);
        // TODO might add to FrequencyTable constructor
        for (string className : getClasses(trainingData)) {
            vector<FeaturePoint> featurePointVector;
            for (int i = 0; i < 16; i++) {
                featurePointVector.push_back(FeaturePoint());
            }
            table.getTable()[className] = featurePointVector;
        }
        for(FeatureVector featureVector : trainingData) {
            vector<string> featureValues = featureVector.featureValues;
            for(int i = 0 ; i < featureValues.size(); i++) {
                if (featureValues[i] == "y") {
                    table.getTable()[featureVector.className][i]._numOfYes++;
                }
                else if (featureValues[i] == "n") {
                    table.getTable()[featureVector.className][i]._numOfNo++;
                }
            }
        }
        return table;
    }
    // on the generated frequency table do the Bayes calculations on the test data
    double test(FrequencyTable frequencyTable, vector<FeatureVector> testingData) {
        double correctCount = 0.0;
                for (FeatureVector featureVector : testingData) {
                    string guess = findBestProbability(frequencyTable, featureVector.featureValues);
                    if (featureVector.className == guess) {
                        correctCount++;
                    }
                }

        return correctCount / testingData.size();
    }

    void run() {
        unsigned seed = 0;
        shuffle(_dataset.begin(), _dataset.end(),std::default_random_engine(seed));

        int sizeOfSubset = _dataset.size() / 10;
        double average = 0.0;
        int tries = 0;
        for (int i = 0; i < sizeOfSubset * 10; i += sizeOfSubset) {
            int startIdx = i;
            int endIdx = (i + sizeOfSubset >= _dataset.size()) ? _dataset.size() - 1 : i + sizeOfSubset;

            vector<FeatureVector> testingData(_dataset.begin() + startIdx, _dataset.begin() + endIdx);
            vector<FeatureVector> trainingData(_dataset.begin(), _dataset.begin() + startIdx);

            trainingData.insert(trainingData.end(), _dataset.begin() + endIdx, _dataset.end());

            FrequencyTable table = train(trainingData);
            double percentage = test(table, testingData);
            
            std::cout << "Acuraccy on try " << tries++ << " " << 100 * percentage << " %" << std::endl;
            average += percentage;
        }
        std::cout << "Average " << 100*average/10  << " %" << std::endl;
    }
private:
    vector<FeatureVector> _dataset;

    vector<string> splitString(string str) {
        std::istringstream ss(str);
        std::string token;

        vector<string> row;
        while(std::getline(ss, token, ',')) {
            row.push_back(token);
        }
        return row;
    }
    
    bool checkIfContainsUnknown(vector<string> vect) {
        for (int i = 0; i < vect.size(); i++) {
            if (vect[i] == "?")
                return true;
        }
        return false;
    }

    void readFromFile(string filename) {
        std::ifstream file(filename);
        
        vector<FeatureVector> fileData;
        if (file.is_open()) {
            while(true) {
                if (file.eof()) { break; }
                string line;
                file >> line;
                if (line == "") continue;
                vector<string> newLine = splitString(line);
                FeatureVector featureVector;
                featureVector.className = newLine[0];
                featureVector.featureValues = vector<string>(newLine.begin() + 1, newLine.end());
                if (!checkIfContainsUnknown(featureVector.featureValues))
                    _dataset.push_back(featureVector);
            }
            file.close();
        }
    }
    double BayesCalculation(double probability, vector<FeaturePoint> FeaturePoints, vector<string> featureValues) {
                for (int i = 0; i < featureValues.size(); i++) {
                    string featureValue = featureValues[i];
                    FeaturePoint point = FeaturePoints[i];
                    if (featureValue == "y")
                        probability += log((double)(1 + point._numOfYes) / (point.getTotal() + 2));
                    else if (featureValue == "n")
                        probability += log((double)(1+ point._numOfNo) / (point.getTotal() + 2));
                    else if (featureValue == "?") {
                        probability *= log((double)(1+ point._numOfUnknown) / (point.getTotal() + 2));
                    }
                }
        return probability;
    }
        string findBestProbability(FrequencyTable table, vector<string> featureValues) {
            double bestResult = std::numeric_limits<double>::lowest();
            string bestClass = "";
                for (auto const& elem : table.getTable()) {
                    double probability = (elem.first == "democrat") ? table.getNumOfDemocrats() :
                    table.getNumOfRepublicans();
                    probability = (probability + 1) / (table.getAll() + 2);
                    double bayes = BayesCalculation(probability, elem.second, featureValues);
                    if (bestResult < bayes) {
                        bestResult = bayes;
                        bestClass = elem.first;
                    }
                }

            return bestClass;
        }

};

int main() {
    std::cout << "Hello, World!" << std::endl;
    NaiveBayesClassifier naiveBayesClassifier = NaiveBayesClassifier("/Users/kolevak/Desktop/artificial_intelligence_course/NaiveBayesClassifier/NaiveBayesClassifier/house-votes-84.data");
    naiveBayesClassifier.run();
    return 0;
}
