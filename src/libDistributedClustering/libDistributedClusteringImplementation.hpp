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

  $Id::                                           $:  Id
  $Rev::                                          $:  Revision of last commit
  $Author::                                       $:  Author of last commit
  $Date::                                         $:  Date of last commit

\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifndef _LIB_DISTRIBUTED_CLUSTERING_IMPLEMENTATION_HPP_
#define _LIB_DISTRIBUTED_CLUSTERING_IMPLEMENTATION_HPP_

#include <Error.hpp>
using cepba_tools::Error;

#include <libClustering.hpp>
#include <TraceData.hpp>

#include <trace_clustering_types.h>

#include "ConvexHullModel.hpp"

class libDistributedClusteringImplementation;

class libDistributedClusteringImplementation: public Error
{
  private:
    bool           Root;
    TraceData     *Data;
    libClustering *ClusteringCore;
    Partition      LastPartition;
    Partition      ClassificationPartition;

    bool                 UsingExternalData;
    vector<const Point*> ExternalData;

    string         InputFileName;
    input_file_t   InputFileType;

    double         Epsilon;
    INT32          MinPoints;
    
    bool              PRVEventsParsing;
    set<event_type_t> EventsToDealWith;
    
    /*
    bool                  ClusteringExecuted;
    bool                  HWCGroupSet;
    vector<INT32>         CurrentHWCGroup;
    bool                  StoreClassificationPoints;
    */

  public:
    libDistributedClusteringImplementation(int verbose);
    
    /* This methods are intended to be used in the MRNet BE nodes, to perform
     * a local cluster analysis */

    bool InitClustering(string ClusteringDefinitionXML,
                        double Eps,
                        INT32  MinPoints,
                        bool   Root,
                        INT32  MyRank,
                        INT32  TotalRanks);

    bool ExtractData(string            InputFileName,
                     set<int>&         TasksToRead,
                     set<event_type_t> EventsToDealWith = set<event_type_t> ());

    bool ExtractData(string            InputFileName,
                     set<event_type_t> EventsToDealWith = set<event_type_t> ());

    bool ClusterAnalysis(vector<ConvexHullModel>& ClusterModels);

    bool ClassifyData(vector<ConvexHullModel>& ClusterModels);

    bool GenerateStatistics(bool UseClassificationPartition);
    
    bool ReconstructInputTrace(string OutputTraceName);

    /* This second interface is focused to implement the cluster of local noise
     * points */

    bool InitClustering(double Epsilon,
                        int    MinPoints);

    bool ClusterAnalysis(const vector<const Point*>& Points,
                         vector<ConvexHullModel>&    ClusterModels);
    
    bool GetNoisePoints(vector<const Point*>& NoisePoints);

    /* A method to retrieve all information to perform the cross-process 
     * analysis */
     
    bool GetFullBurstsInformation(vector<Point*>&       Points,
                                  vector<task_id_t>&    TaskIDs,
                                  vector<thread_id_t>&  ThreadIDs,
                                  vector<cluster_id_t>& ClusterIDs);


    /* Methods to print the models and the scatter plots of data. To be used
     * in BE nodes */

    bool PrintPlotScripts(string DataFileName,
                          string ScriptsFileNamePrefix = "",
                          bool   LocalPartition = false);

    bool PrintModels(vector<ConvexHullModel>& ClusterModels,
                     string                   ModelsFileName,
                     string                   ScriptsFileNamePrefix = "");

    /* Error/Warning retrieveng methods */

    string GetErrorMessage(void);

    string GetWarningMessage(void);

protected:

private:

  bool FlushData(string DataFileName, bool LocalPartition);
  
  bool GenerateClusterModels(vector<ConvexHullModel>& Models);
  
  vector<const Point*>& GetDataPoints(void);

};

#endif // _LIB_DISTRIBUTED_CLUSTERING_IMPLEMENTATION_HPP_
