// Main program for OrbSim

//#include "include/Functions.h"


#include <iostream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <cstdlib>

#include <TFile.h>
#include <TTree.h>



#include "../include/functions.h"
#include "../include/atFunctions.h"
#include "../include/read_ephem.h"
#include "../include/OrbSim.h"



#include "../include/IniReader.h"

#include "DmpEvtOrbit.h"  
#include "DmpMapOrbit.h"  
//class DmpEvtOrbit;

#include "../include/IGRField.h"
using namespace astro;
using namespace std;


///////////////////////////////////////////////////////////////
// interface to ppfrag fortran routines in igrf_sub.f
//extern "C" void   INITIZE_();
///////////////////////////////////////////////////////////////



int main(int argc, char** argv)
{

  // READ INIT FILE

  char* Ini_Filename = argv[1];// = "/lustrehome/gargano/DAMPE/DAMPESW/DmpSoftware/branches/I_0.0_OrbitSimulation/test/DAMPE.ini";
  std::cout << "Reading Ini_FileName : " <<  Ini_Filename<< std::endl;
 
  char* EphemFileName = IniReadString(Ini_Filename, "EphemName", "./DAMPE.tle");
  //std::cout << "Ephem File : " << EphemFileName << std::endl;
  
  double start_MJD = IniReadDouble(Ini_Filename, "Start_MJD", 57388); 
  double stop_MJD = IniReadDouble(Ini_Filename, "Stop_MJD",57389);

  double Ira = IniReadDouble(Ini_Filename, "Ira",0.);   // NOTE: for DAMPE, decending node is 317.2335
  double Idec = IniReadDouble(Ini_Filename, "Idec",0.);
  double Units = IniReadDouble(Ini_Filename, "Units",1.);
  double Resolution = IniReadDouble(Ini_Filename, "Resolution",1.);
  char* TLname =  IniReadString(Ini_Filename, "TimelnCmd", "|SURVEY|+0.1|");
  char* saafile = IniReadString(Ini_Filename, "saafile", "./L_SAA_2008198.03");   // NOTE: satellite accumulation area
  std::cout <<  "saafile: " << saafile << std::endl;
  char* RootFileName = IniReadString(Ini_Filename, "RootFileName", Form("./Orbit_MJD%g_%g_R%g.root",start_MJD,stop_MJD,Resolution));
  char* IGRFDataPath= IniReadString(Ini_Filename, "IGRFDataPath", "./data/");

  bool BuildMap = IniReadBool(Ini_Filename, "BuildMap", false);

  int latSteps = IniReadInt(Ini_Filename, "LatMapNStep",720);
  int lonSteps = IniReadInt(Ini_Filename, "LonMapNStep",360);
  double RMap = IniReadDouble(Ini_Filename, "RMap",500);    // NOTE: fixed value??
  const char* SATname = IniReadString(Ini_Filename, "SATname", "DAMPE");
  std::cout << "SATname " << SATname << std::endl; 


  AtVect vSun;
  AtPolarVect pSun;

  //AtVect vSat;
  //AtPolarVect pSat;

  AtVect vSatX;
  AtPolarVect pSatX;
  //AtVect vSatRX;

  AtVect vSatY;
  AtPolarVect pSatY;  
  //AtVect vSatRY;

  AtVect vSatZ;
  //AtVect vSatZInv;

  AtPolarVect pSatZ;  
  //AtVect vSatRZ;

  AtVect vSatZenith;
  AtPolarVect pSatZenith;  
  


  InitI initf;  // NOTE: ../include/orbitSimStruct.h
  /// starting time in MJD
  initf.start_MJD =start_MJD;
  /// stopping time in MJD
  initf.stop_MJD =stop_MJD;
  /// SATname Satellite Name as is reported in tle file
  initf.SATname = SATname; 
  /// Ephemeris file name, path included  
  initf.EPHname = EphemFileName;
  /// function name to read/propagate ephemeris   
  initf.EPHfunc = "tlederive"; // fixed since is the only implemented way to pass DAMPE Ephemeris
  /// Timeline type (TAKO, ASFLOWN, or SINGLE)
  initf.TLtype = "SURVAY"; // fixed since is the only way implemented to simulate DAMPE Attitude
  /// Initial RA                    
  initf.Ira = Ira;   
  /// Initial DEC      
  initf.Idec =Idec; 
  /// conversion factor to convert in Km    
  initf.Units = Units;  
  /// Ephemeris resolution, and therefore   
  /// orbit simulator resolution in minutes
  initf.Resolution = Resolution;
  /// Timeline file name, path included
  initf.TLname = TLname;
  /// SAA file definition    
  initf.saafile = saafile ;
  std::cout <<  "saafile: " << saafile << std::endl;
  int yy, MM, dd, hh, mm, ss;
  do_mjd2cal(initf.start_MJD, &yy, &MM, &dd, &hh, &mm, &ss);
  printf("\n Start Date: %4d-%02d-%02dT%02d:%02d:%02d \n", yy, MM, dd, hh, mm, ss);
  do_mjd2cal(initf.stop_MJD, &yy, &MM, &dd, &hh, &mm, &ss);
  printf("\n Stop Date: %4d-%02d-%02dT%02d:%02d:%02d \n", yy, MM, dd, hh, mm, ss);


  // Add a little buffer to the end time so we calculate enough ephem
  // and attitude point to get positions up to the end of the desired time.
  //double stop_buffer = 0.1;     // TODO
  //initf.stop_MJD=initf.stop_MJD+stop_buffer;

  // Fixing the start time according to the resolution
  double stmjd = initf.start_MJD;
  double fday = (stmjd - (double)((int)stmjd))*minInDay;
  fday = (double)((int)(fday/initf.Resolution)-1)*initf.Resolution;
  stmjd = (double)((int)stmjd)+fday/minInDay;

  if((initf.start_MJD - stmjd)- initf.Resolution > 1.0E-6){
    std::cout <<"Initial start time=" << initf.start_MJD << ", corrected time=" << stmjd  << std::endl;
    initf.start_MJD = stmjd; 
  }
  
  // Also fix the stop time based on resolution.
  double enmjd = initf.stop_MJD;
  fday = (enmjd - (double)((int)enmjd))*minInDay;
  fday = (double)((int)(fday/initf.Resolution)+1)*initf.Resolution;
  enmjd = (double)((int)enmjd)+fday/minInDay;

  if((initf.stop_MJD - enmjd)- initf.Resolution > 1.0E-6){
    std::cout  <<"Initial stop time=" << initf.stop_MJD << ", corrected time=" << enmjd  << std::endl;
    initf.stop_MJD = enmjd;
  }
  

  initf.Resolution = initf.Resolution/minInDay; // conversion of resolution in fraction of day
  

  // EPHEM DEFINTION FROM TLE
  FILE * ephF = NULL;
  if ((ephF=fopen(initf.EPHname.c_str(),"r")) == NULL){
    std::cout <<"Could not open Ephemeredis file" << std::endl;
    return -1;
  }
  EphemData *ephemeris = tlederive(ephF, initf.start_MJD,initf.stop_MJD , initf.Units, initf.Resolution,initf.SATname );       // NOTE: important! src/read_ephem
  fclose(ephF);

  if (ephemeris == NULL){
    std::cout << "\nPossibly something went wrong while reading/generating ephemeris.\nThe Ephemeris structure is still \"NULL\"\n\n" << std::endl;
    return -1;
  }
  // just for test
  std::cout<<"DEBUG: "<<__FILE__<<"("<<__LINE__<<")\tstop mjd = "<<initf.stop_MJD<<"\tstart mjd = "<<initf.start_MJD<<std::endl;
  std::cout << "From Ephem file, start mjd = " << ephemeris->MJD[0] <<  std::endl;

  // ATTITUDE DEFINITION
  //Make an empty Attitude structure Oat. 
  Attitude *Oat = doCmd(&initf, ephemeris);       // NOTE:  important!! OrbSim.h
  // Restore stop_MJD to its correct, but smaller value
  //initf.stop_MJD=initf.stop_MJD-stop_buffer;
  if(Oat == NULL){
    std::cout << "\nPossibly something went wrong while calculating the spacecraft attitude.\nThe Attitude structure is still \"NULL\"\n\n" << std::endl;
    return -1;
  }  

  // Output root file

  TFile *fout = new TFile(RootFileName,"recreate");
  std::cout << "Opening Root File " << RootFileName << std::endl; 

  DmpEvtOrbit *dmpEvtOrbit =new DmpEvtOrbit()  ;
  dmpEvtOrbit->Reset();
  TTree *OrbitTree = new TTree("OrbitTree","Tree with orbit parameters");
  OrbitTree->Branch("DmpEvtOrbit",  "DmpEvtOrbit",     &dmpEvtOrbit  );
  
  // create the branches and assign the fill-variables to them
  // Int_t ev;
  // Double_t lat_geo, lon_geo, rad_geo, ra_zenith, dec_zenith, ra_scz, dec_scz, ra_scx, dec_scx, ra_scy, dec_scy,modjuldate;
  // Double_t pos[3];
  
  // UInt_t stop,start;
  //Double_t dipoleMoment, L,invariantLatitude;
  //Double_t bEast, bNorth, bDown, bAbs, bEquator, R, verticalRigidityCutoff; 
  // Double_t BVect[3];
  // Double_t SunLat, SunLon;
  

  //fill the tree
  for (Int_t i=0; i<Oat->ent; i++) {

    dmpEvtOrbit->stop = (UInt_t)((Oat->mjd[i]-MJDREF)*secInDay+0.5);
    dmpEvtOrbit->start = dmpEvtOrbit->stop - (UInt_t)(initf.Resolution*secInDay);
    
    dmpEvtOrbit->mjd = Oat->mjd[i];
    dmpEvtOrbit->met = do_mjd2met(Oat->mjd[i]); // NOTE: here is FERMI lauch time 2001-01-01
    //std::cout << " MJD " << Oat->mjd[i] << " "  << dmpEvtOrbit->mjd << std::endl;
    //std::cout << " MJD " << Oat->mjd[i] << std::endl;
    dmpEvtOrbit->ev = i;


    dmpEvtOrbit->lat_geo = Oat->Lat[i];
    dmpEvtOrbit->lon_geo = Oat->Lon[i];
    dmpEvtOrbit->rad_geo = Oat->Hei[i]*1000.0;

    //pSat.lat = dmpEvtOrbit->lat_geo;
    //pSat.lon = dmpEvtOrbit->lon_geo;
    //pSat.r = dmpEvtOrbit->rad_geo;
 

    dmpEvtOrbit->pos[0]=Oat->X[i]*1000.0;
    dmpEvtOrbit->pos[1]=Oat->Y[i]*1000.0;
    dmpEvtOrbit->pos[2]=Oat->Z[i]*1000.0;



    dmpEvtOrbit->ra_zenith = Oat->SatRA[i];
    dmpEvtOrbit->dec_zenith = Oat->SatDEC[i];

    pSatZenith.lon = Oat->SatRA[i]*DEG2RAD;
    pSatZenith.lat =  Oat->SatDEC[i]*DEG2RAD;
    pSatZenith.r = 1; // fake only for direction purposes
    atPolToVect(&pSatZenith,vSatZenith);
   

    pSatZ.lon = Oat->Zra[i]*DEG2RAD;
    pSatZ.lat =  Oat->Zdec[i]*DEG2RAD;
    pSatZ.r = 1; // fake only for direction purposes
    atPolToVect(&pSatZ,vSatZ);
   
    pSatX.lon = Oat->Xra[i]*DEG2RAD;
    pSatX.lat =  Oat->Xdec[i]*DEG2RAD;
    pSatX.r = 1; // fake only for direction purposes
    atPolToVect(&pSatX,vSatX);
  
    pSatY.lon = Oat->Yra[i]*DEG2RAD;
    pSatY.lat =  Oat->Ydec[i]*DEG2RAD;
    pSatY.r = 1; // fake only for direction purposes
    atPolToVect(&pSatY,vSatY);



    dmpEvtOrbit->ra_scz = pSatZ.lon*RAD2DEG;
    dmpEvtOrbit->dec_scz = pSatZ.lat*RAD2DEG;
    
    dmpEvtOrbit->ra_scx = pSatX.lon*RAD2DEG;
    dmpEvtOrbit->dec_scx = pSatX.lat*RAD2DEG;
              
    dmpEvtOrbit->ra_scy = pSatY.lon*RAD2DEG;
    dmpEvtOrbit->dec_scy = pSatY.lat*RAD2DEG;



        
    // Geomagnetic  section
    // the year is fixed at 2015 since is the last valid yesr for IGRF at the moment
    IGRField::Model().compute(dmpEvtOrbit->lat_geo, dmpEvtOrbit->lon_geo, dmpEvtOrbit->rad_geo, 2015, IGRFDataPath); // NOTE: important
   
    dmpEvtOrbit->dipoleMoment = IGRField::Model().dipoleMoment(); // Get dipole moment    
    dmpEvtOrbit->L = IGRField::Model().L(); // Get McIlwain L
    //B = IGRField::Model().B(); // Get B/B0 bAbs/bEquator
    /*BNORTH, BEAST, BDOWN   COMPONENTS OF THE FIELD WITH RESPECT */
    /*                 TO THE LOCAL GEODETIC COORDINATE SYSTEM, WITH AXIS */
    /*                 POINTING IN THE TANGENTIAL PLANE TO THE NORTH, EAST */
    /*                 AND DOWNWARD. */
    dmpEvtOrbit->bEast = IGRField::Model().bEast();// Get B_East in G
    dmpEvtOrbit->bNorth = IGRField::Model().bNorth();// Get B_North in G 
    dmpEvtOrbit->bDown = IGRField::Model().bDown();// Get B_Down in G 
   
    dmpEvtOrbit->BVect[0] = IGRField::Model().bX();// Get B_X in G
    dmpEvtOrbit->BVect[1] = IGRField::Model().bY();// Get B_Y in G 
    dmpEvtOrbit->BVect[2] = IGRField::Model().bZ();// Get B_Z in G 


    /* BABS  MAGNETIC FIELD STRENGTH IN GAUSS  */
    dmpEvtOrbit->bAbs = IGRField::Model().bAbs();// Get absolute value of B
    dmpEvtOrbit->bEquator = IGRField::Model().bEquator();// Get absolute value of B at the equator
    dmpEvtOrbit->R = IGRField::Model().R();// Get R alternative from http://www.spenvis.oma.be/spenvis/help/background/magfield/rlambda.html
    dmpEvtOrbit->verticalRigidityCutoff  = IGRField::Model().verticalRigidityCutoff();// Get vertical rigidity cutoff
    dmpEvtOrbit->invariantLatitude = IGRField::Model().invariantLatitude();// Get invariant latitude


    
    // Solar position section
    // evaluate sun position in cartesina
    atSun(Oat->mjd[i],vSun);   
    //    std::cout << "Sun x " << vSun[0] << "Sun y " << vSun[1] << " Sun z " << vSun[2] <<std::endl;
    // transform in polar
    atVectToPol( vSun, &pSun);
    //std::cout << "Sun lon " << pSun.lon*180/3.14 << "Sun lat " << pSun.lat*180/3.14 << " Sun R " << pSun.r <<std::endl;

    dmpEvtOrbit->SunLon = pSun.lon*RAD2DEG;
    dmpEvtOrbit->SunLat = pSun.lat *RAD2DEG;


    OrbitTree->Fill();
  }

  if(BuildMap){
    DmpMapOrbit *dmpMapOrbit =new DmpMapOrbit()  ;
    //Int_t LatSteps,LonSteps;

    TTree *MapTree = new TTree("MapTree","Tree with parameters to build geomagnetic maps");
    MapTree->Branch("DmpMapOrbit",  "DmpMapOrbit",     &dmpMapOrbit  );

   
      
    for (Int_t lat=0; lat<=latSteps; lat++) {
      for (Int_t lon=0; lon<=lonSteps; lon++) {
	
	dmpMapOrbit->LatSteps = latSteps;
	dmpMapOrbit->LonSteps = lonSteps;
	dmpMapOrbit->lat_geo = lat*180./latSteps - 90;
	dmpMapOrbit->lon_geo = lon*360./lonSteps;
	dmpMapOrbit->rad_geo = RMap;
	//std::cout << "lat " << lat_geo << " lon " << lon_geo << std::endl;  
	// Geomagnetic  section
	// the year is fixed at 2015 since is the last valid yesr for IGRF at the moment
	IGRField::Model().compute(dmpMapOrbit->lat_geo, dmpMapOrbit->lon_geo, dmpMapOrbit->rad_geo, 2015, IGRFDataPath);
	
	dmpMapOrbit->dipoleMoment = IGRField::Model().dipoleMoment(); // Get dipole moment    
	dmpMapOrbit->L = IGRField::Model().L(); // Get McIlwain L
	//B = IGRField::Model().B(); // Get B/B0 bAbs/bEquator
	/*BNORTH, BEAST, BDOWN   COMPONENTS OF THE FIELD WITH RESPECT */
	/*                 TO THE LOCAL GEODETIC COORDINATE SYSTEM, WITH AXIS */
	/*                 POINTING IN THE TANGENTIAL PLANE TO THE NORTH, EAST */
	/*                 AND DOWNWARD. */
	dmpMapOrbit->bEast = IGRField::Model().bEast();// Get B_East
	dmpMapOrbit->bNorth = IGRField::Model().bNorth();// Get B_North
	dmpMapOrbit->bDown = IGRField::Model().bDown();// Get B_Down
	dmpMapOrbit->BVect[0] = IGRField::Model().bX();// Get B_X in G
	dmpMapOrbit->BVect[1] = IGRField::Model().bY();// Get B_Y in G 
	dmpMapOrbit->BVect[2] = IGRField::Model().bZ();// Get B_Z in G 
	/* BABS  MAGNETIC FIELD STRENGTH IN GAUSS  */
	dmpMapOrbit->bAbs = IGRField::Model().bAbs();// Get absolute value of B
	
	dmpMapOrbit->verticalRigidityCutoff  = IGRField::Model().verticalRigidityCutoff();// Get vertical rigidity cutoff
	dmpMapOrbit->invariantLatitude = IGRField::Model().invariantLatitude();// Get invariant latitude
	MapTree->Fill();
	
      }
    }
    MapTree->Write();
  }
  
  OrbitTree->Write();
  fout->Close();
  std::cout << "Closing Root File " << RootFileName << std::endl; 
 

  return 0;

}
