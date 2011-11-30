/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                             ClusteringSuite                               *
 *   Infrastructure and tools to apply clustering analysis to Paraver and    *
 *                              Dimemas traces                               *
 *                                                                           *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\

  $Id:: libDistributedClustering.cpp 51 2011-11-2#$:  Id
  $Rev:: 51                                       $:  Revision of last commit
  $Author:: jgonzale                              $:  Author of last commit
  $Date:: 2011-11-24 15:47:29 +0100 (Thu, 24 Nov #$:  Date of last commit

\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#include "HullManager.h"
#include "ClusteringTags.h"
#include "Utils.h"

const char *HullFormatString    = "%ld %d %d %ald %ald %alf";
const char *AllSentFormatString = "";

HullManager::HullManager(void) { }

/**
 * Front-end, filter and back-end call to receive a hull from a MRNet packet.
 */
HullModel* HullManager::Unpack(PACKET_PTR InputPacket)
{
  long long  Density=0;
  int        NumberOfPoints=0, NumberOfDimensions=0, DimensionsValuesSize=0;
  long long *Instances=NULL, *NeighbourhoodSizes=NULL;
  double    *DimensionsValues=NULL;

  PACKET_unpack(InputPacket, HullFormatString,
    &Density,
    &NumberOfPoints,
    &NumberOfDimensions,
    &Instances, &NumberOfPoints,
    &NeighbourhoodSizes, &NumberOfPoints,
    &DimensionsValues, &DimensionsValuesSize);

  HullModel *Hull = new HullModel(Density,
                                  NumberOfPoints,
                                  NumberOfDimensions,
                                  Instances,
                                  NeighbourhoodSizes,
                                  DimensionsValues);

  /* DEBUG -- dump current hull
  std::cout << "[DUMP HULL] << std::endl;
  Hull->Print();
  */

  xfree(Instances);
  xfree(NeighbourhoodSizes);
  xfree(DimensionsValues);

  return Hull;
}

/**************************************************************************\
 *                           BACK-END INTERFACE                           *
\**************************************************************************/

/**
 * Back-end call to send a list of hulls through the given stream.
 * @param OutputStream The MRNet stream.
 * @param HullsList A vector of hulls to send.
 */
void HullManager::Serialize(STREAM *OutputStream, vector<HullModel*> &HullsList)
{
  for (size_t i = 0; i < HullsList.size(); i++)
  {
    SerializeOne(OutputStream, HullsList[i]);
  }

  SerializeDone(OutputStream);
}

/**
 * Back-end call to send a single hull through the given stream.
 * @param OutputStream The MRNet stream.
 * @param Hull The hull to send.
 */
void HullManager::SerializeOne(STREAM *OutputStream, HullModel *Hull)
{
  long long  Density=0;
  int        NumberOfPoints=0, NumberOfDimensions=0;
  long long *Instances=NULL, *NeighbourhoodSizes=NULL;
  double    *DimensionsValues=NULL;

  /* Serialize the hull object into basic data types that can be sent through the MRNet */
  Hull->Serialize(Density,
                  NumberOfPoints,
                  NumberOfDimensions,
                  Instances,
                  NeighbourhoodSizes,
                  DimensionsValues);

  STREAM_send(OutputStream, TAG_HULL, HullFormatString,
    Density,
    NumberOfPoints,
    NumberOfDimensions,
    Instances, NumberOfPoints,
    NeighbourhoodSizes, NumberOfPoints,
    DimensionsValues, NumberOfPoints * NumberOfDimensions);

  xfree(Instances);
  xfree(NeighbourhoodSizes);
  xfree(DimensionsValues);
}

/**
 * Back-end call to notify that all hulls were sent.
 * @param OutputStream The MRNet stream.
 */
void HullManager::SerializeDone(STREAM *OutputStream)
{
  STREAM_send(OutputStream, TAG_ALL_HULLS_SENT, AllSentFormatString);
}

/**************************************************************************\
 *                            FILTER INTERFACE                            *
\**************************************************************************/

/**
 * Filter call to prepare a list of output packets to send the given hulls.
 * @param StreamID The stream ID where the packet will be sent.
 * @param OutputPackets Queue of output packets that has to be filled (by reference).
 * @param HullsList A vector of hulls to send.
 */
void HullManager::Serialize(int StreamID, vector<PacketPtr> &OutputPackets, vector<HullModel*> &HullsList)
{
  vector<HullModel*>::iterator it;

  for (it = HullsList.begin(); it != HullsList.end(); ++it)
  {
    HullModel *Hull = *it;

    SerializeOne(StreamID, OutputPackets, Hull);
  }
  SerializeDone(StreamID, OutputPackets);
}

/**
 * Filter call to prepare an output packet to send the given hull.
 * @param StreamID The stream ID where the packet will be sent.
 * @param OutputPackets Queue of output packets that has to be filled (by reference).
 * @param Hull The hull to send.
 */
void HullManager::SerializeOne(int StreamID, vector<PacketPtr> &OutputPackets, HullModel *Hull)
{
  long long  Density=0;
  int        NumberOfPoints=0, NumberOfDimensions=0;
  long long *Instances=NULL, *NeighbourhoodSizes=NULL;
  double    *DimensionsValues=NULL;

  /* Serialize the hull object into basic data types that can be sent through the MRNet */
  Hull->Serialize(Density,
                  NumberOfPoints,
                  NumberOfDimensions,
                  Instances,
                  NeighbourhoodSizes,
                  DimensionsValues);

  PacketPtr new_packet( new Packet( StreamID, TAG_HULL, HullFormatString,
                                    Density,
                                    NumberOfPoints,
                                    NumberOfDimensions,
                                    Instances, NumberOfPoints,
                                    NeighbourhoodSizes, NumberOfPoints,
                                    DimensionsValues, NumberOfPoints * NumberOfDimensions ) );

  new_packet->set_DestroyData(true);
  OutputPackets.push_back( new_packet );
}

/**
 * Filter call to prepare an output packet to send that notifies that all hulls were sent.
 * @param StreamID The stream ID where the packet will be sent.
 * @param OutputPackets Queue of output packets that has to be filled (by reference).
 */
void HullManager::SerializeDone(int StreamID, vector<PacketPtr> &OutputPackets)
{
  /* Notify all hulls were sent */
  PacketPtr new_packet( new Packet( StreamID, TAG_ALL_HULLS_SENT, AllSentFormatString ) );
  OutputPackets.push_back( new_packet );
}

