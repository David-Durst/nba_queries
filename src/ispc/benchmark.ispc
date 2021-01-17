export void square_sequential(uniform int N, uniform int input[], uniform int result[]) {
        foreach (i = 0 ... N) {
            result[i] = input[i] * input[i];
        }
}

task void square_task(uniform int N, uniform int span, uniform int input[], uniform int result[]) {
        uniform int indexStart = taskIndex * span;
        uniform int indexEnd = min(N, indexStart + span);

        foreach (i = indexStart ... indexEnd) {
            result[i] = input[i] * input[i];
        }
}

export void square_parallel(uniform int N, uniform int input[], uniform int result[]) {
    uniform int span = N / 64;

    launch[N/span] square_task(N, span, input, result);
}



task void four_ops_unfused_task(uniform int N, uniform int span, uniform int a[], uniform int b[], 
                              uniform int c[], uniform int d[], uniform int result[]) {
        uniform int indexStart = taskIndex * span;
        uniform int indexEnd = min(N, indexStart + span);

        foreach (i = indexStart ... indexEnd) {
            result[i] = a[i] * b[i];
        }

        foreach (i = indexStart ... indexEnd) {
            result[i] += c[i];
        }

        foreach (i = indexStart ... indexEnd) {
            result[i] -= d[i];
        }
}

export void four_ops_unfused(uniform int N, uniform int a[], uniform int b[], uniform int c[], 
                           uniform int d[], uniform int result[]) {
    uniform int span = N / 64;

    launch[N/span] four_ops_unfused_task(N, span, a, b, c, d, result);
}

task void four_ops_fused_task(uniform int N, uniform int span, uniform int a[], uniform int b[], 
                              uniform int c[], uniform int d[], uniform int result[]) {
        uniform int indexStart = taskIndex * span;
        uniform int indexEnd = min(N, indexStart + span);

        foreach (i = indexStart ... indexEnd) {
            result[i] = ((a[i] * b[i]) + c[i]) - d[i];
        }
}

export void four_ops_fused(uniform int N, uniform int a[], uniform int b[], uniform int c[], 
                           uniform int d[], uniform int result[]) {
    uniform int span = N / 64;

    launch[N/span] four_ops_fused_task(N, span, a, b, c, d, result);
}
