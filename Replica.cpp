
#include "Replica.h"

Chunk::Chunk()
{
  this->data = "";
}

Chunk::Chunk(std::string arg_data)
{
  this->data = arg_data;
}

Json::Value *
Chunk::dumpJ()
{
  Json::Value * result_ptr = new Json::Value();

  if (this->data != "")
    {
      (*result_ptr)["data"] = this->data;
    }

  return result_ptr;
}

bool
Chunk::Jdump(Json::Value *input_json_ptr)
{
  if ((input_json_ptr == NULL) ||
      ((*input_json_ptr).isNull() == true) ||
      ((*input_json_ptr).isObject() != true))
    {
      return false;
    }

  if ((((*input_json_ptr)["data"]).isNull() == true) ||
      (((*input_json_ptr)["data"]).isString() != true))
    {
      return false;
    }

  this->data = ((*input_json_ptr)["data"]).asString();

  return true;
}

Replica::Replica
(std::string core_arg_host_url, std::string core_arg_owner_vsID,
 std::string core_arg_class_id, std::string core_arg_object_id)
  : Core { core_arg_host_url, core_arg_owner_vsID,
    core_arg_class_id, core_arg_object_id }
{
  	std::cout << "a shadow has been created" << std::endl;
	this->committed_data[0].data = "";
	this->committed_data[1].data = "";
}

Replica::Replica
(std::string core_arg_host_url, std::string core_arg_owner_vsID,
 std::string core_arg_class_id, std::string core_arg_object_id,
 std::string arg_data)
  : Core { core_arg_host_url, core_arg_owner_vsID,
    core_arg_class_id, core_arg_object_id }
{
  this->committed_data[0].data = arg_data;
  this->committed_data[1].data = arg_data;
}

Json::Value
Replica::CommitAbort
(const std::string& arg_name, const std::string& arg_fhandle, const std::string& arg_chunk_index,
 const std::string& arg_commitorabort)
{
	Json::Value result;
	//
	result["commitOrAbort"] = "abort";
	int chunkIndex = std::stoi(arg_chunk_index);
	if (chunkIndex != 0 && chunkIndex != 1)
	{
		result["commitOrAbort"] = "abort. chunk index out of bounds in Replica::CommitAbort()";
		return result;
	}
	if (arg_commitorabort == "commit")
	{
	  this->committed_data[chunkIndex].data = this->uncommitted_data[chunkIndex].data;
	  result["commitOrAbort"] = "commit";
	}
	
	return result;
}

Json::Value
Replica::PushChunk2Replica
(const std::string& arg_name, const std::string& arg_fhandle, const std::string& arg_chunk_index, const std::string& arg_chunk)
{
	Json::Value result;
	//
	int chunkIndex = std::stoi(arg_chunk_index);
	if (chunkIndex != 0 && chunkIndex != 1)
	{
		result["status"] = "fail. chunk_index out of bounds in Replica::PushChunkk2Replica()";
		return result;
	}
	this->uncommitted_data[chunkIndex].data = arg_chunk;
	result["status"] = "success";
	return result;
}

Json::Value *
Replica::dumpJ()
{
  Json::Value * result_ptr = new Json::Value();

  if (this->name != "")
    {
      (*result_ptr)["name"] = this->name;
    }

  if (this->fhandle != "")
    {
      (*result_ptr)["fhandle"] = this->fhandle;
    }

  if (this->chunk_index != "")
    {
      (*result_ptr)["chunk_index"] = this->chunk_index;
    }

  return result_ptr;
}

bool
Replica::Jdump(Json::Value *input_json_ptr)
{
  if ((input_json_ptr == NULL) ||
      ((*input_json_ptr).isNull() == true) ||
      ((*input_json_ptr).isObject() != true))
    {
      return false;
    }

  if ((((*input_json_ptr)["name"]).isNull() == true) ||
      (((*input_json_ptr)["fhandle"]).isNull() == true) ||
      (((*input_json_ptr)["name"]).isString() != true) ||
      (((*input_json_ptr)["fhandle"]).isString() != true))
    {
      return false;
    }

  this->name    = ((*input_json_ptr)["name"]).asString();
  this->fhandle = ((*input_json_ptr)["fhandle"]).asString();
  return true;
}


Json::Value
Replica::ReadChunk(const std::string& fileName, const std::string& fileHandle, const std::string& chunkIndex)
{
	Json::Value result;
	int index = std::stoi(chunkIndex);
	if (index != 0 && index != 1)
	{
		result["status"] = "fail. invalid chunk Index in Replica::ReadChunk()";
		return result;
	}
	
	result["status"] = "success";
	result["data"] = this->committed_data[index].data;
	return result;
}

