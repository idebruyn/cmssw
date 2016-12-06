/*! \brief   Implementation of methods of TTCluster
 *  \details Here, in the source file, the methods which do depend
 *           on the specific type <T> that can fit the template.
 *
 *  \author Nicola Pozzobon
 *  \author Emmanuele Salvati
 *  \date   2013, Jul 12
 *
 */

#include "DataFormats/L1TrackTrigger/interface/TTCluster.h"

/// Cluster width
template< >
unsigned int TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findWidth() const
{
  int rowMin = 99999999;
  int rowMax = 0;
  /// this is only the actual size in RPhi
  for ( unsigned int i = 0; i < theHits.size(); i++ )
  {
    int row = 0;
    if ( this->getRows().size() == 0 )
    {
      row = theHits[i]->row();
    }
    else
    {
      row = this->getRows()[i];
    }
    if ( row < rowMin )
      rowMin = row;
    if ( row > rowMax )
      rowMax = row;
  }
  return abs( rowMax - rowMin + 1 ); /// This takes care of 1-Pixel clusters
}

/// Get hit local coordinates
template< >
MeasurementPoint TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findHitLocalCoordinates( unsigned int hitIdx ) const
{
  /// NOTE in this case, DO NOT add 0.5
  /// to get the center of the pixel
  if ( this->getRows().size() == 0 || this->getCols().size() == 0 )
  {
    MeasurementPoint mp( theHits[hitIdx]->row(), theHits[hitIdx]->column() );
    return mp;
  }
  else
  {
    int row = this->getRows().at(hitIdx);
    int col = this->getCols().at(hitIdx);
    MeasurementPoint mp( row, col );
    return mp;
  }
}

/// Unweighted average local cluster coordinates
template< >
MeasurementPoint TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findAverageLocalCoordinates() const
{
  double averageCol = 0.0;
  double averageRow = 0.0;

  /// Loop over the hits and calculate the average coordinates
  if ( theHits.size() != 0 )
  {
    if ( this->getRows().size() == 0 || this->getCols().size() == 0 )
    {
      typename std::vector< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::const_iterator hitIter;
      for ( hitIter = theHits.begin();
            hitIter != theHits.end();
            hitIter++ )
      {
        averageCol += (*hitIter)->column();
        averageRow += (*hitIter)->row();
      }
      averageCol /= theHits.size();
      averageRow /= theHits.size();
    }
    else
    {
      for ( unsigned int j = 0; j < theHits.size(); j++ )
      {
        averageCol += theCols[j];
        averageRow += theRows[j];
      }
      averageCol /= theHits.size();
      averageRow /= theHits.size();
    }
  }
  return MeasurementPoint( averageRow, averageCol );
}

/// Get hit local position
template< >
LocalPoint TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findHitLocalPosition( const GeomDet* theGeomDet, unsigned int hitIdx ) const
{
  //const GeomDetUnit* geomDetUnit = idToDetUnit( this->getDetId(), this->getStackMember() );
  MeasurementPoint mp( theHits[hitIdx]->row(), theHits[hitIdx]->column() );
  return theGeomDet->topology().localPosition( mp );
}

/// Get hit global position
template< >
GlobalPoint TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findHitGlobalPosition( const GeomDet* theGeomDet, unsigned int hitIdx ) const
{
  //const GeomDetUnit* geomDetUnit = idToDetUnit( this->getDetId(), this->getStackMember() );
  MeasurementPoint mp( theHits[hitIdx]->row(), theHits[hitIdx]->column() );
  return theGeomDet->surface().toGlobal( theGeomDet->topology().localPosition( mp ) );
}

/// Unweighted average local cluster position
template< >
LocalPoint TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findAverageLocalPosition( const GeomDet* theGeomDet ) const
{
  double averageX = 0.0;
  double averageY = 0.0;

  /// Loop over the hits and calculate the average coordinates
  if ( theHits.size() != 0 )
  {
    for ( unsigned int i = 0; i < theHits.size(); i++ )
    {
      LocalPoint thisHitPosition = findHitLocalPosition( theGeomDet, i );
      averageX += thisHitPosition.x();
      averageY += thisHitPosition.y();
    }
    averageX /= theHits.size();
    averageY /= theHits.size();
  }
  return LocalPoint( averageX, averageY );
}

/// Unweighted average cluster position
template< >
GlobalPoint TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findAverageGlobalPosition( const GeomDet* theGeomDet ) const
{
  double averageX = 0.0;
  double averageY = 0.0;
  double averageZ = 0.0;

  /// Loop over the hits and calculate the average coordinates
  if ( theHits.size() != 0 )
  {
    for ( unsigned int i = 0; i < theHits.size(); i++ )
    {
      GlobalPoint thisHitPosition = findHitGlobalPosition( theGeomDet, i );
      averageX += thisHitPosition.x();
      averageY += thisHitPosition.y();
      averageZ += thisHitPosition.z();
    }
    averageX /= theHits.size();
    averageY /= theHits.size();
    averageZ /= theHits.size();
  }
  return GlobalPoint( averageX, averageY, averageZ );
}

/// Coordinates stored locally
template< >
std::vector< int > TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findRows() const
{
  std::vector< int > temp;
  temp.reserve(theHits.size());
  for ( unsigned int i = 0; i < theHits.size(); i++ )
  {
    temp.push_back( theHits[i]->row() );
  }
  return temp;
}

template< >
std::vector< int > TTCluster< edm::Ref< edm::DetSetVector< Phase2TrackerDigi >, Phase2TrackerDigi > >::findCols() const
{
  std::vector< int > temp;
  temp.reserve(theHits.size());
  for ( unsigned int i = 0; i < theHits.size(); i++ )
  {
    temp.push_back( theHits[i]->column() );
  }
  return temp;
}

