#include "../include/class_layer3.h"

// // Constructor for AtoC case with absThreshSS_BlocksConsolidation
MeasurementFilter::MeasurementFilter(int k, double absThreshSS_BlocksConsolidation, int maxNrofRSIndexesToReport )
    : absThreshSS_BlocksConsolidation(absThreshSS_BlocksConsolidation), maxNrofRSIndexesToReport (maxNrofRSIndexesToReport ){
    
    filterCoefficient = k;
    
}
// 
// 
// // Update method for AtoC case
// void MeasurementFilter::updateParallel(const std::vector<std::vector<double>>& reshapedMeasurements) {
//     size_t filterCount = reshapedMeasurements.size();
//     std::vector<double> finalResults(filterCount, 0.0);

//     for (size_t filterIdx = 0; filterIdx < filterCount; ++filterIdx) {
//         double localFn_prev = 0.0;
//         for (const double& measurement : reshapedMeasurements[filterIdx]) {
//             double a = 1.0 / std::pow(2, filterCoefficient / 4.0);
//             double localFn = (1 - a) * localFn_prev + a * measurement;
//             localFn_prev = localFn;
//         }
//         finalResults[filterIdx] = localFn_prev;
//     }

//     // Store the final results of all filters
//     this->filteredMeasurements.assign(finalResults.begin(), finalResults.begin() + std::min(this->maxNrofRS_IndexesToReport, finalResults.size()));
// }

// // New update method for AtoE case without absThreshSS_BlocksConsolidation
// void MeasurementFilter::updateParallelAtoE(const std::vector<std::vector<double>>& reshapedMeasurements) {
//     size_t filterCount = reshapedMeasurements.size();
//     std::vector<double> finalResults(filterCount, 0.0);

//     for (size_t filterIdx = 0; filterIdx < filterCount; ++filterIdx) {
//         double localFn_prev = 0.0;
//         for (const double& measurement : reshapedMeasurements[filterIdx]) {
//             double a = 1.0 / std::pow(2, filterCoefficient / 4.0);
//             double localFn = (1 - a) * localFn_prev + a * measurement;
//             localFn_prev = localFn;
//         }
//         finalResults[filterIdx] = localFn_prev;
//     }

//     // Ensuring maxNrofRSIndexesToReport  condition at the end
//     if (finalResults.size() < maxNrofRSIndexesToReport ) {
//         finalResults.resize(maxNrofRSIndexesToReport , 0.0);
//     }

//     // Store the final results of all filters
//     this->filteredMeasurements.assign(finalResults.begin(), finalResults.begin() + std::min(this->maxNrofRS_IndexesToReport, finalResults.size()));
// }

// Getter method to retrieve the filtered measurements
vec MeasurementFilter::getBeamMeasurements() {
    return Fn_prev_BeamMeas;
}

// New method to retrieve the final Fn_prev value
double MeasurementFilter::getCellMeasurement() {
    return Fn_prev_CellMeas;
}
// Update the filter with new measurements
void MeasurementFilter::update(vec& latestBeamMeasurementsIndB) {
    
    vec latestBeamMeasurements = inv_dB(latestBeamMeasurementsIndB);
    
    if(Fn_prev_BeamMeas.length()==0)
        Fn_prev_BeamMeas = latestBeamMeasurements;
    else
    {
        if(Fn_prev_BeamMeas.length()!=latestBeamMeasurements.length())
        {
            cout<<"Length mismatch in MeasurementFilter::update()."<<endl;
            abort();
        }

        double a = 1.0 / std::pow(2, filterCoefficient / 4.0);
        Fn_prev_BeamMeas = (1 - a) * Fn_prev_BeamMeas + a * latestBeamMeasurements;
    }
    
    updateCellMeasurement(latestBeamMeasurements);
    
}

// Method to process slot data, apply absThreshSS_BlocksConsolidation and min entries, and return average
void MeasurementFilter::updateCellMeasurement(vec latestBeamMeasurements) {
    
    vec filteredData=latestBeamMeasurements(find(latestBeamMeasurements>absThreshSS_BlocksConsolidation));
    
    sort(filteredData);

    if (filteredData.size() > maxNrofRSIndexesToReport ) 
        filteredData=filteredData.right(maxNrofRSIndexesToReport);
    
    double latestCellMeasurement = sum(filteredData) / filteredData.size();
    
    if(Fn_prev_CellMeas == 0)
        Fn_prev_CellMeas = latestCellMeasurement;
    else
    {
        double a = 1.0 / std::pow(2, filterCoefficient / 4.0);
        Fn_prev_CellMeas = (1 - a) * Fn_prev_CellMeas + a * latestCellMeasurement;
    }
}
