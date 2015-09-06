void PlotTool(){}

void OrbitPlot(TString filename){
  // Tool to make some maps plots 
  
  // import library and classes
  
  TString myincludepath = new TString::TString();
  myincludepath.Append("-I");
  myincludepath.Append(gSystem->Getenv("DMPSWWORK"));
  myincludepath.Append("/include/");
  cout << "Include path " << myincludepath << endl;
  gSystem->AddIncludePath(myincludepath.Data());
  TString mylibpath = new TString::TString();
  mylibpath.Append(gSystem->Getenv("DMPSWWORK"));
  mylibpath.Append("/lib/libDmpMyEvt.so");
  cout << "Lib path " << mylibpath << endl;  
  gSystem->Load(mylibpath);

 
  gROOT->Reset(); 
  gStyle->SetOptStat(0); 
  gStyle->SetPalette(1);
  /*gStyle->SetCanvasColor(33);*/
  gStyle->SetFrameFillColor(18);
  cout << "Opening ROOT file: " << filename <<endl; 

  TFile *f = TFile::Open(filename);
 
  if (f->IsZombie()) {
    cout << "Error opening file" << endl;
    return -1;
  }
  f->ls();

   
  TTree *mytree = (TTree *)f->Get("OrbitTree"); 
  DmpEvtOrbit* dmpMapOrbit = new DmpEvtOrbit();
  TBranch *b_dmpMapOrbit = mytree->GetBranch("DmpEvtOrbit");
  b_dmpMapOrbit->SetAddress(&dmpMapOrbit);

  mytree->GetEntry(1);
  /*Int_t iLatSteps = dmpMapOrbit->LatSteps;*/
  /*Int_t iLonSteps = dmpMapOrbit->LonSteps;*/
  
  //cout << "iLatSteps " << iLatSteps <<endl;
  //cout << "iLonSteps " << iLonSteps <<endl;

  TH2D *h_BAbs_Hei = new TH2D("h_Hei_Map", "Map of heigh in m;Lon;Lat", 3600,0., 360.,1800, -90., 90.); 
  TH2D *h_BAbs_Map = new TH2D("h_BAbs_Map", "Map of Magnetic field in Gauss;Lon;Lat", 3600,0., 360.,1800, -90., 90.); 
  TH3D *h_BAbs_Map2 = new TH3D("h_BAbs_Map2", "Map of Magnetic field in Gauss;Lon;Lat;High", 360,0., 360.,180, -90., 90.,10000,450000,550000); 
  TH2D *h_VRC_Map = new TH2D("h_VRC_Map", "Map of Vertical Rigidity Cutoff;Lon;Lat", 3600,0., 360.,1800, -90., 90.);

  TH2D *h_scz_dir = new TH2D("h_scz_dir", "Z Axis;RA;Dec", 360,0., +360.,90, -90., 90. );
  TH2D *h_scy_dir = new TH2D("h_scy_dir", "Y Axis;RA;Dec", 360,0., +360.,90, -90., 90. );
  TH2D *h_scx_dir = new TH2D("h_scx_dir", "X Axis;RA;Dec", 360,0., +360.,90, -90., 90. );
  TH2D *h_zenith_dir = new TH2D("h_zenith_dir", "X Axis;RA;Dec", 360,0., +360.,90, -90., 90. );
  TH2D *h_sun_dir = new TH2D("h_sun_sir", "Sun;RA;Dec", 360,0., +360.,90, -90., 90. );

  Int_t nentries = (Int_t)mytree->GetEntries();
  double log=0;
  for (Int_t i=0; i<nentries; i++) {
    mytree->GetEntry(i);
    //cout << "lat_geo: " << lat_geo << " lon_geo "<<lon_geo << endl;
    log = dmpMapOrbit->lon_geo;
    /*if(log > 180) log = log - 360;*/
    h_BAbs_Map->Fill(log, dmpMapOrbit->lat_geo, dmpMapOrbit->bAbs);
    h_BAbs_Map2->Fill(log, dmpMapOrbit->lat_geo,dmpMapOrbit->rad_geo, dmpMapOrbit->bAbs);
    h_BAbs_Hei->Fill(log, dmpMapOrbit->lat_geo, dmpMapOrbit->rad_geo);
    /*if(dmpMapOrbit->rad_geo > 530000) std::cout<<"i = "<<i<<std::endl;*/
    h_VRC_Map->Fill(log, dmpMapOrbit->lat_geo,dmpMapOrbit->verticalRigidityCutoff );

    h_scz_dir->Fill(dmpMapOrbit->ra_scz,dmpMapOrbit->dec_scz);
    h_scy_dir->Fill(dmpMapOrbit->ra_scy,dmpMapOrbit->dec_scy);
    h_scx_dir->Fill(dmpMapOrbit->ra_scx,dmpMapOrbit->dec_scx);
    h_sun_dir->Fill(dmpMapOrbit->SunLon,dmpMapOrbit->SunLat);
    h_zenith_dir->Fill(dmpMapOrbit->ra_zenith,dmpMapOrbit->dec_zenith);
  }

  TCanvas *c1 = new TCanvas("BAbs", "BAbs");// 900, 600);
  /*c1->Divide(1,2);*/
  /*c1->(1);*/
  h_BAbs_Map->DrawCopy("COLZ");
  TCanvas *c2 = new TCanvas("VR", "VR", 900, 600);
  /*c1->cd(2);*/
  h_VRC_Map->DrawCopy("COLZ");
  TCanvas *c3 = new TCanvas("Hei", "Hei", 900, 600);
  h_BAbs_Hei->DrawCopy("COLZ");

  TCanvas *c0 = new TCanvas("dirction", "dir", 900, 600);
  h_scz_dir->DrawCopy();
  h_scy_dir->SetMarkerColor(kRed);
  h_scy_dir->DrawCopy("SAME");

  h_scx_dir->SetMarkerColor(kBlue);
  h_scx_dir->DrawCopy("SAME");

  h_sun_dir->SetMarkerColor(kGreen);
  h_sun_dir->DrawCopy("SAME");

  TCanvas *v0 = new TCanvas("3D map magnetic");
  h_BAbs_Map2->DrawCopy();

  /*h_zenith_dir->SetMarkerColor(kGreen);*/
  /*h_zenith_dir->DrawCopy("SAME");*/

  f->Close();

}

