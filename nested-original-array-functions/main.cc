
#include <iostream>

// Fourth-level inner function that processes the array
void fourthLevelFunction(float* array, int size) {
    for (int i = 0; i < size; ++i) {
        array[i] /= 2; // Example processing: dividing each element by 2
    }
}

// Third-level inner function that calls the fourth-level function
void thirdLevelFunction(float* array, int size) {
    for (int i = 0; i < size; ++i) {
        array[i] -= 1; // Example processing: decrementing each element by 1
    }

    // Call the fourth-level function
    fourthLevelFunction(array, size);
}

// Second-level inner function that calls the third-level function
void secondLevelFunction(float* array, int size) {
    for (int i = 0; i < size; ++i) {
        array[i] *= 2; // Example processing: multiplying each element by 2
    }

    // Call the third-level function
    thirdLevelFunction(array, size);
}

// Outer function that calls the second-level function
void outerFunction(float** array, int size) {
    for (int i = 0; i < size; ++i) {
        (*array)[i] += 1; // Example processing: incrementing each element by 1
    }

    // Call the second-level function
    secondLevelFunction(*array, size);
}

int main() {
    int size = 5;
    float* array = new float[size]{1.0, 2.0, 3.0, 4.0, 5.0};

    // Call the outer function
    outerFunction(&array, size);

    // Print the modified array
    for (int i = 0; i < size; ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;

    // Clean up
    delete[] array;
    return 0;
}
