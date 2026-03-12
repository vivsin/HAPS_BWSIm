#ifndef MEASUREMENTFILTER_H
#define MEASUREMENTFILTER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <numeric> // Required for std::accumulate
#include "../../../../lib/Frozen/simSupport/include/simSupport.h"

// MeasurementFilter class declaration
class MeasurementFilter {
private:
    double filterCoefficient; // Filter coefficient
    size_t maxNrofRS_IndexesToReport = 18; // Number of reports to take

    vec Fn_prev_BeamMeas; // Filtered beam measurements

    double absThreshSS_BlocksConsolidation; // Fixed absThreshSS_BlocksConsolidation value
    size_t maxNrofRSIndexesToReport ; // Fixed number of entries to process
    double Fn_prev_CellMeas; // Filtered cell measurement

public:
    MeasurementFilter(){};
    MeasurementFilter(int k, double absThreshSS_BlocksConsolidation, int maxNrofRSIndexesToReport ); 

    void update(vec& latestBeamMeasurementsIndB);
    
    vec getBeamMeasurements();
    double getCellMeasurement(); // New method to get the final Fn_prev value

private:
    void updateCellMeasurement(vec latestBeamMeasurements);
};

class MeasurementModel {
    ivec cellIDs;
    Array<MeasurementFilter> measurementFilterPerCell;

public:
    MeasurementModel()
    {
        cellIDs.set_length(0);
        measurementFilterPerCell.set_length(0);
    }
    MeasurementModel(ivec cellids)
    {
        cellIDs = cellids;
        measurementFilterPerCell.set_length(cellIDs.length());
        for(int cell_cnt = 0; cell_cnt < cellIDs.length(); cell_cnt++)
            measurementFilterPerCell(cell_cnt) = MeasurementFilter(4, -180, 18);
    }
    void update(int cellID, vec latestBeamMeasurements)
    {
        int cellIndx = find(cellIDs, cellID);
        if(cellIndx != -1)
        {
            measurementFilterPerCell(cellIndx).update(latestBeamMeasurements);
            return;
        }
        std::cout << "Undefined cellID to MeasurementModel::update()" << std::endl; abort();
    }
    //retrieves the measurement for each cell, stores it in the cellMeasurements vector
    vec getCellMeasurements() 
    {
        
        vec cellMeasurements(cellIDs.length());
        for (int all_cell = 0; all_cell < cellIDs.length(); all_cell++) 
        {
            cellMeasurements(all_cell) = measurementFilterPerCell(all_cell).getCellMeasurement();
        }
        return cellMeasurements;
    }
    //retrieves the beam measurement for the given cell
    vec getBeamMeasurements(int cellID) {
        
        int cellIndx = find(cellIDs, cellID);
        if(cellIndx != -1)
        {
            return measurementFilterPerCell(cellIndx).getBeamMeasurements();
        }
        std::cout << "Undefined cellID to MeasurementModel::getBeamMeasurements()" << std::endl; abort();
    }
    
};

std::vector<std::vector<double>> readCSV(const std::string& filename, bool& has_negative);
std::vector<std::vector<double>> filterValues(const std::vector<std::vector<double>>& values, double absThreshSS_BlocksConsolidation_value);
std::vector<std::vector<double>> maxNrofRSIndexesToReport_func(const std::vector<std::vector<double>>& filtered_users, size_t maxNrofRSIndexesToReport);
std::vector<double> readMeasurementsFromFile(const std::string& filename);
std::vector<std::vector<double>> reshapeMeasurementsForParallel(const std::vector<std::vector<double>>& values);

#endif // MEASUREMENTFILTER_H

