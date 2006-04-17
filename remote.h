/*
 * remote.h: General Remote Control handling
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: remote.h 1.35 2006/04/17 08:59:48 kls Exp $
 */

#ifndef __REMOTE_H
#define __REMOTE_H

#include <stdio.h>
#include <termios.h>
#include <time.h>
#include "keys.h"
#include "thread.h"
#include "tools.h"

class cRemote : public cListObject {
private:
  enum { MaxKeys = MAXKEYSINMACRO };
  static eKeys keys[MaxKeys];
  static int in;
  static int out;
  static cTimeMs repeatTimeout;
  static cRemote *learning;
  static char *unknownCode;
  static cMutex mutex;
  static cCondVar keyPressed;
  static const char *plugin;
  char *name;
protected:
  cRemote(const char *Name);
  const char *GetSetup(void);
  void PutSetup(const char *Setup);
  bool Put(uint64 Code, bool Repeat = false, bool Release = false);
  bool Put(const char *Code, bool Repeat = false, bool Release = false);
public:
  virtual ~cRemote();
  virtual bool Ready(void) { return true; }
  virtual bool Initialize(void);
  const char *Name(void) { return name; }
  static void SetLearning(cRemote *Learning) { learning = Learning; }
  static bool IsLearning() { return learning != NULL; }
  static void Clear(void);
  static bool Put(eKeys Key, bool AtFront = false);
  static bool PutMacro(eKeys Key);
  static bool CallPlugin(const char *Plugin);
      ///< Initiates calling the given plugin's main menu function.
      ///< The Plugin parameter is the name of the plugin, and must be
      ///< a static string. Returns true if the plugin call was successfully
      ///< initiated (the actual call to the plugin's main menu function
      ///< will take place some time later, during the next execution
      ///< of VDR's main loop). If there is already a plugin call pending
      ///< false will be returned and the caller should try again later.
  static const char *GetPlugin(void);
      ///< Returns the name of the plugin that was set with a previous
      ///< call to CallPlugin(). The internally stored pointer to the
      ///< plugin name will be reset to NULL by this call.
  static bool HasKeys(void);
  static eKeys Get(int WaitMs = 1000, char **UnknownCode = NULL);
  };

class cRemotes : public cList<cRemote> {};

extern cRemotes Remotes;

enum eKbdFunc {
  kfNone,
  kfF1 = 0x100,
  kfF2,
  kfF3,
  kfF4,
  kfF5,
  kfF6,
  kfF7,
  kfF8,
  kfF9,
  kfF10,
  kfF11,
  kfF12,
  kfUp,
  kfDown,
  kfLeft,
  kfRight,
  kfHome,
  kfEnd,
  kfPgUp,
  kfPgDown,
  kfIns,
  kfDel,
  };

class cKbdRemote : public cRemote, private cThread {
private:
  static bool kbdAvailable;
  static bool rawMode;
  struct termios savedTm;
  virtual void Action(void);
  int ReadKey(void);
  uint64 ReadKeySequence(void);
  int MapCodeToFunc(uint64 Code);
public:
  cKbdRemote(void);
  virtual ~cKbdRemote();
  static bool KbdAvailable(void) { return kbdAvailable; }
  static uint64 MapFuncToCode(int Func);
  static void SetRawMode(bool RawMode);
  };

#endif //__REMOTE_H
