// -*- C++ -*-
//
// Package:    Phase2OuterTracker
// Class:      Phase2OuterTracker
//
/**\class Phase2OuterTracker OuterTrackerMonitorTTCluster.cc DQM/Phase2OuterTracker/plugins/OuterTrackerMonitorTTCluster.cc
 
 Description: [one line class summary]
 
 Implementation:
 [Notes on implementation]
 */
//
// Original Author:  Isabelle Helena J De Bruyn
//         Created:  Mon, 10 Feb 2014 13:57:08 GMT
//

// system include files
#include <memory>
#include <vector>
#include <numeric>
#include <iostream>
#include <fstream>
#include <math.h>
#include "TMath.h"
#include "TNamed.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DQM/SiStripCommon/interface/SiStripFolderOrganizer.h"
#include "DQM/Phase2OuterTracker/interface/OuterTrackerMonitorTTCluster.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "DataFormats/L1TrackTrigger/interface/TTTypes.h"
#include "DataFormats/L1TrackTrigger/interface/TTCluster.h"

#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetType.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
//#include "Geometry/CommonDetUnit/interface/GeomDetType.h"
//#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"


//
// constructors and destructor
//
OuterTrackerMonitorTTCluster::OuterTrackerMonitorTTCluster(const edm::ParameterSet& iConfig)
: dqmStore_(edm::Service<DQMStore>().operator->()), conf_(iConfig)
{
  topFolderName_ = conf_.getParameter<std::string>("TopFolderName");
  tagTTClustersToken_ = consumes<edmNew::DetSetVector< TTCluster< Ref_Phase2TrackerDigi_ > > > (conf_.getParameter<edm::InputTag>("TTClusters") );
}

OuterTrackerMonitorTTCluster::~OuterTrackerMonitorTTCluster()
{
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//

// ------------ method called for each event  ------------
void OuterTrackerMonitorTTCluster::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  /// Track Trigger Clusters
  edm::Handle< edmNew::DetSetVector< TTCluster< Ref_Phase2TrackerDigi_ > > > Phase2TrackerDigiTTClusterHandle;
  iEvent.getByToken( tagTTClustersToken_, Phase2TrackerDigiTTClusterHandle );
  
  /// Geometry
  edm::ESHandle<TrackerTopology> tTopoHandle;
  const TrackerTopology* tTopo;
  iSetup.get< TrackerTopologyRcd >().get(tTopoHandle);
  tTopo = tTopoHandle.product();
  
//   edm::ESHandle< TrackerGeometry > tGeometryHandle;
//   const TrackerGeometry* theTrackerGeometry;
//   iSetup.get< TrackerDigiGeometryRecord >().get( tGeometryHandle );
//   theTrackerGeometry = tGeometryHandle.product();
  edm::ESHandle< TrackerGeometry > geomHandle;
  iSetup.get< TrackerDigiGeometryRecord >().get(geomHandle);
  const TrackerGeometry* tkGeom = &(*geomHandle);
  
  
  /// Loop over the input Clusters
  typename edmNew::DetSetVector< TTCluster< Ref_Phase2TrackerDigi_ > >::const_iterator inputIter;
  typename edmNew::DetSet< TTCluster< Ref_Phase2TrackerDigi_ > >::const_iterator contentIter;
  for ( inputIter = Phase2TrackerDigiTTClusterHandle->begin();
        inputIter != Phase2TrackerDigiTTClusterHandle->end();
        ++inputIter )
  {
    for(contentIter = inputIter->begin(); contentIter != inputIter->end(); ++contentIter)
    { 
      //Make reference cluster
      edm::Ref< edmNew::DetSetVector< TTCluster< Ref_Phase2TrackerDigi_ > >, TTCluster< Ref_Phase2TrackerDigi_ > > tempCluRef = edmNew::makeRefTo( Phase2TrackerDigiTTClusterHandle, contentIter );
      
      DetId detIdClu = tkGeom->idToDet( tempCluRef->getDetId() )->geographicalId();
      //DetId detIdClu(tempCluRef->getDetId());
      unsigned int memberClu = tempCluRef->getStackMember();
      unsigned int widClu = tempCluRef->findWidth();
      
      
      //MeasurementPoint mp = tempCluRef->findAverageLocalCoordinates();
      //const GeomDet* theDetUnit = tkGeom->idToDet(detIdClu.rawId());
      const GeomDet* theDetUnit = tkGeom->idToDet(detIdClu);
      LocalPoint localPos = tempCluRef->findAverageLocalPosition(theDetUnit);
      //Global3DPoint posClu = (theDetUnit)->surface().toGlobal( (theDetUnit)->topology().localPosition(mp) );
      Global3DPoint posClu = (theDetUnit)->surface().toGlobal( localPos );
      
      //GlobalPoint posClu  = theStacXXXkedGeometry->findAverageGlobalPosition( &(*tempCluRef) );
      
      //GlobalPoint posClu =  tempCluRef->findAverageGlobalPosition(theDetUnit);
      double eta = posClu.eta();
      
      
      // Replacement of memberClu? Does not work!
      //bool isLowerDet = tTopo->isLower(detIdClu);
      //bool isUpperDet = tTopo->isUpper(detIdClu);
      //const GeomDetUnit* detInner = theTrackerGeometry->idToDetUnit( tTopo->Lower(detIdClu) ); // not used atm
      //const GeomDetUnit* detOuter = theTrackerGeometry->idToDetUnit( tTopo->Upper(detIdClu) );
      
      
      Cluster_W->Fill(widClu, memberClu);
      Cluster_Eta->Fill(eta);
      
      Cluster_RZ->Fill( posClu.z(), posClu.perp() );
      
      if ( detIdClu.subdetId() == static_cast<int>(StripSubdetector::TOB) )  // Phase 2 Outer Tracker Barrel
      {
        
        if ( memberClu == 0 ) Cluster_IMem_Barrel->Fill(tTopo->layer(detIdClu));
        else Cluster_OMem_Barrel->Fill(tTopo->layer(detIdClu));
        
        Cluster_Barrel_XY->Fill( posClu.x(), posClu.y() );
        Cluster_Barrel_XY_Zoom->Fill( posClu.x(), posClu.y() );
        
      }	// end if isBarrel
      else if ( detIdClu.subdetId() == static_cast<int>(StripSubdetector::TID) )  // Phase 2 Outer Tracker Endcap
      {
        
        if ( memberClu == 0 )
        {
          Cluster_IMem_Endcap_Disc->Fill(tTopo->layer(detIdClu)); // returns wheel
          Cluster_IMem_Endcap_Ring->Fill(tTopo->tidRing(detIdClu));
        }
        else
        {
          Cluster_OMem_Endcap_Disc->Fill(tTopo->layer(detIdClu)); // returns wheel
          Cluster_OMem_Endcap_Ring->Fill(tTopo->tidRing(detIdClu));
        }
        
        if ( posClu.z() > 0 )
        {
          Cluster_Endcap_Fw_XY->Fill( posClu.x(), posClu.y() );
          Cluster_Endcap_Fw_RZ_Zoom->Fill( posClu.z(), posClu.perp() );
          if (memberClu == 0) Cluster_IMem_Endcap_Ring_Fw[tTopo->layer(detIdClu)-1]->Fill(tTopo->tidRing(detIdClu));
          else Cluster_OMem_Endcap_Ring_Fw[tTopo->layer(detIdClu)-1]->Fill(tTopo->tidRing(detIdClu));
        }
        else
        {
          Cluster_Endcap_Bw_XY->Fill( posClu.x(), posClu.y() );
          Cluster_Endcap_Bw_RZ_Zoom->Fill( posClu.z(), posClu.perp() );
          if (memberClu == 0) Cluster_IMem_Endcap_Ring_Bw[tTopo->layer(detIdClu)-1]->Fill(tTopo->tidRing(detIdClu));
          else Cluster_OMem_Endcap_Ring_Bw[tTopo->layer(detIdClu)-1]->Fill(tTopo->tidRing(detIdClu));
        }
        
      } // end if isEndcap
    } // end loop contentIter
  } // end loop inputIter
} // end of method

// ------------ method called once each job just before starting event loop  ------------
void
OuterTrackerMonitorTTCluster::beginRun(const edm::Run& run, const edm::EventSetup& es)
{
  SiStripFolderOrganizer folder_organizer;
  folder_organizer.setSiStripFolderName(topFolderName_);
  folder_organizer.setSiStripFolder();	
  std::string HistoName;
  
  dqmStore_->setCurrentFolder(topFolderName_+"/Clusters/NClusters");
  
  // NClusters
  edm::ParameterSet psTTCluster_Barrel =  conf_.getParameter<edm::ParameterSet>("TH1TTCluster_Barrel");
  HistoName = "NClusters_IMem_Barrel";
  Cluster_IMem_Barrel = dqmStore_->book1D(HistoName, HistoName,
      psTTCluster_Barrel.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Barrel.getParameter<double>("xmin"),
      psTTCluster_Barrel.getParameter<double>("xmax"));
  Cluster_IMem_Barrel->setAxisTitle("Barrel Layer", 1);
  Cluster_IMem_Barrel->setAxisTitle("# L1 Clusters", 2);
  
  HistoName = "NClusters_OMem_Barrel";
  Cluster_OMem_Barrel = dqmStore_->book1D(HistoName, HistoName,
      psTTCluster_Barrel.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Barrel.getParameter<double>("xmin"),
      psTTCluster_Barrel.getParameter<double>("xmax"));
  Cluster_OMem_Barrel->setAxisTitle("Barrel Layer", 1);
  Cluster_OMem_Barrel->setAxisTitle("# L1 Clusters", 2);
  
  edm::ParameterSet psTTCluster_ECDisc =  conf_.getParameter<edm::ParameterSet>("TH1TTCluster_ECDiscs");
  HistoName = "NClusters_IMem_Endcap_Disc";
  Cluster_IMem_Endcap_Disc = dqmStore_->book1D(HistoName, HistoName,
      psTTCluster_ECDisc.getParameter<int32_t>("Nbinsx"),
      psTTCluster_ECDisc.getParameter<double>("xmin"),
      psTTCluster_ECDisc.getParameter<double>("xmax"));
  Cluster_IMem_Endcap_Disc->setAxisTitle("Endcap Disc", 1);
  Cluster_IMem_Endcap_Disc->setAxisTitle("# L1 Clusters", 2);
  
  HistoName = "NClusters_OMem_Endcap_Disc";
  Cluster_OMem_Endcap_Disc = dqmStore_->book1D(HistoName, HistoName,
      psTTCluster_ECDisc.getParameter<int32_t>("Nbinsx"),
      psTTCluster_ECDisc.getParameter<double>("xmin"),
      psTTCluster_ECDisc.getParameter<double>("xmax"));
  Cluster_OMem_Endcap_Disc->setAxisTitle("Endcap Disc", 1);
  Cluster_OMem_Endcap_Disc->setAxisTitle("# L1 Clusters", 2);
  
  edm::ParameterSet psTTCluster_ECRing =  conf_.getParameter<edm::ParameterSet>("TH1TTCluster_ECRings");  
  HistoName = "NClusters_IMem_Endcap_Ring";
  Cluster_IMem_Endcap_Ring = dqmStore_->book1D(HistoName, HistoName,
      psTTCluster_ECRing.getParameter<int32_t>("Nbinsx"),
      psTTCluster_ECRing.getParameter<double>("xmin"),
      psTTCluster_ECRing.getParameter<double>("xmax"));
  Cluster_IMem_Endcap_Ring->setAxisTitle("Endcap Ring", 1);
  Cluster_IMem_Endcap_Ring->setAxisTitle("# L1 Clusters", 2);
  
  HistoName = "NClusters_OMem_Endcap_Ring";
  Cluster_OMem_Endcap_Ring = dqmStore_->book1D(HistoName, HistoName,
      psTTCluster_ECRing.getParameter<int32_t>("Nbinsx"),
      psTTCluster_ECRing.getParameter<double>("xmin"),
      psTTCluster_ECRing.getParameter<double>("xmax"));
  Cluster_OMem_Endcap_Ring->setAxisTitle("Endcap Ring", 1);
  Cluster_OMem_Endcap_Ring->setAxisTitle("# L1 Clusters", 2);
  
  for(int i=0;i<5;i++){
    Char_t histo[200];
    sprintf(histo, "NClusters_IMem_Disc+%d", i+1);  
    Cluster_IMem_Endcap_Ring_Fw[i] = dqmStore_ ->book1D(histo, histo, 
        psTTCluster_ECRing.getParameter<int32_t>("Nbinsx"), 
        psTTCluster_ECRing.getParameter<double>("xmin"), 
        psTTCluster_ECRing.getParameter<double>("xmax")); 
    Cluster_IMem_Endcap_Ring_Fw[i]->setAxisTitle("Endcap Ring",1); 
    Cluster_IMem_Endcap_Ring_Fw[i]->setAxisTitle("# L1 Clusters ",2);
  }
  
  for(int i=0;i<5;i++){
    Char_t histo[200];
    sprintf(histo, "NClusters_IMem_Disc-%d", i+1);  
    Cluster_IMem_Endcap_Ring_Bw[i] = dqmStore_ ->book1D(histo, histo, 
        psTTCluster_ECRing.getParameter<int32_t>("Nbinsx"), 
        psTTCluster_ECRing.getParameter<double>("xmin"), 
        psTTCluster_ECRing.getParameter<double>("xmax")); 
    Cluster_IMem_Endcap_Ring_Bw[i]->setAxisTitle("Endcap Ring",1); 
    Cluster_IMem_Endcap_Ring_Bw[i]->setAxisTitle("# L1 Clusters ",2);
  }
  
  for(int i=0;i<5;i++){
    Char_t histo[200];
    sprintf(histo, "NClusters_OMem_Disc+%d", i+1);  
    Cluster_OMem_Endcap_Ring_Fw[i] = dqmStore_ ->book1D(histo, histo, 
        psTTCluster_ECRing.getParameter<int32_t>("Nbinsx"), 
        psTTCluster_ECRing.getParameter<double>("xmin"), 
        psTTCluster_ECRing.getParameter<double>("xmax")); 
    Cluster_OMem_Endcap_Ring_Fw[i]->setAxisTitle("Endcap Ring",1); 
    Cluster_OMem_Endcap_Ring_Fw[i]->setAxisTitle("# L1 Clusters ",2);
  }
  
  for(int i=0;i<5;i++){
    Char_t histo[200];
    sprintf(histo, "NClusters_OMem_Disc-%d", i+1);  
    Cluster_OMem_Endcap_Ring_Bw[i] = dqmStore_ ->book1D(histo, histo, 
        psTTCluster_ECRing.getParameter<int32_t>("Nbinsx"), 
        psTTCluster_ECRing.getParameter<double>("xmin"), 
        psTTCluster_ECRing.getParameter<double>("xmax")); 
    Cluster_OMem_Endcap_Ring_Bw[i]->setAxisTitle("Endcap Ring",1); 
    Cluster_OMem_Endcap_Ring_Bw[i]->setAxisTitle("# L1 Clusters ",2);
  }
  
  
  dqmStore_->setCurrentFolder(topFolderName_+"/Clusters");
        
  //Cluster Width
  edm::ParameterSet psTTClusterWidth =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_Width");
  HistoName = "Cluster_W";
  Cluster_W = dqmStore_->book2D(HistoName, HistoName,
      psTTClusterWidth.getParameter<int32_t>("Nbinsx"),
      psTTClusterWidth.getParameter<double>("xmin"),
      psTTClusterWidth.getParameter<double>("xmax"),
      psTTClusterWidth.getParameter<int32_t>("Nbinsy"),
      psTTClusterWidth.getParameter<double>("ymin"),
      psTTClusterWidth.getParameter<double>("ymax"));
  Cluster_W->setAxisTitle("L1 Cluster Width", 1);
  Cluster_W->setAxisTitle("Stack Member", 2);
  
  //Cluster eta distribution
  edm::ParameterSet psTTClusterEta = conf_.getParameter<edm::ParameterSet>("TH1TTCluster_Eta");
  HistoName = "Cluster_Eta";
  Cluster_Eta = dqmStore_->book1D(HistoName, HistoName, 
      psTTClusterEta.getParameter<int32_t>("Nbinsx"),
      psTTClusterEta.getParameter<double>("xmin"),
      psTTClusterEta.getParameter<double>("xmax"));
  Cluster_Eta->setAxisTitle("#eta", 1);
  Cluster_Eta->setAxisTitle("# L1 Clusters", 2);
  
  dqmStore_->setCurrentFolder(topFolderName_+"/Clusters/Position");
  
  //Position plots
  edm::ParameterSet psTTCluster_Barrel_XY =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_Position");
  HistoName = "Cluster_Barrel_XY";
  Cluster_Barrel_XY = dqmStore_->book2D(HistoName, HistoName,
      psTTCluster_Barrel_XY.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Barrel_XY.getParameter<double>("xmin"),
      psTTCluster_Barrel_XY.getParameter<double>("xmax"),
      psTTCluster_Barrel_XY.getParameter<int32_t>("Nbinsy"),
      psTTCluster_Barrel_XY.getParameter<double>("ymin"),
      psTTCluster_Barrel_XY.getParameter<double>("ymax"));
  Cluster_Barrel_XY->setAxisTitle("L1 Cluster Barrel position x [cm]", 1);
  Cluster_Barrel_XY->setAxisTitle("L1 Cluster Barrel position y [cm]", 2);
  
  edm::ParameterSet psTTCluster_Barrel_XY_Zoom =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_Barrel_XY_Zoom");
  HistoName = "Cluster_Barrel_XY_Zoom";
  Cluster_Barrel_XY_Zoom = dqmStore_->book2D(HistoName, HistoName,
      psTTCluster_Barrel_XY_Zoom.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Barrel_XY_Zoom.getParameter<double>("xmin"),
      psTTCluster_Barrel_XY_Zoom.getParameter<double>("xmax"),
      psTTCluster_Barrel_XY_Zoom.getParameter<int32_t>("Nbinsy"),
      psTTCluster_Barrel_XY_Zoom.getParameter<double>("ymin"),
      psTTCluster_Barrel_XY_Zoom.getParameter<double>("ymax"));
  Cluster_Barrel_XY_Zoom->setAxisTitle("L1 Cluster Barrel position x [cm]", 1);
  Cluster_Barrel_XY_Zoom->setAxisTitle("L1 Cluster Barrel position y [cm]", 2);
  
  edm::ParameterSet psTTCluster_Endcap_Fw_XY =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_Position");
  HistoName = "Cluster_Endcap_Fw_XY";
  Cluster_Endcap_Fw_XY = dqmStore_->book2D(HistoName, HistoName,
      psTTCluster_Endcap_Fw_XY.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Endcap_Fw_XY.getParameter<double>("xmin"),
      psTTCluster_Endcap_Fw_XY.getParameter<double>("xmax"),
      psTTCluster_Endcap_Fw_XY.getParameter<int32_t>("Nbinsy"),
      psTTCluster_Endcap_Fw_XY.getParameter<double>("ymin"),
      psTTCluster_Endcap_Fw_XY.getParameter<double>("ymax"));
  Cluster_Endcap_Fw_XY->setAxisTitle("L1 Cluster Forward Endcap position x [cm]", 1);
  Cluster_Endcap_Fw_XY->setAxisTitle("L1 Cluster Forward Endcap position y [cm]", 2);
  
  edm::ParameterSet psTTCluster_Endcap_Bw_XY =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_Position");
  HistoName = "Cluster_Endcap_Bw_XY";
  Cluster_Endcap_Bw_XY = dqmStore_->book2D(HistoName, HistoName,
      psTTCluster_Endcap_Bw_XY.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Endcap_Bw_XY.getParameter<double>("xmin"),
      psTTCluster_Endcap_Bw_XY.getParameter<double>("xmax"),
      psTTCluster_Endcap_Bw_XY.getParameter<int32_t>("Nbinsy"),
      psTTCluster_Endcap_Bw_XY.getParameter<double>("ymin"),
      psTTCluster_Endcap_Bw_XY.getParameter<double>("ymax"));
  Cluster_Endcap_Bw_XY->setAxisTitle("L1 Cluster Backward Endcap position x [cm]", 1);
  Cluster_Endcap_Bw_XY->setAxisTitle("L1 Cluster Backward Endcap position y [cm]", 2);
  
  //TTCluster #rho vs. z
  edm::ParameterSet psTTCluster_RZ =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_RZ");
  HistoName = "Cluster_RZ";
  Cluster_RZ = dqmStore_->book2D(HistoName, HistoName,
      psTTCluster_RZ.getParameter<int32_t>("Nbinsx"),
      psTTCluster_RZ.getParameter<double>("xmin"),
      psTTCluster_RZ.getParameter<double>("xmax"),
      psTTCluster_RZ.getParameter<int32_t>("Nbinsy"),
      psTTCluster_RZ.getParameter<double>("ymin"),
      psTTCluster_RZ.getParameter<double>("ymax"));
  Cluster_RZ->setAxisTitle("L1 Cluster position z [cm]", 1);
  Cluster_RZ->setAxisTitle("L1 Cluster position #rho [cm]", 2);
  
  //TTCluster Forward Endcap #rho vs. z
  edm::ParameterSet psTTCluster_Endcap_Fw_RZ_Zoom =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_Endcap_Fw_RZ_Zoom");
  HistoName = "Cluster_Endcap_Fw_RZ_Zoom";
  Cluster_Endcap_Fw_RZ_Zoom = dqmStore_->book2D(HistoName, HistoName,
      psTTCluster_Endcap_Fw_RZ_Zoom.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Endcap_Fw_RZ_Zoom.getParameter<double>("xmin"),
      psTTCluster_Endcap_Fw_RZ_Zoom.getParameter<double>("xmax"),
      psTTCluster_Endcap_Fw_RZ_Zoom.getParameter<int32_t>("Nbinsy"),
      psTTCluster_Endcap_Fw_RZ_Zoom.getParameter<double>("ymin"),
      psTTCluster_Endcap_Fw_RZ_Zoom.getParameter<double>("ymax"));
  Cluster_Endcap_Fw_RZ_Zoom->setAxisTitle("L1 Cluster Forward Endcap position z [cm]", 1);
  Cluster_Endcap_Fw_RZ_Zoom->setAxisTitle("L1 Cluster Forward Endcap position #rho [cm]", 2);
  
  //TTCluster Backward Endcap #rho vs. z
  edm::ParameterSet psTTCluster_Endcap_Bw_RZ_Zoom =  conf_.getParameter<edm::ParameterSet>("TH2TTCluster_Endcap_Bw_RZ_Zoom");
  HistoName = "Cluster_Endcap_Bw_RZ_Zoom";
  Cluster_Endcap_Bw_RZ_Zoom = dqmStore_->book2D(HistoName, HistoName,
      psTTCluster_Endcap_Bw_RZ_Zoom.getParameter<int32_t>("Nbinsx"),
      psTTCluster_Endcap_Bw_RZ_Zoom.getParameter<double>("xmin"),
      psTTCluster_Endcap_Bw_RZ_Zoom.getParameter<double>("xmax"),
      psTTCluster_Endcap_Bw_RZ_Zoom.getParameter<int32_t>("Nbinsy"),
      psTTCluster_Endcap_Bw_RZ_Zoom.getParameter<double>("ymin"),
      psTTCluster_Endcap_Bw_RZ_Zoom.getParameter<double>("ymax"));
  Cluster_Endcap_Bw_RZ_Zoom->setAxisTitle("L1 Cluster Backward Endcap position z [cm]", 1);
  Cluster_Endcap_Bw_RZ_Zoom->setAxisTitle("L1 Cluster Backward Endcap position #rho [cm]", 2);
                                  
}//end of method

// ------------ method called once each job just after ending the event loop  ------------
void 
OuterTrackerMonitorTTCluster::endJob(void) 
{
	
}

DEFINE_FWK_MODULE(OuterTrackerMonitorTTCluster);
