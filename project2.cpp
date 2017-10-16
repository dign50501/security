#include <iostream>
#include <string>
#include <bitset>
#include <utility>
#include  <iomanip>
#include <cmath>

using namespace std;

bitset<32> sixteenBitMask = bitset<32>(0xFFFF);

int s1[] = {0xE, 0x4, 0xD, 0x1, 0x2, 0xF, 0xB, 0x8, 0x3, 0xA, 0x6, 0xC, 0x5, 0x9, 0x0, 0x7};
int s2[] = {0x5, 0x6, 0xC, 0xF, 0x8, 0xA, 0x0, 0x4, 0xB, 0x3, 0x7, 0xD, 0xE, 0x1, 0x2, 0x9};


int p[] = {29, 1, 17, 8,
           30, 22, 28, 6,
           18, 4, 12, 19,
           21, 26, 2, 20,
           31, 10, 9, 25,
           13, 0, 23, 15,
           3, 27, 5, 11,
           7, 14, 24, 16
};

bitset<32> getInput(int numberOfInputs) {
    string output;
    for (int i = 0; i < numberOfInputs; i++) {
        int input;
        cin >> hex >> input;
        string s = bitset<8>(static_cast<unsigned long long int>(input)).to_string();
        output += s;
    }

    return bitset<32>(output);
}

bitset<32> leftRotateBy(int i, const bitset<32> &key) {
    bitset<32> rotatedKey = ((key << i) | key >> (16 - i)) & sixteenBitMask;
    return rotatedKey;
}


bitset<32> getSubKey(int i, const bitset<32> &leftKey, const bitset<32> &rightKey) {
    return (leftRotateBy(i, leftKey) << 16) | leftRotateBy(i, rightKey);
}


bitset<32> SBox(const bitset<32> &intermediate) {
    bitset<32> intermediateAfterSBox = bitset<32>(0);
    long longIntermediate = intermediate.to_ulong();
    long validBits = 0b1111;
    for (unsigned long i = 0; i <= 28; i += 4) {
        long temp = (longIntermediate >> i) & validBits;
        bitset<32> tempAfterSBox;
        if (i % 8 != 0) { // use s1-box
            tempAfterSBox = bitset<32>(static_cast<unsigned long long int>(s1[temp]));
        } else { // use s2-box
            tempAfterSBox = bitset<32>(static_cast<unsigned long long int>(s2[temp]));
        }
        intermediateAfterSBox |= tempAfterSBox << i;
    }

    return intermediateAfterSBox;

}

bitset<32> permutation(const bitset<32> &s) {
    bitset<32> temp = bitset<32>(0);
    for (int i = 0; i < 32; i++) {
        int beforeBit = i;
        int secondBit = p[i];
        temp[31 - secondBit] = s[31 - beforeBit];
    }
    return temp;
}

bitset<32> Ffunction(const bitset<32> &text, const bitset<32> &key) {
    bitset<32> intermediate = bitset<32>(text) ^bitset<32>(key);
    bitset<32> intermediateAfterSBox = SBox(intermediate);
    bitset<32> permutationString = permutation(intermediateAfterSBox);

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

void printKey(const bitset<32> &key) {
    for (unsigned long i = 0; i <= 24; i += 8) {
        bitset<32> maskBit = bitset<32>(0xFF);
        bitset<32> subNum = (key >> (24 - i)) & maskBit;
        long num = subNum.to_ulong();
        cout << "0x" << uppercase << setfill('0') << setw(2) << hex << num << ' ';
    }
    cout << '\n';
}


int main() {
    bitset<32> leftPlain, rightPlain;
    bitset<32> leftCipher, rightCipher, LeftGuessedCipher, rightGuessedCipher;

    cout << "Enter the Plaintext(ex.  0x40 0xFF 0x24 0x33 0x09 0x47 0xF6 0x10): ";
    leftPlain = getInput(4);
    rightPlain = getInput(4);

    cout << "Enter the CipherText(ex. 0xEC 0x2D 0xE1 0x30 0x5B 0x5F 0x5B 0x02): ";
    leftCipher = getInput(4);
    rightCipher = getInput(4);

    auto bound = (long) pow(2, 32);

    for (long k = 0; k < bound; k++) {
        bitset<32> leftText = leftPlain;
        bitset<32> rightText = rightPlain;
        bitset<32> key = bitset<32>(static_cast<unsigned long long int>(k));
        bitset<32> leftKey = key >> 16;
        bitset<32> rightKey = bitset<32>(0b1111111111111111) & key;;

        for (int i = 1; i <= 10; i++) {
            bitset<32> subKey;
            subKey = getSubKey(i, leftKey, rightKey);

            bitset<32> tempNextLeftText = rightText;
            rightText = Ffunction(rightText, subKey);
            rightText = (bitset<32>(rightText) ^ bitset<32>(leftText));
            leftText = tempNextLeftText;
        }

        if (k % 100000 == 0) {
            double ttt = (k / (double) bound) * 100;
            cout << ttt << '\n';
        }

        LeftGuessedCipher = rightText;
        rightGuessedCipher = leftText;
        long LeftGuess = (LeftGuessedCipher ^ leftCipher).to_ulong();
        long RightGuess = (rightGuessedCipher ^ rightCipher).to_ulong();
        if (LeftGuess == 0 && RightGuess == 0) {
            cout << "The Key is: ";
            printKey(key);
            break;
        }


    }


}