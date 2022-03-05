
// ecs251 miniGFS

#include <iostream>
#include "Shadow_Directory.h"
#include "Shadow_Replica.h"
#include <thread>
#include <unistd.h>

void writeChunksToReplicas(const Json::Value& result, const std::string& chunkIdStr, const std::string& data)
{
    Json::Value result_P, result_A, result_B;

    std::string url_primary = (result["primary"]).asString();
    Shadow_Replica gfs_primary
            { url_primary, "1234567890", "Replica", "00000001" };

    std::string url_secondary_A = (result["secondary_A"]).asString();
    Shadow_Replica gfs_secondary_A
            { url_secondary_A, "1234567890", "Replica", "00000002" };

    std::string url_secondary_B = (result["secondary_B"]).asString();
    Shadow_Replica gfs_secondary_B
            { url_secondary_B, "1234567890", "Replica", "00000003" };

    result_P = gfs_primary.PushChunk2Replica("my_ecs251_file", "00000002", chunkIdStr, data);
    result_A = gfs_secondary_A.PushChunk2Replica("my_ecs251_file", "00000002", chunkIdStr, data);
    result_B = gfs_secondary_B.PushChunk2Replica("my_ecs251_file", "00000002", chunkIdStr, data);

    if (((result_P["vote"]).asString() == "commit") &&
        ((result_A["vote"]).asString() == "commit") &&
        ((result_B["vote"]).asString() == "commit"))
    {
        std::cout << "All replicas voted commit to write to " << chunkIdStr << "\n";
        result_P = gfs_primary.CommitAbort("my_ecs251_file", "00000002", chunkIdStr, "commit");
        result_A = gfs_secondary_A.CommitAbort("my_ecs251_file", "00000002", chunkIdStr, "commit");
        result_B = gfs_secondary_B.CommitAbort("my_ecs251_file", "00000002", chunkIdStr, "commit");
    }
    if(result_P["commitOrAbort"] == "commit" &&
       result_A["commitOrAbort"] == "commit" &&
       result_B["commitOrAbort"] == "commit")
    {
        std::cout << "All replicas committed data on write: " << data << "\t at chunk: " << chunkIdStr << std::endl;
    }
}

void writeChunk(int chunkId, const std::string& data)
{
    std::string chunkIdStr = std::to_string(chunkId);

    Shadow_Directory gfs_master
            { "http://127.0.0.1:8384", "1234567890", "Directory", "00000002" };

    Json::Value result = gfs_master.ObtainChunkURL("my_ecs251_file", "00000002", chunkIdStr);

    writeChunksToReplicas(result, chunkIdStr, data);
}

void readChunk(int chunkId)
{
    std::string chunkIdStr = std::to_string(chunkId);

    Shadow_Directory gfs_master
            { "http://127.0.0.1:8384", "1234567890", "Directory", "00000002" };

    Json::Value result, result_P, result_A, result_B;

    result = gfs_master.ObtainChunkURL("my_ecs251_file", "00000002", chunkIdStr);

    std::string url_primary = (result["primary"]).asString();
    Shadow_Replica gfs_primary
            { url_primary, "1234567890", "Replica", "00000001" };

    Json::Value readResult = gfs_primary.ReadChunk("my_ecs251_file", "00000002", chunkIdStr);
}

void readFile()
{
    Shadow_Directory gfs_master
            { "http://127.0.0.1:8384", "1234567890", "Directory", "00000002" };

    Json::Value result, result_P, result_A, result_B;

    result = gfs_master.ObtainChunkURL("my_ecs251_file", "00000002", "0");

    std::string url_primary = (result["primary"]).asString();
    Shadow_Replica gfs_primary
            { url_primary, "1234567890", "Replica", "00000001" };

    std::string url_secondary_A = (result["secondary_A"]).asString();
    Shadow_Replica gfs_secondary_A
            { url_secondary_A, "1234567890", "Replica", "00000002" };

    std::string url_secondary_B = (result["secondary_B"]).asString();
    Shadow_Replica gfs_secondary_B
            { url_secondary_B, "1234567890", "Replica", "00000003" };

    Json::Value readResultP = gfs_primary.ReadFile("my_ecs251_file", "00000002");
    Json::Value readResultSA = gfs_secondary_A.ReadFile("my_ecs251_file", "00000002");
    Json::Value readResultSB = gfs_secondary_B.ReadFile("my_ecs251_file", "00000002");

    if(readResultP["data"] == readResultSA["data"] && readResultSA["data"] == readResultSB["data"])
    {
        std::cout << "Reading File is consistent\n";
        std::cout << "Read data is: " << readResultP["data"] << std::endl;
    }
    else
    {
        std::cout << "Reading File is inconsistent\n";
        std::cout << "Primary read: " << readResultP["data"] << "\n";
        std::cout << "SecondaryA read: " << readResultSA["data"] << "\n";
        std::cout << "SecondaryB read: " << readResultSB["data"] << "\n";
    }
}

void SerialWrite()
{
    std::string chunkData[2] = {"ecs 251 serial chunk 0", "ecs 251 serial chunk 1"};

    Shadow_Directory gfs_master
            { "http://127.0.0.1:8384", "1234567890", "Directory", "00000002" };

    Json::Value result[2], result_P, result_A, result_B;

    result[0] = gfs_master.ObtainChunkURL("my_ecs251_file", "00000002", "1");
    result[1] = gfs_master.ObtainChunkURL("my_ecs251_file", "00000002", "0");

    // serialize chunk writes
    if(result[0]["chunkIndex"] > result[1]["chunkIndex"])
    {
        Json::Value temp = result[0];
        result[0] = result[1];
        result[1] = temp;
    }

//    writeChunksToReplicas(result[0], )

}

int
main()
{
    std::thread writeClient1 {writeChunk, 0, "ecs 251 chunk 0"};
    std::thread writeClient2 {writeChunk, 1, "ecs 251 chunk 1"};
    usleep(100000); // 0.1 second

    std::thread readFileClient1 {readFile};

    writeClient1.join();
    writeClient2.join();
    readFileClient1.join();

    // erase data

    std::cout << "\n*****" << "erasing data" << "*******" << std::endl;
    std::thread eraseClient1 {writeChunk, 0, ""};
    std::thread eraseClient2 {writeChunk, 1, ""};

    eraseClient1.join();
    eraseClient2.join();
    std::cout << "*****" << "erasing data completed" << "*******\n" << std::endl;

    return 0;
}
