#ifndef NBA_QUERIES_QUERY_STRUCTS_H
#define NBA_QUERIES_QUERY_STRUCTS_H

class car_col_store {
public:
    int num_ticks;
    int num_cars;
    int64_t ** t;
    double ** x;
    double ** y;
    double ** z;

    car_col_store(int64_t num_ticks, int num_cars) {
        this->num_ticks = num_ticks;
        this->num_cars = num_cars;

        t = (int64_t **) malloc(sizeof(int64_t*) * num_cars);
        x = (double **) malloc(sizeof(double*) * num_cars);
        y = (double **) malloc(sizeof(double*) * num_cars);
        z = (double **) malloc(sizeof(double*) * num_cars);

        for (int i = 0; i < num_cars; i++) {
            x[i] = (double *) malloc(sizeof(double) * num_ticks);
            y[i] = (double *) malloc(sizeof(double) * num_ticks);
            z[i] = (double *) malloc(sizeof(double) * num_ticks);
        }
    }

    ~car_col_store(){
        for (int i = 0; i < num_cars; i++) {
            free(x[i]);
            free(y[i]);
            free(z[i]);
        }
        free(x);
        free(y);
        free(z);
    }
};

#endif //NBA_QUERIES_QUERY_STRUCTS_H
