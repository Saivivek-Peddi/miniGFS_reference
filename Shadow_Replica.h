
#ifndef _SHADOW_REPLICA_H_
#define _SHADOW_REPLICA_H_

#include "Replica.h"

class Shadow_Replica : public Replica
{
 private:
 public:
  Shadow_Replica(std::string, std::string, std::string, std::string);
  virtual Json::Value PushChunk2Replica(std::string, std::string, std::string, std::string);
  virtual Json::Value CommitAbort(std::string, std::string, std::string, std::string);
  virtual Json::Value * dumpJ();
  virtual bool Jdump(Json::Value *);
  virtual Json::Value ReadChunk(const std::string& fileName, const std::string& fileHandle, const std::string& chunkIndex);
};

#endif // _SHADOW_REPLICATE_H_
