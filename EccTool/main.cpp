//
//  main.cpp
//  EccTool
//
//  Created by Josh Strom on 1/9/14.
//  Copyright (c) 2014 Josh Strom. All rights reserved.
//

#include <iostream>

#include "EccAlg.h"
#include "BigInteger.h"
#include "DefinedCurveDomainParameters.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <algorithm>
#include "Version1KeySerializer.h"

using namespace std;
using namespace ecc;

void PrintHelpMessage();
void ListCurves();
void GenerateKeys(int curveId, string keyName);
void LoadKey(string keyname, bool printPrivate);
void Encrypt(string keyName, string outFile, string msg);
void Decrypt(string keyName, string inFile);

void SaveToFile(const string& fileName, const string& data);
string ReadFromFile(const string& fileName);
bool CheckFileExists(const string& fileName);
void DeleteFileWithConfirmation(const string& fileName);
void ConfirmOverwriteFile(const string& fileName);

void ConfirmOverwriteKeyFile(const string& keyName);
void SaveKeyToFile(const EccAlg& key, const string& keyName);
EccAlg ReadKeyFromFile(const string& keyName);

string MakePrivKeyFileName(const string& keyName);
string MakePubKeyFileName(const string& keyName);

string ToLower(string str);
string TrimWhitespace(string str);

bool UserApproved(const string& question);

int main(int argc, const char * argv[])
{
    try
    {
        // Display help if no args (except the always passed in name) were given.
        if(argc <= 1)
        {
            PrintHelpMessage();
            cout << endl;
            return 0;
        }
        
        // At least one additional arg was given.
        string firstArg = ToLower(argv[1]);
        
        // Look for one of the defined flags.
        if(firstArg == "-c") // The list curves flag.
        {
            ListCurves();
            cout << endl;
            return 0;
        }
        if(firstArg == "-g") // The generate keys flag.
        {
            if(argc < 4)
            {
                throw invalid_argument("Curve ID and key name required.");
            }
            
            int curveId = atoi(argv[2]);
            string keyName(argv[3]);
            GenerateKeys(curveId, TrimWhitespace(keyName));
            cout << endl;
            return 0;
        }
        if(firstArg == "-e") // The encrypt message flag.
        {
            if(argc < 5)
                throw invalid_argument("Key name, out file, and message are required.");
            
            string key = argv[2];
            string outFile = argv[3];
            string message = argv[4];
            
            Encrypt(key, outFile, message);
            cout << endl;
            return 0;
        }
        if(firstArg == "-d") // The decrypt message flag.
        {
            if(argc < 4)
                throw invalid_argument("Key name and in file required.");
            
            string key = argv[2];
            string inFile = argv[3];
            
            Decrypt(key, inFile);
            cout << endl;
            return 0;

        }
        if(firstArg == "-l") // The load key (print info) flag.
        {
            if(argc < 3)
            {
                throw invalid_argument("Key name required.");
                return 0;
            }
            
            string keyName(argv[2]);
            
            bool printPrivateKey = false;
            if(argc > 3)
            {
                if(ToLower(argv[3]) == "-p")
                    printPrivateKey = true;
            }
            
            LoadKey(keyName, printPrivateKey);
            cout << endl;
            return 0;
        }
        else // Any other flag, print help message.
        {
            PrintHelpMessage();
            cout << endl;
            return 0;
        }
    }
    catch(exception& ex) // Catch any thrown exceptions and print to console.
    {
        cout << "Error: " << ex.what() << endl << endl;
    }
}

void PrintHelpMessage()
{
    cout << "The following commands are supported:" << endl;
    cout << "    -c                            List supported ECC curves." << endl;
    cout << "    -g <curve ID><key name>       Generate keys with specified algorithm ID " << endl;
    cout << "                                  (from list of ECC curves) and save with" << endl;
    cout << "                                  the specified name." << endl;
    cout << "    -l <keyname> [-p]             Print key information for the specified key." << endl;
    cout << "    -e <keyname> <outfile> <msg>  Encrypt the message with the indicated key. " << endl;
    cout << "                                  Store in indicated file." << endl;
    cout << "    -d <keyname> <infile>         Decrypt the message in the indicated " << endl;
    cout << "                                  file with the indicated key." << endl;
    cout << "    -h                            Display this help message." << endl;
}

void ListCurves()
{
    auto curves = GetSupportedCurves();
    cout << "Supported curves:" << endl;
    for(size_t i = 0; i < curves.size(); i++)
    {
        cout << i + 1 << "     " << curves[i] << endl;
    }
}

// Generates a keypair and saves it to disk.
void GenerateKeys(int curveId, string keyName)
{
    auto curves = GetSupportedCurves();
    
    // Check curveId.
    if((curveId <= 0) || (curveId > static_cast<int>(curves.size())))
    {
        throw runtime_error("Invalid curve ID.");
    }
    
    // Check size.
    if(keyName.size() == 0)
    {
        throw runtime_error("Invalid key name.");
    }
    
    // Check for key already existing.
    ConfirmOverwriteKeyFile(keyName);
    
    // Generate keys.
    string curveName = curves[curveId-1];
    cout << "Preparing ECC domain parameters for curve \'" << curveName << "\'..." << endl;
    DomainParameters params = GetCurveByName(curveName);
    EllipticCurve curve(params);
    EccAlg alg(curve);
    
    cout << "Generating keys..." << endl;
    alg.GenerateKeys();
    cout << "Keys generated successfully." << endl;
    cout << "Saving..." << endl;
    
    SaveKeyToFile(alg, keyName);
    cout << "Done. Access using \"" << keyName << "\"" << endl;
}

void LoadKey(string keyName, bool printPrivate)
{
    // Open the key and load it in memory.
    EccAlg alg = ReadKeyFromFile(keyName);
    
    cout << "Successfully loaded key. Curve: " << alg.GetCurveName() << endl;
    cout << "Printing key information..." << endl;
    cout << alg.KeysToString(printPrivate) << endl;
}

bool CheckFileExists(const string& fileName)
{
    ifstream file(fileName);
    return file.good();
}

void Encrypt(string keyName, string outFile, string msg)
{
    EccAlg alg = ReadKeyFromFile(keyName);
    cout << "Successfully loaded key. Encrypting message..." << endl;
    
    vector<uint8_t> plaintext(msg.begin(), msg.end());
    auto ciphertext = alg.Encrypt(plaintext);
    
    ConfirmOverwriteFile(outFile);
    string encryptedMessageText = utilities::BytesToHexString(ciphertext);
    SaveToFile(outFile, encryptedMessageText);
    
    cout << "Done. Encrypted message: \'" << encryptedMessageText << "\'" << endl;
    cout << "Message encyprted and stored in \'" << outFile << "\'" << endl;
}

void Decrypt(string keyName, string inFile)
{
    EccAlg alg = ReadKeyFromFile(keyName);
    if(!alg.HasPrivateKey())
        throw runtime_error("Cannot decrypt without private key.");
    cout << "Successfully loaded key. Loading encrypted message..." << endl;
    
    string ciphertextStr = ReadFromFile(inFile);
    auto ciphertext = utilities::HexStringToBytes(ciphertextStr);
    cout << "Encrypted message loaded. Ciphertext: \'" << ciphertextStr << "\'" << endl;
    cout << "Decrypting..." << endl;
        
    auto message = alg.Decrypt(ciphertext);
    string messageStr(message.begin(), message.end());
    
    cout << "Done. Decrypted Message:" << endl;
    cout << messageStr;
    cout << endl;
}

void ConfirmOverwriteKeyFile(const string& keyName)
{
    string pubKeyFileName = MakePubKeyFileName(keyName);
    string privKeyFileName = MakePrivKeyFileName(keyName);
    
    bool keyFilesExist = CheckFileExists(pubKeyFileName) || CheckFileExists(privKeyFileName);
    if(!keyFilesExist)
        return;
    
    if(!UserApproved(string("Key \'").append(keyName).append("\' already exists. Overwrite?")))
        throw runtime_error("Key already exists.");
    
    remove(pubKeyFileName.c_str());
    remove(privKeyFileName.c_str());
}

void ConfirmOverwriteFile(const string& fileName)
{
    if(!CheckFileExists(fileName))
        return;
    
    if(!UserApproved(string("File \'").append(fileName).append("\' already exists. Overwrite?")))
        throw runtime_error("File already exists.");
    
    remove(fileName.c_str());
}

string MakePubKeyFileName(const string& keyName)
{
    return string("pub_").append(keyName).append(".ecckey");
}

string MakePrivKeyFileName(const string& keyName)
{
    return string("priv_").append(keyName).append(".ecckey");
}

void SaveKeyToFile(const EccAlg& key, const string& keyName)
{
    // This routine is only for full keys and will delete existing files
    //  without confirmation.
    if(!key.HasPrivateKey())
        throw runtime_error("Cannot store incomplete keys.");
    
    string pubKeyFileName = MakePubKeyFileName(keyName);
    string privKeyFileName = MakePrivKeyFileName(keyName);
    
    Version1KeySerializer serializer;
    SaveToFile(pubKeyFileName, serializer.SerializePublicKeys(key));
    SaveToFile(privKeyFileName, serializer.SerializePrivateKeys(key));
}

EccAlg ReadKeyFromFile(const string& keyName)
{
    string pubKeyFileName = MakePubKeyFileName(keyName);
    string privKeyFileName = MakePrivKeyFileName(keyName);
    
    // If the private key file exists, that also contains the public key. Load it.
    //  Otherwise, load the public key file.
    
    string keyData;
    if(CheckFileExists(privKeyFileName))
    {
        keyData = ReadFromFile(privKeyFileName);
    }
    else if(CheckFileExists(pubKeyFileName))
    {
        keyData = ReadFromFile(pubKeyFileName);
    }
    
    // At least one must have been found.
    if(keyData.empty())
        throw runtime_error(string("Key \'").append(keyName).append("\' not found."));
    
    return Version1KeySerializer().ParseKeys(keyData);
}

void SaveToFile(const string& fileName, const string& data)
{
    ofstream outStream(fileName, ios::out | ios::binary);
    if(outStream.bad())
        throw runtime_error("Unable to open file for writing.");
    
    outStream << data;
}

string ReadFromFile(const string& fileName)
{
    ifstream inStream(fileName, ios::in | ios::binary);
    if(inStream.bad())
        throw runtime_error("Unable to open file for reading.");
    
    string data;
    inStream >> data;
    
    return data;
}

string ToLower(string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

string TrimWhitespace(string str)
{
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

bool UserApproved(const string& question)
{
    char answer = '0';
    while(answer != 'y' && answer != 'n')
    {
        cout << question << " (y|n): ";
        cin >> answer;
    }
    
    return (answer == 'y');
}

