#include "RecursionToTheRescue.h"
#include "map.h"
#include <climits>
#include <iostream>
#include "Disasters.h"
using namespace std;

/* * * * Doctors Without Orders * * * */
bool allocatePatientsToDoctors(Vector<Doctor>& doctors,
                               Vector<Patient>& patients,
                               Map<string, Set<string>>& schedule,
                               int patientsIndex) {
    if (patientsIndex == patients.size()) {
        return true;
    }
    for (Doctor &d : doctors) {
        if (d.hoursFree >= patients[patientsIndex].hoursNeeded) {
            d.hoursFree -= patients[patientsIndex].hoursNeeded;
            schedule[d.name].add(patients[patientsIndex].name);
            if (allocatePatientsToDoctors(doctors, patients, schedule, patientsIndex + 1))
                return true;
            schedule[d.name].remove(patients[patientsIndex].name);
            d.hoursFree += patients[patientsIndex].hoursNeeded;
        }
    }
    return false;
}
/**
 * Given a list of doctors and a list of patients, determines whether all the patients can
 * be seen. If so, this function fills in the schedule outparameter with a map from doctors
 * to the set of patients that doctor would see.
 *
 * @param doctors  The list of the doctors available to work.
 * @param patients The list of the patients that need to be seen.
 * @param schedule An outparameter that will be filled in with the schedule, should one exist.
 * @return Whether or not a schedule was found.
 */
bool canAllPatientsBeSeen(const Vector<Doctor>& doctors,
                          const Vector<Patient>& patients,
                          Map<string, Set<string>>& schedule) {
    // [TODO: Delete these lines and implement this function!]
    Vector<Doctor> doctorsCopy;
    Vector<Patient> patientsCopy;
    doctorsCopy.addAll(doctors);
    patientsCopy.addAll(patients);
    return allocatePatientsToDoctors(doctorsCopy,patientsCopy,schedule,0);
}


/* * * * Disaster Planning * * * */
bool searchSupplyCities(const Map<string, Set<string>>& roadNetwork,
                     int numCities,
                     Set<string>& locations,
                     Set<string> cities) {
    if (numCities >= 0 && cities.isEmpty())
        return true;
    if (numCities <= 0)
        return false;
    Set<string> cityAndNeighbers = roadNetwork.get(cities.first()) + cities.first();
    //cities -= cityAndNeighbers;
    for (string city : cityAndNeighbers) {
        if (!locations.contains(city)) {
            locations.add(city);
            //The following statements can't be used
            //cities = cities - roadNetwork.get(city) - city;
            if (searchSupplyCities(roadNetwork, numCities - 1, locations, cities - roadNetwork.get(city) - city))
                return true;
            /*The problems is here, some cities were covered twice or more, so this following statement
            could make cities become bigger than upper cities.
            cities = cities + roadNetwork.get(city) + city;
            */
            locations.remove(city);
        }
    }
    //cities += cityAndNeighbers;
    return false;
}
/**
 * Given a transportation grid for a country or region, along with the number of cities where disaster
 * supplies can be stockpiled, returns whether it's possible to stockpile disaster supplies in at most
 * the specified number of cities such that each city either has supplies or is connected to a city that
 * does.
 * <p>
 * This function can assume that every city is a key in the transportation map and that roads are
 * bidirectional: if there's a road from City A to City B, then there's a road from City B back to
 * City A as well.
 *
 * @param roadNetwork The underlying transportation network.
 * @param numCities   How many cities you can afford to put supplies in.
 * @param locations   An outparameter filled in with which cities to choose if a solution exists.
 * @return Whether a solution exists.
 */
bool canBeMadeDisasterReady(const Map<string, Set<string>>& roadNetwork,
                            int numCities,
                            Set<string>& locations) {
    // [TODO: Delete these lines and implement this function!]
    Set<string> cities;
    Vector<string> city = roadNetwork.keys();
    for (string s : city) {
        cities.add(s);
    }
    return searchSupplyCities(roadNetwork, numCities, locations, cities);
}


/* * * * Winning the Election * * * */

/**
 * Given a list of the states in the election, including their popular and electoral vote
 * totals, and the number of electoral votes needed, as well as the index of the lowest-indexed
 * state to consider, returns information about how few popular votes you'd need in order to
 * win that at least that many electoral votes.
 *
 * @param electoralVotesNeeded the minimum number of electoral votes needed
 * @param states All the states in the election (plus DC, if appropriate)
 * @param minStateIndex the lowest index in the states Vector that should be considered
 */
MinInfo minPopularVoteToGetAtLeast(int electoralVotesNeeded, const Vector<State>& states, int minStateIndex) {
    // [TODO: Delete these lines and implement this function!]
    (void)(electoralVotesNeeded);
    (void)(states);
    (void)(minStateIndex);
    return { 0, {} };
}

/**
 * Given a list of all the states in an election, including their popular and electoral vote
 * totals, returns information about how few popular votes you'd need to win in order to win
 * the presidency.
 *
 * @param states All the states in the election (plus DC, if appropriate)
 * @return Information about how few votes you'd need to win the election.
 */
MinInfo minPopularVoteToWin(const Vector<State>& states) {
    // [TODO: Delete these lines and implement this function!]
    (void)(states);
    return { 0, {} };
}
