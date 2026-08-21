#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Find the number of parity bits required
int calculateParityBits(int dataBits)
{
    int r = 0;

    while ((1 << r) < (dataBits + r + 1))
    {
        r++;
    }

    return r;
}

// Generate Hamming Code
string generateHammingCode(string data)
{
    int m = data.length();
    int r = calculateParityBits(m);

    int totalBits = m + r;

    // 1-indexed array
    vector<int> hamming(totalBits + 1, 0);

    // Place data bits in positions that are NOT powers of 2
    int dataIndex = 0;

    for (int position = 1; position <= totalBits; position++)
    {
        // Check whether position is NOT a power of 2
        if ((position & (position - 1)) != 0)
        {
            hamming[position] = data[dataIndex] - '0';
            dataIndex++;
        }
    }

    // Calculate parity bits
    for (int i = 0; i < r; i++)
    {
        int parityPosition = 1 << i;
        int parity = 0;

        for (int position = 1; position <= totalBits; position++)
        {
            if ((position & parityPosition) != 0)
            {
                if (position != parityPosition)
                {
                    parity = parity ^ hamming[position];
                }
            }
        }

        hamming[parityPosition] = parity;
    }

    // Convert array to string
    string code = "";

    for (int position = 1; position <= totalBits; position++)
    {
        code += char(hamming[position] + '0');
    }

    return code;
}

// Find error position using syndrome
int findErrorPosition(string received)
{
    int n = received.length();

    int errorPosition = 0;

    int r = 0;

    while ((1 << r) <= n)
    {
        r++;
    }

    for (int i = 0; i < r; i++)
    {
        int parityPosition = 1 << i;
        int parity = 0;

        for (int position = 1; position <= n; position++)
        {
            if ((position & parityPosition) != 0)
            {
                parity = parity ^ (received[position - 1] - '0');
            }
        }

        if (parity != 0)
        {
            errorPosition += parityPosition;
        }
    }

    return errorPosition;
}

// Correct the error
string correctError(string received, int errorPosition)
{
    if (errorPosition > 0 && errorPosition <= received.length())
    {
        int index = errorPosition - 1;

        if (received[index] == '0')
            received[index] = '1';
        else
            received[index] = '0';
    }

    return received;
}

// Extract original data bits
string extractData(string hammingCode)
{
    string data = "";

    for (int position = 1; position <= hammingCode.length(); position++)
    {
        // Skip parity bit positions
        if ((position & (position - 1)) != 0)
        {
            data += hammingCode[position - 1];
        }
    }

    return data;
}

// Introduce a single-bit error
string introduceError(string code, int position)
{
    if (position >= 1 && position <= code.length())
    {
        int index = position - 1;

        if (code[index] == '0')
            code[index] = '1';
        else
            code[index] = '0';
    }

    return code;
}

int main()
{
    cout << "=============================================\n";
    cout << "       HAMMING CODE - ERROR CONTROL\n";
    cout << "=============================================\n";

    string data;

    cout << "\nEnter data bits (example: 1011): ";
    cin >> data;

    // Validate input
    for (char bit : data)
    {
        if (bit != '0' && bit != '1')
        {
            cout << "Invalid input. Enter only 0 and 1.\n";
            return 0;
        }
    }

    // -----------------------------------------
    // TRANSMITTER SIDE
    // -----------------------------------------

    cout << "\n----------- TRANSMITTER SIDE -----------\n";

    int parityBits = calculateParityBits(data.length());

    string hammingCode = generateHammingCode(data);

    cout << "Original Data        : " << data << endl;
    cout << "Number of Data Bits  : " << data.length() << endl;
    cout << "Parity Bits Required : " << parityBits << endl;
    cout << "Generated Hamming Code: " << hammingCode << endl;

    // -----------------------------------------
    // RECEIVER SIDE - NO ERROR
    // -----------------------------------------

    cout << "\n-------- RECEIVER SIDE - NO ERROR --------\n";

    string receivedCorrect = hammingCode;

    int errorPosition = findErrorPosition(receivedCorrect);

    cout << "Received Code : " << receivedCorrect << endl;
    cout << "Syndrome      : " << errorPosition << endl;

    if (errorPosition == 0)
    {
        cout << "Status        : No error detected." << endl;
    }

    cout << "Decoded Data  : " << extractData(receivedCorrect) << endl;

    // -----------------------------------------
    // RECEIVER SIDE - WITH ERROR
    // -----------------------------------------

    cout << "\n--------- RECEIVER SIDE - ERROR ---------\n";

    int position;

    cout << "Enter bit position to introduce error (1-"
         << hammingCode.length() << "): ";

    cin >> position;

    string corruptedCode = introduceError(hammingCode, position);

    cout << "\nOriginal Code : " << hammingCode << endl;
    cout << "Corrupted Code: " << corruptedCode << endl;

    errorPosition = findErrorPosition(corruptedCode);

    cout << "Syndrome      : " << errorPosition << endl;

    if (errorPosition != 0)
    {
        cout << "Error detected at position: "
             << errorPosition << endl;

        string correctedCode =
            correctError(corruptedCode, errorPosition);

        cout << "Corrected Code: " << correctedCode << endl;
        cout << "Decoded Data  : "
             << extractData(correctedCode) << endl;
    }
    else
    {
        cout << "No error detected." << endl;
    }

    cout << "\n=============================================\n";
    cout << "       Hamming Code Execution Complete\n";
    cout << "=============================================\n";

    return 0;
}