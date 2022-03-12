
// ecs251 miniGFS

#include <iostream>
#include "Shadow_Directory.h"
#include "Shadow_Replica.h"
#include <thread>
#include <unistd.h>
#include <vector>

#define simulateSerial

#define simulateConcurrent

#ifdef simulateConcurrent
#undef simulateSerial
#endif

Json::Value retrieveDataAndTimeStamp(Json::Value in)
{
    Json::Value result;
    std::stringstream ss { in["data"].asString()};
    std::string segment;
    std::vector<std::string> segList;

    while(std::getline(ss, segment, '|'))
    {
        segList.push_back(segment);
    }
    result["data"] = segList[0];
    result["timestamp"] = segList[1];
    return result;
}

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

void readFile()
{
    Shadow_Directory gfs_master
            { "http://127.0.0.1:8384", "1234567890", "Directory", "00000002" };

    Json::Value result[2];

    result[0] = gfs_master.ObtainChunkURL("my_ecs251_file", "00000002", "0");
    result[1] = gfs_master.ObtainChunkURL("my_ecs251_file", "00000002", "1");

    std::string url_primary[2], url_secondary_A[2], url_secondary_B[2];

    url_primary[0] = (result[0]["primary"]).asString();
    url_primary[1] = (result[1]["primary"]).asString();

    Shadow_Replica gfs_primary0
            { url_primary[0], "1234567890", "Replica", "00000001" };

    Shadow_Replica gfs_primary1
            { url_primary[1], "1234567890", "Replica", "00000001" };

    url_secondary_A[0] = (result[0]["secondary_A"]).asString();
    url_secondary_A[1] = (result[1]["secondary_A"]).asString();

    Shadow_Replica gfs_secondary_A0
            { url_secondary_A[0], "1234567890", "Replica", "00000002" };

    Shadow_Replica gfs_secondary_A1
            { url_secondary_A[1], "1234567890", "Replica", "00000002" };

    url_secondary_B[0] = (result[0]["secondary_B"]).asString();
    url_secondary_B[1] = (result[1]["secondary_B"]).asString();

    Shadow_Replica gfs_secondary_B0
            { url_secondary_B[0], "1234567890", "Replica", "00000003" };

    Shadow_Replica gfs_secondary_B1
            { url_secondary_B[1], "1234567890", "Replica", "00000003" };

    Json::Value readResultP[2], readResultSA[2], readResultSB[2];

    readResultP[0] = gfs_primary0.ReadChunk("my_ecs251_file", "00000002", "0");
    readResultP[1] = gfs_primary1.ReadChunk("my_ecs251_file", "00000002", "1");

     readResultSA[0] = gfs_secondary_A0.ReadChunk("my_ecs251_file", "00000002", "0");
     readResultSA[1] = gfs_secondary_A1.ReadChunk("my_ecs251_file", "00000002", "1");

     readResultSB[0] = gfs_secondary_B0.ReadChunk("my_ecs251_file", "00000002", "0");
     readResultSB[1] = gfs_secondary_B1.ReadChunk("my_ecs251_file", "00000002", "1");

    std::string readDataP[2], readDataSA[2], readDataSB[2];
    long long readTimestampP[2], readTimestampSA[2], readTimestampSB[2];

    Json::Value temp[2];
    for(uint32_t i = 0; i< 2; ++i) {
        temp[i] = retrieveDataAndTimeStamp(readResultP[i]);
        readDataP[i] = temp[i]["data"].asString();
        readTimestampP[i] = std::stoll(temp[i]["timestamp"].asString());
    }

    for(uint32_t i = 0; i< 2; ++i) {
        temp[i] = retrieveDataAndTimeStamp(readResultSA[i]);
        readDataSA[i] = temp[i]["data"].asString();
        readTimestampSA[i] = std::stoll(temp[i]["timestamp"].asString());
    }

    for(uint32_t i = 0; i< 2; ++i) {
        temp[i] = retrieveDataAndTimeStamp(readResultSB[i]);
        readDataSB[i] = temp[i]["data"].asString();
        readTimestampSB[i] = std::stoll(temp[i]["timestamp"].asString());
    }

    if(readDataP[0] == readDataSA[0] && readDataSA[0] == readDataSB[0] &&
            readDataP[1] == readDataSA[1] && readDataSA[1] == readDataSB[1])
    {
        std::cout << "Reading File is consistent\n";
        std::cout << "Read data is: \tChunk0: " << readDataP[0] << "\tChunk1: " << readDataP[1] << std::endl;

        if(     (readTimestampP[0] <= readTimestampP[1]) &&
                (readTimestampSA[0] <= readTimestampSA[1]) &&
                (readTimestampSB[0] <= readTimestampSB[1])
                )
        {
            std::cout << "Reading File is defined as all replicas wrote Chunk0 first and then Chunk1 \n";
            return;
        }
        std::cout << "Reading File is undefined! as the order of writing chunks is NOT Chunk0 first and then Chunk1\n";
    }
    else
    {
        std::cout << "Reading File is inconsistent\n";
        std::cout << "Primary read:    \tChunk0: " << readDataP[0]  << "\tChunk1: " << readDataP[1]  << std::endl;
        std::cout << "SecondaryA read: \tChunk0: " << readDataSA[0] << "\tChunk1: " << readDataSA[1] << std::endl;
        std::cout << "SecondaryB read: \tChunk0: " << readDataSB[0] << "\tChunk1: " << readDataSB[1] << std::endl;
    }
}

int
main()
{
    std::thread writeClient1 {writeChunk, 0, "ecs 251 client0 writing to chunk 0"};
#ifdef simulateSerial
    usleep(100000); // 0.1 second
#endif
    std::thread writeClient2 {writeChunk, 1, "ecs 251 client1 writing to chunk 1"};
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
    std::cout << "*****" << "erasing data completed" << "*******" << std::endl;

    return 0;
}
