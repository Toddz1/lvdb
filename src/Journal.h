//
// Created by Administrator on 2020/4/5.
//

#ifndef LVDB_JOURNAL_H
#define LVDB_JOURNAL_H

#ifndef WIN32

#include "lvmisc.h"
#include <iostream>
#include <string>
#include <vector>


using namespace  std;


typedef pair<string,string> KV_p;
typedef pair<int,KV_p> OPERATION_p;

class Journal{
private:
    enum{MAX_BLOCK_SIZE=4096,MAX_LOG_SIZE=4096*1000};
    enum {FILE_ZERO_SIZE = 10};

    struct KeyInfo{
        enum{DELETE=0,PUT=1,GET=2};
        ULL keySequence;
        char type;
        string key;
    };
    struct OperationInfo{
        enum{OPINFO_HEADER_SIZE= sizeof(char)+sizeof(int)+sizeof(int)};
        enum{DELETE=0,PUT=1,GET=2};
        char type;
        int keyLength;
        KeyInfo keyInfo;
        int valueLength;
        string value;
    };
    struct LogInfo{
        enum {LOG_INFO_HEADER_SIZE = sizeof(ULL)+sizeof(int)+sizeof(int),LOG_INFO_MAX_SIZE=MAX_BLOCK_SIZE/4};
        LogInfo():logSequence(0),count(0),size(LOG_INFO_HEADER_SIZE){}
        void setLogSequence(int seq){logSequence=seq;}
        ULL logSequence;
        int count;
        int size;
        vector<OperationInfo> operationVector;
    };
    struct LogChunk{
        enum{LOG_CHUNK_HEADER_SIZE = sizeof(ULL)+sizeof(int)+sizeof(char)};
        enum{FULL_CHUNK=0,FIRST_CHUNK=1,MIDDLE_CHUNK=2,LAST_CHUNK=3};
        struct Header{
            ULL checkSum;
            int length;
            char type;
            Header():checkSum(0),length(0),type(MIDDLE_CHUNK){}
        };

        void setCheckSumAndLength(){logChunkHeader.length=LOG_CHUNK_HEADER_SIZE+logInfo.size;}
        void setType(char type){logChunkHeader.type=type;}
        Header logChunkHeader;
        LogInfo logInfo;
    };
    struct BlockHeader{
        enum {BLOCK_HEADER_SIZE = sizeof(ULL)+sizeof(ULL)+sizeof(int)};
        ULL logSequence;
        ULL keySequence;
        off_t nextChunkOffset;
    };
public:
    Journal(string filePath);
    ~Journal();
    void initBlockerHeader();
    void initData(Data& data,OperationInfo& opInfo,OPERATION_p& op);
    vector<LogChunk> initLogChunk(vector<OperationInfo>&vc);
    void _writeLogChunk(LogChunk& logChunk);
    Data _write(OPERATION_p op);
    bool _write(vector<OPERATION_p> ops);
    bool _read();

private:
    BlockHeader _blockHeader;
    int _fileDescriptor;
    off_t _logFileSize;
    off_t _lastBlockOffset;
};

void test_journal();



#endif

#endif //LVDB_JOURNAL_H
