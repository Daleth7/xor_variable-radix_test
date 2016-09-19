#include <cstdint>

#include <iostream>
#include <fstream>
#include <sstream>

#include <chrono>
#include <thread>

using digit_type = unsigned short;
using size_type = std::size_t;

char const * const d_filepath = "xor_test_result.csv";
const size_type base_max = 10001;

digit_type logical_xor_new(digit_type a, digit_type b, size_type base);
digit_type reverse_xor_new(digit_type orig, digit_type key, size_type base);
digit_type logical_xor_old(digit_type a, digit_type b, size_type base);
digit_type reverse_xor_old(digit_type orig, digit_type key, size_type base);
void perform_test(std::ofstream& dest, size_type base);


int main(){
    using namespace std;

    cout << "Performing reversibility test of exclusive-or algorithms...\n";

    ofstream data(d_filepath);
    if(!data.is_open()){
        cerr << "Error. Could not open " << d_filepath << " for writing." << endl;
        return 0;
    }

    data
        << "Radix,"
        << "Left Operand,Right Operand,"
        << "Result (new),Reversed (new),Matches (new),"
        << "Result (old),Reversed (old),Matches (old),"
        << '\n';

    for(size_type i = 0; i < base_max; ++i){
        cout << "Performing test on base " << i << '\n';
        perform_test(data, i);
    }

    cout << "Test completed..." << endl;
    data << flush;
    data.close();

    return 0;
}


digit_type logical_xor_new(digit_type a, digit_type b, size_type base)
    {return (a+b)%base;}

digit_type reverse_xor_new(digit_type orig, digit_type key, size_type base){
    digit_type toreturn = logical_xor_new(orig, key, base);
    for(size_type i = 1; i < base-1; ++i)
        toreturn = logical_xor_new(toreturn, key, base);
    return toreturn;
}

digit_type logical_xor_old(digit_type a, digit_type b, size_type base)
    {return (base-a+b)%base;}

digit_type reverse_xor_old(digit_type orig, digit_type key, size_type base)
    {return logical_xor_old(orig, key, base);}

void perform_test(std::ofstream& dest, size_type base){
    std::stringstream buf;

    buf << base;
    for(digit_type left_op = 0; left_op < base; ++left_op){
        for(digit_type right_op = 0; right_op < base; ++right_op){
            buf << ','; // Print "Radix,"
                // Print "Left Operand,Right Operand,"
            buf << left_op << ',' << right_op << ',';
                // Print "Result (new),Reversed (new),Matches (new)"
                //       "Result (old),Reversed (old),Matches (old)"
            const digit_type
                        res_new = logical_xor_new(left_op, right_op, base),
                        rev_new = reverse_xor_new(res_new, right_op, base),
                        res_old = logical_xor_old(left_op, right_op, base),
                        rev_old = reverse_xor_old(res_new, right_op, base)
                        ;
            buf
                << res_new << ',' << rev_new << ',' << (left_op == rev_new) << ','
                << res_old << ',' << rev_old << ',' << (left_op == rev_old) << ','
                ;
            buf << '\n';

                // Allow the thread to sleep, so the cpu is not totally
                //  consumed for this brute force task.
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }

    dest << buf.rdbuf();
}