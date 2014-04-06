//
//  The MIT License (MIT)
//
//  Copyright (c) 2014 Joshua Strom
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//
#include <iostream>
#include "EccAlg.h"
#include "BigInteger.h"
#include "DefinedCurveDomainParameters.h"
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <algorithm>
#include <tuple>
#include "KeySerializer.h"

using namespace std;
using namespace ecc;

void PrintHelpMessage();
void ListCurves();
void GenerateKeys(int curveId, string keyName);
void LoadKey(string keyname, bool printPrivate);
void Encrypt(string keyName, string outFile, string msg);
void Decrypt(string keyName, string inFile);
void Sign(const string& keyname, const string& inFile, const string& outFile);
void Verify(const string& keyname, const string& inFile);

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
        if(firstArg == "-s")
        {
            if(argc < 5)
                throw invalid_argument("Need keyname, infile, and outfile.");
            
            string keyname(argv[2]);
            string inFile(argv[3]);
            string outFile(argv[4]);
            
            Sign(keyname, inFile, outFile);
            cout << endl;
            return 0;
                
        }
        if(firstArg == "-v")
        {
            if(argc < 4)
                throw invalid_argument("Need keyname and infile.");
            
            string keyname(argv[2]);
            string inFile(argv[3]);
            
            Verify(keyname, inFile);
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
    cout << "    -g <curve ID><keyname>        Generate keys with specified algorithm ID " << endl;
    cout << "                                  (from list of ECC curves) and save with" << endl;
    cout << "                                  the specified name." << endl;
    cout << "    -l <keyname> [-p]             Print key information for the specified key." << endl;
    cout << "                                  Optionally print private key information." << endl;
    cout << "    -e <keyname> <out> <msg>      Encrypt the message with the indicated key. " << endl;
    cout << "                                  Store in indicated file." << endl;
    cout << "    -d <keyname> <in>             Decrypt the message in the indicated " << endl;
    cout << "                                  file with the indicated key." << endl;
    cout << "    -s <keyname> <in> <out>       Signs the message in the indicated " << endl;
    cout << "                                  file with the indicated key. Saves the" << endl;
    cout << "                                  file with signature append to out file." << endl;
    cout << "    -v <keyname> <in>             Verfies the signed message in the" << endl;
    cout << "                                  indicated file. Prints \"Valid\"" << endl;
    cout << "                                  or \"Invalid\"." << endl;
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

// Loads the specified key and prints details to console.
void LoadKey(string keyName, bool printPrivate)
{
    // Open the key and load it in memory.
	cout << "Loading key \"" << keyName << "\"..." << endl;
    EccAlg alg = ReadKeyFromFile(keyName);
    
    cout << "Successfully loaded key. Curve: " << alg.GetCurveName() << endl;
    cout << "Printing key information..." << endl;
    cout << alg.KeysToString(printPrivate) << endl;
}

// Encrypt the message with the key and store in the file.
void Encrypt(string keyName, string outFile, string msg)
{
	cout << "Loading key \"" << keyName << "\"..." << endl;
    EccAlg alg = ReadKeyFromFile(keyName);
	cout << "Successfully loaded key." << endl;
	cout << "Encrypting message..." << endl;
    
    vector<uint8_t> plaintext(msg.begin(), msg.end());
    auto ciphertext = alg.Encrypt(plaintext);
    
    ConfirmOverwriteFile(outFile);
    string encryptedMessageText = utilities::BytesToHexString(ciphertext);
    SaveToFile(outFile, encryptedMessageText);
    
	cout << "Message successfully encrypted." << endl;
	cout << "Encrypted message:" << endl;
	cout << encryptedMessageText << endl;
    cout << "Encrypted message stored in \"" << outFile << "\"" << endl;
}

// Decrypts the message from file with the key.
void Decrypt(string keyName, string inFile)
{
	cout << "Loading key \"" << keyName << "\"..." << endl;
    EccAlg alg = ReadKeyFromFile(keyName);
    if(!alg.HasPrivateKey())
        throw runtime_error("Cannot decrypt without private key.");
	cout << "Successfully loaded key." << endl;
	cout << "Loading encrypted message..." << endl;
    
    string ciphertextStr = ReadFromFile(inFile);
    auto ciphertext = utilities::HexStringToBytes(ciphertextStr);
	cout << "Encrypted message loaded." << endl;
	cout << "Ciphertext:" << endl;
	cout << ciphertextStr << endl;
    cout << "Decrypting..." << endl;
        
    auto message = alg.Decrypt(ciphertext);
    string messageStr(message.begin(), message.end());
    
    cout << "Message successfully decrypted. Decrypted Message:" << endl;
    cout << messageStr;
    cout << endl;
}

string BuildSignatureBlock(const string& message, const vector<uint8_t>& signature)
{
    stringstream ss;
    ss << message << "--SIGNATURE--" << utilities::BytesToHexString(signature) << "--END SIGNATURE--";
    
    return ss.str();
}

tuple<string, vector<uint8_t>> ReadSignatureBlock(const string& signatureBlock)
{
    string signatureBeginDelimiter("--SIGNATURE--");
    string signatureEndDelimiter("--END SIGNATURE--");
    
    // Find the beginning of the signature. This describes the message.
    size_t signatureSegmentBeginIndex = signatureBlock.find(signatureBeginDelimiter);
    if(signatureSegmentBeginIndex == string::npos)
        throw invalid_argument("Invalid signed message format.");
    auto message = signatureBlock.substr(0, signatureSegmentBeginIndex);
    
    // Find the end if the signature. This describes the signature.
    size_t signatureEndIndex = signatureBlock.find(signatureEndDelimiter, signatureSegmentBeginIndex);
    if(signatureEndIndex == string::npos)
        throw invalid_argument("Invalid signed message format.");
    
    auto signatureBeginIndex = signatureSegmentBeginIndex + signatureBeginDelimiter.size();
    auto signatureSize = signatureEndIndex - signatureBeginIndex;
    auto signatureStr = signatureBlock.substr(signatureBeginIndex, signatureSize);
    return make_tuple(message, utilities::HexStringToBytes(signatureStr));
}

void Sign(const string& keyName, const string& inFile, const string& outFile)
{
    cout << "Loading key \"" << keyName << "\"..." << endl;
    EccAlg alg = ReadKeyFromFile(keyName);
    if(!alg.HasPrivateKey())
        throw runtime_error("Cannot sign without private key.");
	cout << "Successfully loaded key." << endl;
	cout << "Loading message to sign..." << endl;
    
    string message = ReadFromFile(inFile);
    vector<uint8_t> messageBytes(message.begin(), message.end());
    cout << "Message loaded." << endl;
    cout << "Signing..." << endl;
    auto signature = alg.Sign(messageBytes);
    
    cout << "Message successfully signed." << endl;
    
    ConfirmOverwriteFile(outFile);
    string signatureBlock = BuildSignatureBlock(message, signature);
    SaveToFile(outFile, signatureBlock);
    
}

void Verify(const string& keyName, const string& inFile)
{
    cout << "Loading key \"" << keyName << "\"..." << endl;
    EccAlg alg = ReadKeyFromFile(keyName);
	cout << "Successfully loaded key." << endl;
	cout << "Loading message and signature to verify..." << endl;
    
    string message = ReadFromFile(inFile);
    auto signatureParts = ReadSignatureBlock(message);
    string parsedMessage = get<0>(signatureParts);
    vector<uint8_t> messageBytes(parsedMessage.begin(), parsedMessage.end());
    vector<uint8_t> parsedSignature = get<1>(signatureParts);
    cout << "Verifying..." << endl;
    bool result = alg.Verify(messageBytes, parsedSignature);
    
    cout << (result ? "Valid" : "Invalid") << endl;
}

// Helper function to see if the given file exists.
bool CheckFileExists(const string& fileName)
{
    ifstream file(fileName);
    return file.good();
}

// Helper function to delete (for later overwrite) the given key file(s) with user confirmation.
void ConfirmOverwriteKeyFile(const string& keyName)
{
    string pubKeyFileName = MakePubKeyFileName(keyName);
    string privKeyFileName = MakePrivKeyFileName(keyName);
    
    bool keyFilesExist = CheckFileExists(pubKeyFileName) || CheckFileExists(privKeyFileName);
    if(!keyFilesExist)
        return;
    
    if(!UserApproved(string("Key \"").append(keyName).append("\" already exists. Overwrite?")))
        throw runtime_error("Key already exists.");
    
    remove(pubKeyFileName.c_str());
    remove(privKeyFileName.c_str());
}

// Helper function to delete (for later overwrite) the given file with user confirmation.
void ConfirmOverwriteFile(const string& fileName)
{
    if(!CheckFileExists(fileName))
        return;
    
    if(!UserApproved(string("File \"").append(fileName).append("\" already exists. Overwrite?")))
        throw runtime_error("File already exists.");
    
    remove(fileName.c_str());
}

// Helper function to create the public key file name.
string MakePubKeyFileName(const string& keyName)
{
    return string("pub_").append(keyName).append(".ecckey");
}

// Helper function to create the private key file name.
string MakePrivKeyFileName(const string& keyName)
{
    return string("priv_").append(keyName).append(".ecckey");
}

// Saves the given key (with the specified key name) to disk. Requires privat key.
void SaveKeyToFile(const EccAlg& key, const string& keyName)
{
    // This routine is only for full keys (private key included).
    if(!key.HasPrivateKey())
        throw runtime_error("Cannot store incomplete keys.");
    
	// Key stored in the following manor:
	//	pubKeyKeyFile = pub key only
	//	privKeyFile = pub + priv
	//	This will allow (for example) the pub key file to be distributed without
	//	disclosing the private key.
    string pubKeyFileName = MakePubKeyFileName(keyName);
    string privKeyFileName = MakePrivKeyFileName(keyName);
    
    KeySerializer serializer;
    SaveToFile(pubKeyFileName, serializer.SerializePublicKeys(key));
    SaveToFile(privKeyFileName, serializer.SerializePrivateKeys(key));
}

// Creates an ECC algorithm by reading the key data from file.
// Note that this may return an algorithm with just the public key
// or the public and private keys.
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
    
    return KeySerializer().ParseKeys(keyData);
}

// Saves the provided string to the specified file.
void SaveToFile(const string& fileName, const string& data)
{
    ofstream outStream(fileName, ios::out | ios::binary);
    if(outStream.bad())
        throw runtime_error("Unable to open file for writing.");
    
    outStream << data;
}

// Reads all data from the specified file (as a string).
string ReadFromFile(const string& fileName)
{
    ifstream inStream(fileName, ios::in | ios::binary);
    if(inStream.bad())
        throw runtime_error("Unable to open file for reading.");
    
    stringstream data;
    data << inStream.rdbuf();
    
    return data.str();
}

// Converts all letters in the string to lowercase.
string ToLower(string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// Returns a copy of the given string with all whitespace removed.
string TrimWhitespace(string str)
{
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

// Asks for user approval with the given (yes/no) question.
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

