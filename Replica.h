
#ifndef _REPLICA_H_
#define _REPLICA_H_

#include "Core.h"

// Replica.h
#include <iostream>

class Chunk
{
 private:
 public:
  std::string data; // base64 encoding
  Chunk();
  Chunk(std::string);
  virtual Json::Value * dumpJ();
  virtual bool Jdump(Json::Value *);
};

class Replica : public Core
{
 private:
 public:
  std::string name;
  std::string fhandle;
  std::string chunk_index;
  Chunk committed_data[2];
  Chunk uncommitted_data[2];
  Replica(std::string, std::string, std::string,
	  std::string);
  Replica(std::string, std::string, std::string,
	  std::string, std::string);
  virtual Json::Value PushChunk2Replica(const std::string&, const std::string&, const std::string&, const std::string&);
  virtual Json::Value CommitAbort(const std::string&, const std::string&, const std::string&, const std::string&);
  virtual Json::Value ReadChunk(const std::string& fileName, const std::string& fileHandle, const std::string& chunkIndex);
  virtual Json::Value ReadFile(const std::string& fileName, const std::string& fileHandle);
  virtual Json::Value * dumpJ();
  virtual bool Jdump(Json::Value *);
};

#endif  /* _REPLICA_H_ */
