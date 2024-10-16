#include "Json_Manager.h"
#include <fstream>

CJson_Manager::CJson_Manager()
{
}

void CJson_Manager::WriteJson(Json::Value _value, const wstring& _strSavePath)
{
    if (_value.empty() || TEXT("") == _strSavePath) {
        MSG_BOX("JSON root is Empty or Wrong PathName");
        return;
    }

    Json::StreamWriterBuilder builder;
    const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    
    std::ofstream ofStream;
    ofStream.open(_strSavePath, ios_base::out);
    
    if (ofStream.is_open()) {
        writer->write(_value, &ofStream);
    }
    else {
        return;
    }
    
    ofStream.close();
}

Json::Value CJson_Manager::ReadJson(const wstring& _strReadPath)
{
    Json::Value root;

    if (TEXT("") == _strReadPath)
        return root;

    ifstream fin(_strReadPath);

    if (fin.is_open()) {
        fin >> root;
    }

    return root;
}

CJson_Manager* CJson_Manager::Create()
{
    CJson_Manager* pInstance = new CJson_Manager();

    return pInstance;
}

void CJson_Manager::Free()
{

}
