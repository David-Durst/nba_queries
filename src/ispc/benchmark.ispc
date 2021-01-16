task void square_task(uniform int N, uniform int span, uniform int input[], uniform int result[]) {
        uniform int indexStart = taskIndex * span;
        uniform int indexEnd = min(N, indexStart + span);

        foreach (i = indexStart ... indexEnd) {
            result[i] = input[i] * input[i];
        }
}

export void square(uniform int input[], uniform int result[], uniform int N) {
    uniform int span = N / 64;

    launch[N/span] square_task(N, span, input, result);
}
