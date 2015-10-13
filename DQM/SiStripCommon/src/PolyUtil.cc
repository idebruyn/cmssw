#include "DQM/SiStripCommon/interface/PolyUtil.h"
#include<utility>
static const int kSubdetOffset       = 25;
		static const unsigned layerMask_ = 0x7;
		static const unsigned layerStartBit_ = 14;
		static const unsigned ringStartBitTID_= 9;
		static const unsigned ringMaskTID_= 0x3;
		static const unsigned ringStartBitTEC_= 5;
		static const unsigned ringMaskTEC_= 0x7;
enum SubDetector { UNKNOWN=0, TIB=3, TID=4, TOB=5, TEC=6, TECM = 7, TECP = 8  };

int PolyUtil::subdet(const unsigned int& id_) { 
return ((id_>>kSubdetOffset)&0x7); 
}

PolyUtil::PolyUtil(){
        
	std::ifstream in;

	in.open(edm::FileInPath("DQM/SiStripCommon/src/tkmap_bare").fullPath().c_str());
	//std::cout<<"WHAT TJHE HELL"<<std::endl;
	UInt_t count = 0;
	if(!in.good()){
	std::cout<<"Error Reading File"<<std::endl;
	return;
	}
	while(in.good()) {
	  	Double_t minx = 0xFFFFFF, maxx = -0xFFFFFF, miny = 0xFFFFFF, maxy = -0xFFFFFFF;
		Long_t detid = 0;
		Double_t x[5],y[5];
		std::string line;
		std::getline(in,line);    
		++count;
		TString string(line);
		TObjArray* array = string.Tokenize(" ");
		int ix = 0, iy = 0;
		 for( Int_t i = 0; i < array->GetEntries(); ++i ) {
		   if( i == 0 ) {detid = static_cast<TObjString*>(array->At(i))->String().Atoll();
		   
		   }
      else {
	if( i % 2 == 0 ) {
	  
	  x[ix] =  static_cast<TObjString*>(array->At(i))->String().Atof();
	if( x[ix] < minx ){ minx = x[ix];
	  //std::cout<<"new minx is: "<<minx<<std::endl;
	}
	if( x[ix] > maxx ){ maxx = x[ix];
	  //std::cout<<"new maxx is : "<<maxx<<std::endl;
	}
	  ++ix;
	} else {
	  y[iy] =  static_cast<TObjString*>(array->At(i))->String().Atof();
	  if( y[iy] < miny ) {miny = y[iy];
	    //std::cout<<"New miny is: "<<miny<<std::endl;
	  }
	  if( y[iy] > maxy ){ maxy = y[iy];
	    //std::cout<<"New maxy is: "<<maxy<<std::endl;
	  }
	  ++iy;
	}
      }
    }
		if( detid == 0 ) {
		  std::cerr << "Could not parse any detid from line " << count << std::endl;
		  continue;
		}
		if(ix != iy ) {
		  std::cerr << "Did not find the same number of x and y points in line " << count << std::endl;
		  continue;
		}
		if( ix == 0 ) { 
		  std::cerr << "Did not find any points for detid " << detid << " in line " << count << std::endl;
		  continue;
		}

		 Int_t sd = subdet(detid);
		 Int_t l = layer(detid);
		 Int_t s = side(detid);
		 //Int_t mul = l*s;
		 //std::pair<Int_t, Int_t> coord =  std::make_pair<Int_t, Int_t>(sd,l*s);
		 std::pair<Int_t, Int_t> coord =  std::make_pair<Int_t, Int_t>(Int_t(sd),l*s);
		 
		 if( mMinx.find(coord) == mMinx.end() || mMinx[coord] > minx) {
		   //std::cout<<"MinX for coord: "<<sd<<" : "<<l*s<<" is : "<<minx<<std::endl; 
		   mMinx[coord] = minx;
		 }
		 if( mMiny.find(coord) == mMiny.end() || mMiny[coord] > miny ) {
		   //std::cout<<"MinY for coord: "<<sd<<" : "<<l*s<<" is : "<<miny<<std::endl;
		   mMiny[coord] = miny;
		 }
	      	//std::cout<<"Entering X in Graph: "<<x<<" Y is: "<<y<<std::endl;
		detIdVector.push_back(detid);
		bins[detid] = new TGraph(ix,x,y);
		bins[detid]->SetName(TString::Format("%ld",detid));
		}
	
	SiStripSubStructure* detFinder  = new SiStripSubStructure();
	//uint32_t bkw_fkw=0,ext=0,string=0;
	 for(int i=1; i<35;i++){
	   if(i>=TIB_L1&&i<=TIB_L4){
	     detFinder->getTIBDetectors(detIdVector,selectedDetIds[i],i-0,0,0,0);
	   }else if (i>=TIDM_D1&&i<=TIDM_D3){
	     //detFinder->getTIBDetectors(detIdVector,selectedDetIds[i],1,bkw_fkw,ext,string);
	     detFinder->getTIDDetectors(detIdVector,selectedDetIds[i],i-4,1,0,0);
	   }else if (i>=TIDP_D1&&i<=TIDP_D3){
	     //detFinder->getTIBDetectors(detIdVector,selectedDetIds[i],1,bkw_fkw,ext,string);
	     detFinder->getTIDDetectors(detIdVector,selectedDetIds[i],i-7,2,0,0);
	   }
	   else if (i>=TOB_L1&&i<=TOB_L6){
	     //detFinder->getTIBDetectors(detIdVector,selectedDetIds[i],i-10,bkw_fkw,ext,string);
	      detFinder->getTOBDetectors(detIdVector,selectedDetIds[i],i-10,0,0);
	   }else if (i>=TECM_W1&&i<=TECM_W9){
	     //detFinder->getTIBDetectors(detIdVector,selectedDetIds[i],i-16,bkw_fkw,ext,string);
	     detFinder->getTECDetectors(detIdVector,selectedDetIds[i],1,i-16,0,0,0,0);
	   }else if (i>=TECP_W1&&i<=TECP_W9){
	     //detFinder->getTIBDetectors(detIdVector,selectedDetIds[i],i-25,,ext,string);
	     detFinder->getTECDetectors(detIdVector,selectedDetIds[i],2,i-25,0,0,0,0);
	   }
}
	}
void PolyUtil:: getComponents(uint32_t& layerVar , Double_t& highX , Double_t& lowX , Double_t& highY , Double_t& lowY){

  //std::vector<uint32_t> selectedDetId;
  
 
  std::vector<uint32_t>::iterator it = selectedDetIds[layerVar].begin();
  highX=0;
  highY=0;
  lowX=0;
  lowY=0;
   

  //  Double_t *  temp;
  //  TGraph* tempGraph;
  for(; it != selectedDetIds[layerVar].end(); ++it) {
    Int_t sd = subdet(*it);
    Int_t l = layer(*it);
    Int_t s = side(*it);
   		 std::pair<Int_t, Int_t> coord =  std::make_pair<Int_t, Int_t>(Int_t(sd),l*s);
		 //std::cout<<"Coord for the selected layer: "<<sd<<" and "<<l*s<<std::endl;
		 // std::cout<<"Minx for Coord: "<<mMinx[coord]<<" Miny for COORD: "<<mMiny[coord]<<std::endl;
		 lowX=mMinx[coord];
		 highX=lowX+1;
		 lowY=mMiny[coord];
		 highY=lowY+1;

    // tempGraph=bins.at(*it);
    //temp=tempGraph->GetX();
   //std::cout<<"Value of X: "<<temp<<std::endl;
   /*for(int i=0;i<5;i++){
   if(lowX>temp[i])
     lowX=temp[i];
   if(highX<temp[i])
     highX=temp[i];
   }
   //std::cout<<"Value of Y: "<<temp<<std::endl;
   temp=tempGraph->GetY();
   for(int i=0;i<5;i++){
   if(lowY>temp[i])
     lowY=temp[i];
   if(highY<temp[i])
   highY=temp[i];
   }*/
  }
  
  //std:: cout<<"Value of LowX: "<<lowX<<" Value of Low Y : "<<lowY<<" At the end of Get COmponenets"<<std::endl;

   
}

void PolyUtil::FillBins(TH2Poly* mePoly , uint32_t& layer){
  //std::cout<<"Size of Vector getting Filled : "<<selectedDetIds[layer].size()<<std::endl;
  //std::cout<<"Capacity of Vector Getting Filled: "<<selectedDetIds[layer].capacity()<<std::endl;
  std::vector<uint32_t>::iterator it = selectedDetIds[layer].begin();
  // Int_t temp ;
  for(; it != selectedDetIds[layer].end(); ++it) {
    //std::cout<<"BINS AT: "<<*it<<std::endl;
    mePoly->AddBin(bins[*it]);
   // std::cout<<"Result of adding bin: "<<temp<<std::endl;
   //std::cout<<"Filling Bin for detId: "<<*it<<std::endl;
  }
  //std::cout<<"Done Filling Bin for Layer: "<<layer<<std::endl;
}
int PolyUtil::side(const unsigned int& id_) {
  int side = 0;
  if( subdet(id_) == TIB ) {
    const unsigned int str_fw_bwStartBit_=       12;
    const unsigned int str_fw_bwMask_=   0x3;
    side = ((id_>>str_fw_bwStartBit_) & str_fw_bwMask_);
  } else if( subdet(id_) == TOB ) {
    const unsigned int rod_fw_bwStartBit_= 12;
    const unsigned int rod_fw_bwMask_=   0x3;
    side = ((id_>>rod_fw_bwStartBit_) & rod_fw_bwMask_);
  } else if( subdet(id_) == TID ) {
    const unsigned int sideStartBit_=          13;
    const unsigned int sideMask_=           0x3;
    side = int((id_>>sideStartBit_) & sideMask_);
  } else if( subdet(id_) == TEC ) {
    const unsigned int sideStartBit_=           18;
    const unsigned int sideMask_=          0x3;
    side = int((id_>>sideStartBit_) & sideMask_);
  } else {
    //std::cerr << "Did not find a valid strip subdetector (" << subdet(id_) << ")" << std::endl;
    return UNKNOWN;
  }
  return (side == 1 ? -1 : 1);
}

int PolyUtil::layer(const unsigned int& id_) {
  if( subdet(id_) == TIB || subdet(id_) == TOB ) {
    return ((id_>>layerStartBit_) & layerMask_);
  } else if( subdet(id_) == TID ) {
    const unsigned int wheelStartBit_=         11; // TID
    const unsigned int wheelMask_=          0x3; // TID
    return int((id_>>wheelStartBit_) & wheelMask_);
  } else if( subdet(id_) == TEC ) {
    const unsigned int wheelStartBit_=          14; // TEC
    const unsigned int wheelMask_=         0xF; // TEC
    return int((id_>>wheelStartBit_) & wheelMask_);
  } else {
    //std::cerr << "Did not find a valid strip subdetector (" << subdet(id_) << ")" << std::endl;
    return UNKNOWN;
  }
}
void PolyUtil:: SyncBins(std::vector<MonitorElement*> &tkHistoMap_,  std::vector<MonitorElement*> &tkHistoMapPoly_){
   std::vector<MonitorElement* >::iterator it = tkHistoMap_.begin();
   std::vector<MonitorElement* >::iterator itPoly = tkHistoMapPoly_.begin();
   for(; it != tkHistoMap_.end(); ++it,++itPoly) {
     MonitorElement* me = *it;
     MonitorElement* mePoly=*itPoly;
     Double_t value =me->getTH2D()->GetBinContent(1);
     std::cout<<value;
     mePoly->getTH2Poly()->ClearBinContents();
     //mePoly->getTH2Poly()->Fill("",value);
     
   }
}
   void PolyUtil::Fill(TH2Poly * mePoly ,uint32_t layerVar ){
     /*  std::vector<uint32_t>::iterator it = selectedDetIds[layerVar].begin();
  // Int_t temp ;
  for(; it != selectedDetIds[layerVar].end(); ++it) {
  mePoly->Fill(TString::Format("%u",*it),gRandom->Gaus(300,100));*/
     
  }
std::vector<uint32_t>* PolyUtil::getSelectedDetIds(){
  return selectedDetIds;
}
  
     

  


