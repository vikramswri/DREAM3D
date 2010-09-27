///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef ANGCONSTANTS_H_
#define ANGCONSTANTS_H_

/**
 * @file AngConstants.h
 * @brief This file contains both c style macro definitions and C++ style const
 * declarations of TSL OIM variables and names. If you are dealing with a .ang
 * file from TSL instrumentation then you MUST use the defines from this file.
 */

#include <string>
// -----------------------------------------------------------------------------
// These are the versions of the constants with capitalization as seen in the ang file
// -----------------------------------------------------------------------------
#define ANG_TEM_PIXPERUM     "TEM_PIXperUM"
#define ANG_X_STAR           "X-Star"
#define ANG_Y_STAR           "Y-Star"
#define ANG_Z_STAR           "Z-Star"
#define ANG_WORKING_DISTANCE "Working Distance"
#define ANG_PHASE             "Phase"
#define ANG_MATERIAL_NAME     "Material Name"
#define ANG_FORMULA           "Formula"
#define ANG_INFO              "Info"
#define ANG_SYMMETRY          "Symmetry"
#define ANG_LATTICE_CONSTANTS "Lattice Constants"
#define ANG_NUMBER_FAMILIES   "Number Families"
#define ANG_HKL_FAMILIES      "hkl Families"
#define ANG_CATEGORIES        "Categories"
#define ANG_GRID              "Grid"
#define ANG_X_STEP            "x-Step"
#define ANG_Y_STEP            "y-Step"
#define ANG_NCOLS_ODD         "Ncols Odd"
#define ANG_NCOLS_EVEN        "Ncols Even"
#define ANG_NROWS             "NRows"
#define ANG_OPERATOR          "Operator"
#define ANG_SAMPLE_ID         "Sampleid"
#define ANG_SCAN_ID           "Scan ID"

#define ANG_PHI1 "Phi1"
#define ANG_PHI "Phi"
#define ANG_PHI2 "Phi2"
#define ANG_IMAGE_QUALITY "Image Quality"
#define ANG_CONFIDENCE_INDEX "Confidence Index"
#define ANG_PHASE_DATA "Phase"
#define ANG_X_POSITION "X Position"
#define ANG_Y_POSITION "Y Position"

namespace TSL {
 namespace OIM
 {
   const std::string FileExt("ang");
   const std::string TEMPIXPerUM(ANG_TEM_PIXPERUM);
   const std::string XStar(ANG_X_STAR  );
   const std::string YStar(ANG_Y_STAR);
   const std::string ZStar(ANG_Z_STAR);
   const std::string WorkingDistance(ANG_WORKING_DISTANCE);
   const std::string Phase(ANG_PHASE);
   const std::string MaterialName(ANG_MATERIAL_NAME);
   const std::string Formula(ANG_FORMULA);
   const std::string Info(ANG_INFO);
   const std::string Symmetry(ANG_SYMMETRY);
   const std::string LatticeConstants(ANG_LATTICE_CONSTANTS);
   const std::string NumberFamilies(ANG_NUMBER_FAMILIES);
   const std::string HKLFamilies(ANG_HKL_FAMILIES);
   const std::string Categories(ANG_CATEGORIES);
   const std::string Grid(ANG_GRID);
   const std::string XStep(ANG_X_STEP);
   const std::string YStep(ANG_Y_STEP);
   const std::string NColsOdd(ANG_NCOLS_ODD);
   const std::string NColsEven(ANG_NCOLS_EVEN);
   const std::string NRows(ANG_NROWS);
   const std::string Operator(ANG_OPERATOR);
   const std::string SampleId(ANG_SAMPLE_ID);
   const std::string ScanId(ANG_SCAN_ID);

   const std::string Phi1(ANG_PHI1);
   const std::string Phi(ANG_PHI);
   const std::string Phi2(ANG_PHI2);
   const std::string ImageQuality(ANG_IMAGE_QUALITY);
   const std::string ConfidenceIndex(ANG_CONFIDENCE_INDEX);
   const std::string PhaseData(ANG_PHASE_DATA);
   const std::string XPosition(ANG_X_POSITION);
   const std::string YPosition(ANG_Y_POSITION);
 }
}

// -----------------------------------------------------------------------------
//  These are the lower case versions of the constants for the ANG file
// -----------------------------------------------------------------------------
#define ANG_TEM_PIXPERUM_LOWER "tem_pixperum"
#define ANG_X_STAR_LOWER "x-star"
#define ANG_Y_STAR_LOWER "y-star"
#define ANG_Z_STAR_LOWER "z-star"
#define ANG_WORKING_DISTANCE_LOWER "workingdistance"

#define ANG_PHASE_LOWER "phase"
#define ANG_MATERIAL_NAME_LOWER "materialname"
#define ANG_FORMULA_LOWER "formula"
#define ANG_INFO_LOWER "info"
#define ANG_SYMMETRY_LOWER "symmetry"
#define ANG_LATTICE_CONSTANTS_LOWER "latticeconstants"
#define ANG_NUMBER_FAMILIES_LOWER "numberfamilies"
#define ANG_HKL_FAMILIES_LOWER "hklfamilies"
#define ANG_CATEGORIES_LOWER "categories"

#define ANG_GRID_LOWER "grid"
#define ANG_X_STEP_LOWER "xstep"
#define ANG_Y_STEP_LOWER "ystep"
#define ANG_NCOLS_ODD_LOWER "ncols_odd"
#define ANG_NCOLS_EVEN_LOWER "ncols_even"
#define ANG_NROWS_LOWER "nrows"

#define ANG_OPERATOR_LOWER "operator"
#define ANG_SAMPLE_ID_LOWER "sampleid"
#define ANG_SCAN_ID_LOWER "scanid"
#define ANG_SQUARE_GRID_LOWER "sqrgrid"

// Constants  to be used by all classes
#define ANG_PHI1_LOWER "phi1"
#define ANG_PHI_LOWER "phi"
#define ANG_PHI2_LOWER "phi2"
#define ANG_IMAGE_QUALITY_LOWER "image quality"
#define ANG_CONFIDENCE_INDEX_LOWER "confidence index"
#define ANG_PHASE_DATA_LOWER "phase"
#define ANG_X_POSITION_LOWER "x position"
#define ANG_Y_POSITION_LOWER "y position"


namespace TSL {


namespace OIM
 {
// These are the Lower Case versions of the constants
   const std::string FileExtLower("ang");
   const std::string TEMPIXPerUMLower(ANG_TEM_PIXPERUM_LOWER);
   const std::string XStarLower(ANG_X_STAR_LOWER);
   const std::string YStarLower(ANG_Y_STAR_LOWER);
   const std::string ZStarLower(ANG_Z_STAR_LOWER);
   const std::string WorkingDistanceLower(ANG_WORKING_DISTANCE_LOWER);
   const std::string PhaseLower(ANG_PHASE_LOWER);
   const std::string MaterialNameLower(ANG_MATERIAL_NAME_LOWER);
   const std::string FormulaLower(ANG_FORMULA_LOWER);
   const std::string InfoLower(ANG_INFO_LOWER);
   const std::string SymmetryLower(ANG_SYMMETRY_LOWER);
   const std::string LatticeConstantsLower(ANG_LATTICE_CONSTANTS_LOWER);
   const std::string NumberFamiliesLower(ANG_NUMBER_FAMILIES_LOWER);
   const std::string HKLFamiliesLower(ANG_HKL_FAMILIES_LOWER);
   const std::string CategoriesLower(ANG_CATEGORIES_LOWER);
   const std::string GridLower(ANG_GRID_LOWER);
   const std::string XStepLower(ANG_X_STEP_LOWER);
   const std::string YStepLower(ANG_Y_STEP_LOWER);
   const std::string NColsOddLower(ANG_NCOLS_ODD_LOWER);
   const std::string NColsEvenLower(ANG_NCOLS_EVEN_LOWER);
   const std::string NRowsLower(ANG_NROWS_LOWER);
   const std::string OperatorLower(ANG_OPERATOR_LOWER);
   const std::string SampleIdLower(ANG_SAMPLE_ID_LOWER);
   const std::string ScanIdLower(ANG_SCAN_ID_LOWER);

   const std::string Phi1Lower(ANG_PHI1_LOWER);
   const std::string PhiLower(ANG_PHI_LOWER);
   const std::string Phi2Lower(ANG_PHI2_LOWER);
   const std::string ImageQualityLower(ANG_IMAGE_QUALITY_LOWER);
   const std::string ConfidenceIndexLower(ANG_CONFIDENCE_INDEX_LOWER);
   const std::string PhaseDataLower(ANG_PHASE_DATA_LOWER);
   const std::string XPositionLower(ANG_X_POSITION_LOWER);
   const std::string YPositionLower(ANG_Y_POSITION_LOWER);
 }

}


#endif /* ANGCONSTANTS_H_ */
