#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <limits>
#include <random>

int COORD_SYSTEM_LIMITS = 500;
// 1% mutations rate
double MUTATION_RATE = 0.01;

double ELITISM_COEFFICIENT = 0.4;


struct City {
    int posX;
    int posY;

    City() {
        posX = posY = 0;
    }

    City(int posX, int posY) {
        this->posY = posY;
        this->posX = posX;
    }

    double Distance(City other) const {
        int DistanceX = std::abs(posX - other.posX);
        int DistanceY = std::abs(posY - other.posY);
        return std::sqrt(DistanceX*DistanceX + DistanceY*DistanceY);
    }

    bool operator!=(const City & city2) {
        return ((this->posX != city2.posX) || (this->posY != city2.posY));
    }

    bool operator==(const City & city2) {
        return !(*this != city2);
    }
};

struct Route {

    std::vector<City> vectorOfCities;
    // used for roulette wheel selection
    double probability;
    double fitness;

    Route(std::vector<City> _vectorOfCities) {
        vectorOfCities = _vectorOfCities;
        fitness = pathDistance();
    }

    bool operator==(const Route & route2) {
        for(int i = 0; i < vectorOfCities.size(); i++) {
            if (this->vectorOfCities[i] != route2.vectorOfCities[i])
                return false;
        }
        return true;
    }

    void print() const {
        for(int i = 0; i < vectorOfCities.size(); i++) {
            printf("(%d,%d) ", vectorOfCities[i].posX,  vectorOfCities[i].posY);
        }
        printf(" - fitness %f\n", fitness);
    }

    void updateFitness() {
        fitness = pathDistance();
    }

private:

    double pathDistance() {
        double pathLength = 0;
        for(int i = 0; i < vectorOfCities.size(); i++) {
            City fromCity = vectorOfCities[i];
            City toCity = (i+1 >= vectorOfCities.size()) ? vectorOfCities[0] : vectorOfCities[i+1];
            pathLength += fromCity.Distance(toCity);
        }
        return pathLength;
    }
};

using Generation = std::vector<Route>;

class TravellingSalesman {
private:
    std::vector<City> cities;
    int numOfGenerations;
    int numOfCities;
    int populationCount;

    Route tournamentSelection(const Generation & currentGeneration, int contestants) {
        Route winner = currentGeneration[generateRandomNumber(currentGeneration.size())];
        for(int i = 1; i < contestants; i++) {
            Route currentPick = currentGeneration[generateRandomNumber(currentGeneration.size())];
            if (winner.fitness > currentPick.fitness)
                winner = currentPick;

        }
        return winner;
    }

    int generateRandomNumber(int size) {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> distrib(1, (size-1));
        return distrib(gen);
    }

public:

    TravellingSalesman(int _numOfCities) {
        numOfCities = _numOfCities;
        numOfGenerations = 100;
        populationCount = 200;
    }

    void generateCities() {
        for(int i = 0; i < numOfCities; i++) {
            int randomPosX = generateRandomNumber((COORD_SYSTEM_LIMITS));
            int randomPosY = generateRandomNumber((COORD_SYSTEM_LIMITS));
            cities.push_back(City(randomPosX,randomPosY));
        }
    }

    Generation generateInitialGeneration() {
        std::random_device rd;
        std::mt19937 g(rd());
        Generation generation;
        // let the initial parent be the vector of generated cities
        generation.push_back(Route(cities));

        for(int i = 0; i < populationCount; i++) {
            std::vector<City> newMember = generation[i].vectorOfCities;

            shuffle(std::begin(newMember) + 1, std::end(newMember),g);
            generation.push_back(Route(newMember));
        }
        return generation;
    }

    Generation getElite(Generation generation) {
        sortGeneration(generation);
        return std::vector<Route>
                (generation.begin(), generation.begin() + (int)(populationCount* ELITISM_COEFFICIENT));
    }

    Generation matingPool(Generation generation) {
        Generation matingPool;

    }

    void sortGeneration(Generation & generation) {
        std::sort(generation.begin(), generation.end(),[](const Route & a, const Route & b) -> bool
        {
            return a.fitness < b.fitness;
        });
    }

    Generation createNextGeneration(Generation currentGeneration) {
        Generation matingPool = selectByRouletteWheel(currentGeneration);
        // elitism
        sortGeneration(currentGeneration);
        Generation nextGeneration = std::vector<Route> (currentGeneration.begin(), currentGeneration.begin() + 20);
        for(int i = 20; i < currentGeneration.size()-1; i++) {

           nextGeneration.push_back(crossover2(matingPool[i], matingPool[i+1],matingPool));
        }

        mutate(nextGeneration);
        return nextGeneration;
    }

    Route crossover2(Route parent1, Route parent2, Generation & nextGeneration) {
        // Create new child tour
        std::vector<City> childRoute(parent1.vectorOfCities.size(), City());

        // Get start and end sub tour positions for parent1's tour
        int startPos = generateRandomNumber(parent1.vectorOfCities.size());
        int endPos = generateRandomNumber(parent1.vectorOfCities.size());

        // Loop and add the sub tour from parent1 to our child
        for (int i = 0; i < childRoute.size(); i++) {
            // If our start position is less than the end position
            if (startPos < endPos && i > startPos && i < endPos) {
                childRoute[i] = parent1.vectorOfCities[i];
            } // If our start position is larger
            else if (startPos > endPos) {
                if (!(i < startPos && i > endPos)) {
                    childRoute[i] =parent1.vectorOfCities[i];
                }
            }
        }

        // Loop through parent2's city tour
        for (int i = 0; i < parent2.vectorOfCities.size(); i++) {
            // If child doesn't have the city add it
            if (!std::any_of(childRoute.begin(), childRoute.end(),
                             [parent2,i](City elem){return elem == parent2.vectorOfCities[i];})) {
                // Loop to find a spare position in the child's tour
                for (int j = 0; j < childRoute.size(); j++) {
                    // Spare position found, add city
                    if (childRoute[j].posX == 0 &&  childRoute[j].posY == 0) {
                        childRoute[j] = parent2.vectorOfCities[i];
                        break;
                    }
                }
            }
        }
        Route child = Route(childRoute);
        return child;
    }

    // one point crossover
    void crossover(Route parent1, Route parent2, Generation & nextGeneration) {
        int point = generateRandomNumber(parent1.vectorOfCities.size());
        std::vector<City> childRoute1;
        std::vector<City> childRoute2;
        childRoute1 = std::vector<City>
                (parent1.vectorOfCities.begin(), parent1.vectorOfCities.begin() + point);
        childRoute2 = std::vector<City>
                (parent2.vectorOfCities.begin(), parent2.vectorOfCities.begin() + point);

        for(int i = 0; i < parent2.vectorOfCities.size(); i++) {
            // copying second parent genes to child1
            if(!std::any_of(childRoute1.begin(), childRoute1.end(),
                            [parent2,i](City elem){return elem == parent2.vectorOfCities[i];}) &&
                    childRoute1.size() < parent2.vectorOfCities.size()) {
                childRoute1.push_back(parent2.vectorOfCities[i]);
            }

            // copying first parent genes to child2
            if(!std::any_of(childRoute2.begin(),childRoute2.end(),
                            [parent1,i](City elem){return elem == parent1.vectorOfCities[i];}) &&
                    childRoute2.size() < parent1.vectorOfCities.size()) {
                childRoute2.push_back(parent1.vectorOfCities[i]);
            }
        }
        Route child1 = Route(childRoute1);
        Route child2 = Route(childRoute2);
        nextGeneration.push_back(child1);
        nextGeneration.push_back(child2);

    }

    // swap mutation
    void mutate(Generation & generation) {
        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> distrib(0, 1);
        for(int offspring = 0; offspring < generation.size();offspring++) {
            double probability = distrib(gen);
            if (probability < MUTATION_RATE) {
                int gene1 = generateRandomNumber(generation[offspring].vectorOfCities.size());
                int gene2 = generateRandomNumber(generation[offspring].vectorOfCities.size());
                std::swap(generation[offspring].vectorOfCities[gene1], generation[offspring].vectorOfCities[gene2]);
                generation[offspring].updateFitness();
            }
        }
    }

    Route getFittest(const Generation & generation) {
        Route winner = generation[0];
        for(int i = 1; i < generation.size(); i++) {
            if (winner.fitness > generation[i].fitness)
                winner = generation[i];

        }
        return winner;
    }

    void averageFitness(const Generation & generation) {
        double avg = 0;
        for(int i = 0; i < generation.size(); i++) {
            avg += generation[i].fitness;

        }
        printf("Average fitness - %f\n ", avg/generation.size());
    }

    // Find path by using Steady State Genetic Algorithm.
    // We use one generation and eliminate the weakest of the population by replacing him
    // with an offspring of parents which are selected by tournament selection.
    void findPathSteadyState() {
        Generation initialGeneration = generateInitialGeneration();
        sortGeneration(initialGeneration);
        printf("Initial Generation best: ");
        initialGeneration[0].print();
        int numOfIndividualsToShow = (numOfGenerations - 2) / 3;
        for(int i = 0; i < numOfGenerations; i++) {
            Route parent1 = tournamentSelection(initialGeneration, 3);
            Route parent2 = tournamentSelection(initialGeneration, 3);

            while(parent1 == parent2) {
                parent2 = tournamentSelection(initialGeneration, 3);
            }
            Generation nextGeneration;
            nextGeneration.push_back(crossover2(parent1, parent2, nextGeneration));

            mutate(nextGeneration);

            initialGeneration[initialGeneration.size()-1] = nextGeneration[0];
            sortGeneration(initialGeneration);

            if(i == numOfIndividualsToShow || i == numOfIndividualsToShow * 2 || i == (numOfIndividualsToShow * 3 - 50)) {
                printf("%d Generation best: ", i+2);
                initialGeneration[0].print();
            }
        }
        printf("Final eneration best: ");
        initialGeneration[0].print();
    }

    Route rouletteWheel(Generation generation) {
        double totalSum = 0;
        std::vector<City> candidate;

        for (int i=0; i< generation.size(); i++){
            totalSum += generation[i].fitness;
        }

        for (int i=0; i< generation.size(); i++) {
            generation[i].probability = generation[i].fitness/totalSum;
        }

        double partialSum = 0;

        std::random_device rd;  //Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
        std::uniform_real_distribution<> distrib(0, 1);
        double roulette = distrib(gen);

        for(int i=0; i< generation.size(); i++) {
            partialSum+= generation[i].probability;

            if(partialSum>=roulette)
            {
                candidate = generation[i].vectorOfCities;
                break;
            }
        }

        return Route(candidate);
    }

    Generation selectByRouletteWheel(Generation generation) {
        Generation nextGeneration;
        for(int i = 0; i < generation.size();i++) {
            nextGeneration.push_back(rouletteWheel(generation));
        }
        return nextGeneration;
    }
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    TravellingSalesman travellingSalesman = TravellingSalesman(25);
    travellingSalesman.generateCities();
    travellingSalesman.findPathSteadyState();
    return 0;
}
