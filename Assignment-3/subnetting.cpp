#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

// Convert dotted decimal IP address into four integers
bool convertIP(string ip, int parts[])
{
    stringstream ss(ip);
    string segment;
    int index = 0;

    while (getline(ss, segment, '.'))
    {
        if (index >= 4)
            return false;

        parts[index] = stoi(segment);

        if (parts[index] < 0 || parts[index] > 255)
            return false;

        index++;
    }

    return index == 4;
}

// Convert four integers into an IP address
string createIP(int a, int b, int c, int d)
{
    return to_string(a) + "." +
           to_string(b) + "." +
           to_string(c) + "." +
           to_string(d);
}

// Calculate subnet mask from CIDR prefix
string calculateSubnetMask(int prefix)
{
    int mask[4] = {0, 0, 0, 0};

    for (int i = 0; i < 4; i++)
    {
        int remainingBits = prefix - (i * 8);

        if (remainingBits >= 8)
        {
            mask[i] = 255;
        }
        else if (remainingBits > 0)
        {
            mask[i] = 256 - pow(2, 8 - remainingBits);
        }
        else
        {
            mask[i] = 0;
        }
    }

    return createIP(mask[0], mask[1], mask[2], mask[3]);
}

// Convert IP address into a 32-bit integer
unsigned long long ipToNumber(int ip[])
{
    unsigned long long result = 0;

    result = ((unsigned long long)ip[0] << 24) |
             ((unsigned long long)ip[1] << 16) |
             ((unsigned long long)ip[2] << 8) |
             ip[3];

    return result;
}

// Convert 32-bit number back into IP address
string numberToIP(unsigned long long number)
{
    int a = (number >> 24) & 255;
    int b = (number >> 16) & 255;
    int c = (number >> 8) & 255;
    int d = number & 255;

    return createIP(a, b, c, d);
}

void calculateSubnetDetails()
{
    string ip;
    int prefix;

    cout << "\nEnter IPv4 address: ";
    cin >> ip;

    cout << "Enter CIDR prefix (example: 24): ";
    cin >> prefix;

    int ipParts[4];

    if (!convertIP(ip, ipParts) || prefix < 0 || prefix > 32)
    {
        cout << "Invalid IP address or prefix.\n";
        return;
    }

    string subnetMask = calculateSubnetMask(prefix);

    unsigned long long ipNumber = ipToNumber(ipParts);

    // Create 32-bit subnet mask
    unsigned long long maskNumber;

    if (prefix == 0)
        maskNumber = 0;
    else
        maskNumber = (0xFFFFFFFFULL << (32 - prefix)) & 0xFFFFFFFFULL;

    unsigned long long networkNumber =
        ipNumber & maskNumber;

    unsigned long long broadcastNumber =
        networkNumber | (~maskNumber & 0xFFFFFFFFULL);

    unsigned long long totalAddresses =
        broadcastNumber - networkNumber + 1;

    cout << "\n=============================================\n";
    cout << "          SUBNET INFORMATION\n";
    cout << "=============================================\n";

    cout << "IP Address       : " << ip << endl;
    cout << "CIDR Prefix      : /" << prefix << endl;
    cout << "Subnet Mask      : " << subnetMask << endl;
    cout << "Network Address  : "
         << numberToIP(networkNumber) << endl;

    cout << "Broadcast Address: "
         << numberToIP(broadcastNumber) << endl;

    cout << "Total Addresses  : "
         << totalAddresses << endl;

    if (prefix <= 30)
    {
        cout << "Usable Hosts     : "
             << totalAddresses - 2 << endl;

        cout << "First Host       : "
             << numberToIP(networkNumber + 1) << endl;

        cout << "Last Host        : "
             << numberToIP(broadcastNumber - 1) << endl;
    }

    cout << "=============================================\n";
}

void createSubnets()
{
    string ip;
    int originalPrefix;
    int requiredSubnets;

    cout << "\nEnter network address: ";
    cin >> ip;

    cout << "Enter original prefix: ";
    cin >> originalPrefix;

    cout << "Enter required number of subnets: ";
    cin >> requiredSubnets;

    int ipParts[4];

    if (!convertIP(ip, ipParts) ||
        originalPrefix < 0 ||
        originalPrefix > 30 ||
        requiredSubnets <= 0)
    {
        cout << "Invalid input.\n";
        return;
    }

    // Find number of bits to borrow
    int borrowedBits = 0;

    while (pow(2, borrowedBits) < requiredSubnets)
    {
        borrowedBits++;
    }

    int newPrefix = originalPrefix + borrowedBits;

    if (newPrefix > 30)
    {
        cout << "Cannot create the requested number of usable subnets.\n";
        return;
    }

    int actualSubnets = pow(2, borrowedBits);

    string subnetMask = calculateSubnetMask(newPrefix);

    unsigned long long baseNetwork =
        ipToNumber(ipParts);

    unsigned long long originalMask;

    if (originalPrefix == 0)
        originalMask = 0;
    else
        originalMask =
            (0xFFFFFFFFULL << (32 - originalPrefix))
            & 0xFFFFFFFFULL;

    baseNetwork = baseNetwork & originalMask;

    unsigned long long subnetSize =
        1ULL << (32 - newPrefix);

    cout << "\n=============================================\n";
    cout << "             SUBNET CALCULATION\n";
    cout << "=============================================\n";

    cout << "Original Network : "
         << numberToIP(baseNetwork) << endl;

    cout << "Original Prefix  : /"
         << originalPrefix << endl;

    cout << "Required Subnets : "
         << requiredSubnets << endl;

    cout << "Bits Borrowed    : "
         << borrowedBits << endl;

    cout << "New Prefix       : /"
         << newPrefix << endl;

    cout << "Subnet Mask      : "
         << subnetMask << endl;

    cout << "Actual Subnets   : "
         << actualSubnets << endl;

    cout << "Usable Hosts/Subnet: "
         << subnetSize - 2 << endl;

    cout << "\n------------- SUBNET DETAILS -------------\n";

    for (int i = 0; i < requiredSubnets; i++)
    {
        unsigned long long subnetNetwork =
            baseNetwork + (i * subnetSize);

        unsigned long long subnetBroadcast =
            subnetNetwork + subnetSize - 1;

        cout << "\nSubnet " << i + 1 << endl;

        cout << "Network Address : "
             << numberToIP(subnetNetwork) << endl;

        cout << "Subnet Mask     : "
             << subnetMask << endl;

        cout << "First Host      : "
             << numberToIP(subnetNetwork + 1) << endl;

        cout << "Last Host       : "
             << numberToIP(subnetBroadcast - 1) << endl;

        cout << "Broadcast       : "
             << numberToIP(subnetBroadcast) << endl;
    }
}

int main()
{
    int choice;

    cout << "=============================================\n";
    cout << "        COMPUTER NETWORKS LAB - SUBNETTING\n";
    cout << "=============================================\n";

    while (true)
    {
        cout << "\n1. Calculate Subnet Details";
        cout << "\n2. Create Required Subnets";
        cout << "\n3. Exit";

        cout << "\n\nEnter your choice: ";
        cin >> choice;

        if (choice == 1)
        {
            calculateSubnetDetails();
        }
        else if (choice == 2)
        {
            createSubnets();
        }
        else if (choice == 3)
        {
            cout << "\nProgram completed.\n";
            break;
        }
        else
        {
            cout << "\nInvalid choice.\n";
        }
    }

    return 0;
}