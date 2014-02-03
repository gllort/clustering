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


#include "libTraceClusteringImplementation.hpp"
#include "libTraceClustering.hpp"

#include <ParametersManager.hpp>

#include <stdio.h>

/**
 * Empty constructor
 */
libTraceClustering::libTraceClustering(bool verbose,
                                       bool paraver_verbosity)
{
  Implementation  = new libTraceClusteringImplementation(verbose,
                                                         paraver_verbosity);
  Error = Warning = false;
}

/**
 * Initializes the clustering library, loading the XML where different parts
 * are defined
 *
 * \param ClusteringDefinitionXML Name of the XML where clustering is defined
 *
 * \param PCFFileName             Name of the PCF File in case we have to
 *                                read the performance counters IDs.
 *
 * \param UseSemanticValue        Boolean to indicate if we have to read the
 *                                semantic value, in case we use a semantic
 *                                CSV generated by Paraver
 *
 * \param ApplyLogToSemanticValue True if we should apply a logarithmic
 *                                normalization to the semantic values read from
 *                                a Paraver CSV file
 *
 * \param UseFlags                Indicates the operation mode of the library
 *
 * \return True if the initialization finished properly. False otherwise
 */
bool libTraceClustering::InitTraceClustering(string        ClusteringDefinitionXML,
                                             string        PCFFileName,
                                             bool          UseSemanticValue,
                                             bool          ApplyLogToSemanticValue,
                                             unsigned char UseFlags)
{
  if (!Implementation->InitTraceClustering(ClusteringDefinitionXML,
                                           PCFFileName,
                                           UseSemanticValue,
                                           ApplyLogToSemanticValue,
                                           UseFlags))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  if (Implementation->GetWarning())
  {
    Warning        = true;
    WarningMessage = Implementation->GetLastWarning();
  }

  return true;
}

/**
 * Overrides the algorithm defined in the XML file with DBSCAN with the
 * parameters provided
 *
 * \param Eps                     Value of Epsilon parameter to be used in
 *                                DBSCAN
 *
 * \param MinPoints               Value of MinPoints parameter to be used in
 *                                DBSCAN
 *
 * \return True if the algorithm has been correctly initialized, false
 *         otherwise
 */
bool libTraceClustering::SetDBSCANParameters(double Eps, int MinPoints)
{
  /* Extract data to memory */
  if (!Implementation->SetDBSCANParameters(Eps, MinPoints))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Load the data to memory from the provided input file. It could be a Paraver trace,
 * a Dimemas trace or a CSV previously generated by the clustering tool
 *
 * \param InputFileName    Name of the input file where data is located
 * \param SampleData       True if data read from trace must be sampled
 * \param MaxSamples       Maximum number of burst to be samples
 * \param EventsToParse    Set of parameters in case we want to do an event
 *                         parsing of a Paraver trace
 * \param ConsecutiveEvts  If true, any event from the EventsToParse set will
 *                         define an entry/exit of a region
 * \param InputSemanticCSV File name of a semantic CSV file produced by Paraver
 *                         in case we want to perform a semantic guided data
 *                         extraction
 *
 * \return True if data extraction worked properly. False otherwise
 */
bool
libTraceClustering::ExtractData(string            InputFileName,
                                bool              SampleData,
                                unsigned int      MaxSamples,
                                set<unsigned int> EventsToParse,
                                bool              ConsecutiveEvts,
                                string            InputSemanticCSV)
{
  /* Extract data to memory */
  if (!Implementation->ExtractData(InputFileName,
                                   SampleData,
                                   MaxSamples,
                                   EventsToParse,
                                   ConsecutiveEvts,
                                   InputSemanticCSV))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Load the data to memory from the provided input file. It could be a Paraver trace,
 * a Dimemas trace or a CSV previously generated by the clustering tool
 *
 * \param InputFileName    Name of the input file where data is located
 * \param SampleData       True if data read from trace must be sampled
 * \param MaxSamples       Maximum number of burst to be samples
 * \param InputSemanticCSV File name of a semantic CSV file produced by Paraver
 *                         in case we want to perform a semantic guided data
 *                         extraction
 *
 * \return True if data extraction worked properly. False otherwise
 */
bool libTraceClustering::ExtractData(string            InputFileName,
                                     bool              SampleData,
                                     unsigned int      MaxSamples,
                                     string            InputSemanticCSV)
{
  /* Extract data to memory */
  if (!Implementation->ExtractData(InputFileName,
                                   SampleData,
                                   MaxSamples,
                                   set<unsigned int> (),
                                   false,
                                   InputSemanticCSV))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Load the data to memory from the provided input file. It could be a Paraver trace,
 * a Dimemas trace or a CSV previously generated by the clustering tool
 *
 * \param InputFileName    Name of the input file where data is located
 * \param EventsToParse    Set of parameters in case we want to do an event
 *                         parsing of a Paraver trace
 * \param ConsecutiveEvts  If true, any event from the EventsToParse set will
 *                         define an entry/exit of a region
 * \param InputSemanticCSV File name of a semantic CSV file produced by Paraver
 *                         in case we want to perform a semantic guided data
 *                         extraction
 *
 * \return True if data extraction worked properly. False otherwise
 */
bool libTraceClustering::ExtractData(string            InputFileName,
                                     set<unsigned int> EventsToParse,
                                     bool              ConsecutiveEvts,
                                     string            InputSemanticCSV)
{
  /* Extract data to memory */
  if (!Implementation->ExtractData(InputFileName,
                                   false,          /* NO SAMPLING */
                                   0,
                                   EventsToParse,
                                   ConsecutiveEvts,
                                   InputSemanticCSV))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Load the data to memory from the provided input file and flushes it to the
 * desired output file as a CSV.
 *
 * \param InputFileName    Name of the input file where data is located
 * \param SampleData       True if data read from trace must be sampled
 * \param MaxSamples       Maximum number of burst to be samples
 * \param OutputFileName   Name of the output (CSV) file
 * \param EventsToParse    Set of parameters in case we want to do an event
 *                         parsing of a Paraver trace
 * \param ConsecutiveEvts  If true, any event from the EventsToParse set will
 *                         define an entry/exit of a region
 * \param InputSemanticCSV File name of a semantic CSV file produced by Paraver
 *                         in case we want to perform a semantic guided data
 *                         extraction
 *
 * \return True if data extraction and output file generation worked properly. False otherwise
 */
bool
libTraceClustering::ExtractData(string            InputFileName,
                                string            OutputCSVFileName,
                                bool              SampleData,
                                unsigned int      MaxSamples,
                                set<unsigned int> EventsToParse,
                                bool              ConsecutiveEvts,
                                string            InputSemanticCSV)
{
  /* Extract data to memory */
  if (!Implementation->ExtractData(InputFileName,
                                   SampleData,
                                   MaxSamples,
                                   EventsToParse,
                                   ConsecutiveEvts))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Performs a single cluster analysis
 *
 * \result True if the analysis finished correctly, false otherwise
 */
bool libTraceClustering::ClusterAnalysis()
{
  if (!Implementation->ClusterAnalysis())
  {
    Error = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Performs a DBSCAN cluster analysis with auto refinement based on sequence
 * score. The exploration range is guessed automatically
 *
 * \param Divisive             True if the refinement will be top down, false if
 *                             it would be bottom up
 * \param OutputFileNamePrefix Prefix of the output files for each step data and
 *                             plots
 *
 * \result True if the analysis finished correctly, false otherwise
 */
bool libTraceClustering::ClusterRefinementAnalysis(bool   Divisive,
                                                   bool   PrintStepsInformation,
                                                   string OutputFileNamePrefix)
{
  if (!Implementation->ClusterRefinementAnalysis(Divisive,
                                                 PrintStepsInformation,
                                                 OutputFileNamePrefix))
  {
    Error = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Performs a DBSCAN cluster analysis with auto refinement based on sequence
 * score. The exploration range is provided by the user
 *
 * \param Divisive             True if the refinement will be top down, false if
 *                             it would be bottom up
 * \param MinPoints            Fixed MinPoints value used in all runs of DBSCAN
 * \param MaxEps               Maximum value of Epsilon
 * \param MinEps               Minimum value of Epsilon
 * \param Steps                Number of iterarions of the algorithm
 * \param OutputFileNamePrefix Prefix of the output files for each step data and
 *                             plots
 *
 * \result True if the analysis finished correctly, false otherwise
 */
bool libTraceClustering::ClusterRefinementAnalysis(bool   Divisive,
                                                   int    MinPoints,
                                                   double MaxEps,
                                                   double MinEps,
                                                   int    Steps,
                                                   bool   PrintStepsInformation,
                                                   string OutputFileNamePrefix)
{
  if (!Implementation->ClusterRefinementAnalysis(Divisive,
                                                 MinPoints,
                                                 MaxEps,
                                                 MinEps,
                                                 Steps,
                                                 PrintStepsInformation,
                                                 OutputFileNamePrefix))
  {
    Error = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Write clusters information to an output file
 *
 * \param OutputClustersInfoFileName Name of the output file where clusters information will be written
 *
 * \result True if output file is written correctly, false otherwise
 */
bool libTraceClustering::FlushClustersInformation(string OutputClustersInfoFileName)
{
  if (!Implementation->FlushClustersInformation(OutputClustersInfoFileName))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }
  return true;
}

/**
 * Write data to output file (trace or CSV)
 *
 * \param OutputCSVFileNamePrefix Prefix of the output file where data will be written
 *
 * \result True if output file is written correctly, false otherwise
 */
bool libTraceClustering::FlushData(string OutputCSVFileNamePrefix)
{
  if (!Implementation->FlushData(OutputCSVFileNamePrefix))
  {
    Error = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Write clusters sequences to an output file
 *
 * \param OutputFilePrefix Output file prefix for the sequences file and the sequence score file
 * \param FASTASequenceFile True to generate a FASTA aminoacids sequences file
 *
 * \result True if sequences file is written correctly, false otherwise
 */
bool libTraceClustering::ComputeSequenceScore(string OutputFilePrefix,
                                              bool   FASTASequenceFile)
{
  if (!Implementation->ComputeSequenceScore(OutputFilePrefix,
                                            FASTASequenceFile))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Generates an output trace adding the cluster analysis information to the input trace
 *
 * \param OutputTraceName Name of the output trace file
 *
 * \result True if reconstruction worked properly, false otherwise
 */
bool libTraceClustering::ReconstructInputTrace(string OutputTraceName)
{
  if (!Implementation->ReconstructInputTrace(OutputTraceName))
  {
    Error = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Print the plot scripts for GNUPlot defined in the XML
 *
 * \param DataFileNamePrefix Name of the file containg the data to plot
 * \param ScriptsFileNamePrefix Prefix of the output scripts
 *
 * \result True if the scripts were printed correctly, false otherwise
 */
bool libTraceClustering::PrintPlotScripts(string DataFileNamePrefix,
                                          string ScriptsFileNamePrefix)
{
  if (!Implementation->PrintPlotScripts(DataFileNamePrefix,
                                        ScriptsFileNamePrefix))
  {
    Error = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Generates a possible parameter approximation needed by the cluster algorithm
 *
 * \param OutputFileNamePrefix The prefix of the output files that will be generated
 * \param Parameters Map of key and value strings parameters of the approximation
 *
 * \result True if the approximation wero done correctly, false otherwise
 */
bool libTraceClustering::ParametersApproximation(string              OutputFileNamePrefix,
                                                 map<string, string> Parameters)
{
  if (!Implementation->ParametersApproximation(OutputFileNamePrefix,
                                               Parameters))
  {
    Error        = true;
    ErrorMessage = Implementation->GetLastError();
    return false;
  }

  return true;
}

/**
 * Returns the string containing the last error message
 * \return Last error message
 */
string
libTraceClustering::GetErrorMessage(void)
{
  return ErrorMessage;
}

/**
 * Returns the string containing the last warning message
 * \return Last warning message
 */
string
libTraceClustering::GetWarningMessage(void)
{
  return WarningMessage;
}
