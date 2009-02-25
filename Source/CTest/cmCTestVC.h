/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile$
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Kitware, Inc. All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef cmCTestVC_h
#define cmCTestVC_h

#include "cmProcessTools.h"

class cmCTest;

/** \class cmCTestVC
 * \brief Base class for version control system handlers
 *
 */
class cmCTestVC: public cmProcessTools
{
public:
  /** Construct with a CTest instance and update log stream.  */
  cmCTestVC(cmCTest* ctest, std::ostream& log);

  virtual ~cmCTestVC();

  /** Command line tool to invoke.  */
  void SetCommandLineTool(std::string const& tool);

  /** Top-level source directory.  */
  void SetSourceDirectory(std::string const& dir);

  /** Get the date/time specification for the current nightly start time.  */
  std::string GetNightlyTime();

  /** Perform cleanup operations on the work tree.  */
  void Cleanup();

  /** Update the working tree to the new revision.  */
  bool Update();

  /** Get the command line used by the Update method.  */
  std::string const& GetUpdateCommandLine() const
    { return this->UpdateCommandLine; }

  /** Write Update.xml entries for the updates found.  */
  bool WriteXML(std::ostream& xml);

  /** Enumerate non-trivial working tree states during update.  */
  enum PathStatus { PathUpdated, PathModified, PathConflicting };

  /** Get the number of working tree paths in each state after update.  */
  int GetPathCount(PathStatus s) const { return this->PathCount[s]; }

protected:
  // Internal API to be implemented by subclasses.
  virtual void CleanupImpl();
  virtual void NoteOldRevision();
  virtual bool UpdateImpl();
  virtual void NoteNewRevision();
  virtual bool WriteXMLUpdates(std::ostream& xml);

  /** Basic information about one revision of a tree or file.  */
  struct Revision
  {
    std::string Rev;
    std::string Date;
    std::string Author;
    std::string Log;
  };

  struct File;
  friend struct File;

  /** Represent change to one file.  */
  struct File
  {
    PathStatus Status;
    Revision const* Rev;
    Revision const* PriorRev;
    File(): Status(PathUpdated), Rev(0), PriorRev(0) {}
    File(PathStatus status, Revision const* rev, Revision const* priorRev):
      Status(status), Rev(rev), PriorRev(priorRev) {}
  };

  /** Convert a list of arguments to a human-readable command line.  */
  static std::string ComputeCommandLine(char const* const* cmd);

  /** Run a command line and send output to given parsers.  */
  bool RunChild(char const* const* cmd, OutputParser* out,
                OutputParser* err, const char* workDir = 0);

  /** Run VC update command line and send output to given parsers.  */
  bool RunUpdateCommand(char const* const* cmd,
                        OutputParser* out, OutputParser* err = 0);

  /** Write xml element for one file.  */
  void WriteXMLEntry(std::ostream& xml, std::string const& path,
                     std::string const& name, std::string const& full,
                     File const& f);

  // Instance of cmCTest running the script.
  cmCTest* CTest;

  // A stream to which we write log information.
  std::ostream& Log;

  // Basic information about the working tree.
  std::string CommandLineTool;
  std::string SourceDirectory;

  // Record update command info.
  std::string UpdateCommandLine;

  // Placeholder for unknown revisions.
  Revision Unknown;

  // Count paths reported with each PathStatus value.
  int PathCount[3];
};

#endif
