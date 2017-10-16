#include <iostream>
#include <string>
#include <bitset>
#include <utility>
#include  <iomanip>
#include <cmath>

using namespace std;

long sixteenBitMask = 0xFFFF;


long getInput(int numberOfInputs) {
    string output;
    for (int i = 0; i < numberOfInputs; i++) {
        int input;
        cin >> hex >> input;
        string s = bitset<8>(static_cast<unsigned long long int>(input)).to_string();
        output += s;
    }
    long out = bitset<32>(output).to_ulong();

    return out;
}

long leftRotateBy(int i, const long &key) {
    long rotatedKey = ((key << i) | key >> (16 - i)) & sixteenBitMask;
    return rotatedKey;
}


long getSubKey(int i, const long &leftKey, const long &rightKey) {
    return (leftRotateBy(i, leftKey) << 16) | leftRotateBy(i, rightKey);
}


long SBox(const long &intermediate) {

    int s1[] = {0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8, 0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7};
    int s2[] = {0x5, 0x6, 0xC, 0xF, 0x8, 0xA, 0x0, 0x4, 0xB, 0x3, 0x7, 0xD, 0xE, 0x1, 0x2, 0x9};

    long intermediateAfterSBox = 0;
    long validBits = 0b1111;
    for (unsigned long i = 0; i <= 28; i += 4) {
        long temp = (intermediate >> i) & validBits;
        long tempAfterSBox;
        if (i % 8 != 0) { // use s1-box
            tempAfterSBox = s1[temp];
        } else { // use s2-box
            tempAfterSBox = s2[temp];
        }
        intermediateAfterSBox |= tempAfterSBox << i;
    }
    return intermediateAfterSBox;

}


long permutation(const long &s) {
    int p[] = {29, 1, 17, 8,
               30, 22, 28, 6,
               18, 4, 12, 19,
               21, 26, 2, 20,
               31, 10, 9, 25,
               13, 0, 23, 15,
               3, 27, 5, 11,
               7, 14, 24, 16
    };
    long temp = 0;
    long maskBit = 0x80000000;
    for (int i = 0; i < 32; i++) {
        long num = (maskBit >> i) & s;
        if(p[i]-i >=0) {
            temp |= num >> (p[i] - i);
        }else {
            temp |= num << (i - p[i]);
        }

    }
    return temp;
}

long Ffunction(const long &text, const long &key) {
    long intermediate = text ^key;
    long intermediateAfterSBox = SBox(intermediate);
    long permutationString = permutation(intermediateAfterSBox);

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

void printKey(const long &k) {
    bitset<32> key = bitset<32>((unsigned long long int) k);
    for (unsigned long i = 0; i <= 24; i += 8) {
        bitset<32> maskBit = bitset<32>(0xFF);
        bitset<32> subNum = (key >> (24 - i)) & maskBit;
        long num = subNum.to_ulong();
        cout << "0x" << uppercase << setfill('0') << setw(2) << hex << num << ' ';
    }
    cout << '\n';
}


int main() {
    long leftPlain, rightPlain;
    long leftCipher, rightCipher, LeftGuessedCipher, rightGuessedCipher;
    long cipher, guessedCipher;
    cout << "Enter the Plaintext(ex.  0x40 0xFF 0x24 0x33 0x09 0x47 0xF6 0x10): ";
    leftPlain = getInput(4);
    rightPlain = getInput(4);

    cout << "Enter the CipherText(ex. 0xEC 0x2D 0xE1 0x30 0x5B 0x5F 0x5B 0x02): ";
    leftCipher = getInput(4);
    rightCipher = getInput(4);

    cipher = leftCipher << 32 | rightCipher;

    auto bound = (long) pow(2, 32);

    for (long k = 0; k < bound; k++) {
        long leftText = leftPlain;
        long rightText = rightPlain;
        long leftKey = k >> 16;
        long rightKey = 0xFFFF & k;

        for (int i = 1; i <= 10; i++) {
            long subKey;
            subKey = getSubKey(i, leftKey, rightKey);

            long tempNextLeftText = rightText;
            rightText = Ffunction(rightText, subKey);
            rightText = rightText ^ leftText;
            leftText = tempNextLeftText;
        }

        if (k % 1000000 == 0) { //1000000000
            double ttt = (k / (double) bound) * 100;
            cout << ttt << '%'<< '\n';
        }

        LeftGuessedCipher = rightText;
        rightGuessedCipher = leftText;
        guessedCipher = LeftGuessedCipher << 32 | rightGuessedCipher;
        if (guessedCipher == cipher) {
            cout << "The Key is: ";
            printKey(k);
            break;
        }


    }


}