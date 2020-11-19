unsigned int factorial(unsigned int number) {
    return number <= 1 ? 1 : factorial(number-1)*number;
}
