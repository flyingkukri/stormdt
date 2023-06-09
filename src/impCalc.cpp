#include "impCalc.h"
#include <cstdlib>
#include <random>
#include <storm/api/storm.h>
#include <storm-parsers/parser/PrismParser.h>
#include <storm/storage/prism/Program.h>
#include <storm/storage/sparse/PrismChoiceOrigins.h>
#include <storm/modelchecker/results/CheckResult.h>
#include <storm/utility/initialize.h>
#include <storm-parsers/parser/FormulaParser.h>
#include <storm/models/sparse/Mdp.h>
#include <storm/models/sparse/StandardRewardModel.h>

int simulateRun(storm::simulator::DiscreteTimeSparseModelSimulator<double> simulator,
                storm::models::sparse::Mdp<double, storm::models::sparse::StandardRewardModel<double>> model, std::vector<int>& visited, int l, storm::storage::BitVector finalStates) {
    uint64_t seed = random();
    simulator.setSeed(seed);
    int state = simulator.getCurrentState();
    for (int i = 0; i < l; i++) {
        if(finalStates.get(state)) {
            return 1; // We assume that final states are sink states
        }
        if(model.isSinkState(state)) {
            break;
        }
        state = simulator.getCurrentState();
        visited[state] = 1;
        simulator.randomStep();
    }
    simulator.resetToInitial();
    return 0;
}

std::vector<int> calculateImps(storm::models::sparse::Mdp<double, storm::models::sparse::StandardRewardModel<double>> model, int l, int C, const std::string& label) {
    storm::simulator::DiscreteTimeSparseModelSimulator<double> simulator(model);
    int nstates = model.getNumberOfStates();
    std::vector<int> imps(nstates, 0);
    std::vector<int> visited(nstates, 0);
    std::cout << "nstates: " << nstates << std::endl;
    storm::storage::BitVector finalStates = model.getStates(label);

    for (int i = 0; i < C; i++) {
        // simulateRun returns 1 if we are in a final state
        if(simulateRun(simulator, model, visited, l, finalStates)){
            for (int j = 0; j < nstates; j++) {
                assert(visited[j] == 0 || visited[j] == 1);
                imps[j] += visited[j];
                visited[j] = 0;
            }
        }
    }

    return imps;
}