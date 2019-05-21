/*
 * This file is part of Vlasiator.
 * Copyright 2010-2016 Finnish Meteorological Institute
 *
 * For details of usage, see the COPYING file and read the "Rules of the Road"
 * at http://www.physics.helsinki.fi/vlasiator/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <cstdlib>
#include <iostream>

#include "datareducer.h"
#include "../common.h"
#include "dro_populations.h"
using namespace std;

void initializeDataReducers(DataReducer * outputReducer, DataReducer * diagnosticReducer)
{
   typedef Parameters P;

   vector<string>::const_iterator it;
   for (it = P::outputVariableList.begin();
        it != P::outputVariableList.end();
        it++) {

      /* Note: Each data reducer generation should be followed by a call to setUnitMetaData
	 with the following arguments:
	 unit, unit in LaTeX formulation, variable in LaTeX formulation, conversion factor
      */

      if(*it == "fg_B" || *it == "B") { // Bulk magnetic field at Yee-Lattice locations
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_B",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]*3);

               // Iterate through fsgrid cells and extract total magnetic field
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x)] =     (*BgBGrid.get(x,y,z))[fsgrids::BGBX]
                     + (*perBGrid.get(x,y,z))[fsgrids::PERBX];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 1] = (*BgBGrid.get(x,y,z))[fsgrids::BGBY]
                       + (*perBGrid.get(x,y,z))[fsgrids::PERBY];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 2] = (*BgBGrid.get(x,y,z))[fsgrids::BGBZ]
                       + (*perBGrid.get(x,y,z))[fsgrids::PERBZ];
                   }
                 }
               }
               return retval;
	 }
	 ));
         outputReducer->addMetadata(outputReducer->size()-1,"T","$\\mathrm{T}$","$B$","1.0");
         continue;	 
      }
      if(*it == "fg_BackgroundB" || *it == "BackgroundB") { // Static (typically dipole) magnetic field part
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_background_B",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]*3);

               // Iterate through fsgrid cells and extract background B
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x)] =     (*BgBGrid.get(x,y,z))[fsgrids::BGBX];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 1] = (*BgBGrid.get(x,y,z))[fsgrids::BGBY];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 2] = (*BgBGrid.get(x,y,z))[fsgrids::BGBZ];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"T","$\\mathrm{T}$","$B_\\mathrm{bg}$","1.0");
         continue;
      }
      if(*it == "fg_PerturbedB" || *it == "PerturbedB") { // Fluctuating magnetic field part
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_perturbed_B",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]*3);

               // Iterate through fsgrid cells and extract values
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x)] =     (*perBGrid.get(x,y,z))[fsgrids::PERBX];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 1] = (*perBGrid.get(x,y,z))[fsgrids::PERBY];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 2] = (*perBGrid.get(x,y,z))[fsgrids::PERBZ];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"T","$\\mathrm{T}$","$B_\\mathrm{per}$)","1.0");
         continue;
      }
      if(*it == "fg_E" || *it== "E") { // Bulk electric field at Yee-lattice locations
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_E",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]*3);

               // Iterate through fsgrid cells and extract E values
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x)] =     (*EGrid.get(x,y,z))[fsgrids::EX];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 1] = (*EGrid.get(x,y,z))[fsgrids::EY];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 2] = (*EGrid.get(x,y,z))[fsgrids::EZ];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"V/m","$\\mathrm{V}\\,\\mathrm{m}^{-1}$","$E$","1.0");
         continue;
      }
      if(*it == "vg_Rhom" || *it == "Rhom") { // Overall mass density (summed over all populations)
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("rhom",CellParams::RHOM,1));
	 outputReducer->addMetadata(outputReducer->size()-1,"kg/m^3","$\\mathrm{kg}\\,\\mathrm{m}^{-3}$","$\\rho_\\mathrm{m}$","1.0");
         continue;
      }
      if(*it == "fg_Rhom") { // Overall mass density (summed over all populations)
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_rhom",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract rho valuesg
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = (*momentsGrid.get(x,y,z))[fsgrids::RHOM];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"kg/m^3","$\\mathrm{kg}\\,\\mathrm{m}^{-3}$","$\\rho_\\mathrm{m}$","1.0");
         continue;
      }
      if(*it == "vg_Rhoq" || *it == "Rhoq") { // Overall charge density (summed over all populations)
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("rhoq",CellParams::RHOQ,1));
	 outputReducer->addMetadata(outputReducer->size()-1,"C/m^3","$\\mathrm{C}\\,\\mathrm{m}^{-3}$","$\\rho_\\mathrm{q}$","1.0");
         continue;
      }
      if(*it == "fg_Rhoq") { // Overall charge density (summed over all populations)
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_rhoq",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract charge density
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = (*momentsGrid.get(x,y,z))[fsgrids::RHOQ];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"C/m^3","$\\mathrm{C}\\,\\mathrm{m}^{-3}$","$\\rho_\\mathrm{q}$","1.0");
         continue;
      }
      if(*it == "populations_Rho") { // Per-population particle number density
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/rho", i, offsetof(spatial_cell::Population, RHO), 1));
	    outputReducer->addMetadata(outputReducer->size()-1,"1/m^3","$\\mathrm{m}^{-3}$","$\\n_\\mathrm{"+pop+"}$","1.0");
         }
         continue;
      }
      
      if(*it == "V" || *it == "vg_V") { // Overall effective bulk density defining the center-of-mass frame from all populations
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("V",CellParams::VX,3));
	 outputReducer->addMetadata(outputReducer->size()-1,"m/s","$\\mathrm{m}\\,\\mathrm{s}^{-1}$","$V$","1.0");
         continue;
      }
      if(*it == "fg_V") { // Overall effective bulk density defining the center-of-mass frame from all populations
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_V",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]*3);

               // Iterate through fsgrid cells and extract bulk Velocity
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x)] =     (*momentsGrid.get(x,y,z))[fsgrids::VX];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 1] = (*momentsGrid.get(x,y,z))[fsgrids::VY];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 2] = (*momentsGrid.get(x,y,z))[fsgrids::VZ];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"m/s","$\\mathrm{m}\\,\\mathrm{s}^{-1}$","$V$","1.0");
         continue;
      }
      if(*it == "populations_V") { // Per population bulk velocities
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/V", i, offsetof(spatial_cell::Population, V), 3));
	    outputReducer->addMetadata(outputReducer->size()-1,"m/s","$\\mathrm{m}\\,\\mathrm{s}^{-1}$","$V_\\mathrm{"+pop+"}$","1.0");
         }
         continue;
      }
      if(*it == "populations_moments_Backstream") { // Per-population moments of the backstreaming part
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::VariableRhoBackstream(i));
            outputReducer->addOperator(new DRO::VariableVBackstream(i));
            outputReducer->addOperator(new DRO::VariablePTensorBackstreamDiagonal(i));
            outputReducer->addOperator(new DRO::VariablePTensorBackstreamOffDiagonal(i));
	    outputReducer->addMetadata(outputReducer->size()-4,"1/m^3","$\\mathrm{m}^{-3}$","$n_\\mathrm{"+pop+",st}$","1.0");
	    outputReducer->addMetadata(outputReducer->size()-3,"m/s","$\\mathrm{m}\\,\\mathrm{s}^{-1}$","$V_\\mathrm{"+pop+",st}$","1.0");
	    outputReducer->addMetadata(outputReducer->size()-2,"Pa","$\\mathrm{Pa}$","$\\mathcal{P}_\\mathrm{"+pop+",st}$","1.0");
	    outputReducer->addMetadata(outputReducer->size()-1,"Pa","$\\mathrm{Pa}$","$\\mathcal{\\tilde{P}}_\\mathrm{"+pop+",st}$","1.0");
         }
         continue;
      }
      if(*it == "populations_moments_NonBackstream") { // Per-population moments of the non-backstreaming (thermal?) part.
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::VariableRhoNonBackstream(i));
            outputReducer->addOperator(new DRO::VariableVNonBackstream(i));
            outputReducer->addOperator(new DRO::VariablePTensorNonBackstreamDiagonal(i));
            outputReducer->addOperator(new DRO::VariablePTensorNonBackstreamOffDiagonal(i));
	    outputReducer->addMetadata(outputReducer->size()-4,"1/m^3","$\\mathrm{m}^{-3}$","$n_\\mathrm{"+pop+",th}$","1.0");
	    outputReducer->addMetadata(outputReducer->size()-3,"m/s","$\\mathrm{m}\\,\\mathrm{s}^{-1}$","$V_\\mathrm{"+pop+",th}$","1.0");
	    outputReducer->addMetadata(outputReducer->size()-2,"Pa","$\\mathrm{Pa}$","$\\mathcal{P}_\\mathrm{"+pop+",th}$","1.0");
	    outputReducer->addMetadata(outputReducer->size()-1,"Pa","$\\mathrm{Pa}$","$\\mathcal{\\tilde{P}}_\\mathrm{"+pop+",th}$","1.0");
         }
         continue;
      }
      if(*it == "populations_MinValue" || *it == "populations_EffectiveSparsityThreshold") {
         // Effective sparsity threshold affecting each cell, if dynamic threshould algorithm is used
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::VariableEffectiveSparsityThreshold(i));
	    outputReducer->addMetadata(outputReducer->size()-1,"s^3/m^6","$\\mathrm{m}^{-6}\\,\\mathrm{s}^{3}$","$f_\\mathrm{"+pop+",min}$","1.0");
         }
         continue;
      }
      if(*it == "populations_RhoLossAdjust") {
         // Accumulated lost particle number, per population, in each cell, since last restart
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/rho_loss_adjust", i, offsetof(spatial_cell::Population, RHOLOSSADJUST), 1));
	    outputReducer->addMetadata(outputReducer->size()-1,"1/m^3","$\\mathrm{m}^{-3}$","$\\Delta_\\mathrm{loss} n_\\mathrm{"+pop+"}$","1.0");
         }
         continue;
      }
      if(*it == "LBweight" || *it == "vg_LBweight") {
         // Load balance metric for LB debugging
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("LB_weight",CellParams::LBWEIGHTCOUNTER,1));
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{LB weight}$","");
         continue;
      }
      if(*it == "MaxVdt") {
         // Overall maximum timestep constraint as calculated by the velocity space vlasov update
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("max_v_dt",CellParams::MAXVDT,1));
	 outputReducer->addMetadata(outputReducer->size()-1,"s","$\\mathrm{s}$","$\\Delta t_\\mathrm{V,max}$","1.0");
         continue;
      }
      if(*it == "populations_MaxVdt") {
         // Per-population maximum timestep constraint as calculated by the velocity space vlasov update
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/MaxVdt", i, offsetof(spatial_cell::Population, max_dt[1]), 1));
	    outputReducer->addMetadata(outputReducer->size()-1,"s","$\\mathrm{s}$","$\\Delta t_\\mathrm{"+pop+",V,max}$","1.0");
         }
         continue;
      }
      if(*it == "MaxRdt") {
         // Overall maximum timestep constraint as calculated by the real space vlasov update
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("max_r_dt",CellParams::MAXRDT,1));
	 outputReducer->addMetadata(outputReducer->size()-1,"s","$\\mathrm{s}$","$\\Delta t_\\mathrm{R,max}$","1.0");
         continue;
      }
      if(*it == "populations_MaxRdt") {
         // Per-population maximum timestep constraint as calculated by the real space vlasov update
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/MaxRdt", i, offsetof(spatial_cell::Population, max_dt[0]), 1));
	    outputReducer->addMetadata(outputReducer->size()-1,"s","$\\mathrm{s}$","$\\Delta t_\\mathrm{"+pop+",R,max}$","1.0");
         }
         continue;
      }
      if(*it == "populations_EnergyDensity") {
         // Per-population energy density in three energy ranges
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::VariableEnergyDensity(i));
	    std::stringstream conversion;
	    conversion << (1.0e-6)/physicalconstants::CHARGE;
	    outputReducer->addMetadata(outputReducer->size()-1,"eV/cm^3","$\\mathrm{eV}\\,\\mathrm{cm}^{-3}$","$U_\\mathrm{"+pop+"}$",conversion.str());
         }
         continue;
      }

      // For precipitation:
      //std::stringstream conversion;
      //conversion << (1.0e-4)*physicalconstants::CHARGE;
      // outputReducer->addMetadata(outputReducer->size()-1,"1/(cm^2 sr s eV)","$\\mathrm{cm}^{-2}\\,\\mathrm{sr}^{-1}\\,\\mathrm{s}^{-1}\\,\\mathrm{eV}^{-1}$","$\\mathcal{F}_\\mathrm{"+pop+"}$",conversion.str());
      if(*it == "MaxFieldsdt" || *it == "fg_MaxFieldsdt") {
         // Maximum timestep constraint as calculated by the fieldsolver
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("MaxFieldsdt",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract field solver timestep limit
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.get(x,y,z)->maxFsDt;
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"s","$\\mathrm{s}$","$\\Delta t_\\mathrm{f,max}$","1.0");
         continue;
      }
      if(*it == "MPIrank" || *it == "vg_rank") {
         // Map of spatial decomposition of the DCCRG grid into MPI ranks
         outputReducer->addOperator(new DRO::MPIrank);
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{MPI rank}$","");
         continue;
      }
      if(*it == "FsGridRank" || *it == "fg_rank") {
         // Map of spatial decomposition of the FsGrid into MPI ranks
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("FsGridRank",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2],technicalGrid.getRank());
               return retval;
             }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{fGrid rank}$","");
         continue;
      }
      if(*it == "BoundaryType" || *it == "vg_BoundaryType") {
         // Type of boundarycells
         outputReducer->addOperator(new DRO::BoundaryType);
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{vGrid Boundary type}$","");
         continue;
      }
      if(*it == "fg_BoundaryType") {
         // Type of boundarycells as stored in FSGrid
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("FsGridBoundaryType",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract boundary flag
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.get(x,y,z)->sysBoundaryFlag;
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{fGrid Boundary type}$","");
         continue;
      }
      if(*it == "BoundaryLayer" || *it == "vg_BoundaryLayer") {
         // For boundaries with multiple layers: layer count per cell
         outputReducer->addOperator(new DRO::BoundaryLayer);
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{vGrid Boundary layer}$","");
         continue;
      }
      if(*it == "fg_BoundaryLayer") {
         // Type of boundarycells as stored in FSGrid
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("FsGridBoundaryLayer",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract boundary layer
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.get(x,y,z)->sysBoundaryLayer;
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{fGrid Boundary layer}$","");
         continue;
      }
      if (*it == "populations_Blocks") {
         // Per-population velocity space block counts
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::Blocks(i));
	    outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{"+pop+" blocks}$","");
         }
         continue;
      }
      if(*it == "fSaved") {
         // Boolean marker whether a velocity space is saved in a given spatial cell
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("fSaved",CellParams::ISCELLSAVINGF,1));
	 outputReducer->addMetadata(outputReducer->size()-1,"","","$f(v)_\\mathrm{saved}$","");
         continue;
      }
      if(*it == "populations_accSubcycles") {
         // Per-population number of subcycles performed for velocity space update
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::DataReductionOperatorPopulations<uint>(pop + "/acc_subcycles", i, offsetof(spatial_cell::Population, ACCSUBCYCLES), 1));
	    outputReducer->addMetadata(outputReducer->size()-1,"","","$\\mathrm{"+pop+" Acc subcycles}$","");
         }
         continue;
      }
      if(*it == "VolE" || *it == "vg_VolE") {
         // Volume-averaged E field
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("E_vol",CellParams::EXVOL,3));
	 outputReducer->addMetadata(outputReducer->size()-1,"V/m","$\\mathrm{V}\\,\\mathrm{m}^{-1}$","$E_\\mathrm{vol,vg}$","1.0");
         continue;
      }
      if(*it == "fg_VolE") {
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_volB",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]*3);

               // Iterate through fsgrid cells and extract EVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x)] =  + (*volGrid.get(x,y,z))[fsgrids::EXVOL];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 1] = + (*volGrid.get(x,y,z))[fsgrids::EYVOL];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 2] = + (*volGrid.get(x,y,z))[fsgrids::EZVOL];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"V/m","$\\mathrm{V}\\,\\mathrm{m}^{-1}$","$E_\\mathrm{vol,fg}$","1.0");
         continue;
      }
      if(*it == "fg_HallE") {
         // TODO: Add outputreducer to get EHALL from fsgrid
	 // outputReducer->addMetadata(outputReducer->size()-1,"V/m","$\\mathrm{V}\\,\\mathrm{m}^{-1}$","$E_\\mathrm{Hall}$","1.0");
         continue;
      }
      if(*it =="GradPeE") {
         // Electron pressure gradient contribution to the generalized ohm's law
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("EGRADPE",CellParams::EXGRADPE,3));
	 outputReducer->addMetadata(outputReducer->size()-1,"V/m","$\\mathrm{V}\\,\\mathrm{m}^{-1}$","$E_{\\del P_\\mathrm{e}}$","1.0");
         continue;
      }
      if(*it == "VolB" || *it == "vg_VolB") {
         // Volume-averaged magnetic field
         outputReducer->addOperator(new DRO::VariableBVol);
	 outputReducer->addMetadata(outputReducer->size()-1,"T","$\\mathrm{T}$","$B_\\mathrm{vol,vg}$","1.0");
         continue;
      }
      if(*it == "fg_VolB") { // Static (typically dipole) magnetic field part
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_volB",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]*3);

               // Iterate through fsgrid cells and extract total BVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x)] =     (*BgBGrid.get(x,y,z))[fsgrids::BGBXVOL]
                     + (*volGrid.get(x,y,z))[fsgrids::PERBXVOL];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 1] = (*BgBGrid.get(x,y,z))[fsgrids::BGBYVOL]
                       + (*volGrid.get(x,y,z))[fsgrids::PERBYVOL];
                     retval[3*(gridSize[1]*gridSize[0]*z + gridSize[0]*y + x) + 2] = (*BgBGrid.get(x,y,z))[fsgrids::BGBZVOL]
                       + (*volGrid.get(x,y,z))[fsgrids::PERBZVOL];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"T","$\\mathrm{T}$","$B_\\mathrm{vol,fg}$","1.0");
         continue;
      }
      if(*it == "BackgroundVolB") {
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("BGB_vol",CellParams::BGBXVOL,3));
	 outputReducer->addMetadata(outputReducer->size()-1,"T","$\\mathrm{T}$","$B_\\mathrm{vol,vg,bg}$","1.0");
         continue;
      }
      if(*it == "PerturbedVolB") {
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("PERB_vol",CellParams::PERBXVOL,3));
	 outputReducer->addMetadata(outputReducer->size()-1,"T","$\\mathrm{T}$","$B_\\mathrm{vol,vg,per}$","1.0");
         continue;
      }
      if(*it == "Pressure" || *it== "vg_Pressure") {
         // Overall scalar pressure from all populations
         outputReducer->addOperator(new DRO::VariablePressureSolver);
	 outputReducer->addMetadata(outputReducer->size()-1,"Pa","$\\mathrm{Pa}$","$P_\\mathrm{solver}$","1.0");
         continue;
      }
      if(*it == "fg_Pressure") {
         // Overall scalar pressure from all populations
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_Pressure",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract boundary flag
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     auto& moments=(*momentsGrid.get(x,y,z));
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = 1./3. * (moments[fsgrids::P_11] + moments[fsgrids::P_22] + moments[fsgrids::P_33]);
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"Pa","$\\mathrm{Pa}$","$P_\\mathrm{fg}$","1.0");
         continue;
      }
      if(*it == "populations_PTensor") {
         // Per-population pressure tensor, stored as diagonal and offdiagonal components
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            outputReducer->addOperator(new DRO::VariablePTensorDiagonal(i));
	    outputReducer->addMetadata(outputReducer->size()-1,"Pa","$\\mathrm{Pa}$","$\\mathcal{P}_\\mathrm{"+pop+"}$","1.0");
            outputReducer->addOperator(new DRO::VariablePTensorOffDiagonal(i));
	    outputReducer->addMetadata(outputReducer->size()-1,"Pa","$\\mathrm{Pa}$","$\\mathcal{\\tilde{P}}_\\mathrm{"+pop+"}$","1.0");
         }
         continue;
      }
      if(*it == "BVOLderivs") {
         // Volume-averaged derivatives
         outputReducer->addOperator(new DRO::DataReductionOperatorBVOLDerivatives("dPERBXVOLdy",bvolderivatives::dPERBXVOLdy,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorBVOLDerivatives("dPERBXVOLdz",bvolderivatives::dPERBXVOLdz,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorBVOLDerivatives("dPERBYVOLdx",bvolderivatives::dPERBYVOLdx,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorBVOLDerivatives("dPERBYVOLdz",bvolderivatives::dPERBYVOLdz,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorBVOLDerivatives("dPERBZVOLdx",bvolderivatives::dPERBZVOLdx,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorBVOLDerivatives("dPERBZVOLdy",bvolderivatives::dPERBZVOLdy,1));
	 outputReducer->addMetadata(outputReducer->size()-6,"T/m","$\\mathrm{T}\\,\\mathrm{m}^{-1}$","$\\Delta B_{X,\\mathrm{per,vol,vg}} (\\Delta Y)^{-1}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-5,"T/m","$\\mathrm{T}\\,\\mathrm{m}^{-1}$","$\\Delta B_{X,\\mathrm{per,vol,vg}} (\\Delta Z)^{-1}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-4,"T/m","$\\mathrm{T}\\,\\mathrm{m}^{-1}$","$\\Delta B_{Y,\\mathrm{per,vol,vg}} (\\Delta X)^{-1}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-3,"T/m","$\\mathrm{T}\\,\\mathrm{m}^{-1}$","$\\Delta B_{Y,\\mathrm{per,vol,vg}} (\\Delta Z)^{-1}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-2,"T/m","$\\mathrm{T}\\,\\mathrm{m}^{-1}$","$\\Delta B_{Z,\\mathrm{per,vol,vg}} (\\Delta X)^{-1}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-1,"T/m","$\\mathrm{T}\\,\\mathrm{m}^{-1}$","$\\Delta B_{Z,\\mathrm{per,vol,vg}} (\\Delta Y)^{-1}$","1.0");
         continue;
      }
      if(*it == "vg_GridCoordinates") {
         // Spatial coordinates for each cell
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("vg_X",CellParams::XCRD,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("vg_Y",CellParams::YCRD,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("vg_Z",CellParams::ZCRD,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("vg_DX",CellParams::DX,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("vg_DY",CellParams::DY,1));
         outputReducer->addOperator(new DRO::DataReductionOperatorCellParams("vg_DZ",CellParams::DZ,1));
	 outputReducer->addMetadata(outputReducer->size()-6,"m","$\\mathrm{m}$","$X_\\mathrm{vg}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-5,"m","$\\mathrm{m}$","$Y_\\mathrm{vg}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-4,"m","$\\mathrm{m}$","$Z_\\mathrm{vg}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-3,"m","$\\mathrm{m}$","$\\delta X_\\mathrm{vg}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-2,"m","$\\mathrm{m}$","$\\delta Y_\\mathrm{vg}$","1.0");
	 outputReducer->addMetadata(outputReducer->size()-1,"m","$\\mathrm{m}$","$\\delta Z_\\mathrm{vg}$","1.0");
         continue;
      }
      if(*it == "fg_GridCoordinates") { 
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_X",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract total BVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.getPhysicalCoords(x,y,z)[0];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"m","$\\mathrm{m}$","$X_\\mathrm{fg}$","1.0");
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_Y",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract total BVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.getPhysicalCoords(x,y,z)[1];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"m","$\\mathrm{m}$","$Y_\\mathrm{fg}$","1.0");
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_Z",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract total BVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.getPhysicalCoords(x,y,z)[2];
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"m","$\\mathrm{m}$","$Z_\\mathrm{fg}$","1.0");
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_DX",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract total BVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.DX;
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"m","$\\mathrm{m}$","$\\delta X_\\mathrm{fg}$","1.0");
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_DY",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract total BVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.DY;
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"m","$\\mathrm{m}$","$\\delta Y_\\mathrm{fg}$","1.0");
         outputReducer->addOperator(new DRO::DataReductionOperatorFsGrid("fg_DZ",[](
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid)->std::vector<double> {

               std::array<int32_t,3>& gridSize = technicalGrid.getLocalSize();
               std::vector<double> retval(gridSize[0]*gridSize[1]*gridSize[2]);

               // Iterate through fsgrid cells and extract total BVOL
               for(int z=0; z<gridSize[2]; z++) {
                 for(int y=0; y<gridSize[1]; y++) {
                   for(int x=0; x<gridSize[0]; x++) {
                     retval[gridSize[1]*gridSize[0]*z + gridSize[0]*y + x] = technicalGrid.DZ;
                   }
                 }
               }
               return retval;
         }
         ));
	 outputReducer->addMetadata(outputReducer->size()-1,"m","$\\mathrm{m}$","$\\delta Z_\\mathrm{fg}$","1.0");
         continue;
      }
      if (*it == "MeshData") {
         outputReducer->addOperator(new DRO::VariableMeshData);
	 outputReducer->addMetadata(outputReducer->size()-1,"","","\\mathrm{Mesh data}$","");
         continue;
      }
      // After all the continue; statements one should never land here.
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
      if (myRank == MASTER_RANK) {
         std::cerr << __FILE__ << ":" << __LINE__ << ": The output variable " << *it << " is not defined." << std::endl;
      }
      MPI_Finalize();
      exit(1);
   }

   for (it = P::diagnosticVariableList.begin();
        it != P::diagnosticVariableList.end();
        it++) {
      if (*it == "populations_Blocks") {
         // Per-population total block counts
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            diagnosticReducer->addOperator(new DRO::Blocks(i));
         }
         continue;
      }
      if(*it == "Rhom") {
         // Overall mass density
         diagnosticReducer->addOperator(new DRO::DataReductionOperatorCellParams("rho",CellParams::RHOM,1));
         continue;
      }
      if(*it == "populations_RhoLossAdjust") {
         // Per-particle overall lost particle number
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            diagnosticReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/rho_loss_adjust", i, offsetof(spatial_cell::Population, RHOLOSSADJUST), 1));
         }
         continue;
      }
      //if(*it == "RhoLossVelBoundary") {
      //   diagnosticReducer->addOperator(new DRO::DataReductionOperatorCellParams("rho_loss_velocity_boundary",CellParams::RHOLOSSVELBOUNDARY,1));
      //   continue;
      //}
      if(*it == "LBweight") {
         diagnosticReducer->addOperator(new DRO::DataReductionOperatorCellParams("LB_weight",CellParams::LBWEIGHTCOUNTER,1));
         continue;
      }
      if(*it == "MaxVdt") {
         diagnosticReducer->addOperator(new DRO::DataReductionOperatorCellParams("max_v_dt",CellParams::MAXVDT,1));
         continue;
      }
      if(*it == "MaxRdt") {
         diagnosticReducer->addOperator(new DRO::DataReductionOperatorCellParams("max_r_dt",CellParams::MAXRDT,1));
         continue;
      }
      if(*it == "MaxFieldsdt") {
         diagnosticReducer->addOperator(new DRO::DataReductionOperatorCellParams("max_fields_dt",CellParams::MAXFDT,1));
         continue;
      }
      if(*it == "populations_MaxDistributionFunction") {
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            diagnosticReducer->addOperator(new DRO::MaxDistributionFunction(i));
         }
         continue;
      }
      if(*it == "populations_MinDistributionFunction") {
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            diagnosticReducer->addOperator(new DRO::MinDistributionFunction(i));
         }
         continue;
      }
      if(*it == "populations_MaxRdt") {
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            diagnosticReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/Blocks", i, offsetof(spatial_cell::Population, max_dt[0]), 1));
         }
         continue;
      }
      if(*it == "populations_MaxVdt") {
         for(unsigned int i =0; i < getObjectWrapper().particleSpecies.size(); i++) {
            species::Species& species=getObjectWrapper().particleSpecies[i];
            const std::string& pop = species.name;
            diagnosticReducer->addOperator(new DRO::DataReductionOperatorPopulations<Real>(pop + "/Blocks", i, offsetof(spatial_cell::Population, max_dt[1]), 1));
         }
         continue;
      }
      // After all the continue; statements one should never land here.
      int myRank;
      MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
      if (myRank == MASTER_RANK) {
         std::cerr << __FILE__ << ":" << __LINE__ << ": The diagnostic variable " << *it << " is not defined." << std::endl;
      }
      MPI_Finalize();
      exit(1);
   }
}

// ************************************************************
// ***** DEFINITIONS FOR DATAREDUCER CLASS *****
// ************************************************************

/** Constructor for class DataReducer.
 */
DataReducer::DataReducer() { }

/** Destructor for class DataReducer. All stored DataReductionOperators 
 * are deleted.
 */
DataReducer::~DataReducer() {
   // Call delete for each DataReductionOperator:
   for (vector<DRO::DataReductionOperator*>::iterator it=operators.begin(); it!=operators.end(); ++it) {
      delete *it;
      *it = NULL;
   }
}

/** Add a new DRO::DataReductionOperator which has been created with new operation. 
 * DataReducer will take care of deleting it.
 * @return If true, the given DRO::DataReductionOperator was added successfully.
 */
bool DataReducer::addOperator(DRO::DataReductionOperator* op) {
   operators.push_back(op);
   return true;
}

/** Get the name of a DataReductionOperator.
 * @param operatorID ID number of the operator whose name is requested.
 * @return Name of the operator.
 */
std::string DataReducer::getName(const unsigned int& operatorID) const {
   if (operatorID >= operators.size()) return "";
   return operators[operatorID]->getName();
}

/** Get info on the type of data calculated by the given DataReductionOperator.
 * A DataReductionOperator writes an array on disk. Each element of the array is a vector with n elements. Finally, each
 * vector element has a byte size, as given by the sizeof function.
 * @param operatorID ID number of the DataReductionOperator whose output data info is requested.
 * @param dataType Basic datatype, must be int, uint, or float.
 * @param dataSize Byte size of written datatype, for example double-precision floating points
 * have byte size of sizeof(double).
 * @param vectorSize How many elements are in the vector returned by the DataReductionOperator.
 * @return If true, DataReductionOperator was found and it returned sensible values.
 */
bool DataReducer::getDataVectorInfo(const unsigned int& operatorID,std::string& dataType,unsigned int& dataSize,unsigned int& vectorSize) const {
   if (operatorID >= operators.size()) return false;
   return operators[operatorID]->getDataVectorInfo(dataType,dataSize,vectorSize);
}

/** Add a metadata to the specified DRO::DataReductionOperator. 
 * @param operatorID ID number of the DataReductionOperator to add metadata to
 * @param unit string with the physical unit of the DRO result
 * @param unitLaTeX LaTeX-formatted string with the physical unit of the DRO result
 * @param variableLaTeX LaTeX-formatted string with a descriptive short name for the DRO result
 * @param conversionFactor floating point conversion factor between DRO result and SI units
 * @return If true, the given metadata  was added successfully.
 */
bool DataReducer::addMetadata(const unsigned int operatorID, std::string unit,std::string unitLaTeX,std::string variableLaTeX,std::string unitConversion) {
   if (operatorID >= operators.size()) return false;
   return operators[operatorID]->setUnitMetadata(unit,unitLaTeX,variableLaTeX,unitConversion);
}

/** Get metadata on the unit of data calculated by the given DataReductionOperator.
 * @param operatorID ID number of the DataReductionOperator whose output unit metadata is requested.
 * @param unit Physical unit of variable
 * @param unitLaTeX Physical unit of variable, written using LaTeX notation
 * @param unitConversion Floating point value of conversion factor to SI units
 * @return If true, DataReductionOperator was found and it returned sensible values.
 */
bool DataReducer::getMetadata(const unsigned int& operatorID,std::string& unit,std::string& unitLaTeX,std::string& variableLaTeX,std::string& unitConversion) const {
   if (operatorID >= operators.size()) return false;
   return operators[operatorID]->getUnitMetadata(unit, unitLaTeX, variableLaTeX, unitConversion);
}

/** Ask a DataReductionOperator if it wants to take care of writing the data 
 * to output file instead of letting be handled in iowrite.cpp. 
 * @param operatorID ID number of the DataReductionOperator.
 * @return If true, then VLSVWriter should be passed to the DataReductionOperator.*/
bool DataReducer::handlesWriting(const unsigned int& operatorID) const {
   if (operatorID >= operators.size()) return false;
   return dynamic_cast<DRO::DataReductionOperatorHandlesWriting*>(operators[operatorID]) != nullptr;
}

/** Ask a DataReductionOperator if it wants to write parameters to the vlsv file header
 * @param operatorID ID number of the DataReductionOperator.
 * @return If true, then VLSVWriter should be passed to the DataReductionOperator.*/
bool DataReducer::hasParameters(const unsigned int& operatorID) const {
   if (operatorID >= operators.size()) return false;
   return dynamic_cast<DRO::DataReductionOperatorHasParameters*>(operators[operatorID]) != nullptr;
}

/** Request a DataReductionOperator to calculate its output data and to write it to the given buffer.
 * @param cell Pointer to spatial cell whose data is to be reduced.
 * @param operatorID ID number of the applied DataReductionOperator.
 * @param buffer Buffer in which DataReductionOperator should write its data.
 * @return If true, DataReductionOperator calculated and wrote data successfully.
 */
bool DataReducer::reduceData(const SpatialCell* cell,const unsigned int& operatorID,char* buffer) {
   // Tell the chosen operator which spatial cell we are counting:
   if (operatorID >= operators.size()) return false;
   if (operators[operatorID]->setSpatialCell(cell) == false) return false;

   if (operators[operatorID]->reduceData(cell,buffer) == false) return false;
   return true;
}

/** Request a DataReductionOperator to calculate its output data and to write it to the given variable.
 * @param cell Pointer to spatial cell whose data is to be reduced.
 * @param operatorID ID number of the applied DataReductionOperator.
 * @param result Real variable in which DataReductionOperator should write its result.
 * @return If true, DataReductionOperator calculated and wrote data successfully.
 */
bool DataReducer::reduceDiagnostic(const SpatialCell* cell,const unsigned int& operatorID,Real * result) {
   // Tell the chosen operator which spatial cell we are counting:
   if (operatorID >= operators.size()) return false;
   if (operators[operatorID]->setSpatialCell(cell) == false) return false;
   
   if (operators[operatorID]->reduceDiagnostic(cell,result) == false) return false;
   return true;
}

/** Get the number of DataReductionOperators stored in DataReducer.
 * @return Number of DataReductionOperators stored in DataReducer.
 */
unsigned int DataReducer::size() const {return operators.size();}

/** Write all data from given DataReductionOperator to the output file.
 * @param operatorID ID number of the selected DataReductionOperator.
 * @param mpiGrid Parallel grid library.
 * @param cells Vector containing spatial cell IDs.
 * @param meshName Name of the spatial mesh in the output file.
 * @param vlsvWriter VLSV file writer that has output file open.
 * @return If true, DataReductionOperator wrote its data successfully.*/
bool DataReducer::writeData(const unsigned int& operatorID,
                  const dccrg::Dccrg<spatial_cell::SpatialCell,dccrg::Cartesian_Geometry>& mpiGrid,
                  const std::vector<CellID>& cells,const std::string& meshName,
                  vlsv::Writer& vlsvWriter) {
   if (operatorID >= operators.size()) return false;
   DRO::DataReductionOperatorHandlesWriting* writingOperator = dynamic_cast<DRO::DataReductionOperatorHandlesWriting*>(operators[operatorID]);
   if(writingOperator == nullptr) {
      return false;
   }
   return writingOperator->writeData(mpiGrid,cells,meshName,vlsvWriter);
}

/** Write parameters related to given DataReductionOperator to the output file.
 * @param operatorID ID number of the selected DataReductionOperator.
 * @param vlsvWriter VLSV file writer that has output file open.
 * @return If true, DataReductionOperator wrote its parameters successfully.*/
bool DataReducer::writeParameters(const unsigned int& operatorID, vlsv::Writer& vlsvWriter) {
   if (operatorID >= operators.size()) return false;
   DRO::DataReductionOperatorHasParameters* parameterOperator = dynamic_cast<DRO::DataReductionOperatorHasParameters*>(operators[operatorID]);
   if(parameterOperator == nullptr) {
      return false;
   }
   return parameterOperator->writeParameters(vlsvWriter);
}
/** Write all data thet the given DataReductionOperator wants to obtain from fsgrid into the output file.
 */
bool DataReducer::writeFsGridData(
                      FsGrid< std::array<Real, fsgrids::bfield::N_BFIELD>, 2>& perBGrid,
                      FsGrid< std::array<Real, fsgrids::efield::N_EFIELD>, 2>& EGrid,
                      FsGrid< std::array<Real, fsgrids::ehall::N_EHALL>, 2>& EHallGrid,
                      FsGrid< std::array<Real, fsgrids::egradpe::N_EGRADPE>, 2>& EGradPeGrid,
                      FsGrid< std::array<Real, fsgrids::moments::N_MOMENTS>, 2>& momentsGrid,
                      FsGrid< std::array<Real, fsgrids::dperb::N_DPERB>, 2>& dPerBGrid,
                      FsGrid< std::array<Real, fsgrids::dmoments::N_DMOMENTS>, 2>& dMomentsGrid,
                      FsGrid< std::array<Real, fsgrids::bgbfield::N_BGB>, 2>& BgBGrid,
                      FsGrid< std::array<Real, fsgrids::volfields::N_VOL>, 2>& volGrid,
                      FsGrid< fsgrids::technical, 2>& technicalGrid, const std::string& meshName, const unsigned int operatorID, vlsv::Writer& vlsvWriter) {
   
   if (operatorID >= operators.size()) return false;
   DRO::DataReductionOperatorFsGrid* DROf = dynamic_cast<DRO::DataReductionOperatorFsGrid*>(operators[operatorID]);
   if(!DROf) {
      return false;
   } else {
      return DROf->writeFsGridData(perBGrid, EGrid, EHallGrid, EGradPeGrid, momentsGrid, dPerBGrid, dMomentsGrid, BgBGrid, volGrid, technicalGrid, meshName, vlsvWriter);
   }
}
