void PlotTool(){}

void MapsPlot(TString filename){
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
  gStyle->SetCanvasColor(33);
  gStyle->SetFrameFillColor(18);
  cout << "Opening ROOT file: " << filename <<endl; 

  TFile *f = TFile::Open(filename);
 
  if (f->IsZombie()) {
    cout << "Error opening file" << endl;
    return -1;
  }
  f->ls();

   
  TTree *mytree = (TTree *)f->Get("MapTree"); 
  DmpMapOrbit* dmpMapOrbit = new DmpMapOrbit();
  TBranch *b_dmpMapOrbit = mytree->GetBranch("DmpMapOrbit");
  b_dmpMapOrbit->SetAddress(&dmpMapOrbit);


  mytree->GetEntry(1);
  Int_t iLatSteps = dmpMapOrbit->LatSteps;
  Int_t iLonSteps = dmpMapOrbit->LonSteps;
  
  //cout << "iLatSteps " << iLatSteps <<endl; 
  //cout << "iLonSteps " << iLonSteps <<endl; 

  TH2D *h_BAbs_Map = new TH2D("h_BAbs_Map", "Map of Magnetic field in Gauss", iLonSteps,0., +360.,iLatSteps, -90., 90. ); 
  TH2D *h_VRC_Map = new TH2D("h_VRC_Map", "Map of Vertical Rigidity Cutoff", iLonSteps,0., +360.,iLatSteps, -90., 90. );
  
  


  Int_t nentries = (Int_t)mytree->GetEntries();
  for (Int_t i=0; i<nentries; i++) {
    mytree->GetEntry(i);
    //cout << "lat_geo: " << lat_geo << " lon_geo "<<lon_geo << endl;
    h_BAbs_Map->Fill(dmpMapOrbit->lon_geo, dmpMapOrbit->lat_geo, dmpMapOrbit->bAbs);
    h_VRC_Map->Fill(dmpMapOrbit->lon_geo, dmpMapOrbit->lat_geo,dmpMapOrbit->verticalRigidityCutoff );
  }


  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->Divide(1,2);
  c1->cd(1);
  h_BAbs_Map->DrawCopy("COLZ");
  c1->cd(2);
  h_VRC_Map->DrawCopy("COLZ");

  f->Close();

}
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

  cout << "Opening ROOT file: " << filename <<endl;

  gROOT->Reset();
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(33);
  gStyle->SetFrameFillColor(18);


  TFile *f = TFile::Open(filename);
  if (f->IsZombie()) {
    cout << "Error opening file" << endl;
    return -1;
  }
 
  f->ls();

  TTree *mytree = (TTree *)f->Get("OrbitTree"); 
  DmpEvtOrbit* dmpEvtOrbit = new DmpEvtOrbit();
  TBranch *b_dmpEvtOrbit = mytree->GetBranch("DmpEvtOrbit");
  b_dmpEvtOrbit->SetAddress(&dmpEvtOrbit);

  TH2D *h_scz_dir = new TH2D("h_scz_dir", "Z Axis", 360,0., +360.,90, -90., 90. );
  TH2D *h_scy_dir = new TH2D("h_scy_dir", "Y Axis", 360,0., +360.,90, -90., 90. );
  TH2D *h_scx_dir = new TH2D("h_scy_dir", "X Axis", 360,0., +360.,90, -90., 90. );
  TH2D *h_zenith_dir = new TH2D("h_zenith_dir", "X Axis", 360,0., +360.,90, -90., 90. );
  TH2D *h_sun_dir = new TH2D("h_sun_sir", "Sun", 360,0., +360.,90, -90., 90. );

  TH2D *h_sat_pos = new TH2D("h_sat_pos", "Satellite position", 360,0., +360.,90, -90., 90. );
  TH2D *h_sat_pos_B = new TH2D("h_sat_pos_B", "B(G) @ Satellite position", 360,0., +360.,90, -90., 90. );


 

  Int_t nentries = (Int_t)mytree->GetEntries();
  for (Int_t i=0; i<nentries; i++) {
    mytree->GetEntry(i);
    h_scz_dir->Fill(dmpEvtOrbit->ra_scz,dmpEvtOrbit->dec_scz);
    h_scy_dir->Fill(dmpEvtOrbit->ra_scy,dmpEvtOrbit->dec_scy);

    h_scx_dir->Fill(dmpEvtOrbit->ra_scx,dmpEvtOrbit->dec_scx);
   

    h_sun_dir->Fill(dmpEvtOrbit->SunLon,dmpEvtOrbit->SunLat);
    h_zenith_dir->Fill(dmpEvtOrbit->ra_zenith,dmpEvtOrbit->dec_zenith);

    h_sat_pos->Fill(dmpEvtOrbit->lon_geo,dmpEvtOrbit->lat_geo);
    h_sat_pos_B->Fill(dmpEvtOrbit->lon_geo,dmpEvtOrbit->lat_geo,dmpEvtOrbit->bAbs);

    //cout << rad_geo/1000. << endl;

  }


  TCanvas *c1 = new TCanvas("c1", "c1", 600, 600);
  c1->Divide(1,1);
  c1->cd(1);
  h_scz_dir->DrawCopy();

  h_scy_dir->SetMarkerColor(kRed);
  h_scy_dir->DrawCopy("SAME");

  h_scx_dir->SetMarkerColor(kBlue);    
  h_scx_dir->DrawCopy("SAME");

  h_sun_dir->SetMarkerColor(kYellow);  
  h_sun_dir->DrawCopy("SAME");

  TCanvas *c2 = new TCanvas("c2", "c2", 600, 600);
  c2->Divide(1,2);
  c2->cd(1);
  h_sat_pos->DrawCopy("");
  c2->cd(2);
  h_sat_pos_B->DrawCopy("COLZ");
  
  
  f->Close();

}
