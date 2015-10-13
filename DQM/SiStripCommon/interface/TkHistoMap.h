#ifndef DQM_SiStripCommon_TKHistoMap_h
#define DQM_SiStripCommon_TKHistoMap_h

#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "CalibTracker/Records/interface/SiStripDetCablingRcd.h"
#include "CalibFormats/SiStripObjects/interface/SiStripDetCabling.h"

#include "CalibTracker/SiStripCommon/interface/TkDetMap.h"
#include "CommonTools/TrackerMap/interface/TrackerMap.h"
#include <string>
#include "PolyUtil.h"

class ValueClass{
 private: 
  int number;
  float value;
  Int_t binNumber;
 public:
  ValueClass(){
    number=0;
    value=0;
    binNumber=0;

  }
  int getNumber(){
    return number;
  }
  float getValue(){
    return value;
  }
  void setNumber(int n){
    number=n; 
  }
  void setValue(float v){
    value=v;
  }
  void setBinNumber(Int_t b){
    binNumber=b;
  }
  Int_t getBinNumber(){
    return binNumber;
  }
};


class TkHistoMap{

  typedef std::vector<MonitorElement*> tkHistoMapVect;

 public:
  TkHistoMap(DQMStore::IBooker & ibooker , std::string path, std::string MapName, PolyUtil * pu,const edm::EventSetup& es, float baseline=0, bool mechanicalView=false);
  TkHistoMap(std::string path, std::string MapName, float baseline=0, bool mechanicalView=false);
  TkHistoMap();
  ~TkHistoMap(){};
  void initializeMap(const edm::EventSetup& es);
  void loadServices();

  void loadTkHistoMap(std::string path, std::string MapName, bool mechanicalView=false);

  MonitorElement* getMap(short layerNumber){return tkHistoMap_[layerNumber];};
  std::vector<MonitorElement*>& getAllMaps(){return tkHistoMap_;};

  float getValue(uint32_t& detid);
  float getEntries(uint32_t& detid);
  uint32_t getDetId(std::string title, int ix, int iy){return getDetId(getLayerNum(getLayerName(title)),ix,iy);}
  uint32_t getDetId(int layer, int ix, int iy){return tkdetmap_->getDetFromBin(layer,ix,iy);}
  uint32_t getDetId(MonitorElement*ME, int ix, int iy){return getDetId(ME->getTitle(),ix,iy);}
  std::string getLayerName(std::string title){return title.erase(0,MapName_.size()+1);}
  uint16_t getLayerNum(std::string layerName){return tkdetmap_->getLayerNum(layerName);}

  void fillFromAscii(std::string filename);
  void fill(uint32_t& detid,float value);
  void fillPoly(uint32_t detid , float value,PolyUtil* pu );
  void setBinContent(uint32_t& detid,float value);
  void add(uint32_t& detid,float value);

  void dumpInTkMap(TrackerMap* tkmap, bool dumpEntries=false); //dumpEntries==true? (dump entries) : (dump mean values)
  void save(std::string filename);
  void saveAsCanvas(std::string filename,std::string options="", std::string mode="RECREATE");

 private:

  //fixme: keep single method
  void createTkHistoMap(std::string& path, std::string& MapName, float& baseline, bool mechanicalView);
  void createTkHistoMap(DQMStore::IBooker & ibooker , std::string& path, std::string& MapName, float& baseline, bool mechanicalView,PolyUtil* pu);
  std::map<uint32_t,ValueClass> valueMap;
  std::string folderDefinition(std::string& path, std::string& MapName, int layer , bool mechanicalView, std::string& fullName);

  edm::ESHandle<SiStripDetCabling> SiStripDetCabling_;
  DQMStore* dqmStore_;
  TkDetMap* tkdetmap_;
  uint32_t cached_detid;
  int16_t cached_layer;
  TkLayerMap::XYbin cached_XYbin;
  std::vector<MonitorElement*> tkHistoMap_;
  std::vector<MonitorElement*> tkHistoMapPoly_;
  int HistoNumber;
  std::string MapName_;
};

#endif
