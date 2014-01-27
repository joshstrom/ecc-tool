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

using namespace std;
using namespace ecc;

void PrintHelpMessage();
void ListCurves();
void GenerateKeys(int curveId, string keyName);
void LoadKey(string keyname, bool printPrivate);

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
    cout << "    -c                        List supported ECC curves." << endl;
    cout << "    -g <curve ID><key name>   Generate keys with specified algorithm ID (from list" << endl;
    cout << "                              of ECC curves) and save with specified name." << endl;
    cout << "    -l <keyname> [-p]         Print key information for the specified key. Use -p to " << endl;
    cout << "                              print private key." << endl;
    cout << "    -h                        Display this help message." << endl;
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
        cout << "Invalid curve ID." << endl;
        return;
    }
    
    // Check size.
    if(keyName.size() == 0)
    {
        cout << "Invalid key name." << endl;
        return;
    }
    
    // Check for key already existing.
    string keyFileName = keyName + ".ecckey";
    if(ifstream(keyFileName))
    {
        stringstream ss;
        ss << "Key\'" << keyName << "\' already exists. Overwrite?";
        if(!UserApproved(ss.str()))
        {
            cout << "Opeation Cancelled." << endl;
            return;
        }
        
        remove(keyFileName.c_str());
    }
    
    // Open file for writing (ensures that we can make it).
    ofstream file;
    file.open(keyFileName, ios::out | ios::binary);
    if(!file.good())
    {
        cout << "Unable to access key file." << endl;
        return;
    }
    
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
    
    file << curveName << ':' << alg.SaveKeys();
    file.close();
    
    cout << "Done. Access using \"" << keyName << "\"" << endl;
}

void LoadKey(string keyName, bool printPrivate)
{
    // Open the key and load it in memory.
    string keyFileName = keyName + ".ecckey";
    
    ifstream file;
    file.open(keyFileName, ios::in | ios::binary);
    if(!file.good())
    {
        cout << "Unable to open key \'" << keyName << "\'." << endl;
        return;
    }
    
    string archivedKey;
    file >> archivedKey;
    
    // Read the first segment of the file to get the curve name.
    size_t elementDelimiter = archivedKey.find(":");
    if(elementDelimiter == string::npos)
    {
        cout << "Invalid file format." << endl;
        return;
    }
    
    string curveName = archivedKey.substr(0, elementDelimiter);
    archivedKey.erase(0, elementDelimiter + 1);

    DomainParameters params = GetCurveByName(curveName);
    EllipticCurve curve(params);
    EccAlg alg(curve);
    alg.LoadKeys(archivedKey);
    
    cout << "Successfully loaded key. Curve: " << curveName << endl;
    cout << "Printing key information..." << endl;
    cout << alg.KeysToString(printPrivate) << endl;
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

