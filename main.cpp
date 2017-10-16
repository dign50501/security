#include <iostream>
#include <string>
#include <bitset>
#include <utility>
#include <iomanip>

using namespace std;

string getInput(int numberOfInputs) {
    string output;
    for (int i = 0; i < numberOfInputs; i++) {
        int input;
        cin >> hex >> input;
        string s = bitset<8>(static_cast<unsigned long long int>(input)).to_string();
        output += s;
    }
    return output;
}

string leftRotateBy(int i, const string &key) {
    int num = stoi(key, nullptr, 2);
    num = (num << i) | num >> (16 - i);
    string s = bitset<16>(static_cast<unsigned long long int>(num)).to_string();
    return s;
}

string getSubKey(int i, const string &leftKey, const string &rightKey) {
    return leftRotateBy(i, leftKey) + leftRotateBy(i, rightKey);
}

string SBox(const string &intermediate) {
    int s1[] = {0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8, 0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7};
    int s2[] = {0x5, 0x6, 0xC, 0xF, 0x8, 0xA, 0x0, 0x4, 0xB, 0x3, 0x7, 0xD, 0xE, 0x1, 0x2, 0x9};

    string intermediateAfterSBox;
    for (unsigned long i = 0; i <= 28; i += 4) {
        string temp = intermediate.substr(i, 4);
        string tempAfterSBox;
        if (i % 8 == 0) { // use s1-box
            int n = stoi(temp, nullptr, 2);
            tempAfterSBox = bitset<4>(static_cast<unsigned long long int>(s1[n])).to_string();
            intermediateAfterSBox += tempAfterSBox;
        } else { // use s2-box
            int n = stoi(temp, nullptr, 2);
            tempAfterSBox = bitset<4>(static_cast<unsigned long long int>(s2[n])).to_string();
            intermediateAfterSBox += tempAfterSBox;
        }
    }

    return intermediateAfterSBox;

}

string permutation(const string &s) {
    pair<int, int> p[] = {make_pair(0, 29), make_pair(1, 1), make_pair(2, 17), make_pair(3, 8),
                          make_pair(4, 30), make_pair(5, 22), make_pair(6, 28), make_pair(7, 6),
                          make_pair(8, 18), make_pair(9, 4), make_pair(10, 12), make_pair(11, 19),
                          make_pair(12, 21), make_pair(13, 26), make_pair(14, 2), make_pair(15, 20),
                          make_pair(16, 31), make_pair(17, 10), make_pair(18, 9), make_pair(19, 25),
                          make_pair(20, 13), make_pair(21, 0), make_pair(22, 23), make_pair(23, 15),
                          make_pair(24, 3), make_pair(25, 27), make_pair(26, 5), make_pair(27, 11),
                          make_pair(28, 7), make_pair(29, 14), make_pair(30, 24), make_pair(31, 16)};
    string temp = "22232223222322232223222322232223";
    for (auto const &element : p) {
        int beforeBit = element.first;
        int secondBit = element.second;

        temp[secondBit] = s[beforeBit];
    }

    return temp;
}

string Ffunction(const string &text, const string &key) {
    string intermediate = (bitset<32>(text) ^ bitset<32>(key)).to_string();
    string intermediateAfterSBox = SBox(intermediate);
    string permutationString = permutation(intermediateAfterSBox);

    return permutationString;

}

void printInHex(const string &cipher) {
    for (unsigned long i = 0; i <= 56; i += 8) {
        string subString = cipher.substr(i, 8);
        int num = static_cast<int>(bitset<8>(subString).to_ulong());
        cout << "0x" << uppercase << setfill('0') << setw(2) << hex << num << ' ';
    }
    cout << '\n';
}

int main() {
    string leftKey, rightKey;
    string leftText, rightText;
    string cipher;

    cout << "Enter the Key(ex. 0x04 0x34 0xEF 0x71): ";
    leftKey = getInput(2);
    rightKey = getInput(2);

    cout << "Enter the Plaintext(ex. 0x10 0x24 0xAA 0x9F 0x47 0x3C 0x58 0xC1): ";
    leftText = getInput(4);
    rightText = getInput(4);

    // for 10 rounds
    for (int i = 1; i <= 10; i++) {
        string subKey;
        subKey = getSubKey(i, leftKey, rightKey);

        string tempNextLeftText = rightText;
        rightText = Ffunction(rightText, subKey);
        rightText = (bitset<32>(rightText) ^ bitset<32>(leftText)).to_string();
        leftText = tempNextLeftText;
    }

    cipher = rightText + leftText;

    cout << "Cipher Text: ";
    printInHex(cipher);

    return 0;
}